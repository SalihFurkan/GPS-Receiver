#include "acquisition.h"
#include "initSettings.h"
#include "NE10.h"
#include "NE10_types.h"

void generateCaCode(int PRN, short caCode[]);

void acquisition(int8_t *data, int** acqResults)
{
	printf("Success1!!\n");
	int8_t *signal1 = malloc(SAMPLES_PER_CODE);
	printf("Success2!!\n");
	int8_t *signal2 = malloc(SAMPLES_PER_CODE);
	printf("Success3!!\n");
	int i;
	int j;
	FILE *fid;
	fid = fopen("caCodes.bin", "rb");

	int8_t counter[1023];
	fread(counter,1,1023,fid);
	fclose(fid);

	for(j = 0; j < SAMPLES_PER_CODE; j++ )
	{
		signal1[j] = data[j];
	}

	for(j = 0 ; j < SAMPLES_PER_CODE; j++ )
	{
		signal2[j] = data[j + SAMPLES_PER_CODE];
	}



	//signal0DC

	short *caCodesTable = malloc(sizeof(short)*SAMPLES_PER_CODE);
	int PRN;
	short caCode[1023];
	int *codeValueIndex = malloc(sizeof(int)*SAMPLES_PER_CODE);

	double coef = ((double) CODE_LENGTH/(double) SAMPLES_PER_CODE);
	int ind;

	int *frqBins = malloc(sizeof(int)*NUMBER_OF_FRQ_BINS);

	ne10_fft_cfg_float32_t cfg; // An FFT "configuration structure"
	ne10_fft_cpx_float32_t *caCodesTableForFFT =  calloc(LIMIT, sizeof(float)*2);
	ne10_fft_cpx_float32_t *caCodeFreq = calloc(LIMIT, sizeof(float)*2);

	if (ne10_init() != NE10_OK)
	{
		fprintf(stderr, "Failed to initialise Ne10.\n");
		return;
	}
	printf("NE10 init success.\n");

	int codePhaseIndex1 = 0;
	int codePhaseIndex2 = 0;
	int carrFreq[32];
	for (int w=0;w<2;w++)
		carrFreq[w]=0;
	int codePhase[32];
	float peakMetric[32];



						 //
	float *sinCarr = malloc(sizeof(float)*SAMPLES_PER_CODE);
	float *cosCarr = malloc(sizeof(float)*SAMPLES_PER_CODE);

	ne10_fft_cpx_float32_t *IQtimeDom1 = calloc(LIMIT, sizeof(float)*2);
	ne10_fft_cpx_float32_t *IQtimeDom2 = calloc(LIMIT, sizeof(float)*2);

	float *res1 = calloc(LIMIT, sizeof(float));
	float *res2 = calloc(LIMIT, sizeof(float));

	ne10_fft_cpx_float32_t *IQfreqDom1 = calloc(LIMIT, sizeof(float)*2);
	ne10_fft_cpx_float32_t *IQfreqDom2 = calloc(LIMIT, sizeof(float)*2);

	ne10_fft_cpx_float32_t *convCodeIQ1 = calloc(LIMIT, sizeof(float)*2);
	ne10_fft_cpx_float32_t *convCodeIQ2 = calloc(LIMIT, sizeof(float)*2);

	ne10_fft_cpx_float32_t *acqRes1 = calloc(LIMIT, sizeof(float)*2);
	ne10_fft_cpx_float32_t *acqRes2 = calloc(LIMIT, sizeof(float)*2);
	float peakSize;
	float secondPeakSize;
	float max1;
	float max2;
	int sum = 0;
	for(i = 0; i <1023; i++)
	{
		for(j = 0; j < counter[i]; j++)
		{
			codeValueIndex[sum + j] = i + 1;
		}
		sum = sum + counter[i];
		//printf("%d", counter[i]);
	}

	for(PRN = 0; PRN < 32; PRN++)
	{
		//printf("%d => ", PRN);

		peakSize = 0;
		secondPeakSize = 0;
		generateCaCode(PRN, caCode);
		cfg = ne10_fft_alloc_c2c_float32_c(LIMIT);
		for(int k = 0; k < SAMPLES_PER_CODE; k++)
		{

			caCodesTable[k] = caCode[codeValueIndex[k]-1];
			caCodesTableForFFT[k].r = (float)caCodesTable[k];
			caCodesTableForFFT[k].i = 0;

			//printf("%d => %5.5f + %5.5fi \n", k,caCodesTableForFFT[k].r,caCodesTableForFFT[i].i );
		}
		for(int k = SAMPLES_PER_CODE; k < LIMIT; k++)
		{
			caCodesTableForFFT[k].r = 0;
			caCodesTableForFFT[k].i = 0;
		}
		//*********************************************************************
		ne10_fft_c2c_1d_float32_c(caCodeFreq, caCodesTableForFFT, cfg,0);
		ne10_fft_destroy_c2c_float32(cfg);
		for(j = 0; j < LIMIT; j++)
		{
			caCodeFreq[j].i = (-1)*caCodeFreq[j].i;
			//printf("%d => %5.5f + %5.5fi \n", j,caCodeFreq[j].r,caCodeFreq[j].i );
		}
		//**************************************************************************

		for(i = 0; i < NUMBER_OF_FRQ_BINS; i++)
		{
			max1 = 0;
			max2 = 0;
			frqBins[i] = IF - (ACQ_SEARCH_BAND/2)*1000 + 500*(i);
			for( j = 0; j < SAMPLES_PER_CODE; j++)
			{
				sinCarr[j] = sin(frqBins[i]*2*M_PI*j/SAMPLING_FREQ);
				cosCarr[j] = cos(frqBins[i]*2*M_PI*j/SAMPLING_FREQ);

				IQtimeDom1[j].r = sinCarr[j]*signal1[j];
				IQtimeDom1[j].i = cosCarr[j]*signal1[j];
				IQtimeDom2[j].r = sinCarr[j]*signal2[j];
				IQtimeDom2[j].i = cosCarr[j]*signal2[j];
			}
			for(int k = SAMPLES_PER_CODE; k < LIMIT; k++)
			{
				IQtimeDom1[k].r = 0;
				IQtimeDom1[k].i = 0;
				IQtimeDom2[k].r = 0;
				IQtimeDom2[k].i = 0;
			}
			cfg = ne10_fft_alloc_c2c_float32_c(LIMIT);
			ne10_fft_c2c_1d_float32_c(IQfreqDom1, IQtimeDom1, cfg,0);
			ne10_fft_destroy_c2c_float32(cfg);
			cfg = ne10_fft_alloc_c2c_float32_c(LIMIT);
			ne10_fft_c2c_1d_float32_c(IQfreqDom2, IQtimeDom2, cfg,0);
			ne10_fft_destroy_c2c_float32(cfg);

			for( j = 0; j < LIMIT; j++)
			{
				convCodeIQ1[j].r = (IQfreqDom1[j].r)*(caCodeFreq[j].r)-(IQfreqDom1[j].i)*(caCodeFreq[j].i);
				convCodeIQ1[j].i = (IQfreqDom1[j].r)*(caCodeFreq[j].i)+(IQfreqDom1[j].i)*(caCodeFreq[j].r);
				convCodeIQ2[j].r = (IQfreqDom2[j].r)*(caCodeFreq[j].r)-(IQfreqDom2[j].i)*(caCodeFreq[j].i);
				convCodeIQ2[j].i = (IQfreqDom2[j].r)*(caCodeFreq[j].i)+(IQfreqDom2[j].i)*(caCodeFreq[j].r);
				//printf("%d ==> %5.5f   i%5.5f \n", j, convCodeIQ1[j].r, convCodeIQ1[j].i);
			}

			cfg = ne10_fft_alloc_c2c_float32_c(LIMIT);
			ne10_fft_c2c_1d_float32_c(acqRes1, convCodeIQ1, cfg, 1);
			ne10_fft_destroy_c2c_float32(cfg);
			cfg = ne10_fft_alloc_c2c_float32_c(LIMIT);
			ne10_fft_c2c_1d_float32_c(acqRes2, convCodeIQ2, cfg, 1);
			ne10_fft_destroy_c2c_float32(cfg);
			for( j = 0; j < LIMIT; j++)
			{
				//in order to not use extra memory, we used res1 and res2
				res1[j] = (acqRes1[j].r)*(acqRes1[j].r) + (acqRes1[j].i)*(acqRes1[j].i);
				res2[j] = (acqRes2[j].r)*(acqRes2[j].r) + (acqRes2[j].i)*(acqRes2[j].i);
				if (res1[j] > max1)
				{
					max1 = res1[j];
					codePhaseIndex1 = j;
				}
				if (res2[j] > max2)
				{
					max2 = res2[j];
					codePhaseIndex2 = j;
				}
			}
			if(max1 > max2)
			{
				if(max1 > peakSize)
				{
					codePhase[PRN] = codePhaseIndex1;
					carrFreq[PRN] = i;
					peakSize = max1;
					for(int k = 0; k < SAMPLES_PER_CODE; k++)
					{
						if(codePhaseIndex1 > SAMPLES_PER_CODE_CHIP && codePhaseIndex1 < SAMPLES_PER_CODE - SAMPLES_PER_CODE_CHIP)
						{
							if(k < codePhaseIndex1 - SAMPLES_PER_CODE_CHIP || k > codePhaseIndex1 + SAMPLES_PER_CODE_CHIP)
							{
								if(res1[k] > secondPeakSize)
								{
									secondPeakSize = res1[k];
								}
							}
						}
						else if(codePhaseIndex1 < SAMPLES_PER_CODE_CHIP + 1)
						{
							if(k > codePhaseIndex1 + SAMPLES_PER_CODE_CHIP && k < SAMPLES_PER_CODE - SAMPLES_PER_CODE_CHIP + codePhaseIndex1)
							{
								if(res1[k] > secondPeakSize)
								{
									secondPeakSize = res1[k];
								}
							}
						}
						else if(codePhaseIndex1 + SAMPLES_PER_CODE_CHIP < SAMPLES_PER_CODE)
						{
							if(k > codePhaseIndex1 + SAMPLES_PER_CODE_CHIP - SAMPLES_PER_CODE && k < codePhaseIndex1 - SAMPLES_PER_CODE_CHIP)
							{
								if(res1[k] > secondPeakSize)
								{
									secondPeakSize = res1[k];
								}
							}
						}
					}
				}
			}
			else
			{
				if(max2 > peakSize)
				{
					codePhase[PRN] = codePhaseIndex2;
					carrFreq[PRN] = i;
					peakSize = max2;
					for(int k = 0; k < SAMPLES_PER_CODE; k++)
					{
						if(codePhaseIndex2 > SAMPLES_PER_CODE_CHIP && codePhaseIndex2 < SAMPLES_PER_CODE - SAMPLES_PER_CODE_CHIP)
						{
							if(k < codePhaseIndex2 - SAMPLES_PER_CODE_CHIP || k > codePhaseIndex2 + SAMPLES_PER_CODE_CHIP)
							{
								if(res2[k] > secondPeakSize)
								{
									secondPeakSize = res2[k];
								}
							}
						}
						else if(codePhaseIndex2 < SAMPLES_PER_CODE_CHIP + 1)
						{
							if(k > codePhaseIndex2 + SAMPLES_PER_CODE_CHIP && k < SAMPLES_PER_CODE - SAMPLES_PER_CODE_CHIP + codePhaseIndex2)
							{
								if(res2[k] > secondPeakSize)
								{
									secondPeakSize = res2[k];
								}
							}
						}
						else if(codePhaseIndex2 + SAMPLES_PER_CODE_CHIP < SAMPLES_PER_CODE)
						{
							if(k > codePhaseIndex2 + SAMPLES_PER_CODE_CHIP - SAMPLES_PER_CODE && k < codePhaseIndex2 - SAMPLES_PER_CODE_CHIP)
							{
								if(res2[k] > secondPeakSize)
								{
									secondPeakSize = res2[k];
								}
							}
						}
					}
				}
			}
		}
		peakMetric[PRN] = peakSize/secondPeakSize;

		if(peakMetric[PRN] > ACQ_THRESHOLD)
		{
			printf("Satellite Number: %d DOPPLER: %d Code Phase: %d \n", PRN+1, (carrFreq[PRN])*500+9540000, codePhase[PRN] );
			acqResults[PRN][0] = (carrFreq[PRN])*500+9540000;
			acqResults[PRN][1] = codePhase[PRN];
			acqResults[PRN][2] = peakMetric[PRN] ;
		}
		else
		{

			printf("Satellite Number: %d DOPPLER: %d Code Phase: %d .........\n", PRN+1,0,0 );
			acqResults[PRN][0] = 0;
			acqResults[PRN][1] = 0;
			acqResults[PRN][2] = 0;
		}
	}
	for(int g = 0; g < 32; g++)
	{

			printf("%d",acqResults[g][0]);
			printf("\n");
			printf("%d" ,acqResults[g][1]);
			printf("\n");
			printf("%d",acqResults[g][2]);
			printf("\n");
	}
		printf("\n");
		printf("\n");


		printf("Success!!\n");
		free(signal1);
		printf("Success!!\n");
		free(signal2);
		printf("Success!!\n");
		free(caCodesTable);
		printf("Success!!\n");
		free(sinCarr);
		printf("Success!!\n");
		free(cosCarr);
		printf("Success!!\n");
		free(frqBins);
		printf("Success!!\n");
		free(codeValueIndex);
		printf("Success!!\n");

}

void generateCaCode(int PRN, short caCode[])
{
	static const short g2s[] = {  5,   6,   7,   8,  17,  18, 139, 140, 141, 251,
			252, 254, 255, 256, 257, 258, 469, 470, 471, 472,
			473, 474, 509, 512, 513, 514, 515, 516, 859, 860,
			861, 862 };

	short g2shift = g2s[PRN];

	short g1[1023];
	short reg[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	short saveBit;
	short regShift[]  = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	int i;
	int j;
	int k;
	//--- Generate all G1 signal chips based on the G1 feedback polynomial -----
	for(i = 0; i < 1023; i++)
	{
		g1[i] = reg[9];
		saveBit = reg[2]*reg[9];
		for(j = 0; j < 9; j++)
		{
			reg[j+1] = regShift[j];

		}
		reg[0] = saveBit;

		for(k = 0; k < 10; k++)
		{
			regShift[k] = reg[k];
		}
	}
	//--- Generate G2 code -----------------------------------------------------
	//******************************* 5 1023 olacak
	short g2[1023];
	short reg2[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	short saveBit2;
	short regShift2[]  = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	for(i = 0; i < 1023; i++)
	{
		g2[i] = reg2[9];
		saveBit2 = reg2[1]*reg2[2]*reg2[5]*reg2[7]*reg2[8]*reg2[9];
		//printf("SAVEBIT: %d \n", saveBit2);
		for(j = 0; j < 9; j++)
		{
			reg2[j+1] = regShift2[j];
		}
		reg2[0] = saveBit2;
		for(k = 0; k < 10; k++)
		{
			regShift2[k] = reg2[k];
			//printf(" reg2: %d ",reg2[k]);
		}
		//printf("\n");
	}
	short g3[1023];
	for(i = 0; i < g2shift; i++)
	{
		g3[i] = g2[1023 - g2shift + i];
	}
	for(i = 0; i < 1023 - g2shift; i++)
	{
		g3[g2shift + i] = g2[i];
	}

	for(i = 0; i < 1023; i++)
	{
		caCode[i] = -1*g1[i]*g3[i];

	}

}
