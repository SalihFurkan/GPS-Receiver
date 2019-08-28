#include <stdio.h>
#include "initSettings.h"
#include "probeData.h"
#include "rtwtypes.h"
#include "tracking.h"
#include <math.h>
#include "NE10.h"
#include "NE10_types.h"
int navPartyChck(int *aa);
void navigation(int *IP,int **acqResults  )
{
	printf("IN NAV \n" );
	int searchStartOffset = 0;
	for(int sat=0;sat<1;sat++)
	{
		//	ne10_fft_cfg_float32_t cfg; // An FFT "configuration structure"
		//	ne10_fft_cpx_float32_t *preamble_ms = calloc(MS_TO_PROCESS, sizeof(float)*2);
		//	ne10_fft_cpx_float32_t *FFT_preamble_ms = calloc(MS_TO_PROCESS, sizeof(float)*2);
		//	ne10_fft_cpx_float32_t *IPforFFT = calloc(MS_TO_PROCESS, sizeof(float)*2);
		//	ne10_fft_cpx_float32_t *FFTofIP = calloc(MS_TO_PROCESS, sizeof(float)*2);
		//	ne10_fft_cpx_float32_t *CorrelationResult = calloc((2*MS_TO_PROCESS-1), sizeof(float)*2);
		//	ne10_fft_cpx_float32_t *IFFT_CorrelationResult = calloc((2*MS_TO_PROCESS-1), sizeof(float)*2);
		//
		int preamble_bits[] = {1, - 1, - 1, - 1, 1, - 1, 1, 1};
		int *preamble_ms = calloc(MS_TO_PROCESS, sizeof(int));
		int *IPforFFT = calloc(MS_TO_PROCESS, sizeof(int));
		int *correlation_result = calloc(2*MS_TO_PROCESS-1, sizeof(int));
		int *firstSubframe = calloc(8,sizeof(int));

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				preamble_ms[j+20*i] = preamble_bits[i];
			}
		}

		printf("IN NAV \n" );
		for (int i = 0;i < MS_TO_PROCESS;i++)
		{
			if (IP[i] > 0)
				IPforFFT[i] = 1;
			else
				IPforFFT[i] = -1;
			//printf("preamble_ms[%d] %d \n",i, preamble_ms[i] );
		}

		printf("IN NAV \n" );
		//	cfg = ne10_fft_alloc_c2c_float32_c(MS_TO_PROCESS);
		//	ne10_fft_c2c_1d_float32_c(FFT_preamble_ms, preamble_ms, cfg,0);
		//	ne10_fft_destroy_c2c_float32(cfg);
		//	cfg = ne10_fft_alloc_c2c_float32_c(MS_TO_PROCESS);
		//	ne10_fft_c2c_1d_float32_c(FFTofIP, IPforFFT, cfg,0);
		//	ne10_fft_destroy_c2c_float32(cfg);
		int xcorrLength = MS_TO_PROCESS;
		int c = 0;
		int *findInd = malloc(sizeof(int)*50);

		for( int i = 0; i < 2*MS_TO_PROCESS-1; i++)
		{
			if(i<MS_TO_PROCESS)
			{
				for(int j = 0; j <= i; j++)
				{
					correlation_result[i] = correlation_result[i] + IPforFFT[i-j]*preamble_ms[MS_TO_PROCESS-j-1];
				}
			}
			else
			{
				for(int j = 0; j <= 2*MS_TO_PROCESS-2-i; j++)
				{
					correlation_result[i] = correlation_result[i] + IPforFFT[MS_TO_PROCESS-j-1]*preamble_ms[-i-j+(2*MS_TO_PROCESS-2)];
					//				if(correlation_result[i] < 0)
					//				{
					//					correlation_result[i] = -1*correlation_result[i];
					//				}

				}
			}
			if( i > MS_TO_PROCESS && (correlation_result[i] > 152) || (correlation_result[i] < -152))
			{
				findInd[c] = i+1 - MS_TO_PROCESS+1;
				c++;

			}
			//		if(correlation_result[i] != 0)
			//		printf("[%d] Real Value : %d \n", i, correlation_result[i]);



		}

		int *bits = malloc(sizeof(int)*1240);
		int *bitshalf = malloc(sizeof(int)*32);
		int *bitshalf2 = malloc(sizeof(int)*32);
		int *sumBits = calloc(62, sizeof(int));
		int *index2 = malloc(sizeof(int)*c);
		int ex = 0;

		for(int k = 0; (k < c && ex == 0); k++)
		{
			//printf("[%d] findInd: %d \n", k, findInd[k]);
			for(int o = 0; o < c; o++)
			{
				index2[o]=findInd[o]-findInd[k];
				//printf("[%d][%d] findInd: %d \n", k, o,index2[o]);
				if(index2[o]==6000)
				{
					//*source=IP[findInd[k]-40];
					memcpy(bits,IP+(findInd[k]-41),1240);
					//					for(int z = 0; z < 1240; z++)
					//						printf("[%d] bits : %d \n", z, bits[z]);
					for(int z = 0; z < 62; z++)
					{
						for(int y = 0; y < 20; y++)
						{
							sumBits[z] = sumBits[z] + bits[z*20+y];
						}
						if(sumBits[z] > 0)
							sumBits[z] = 1;
						else
							sumBits[z] = -1;
						//printf("[%d] bits : % d \n", z, sumBits[z]);
					}
					//navpartycheck1

					memcpy(bitshalf,sumBits,32);
					memcpy(bitshalf2,sumBits+30,32);


					if(navPartyChck(bitshalf)!=0 && navPartyChck(bitshalf2)!=0)
					{
						firstSubframe[sat]=findInd[k];
						printf("firstSubframe : % d \n", firstSubframe[sat]);
						ex++;
						break;
					}
				}
			}
		}


		int *navBitsSamples = calloc(30020,sizeof(int));
		int *sumNavBitsSamples = calloc(30020/20,sizeof(int));

		if(firstSubframe[sat]==0)
		{
			//deactivation
			acqResults[sat][3]=1;
		}

		printf( "acqResults[sat][3] :  % d   \n", acqResults[sat][3]);
//		if(acqResults[sat][3]==0)
//		{

			memcpy(navBitsSamples,IP+(firstSubframe[sat]-20),30020*sizeof(int));

			for(int z = 0; z < 30020/20; z++)
			{
				for(int y = 0; y < 20; y++)
				{
					sumNavBitsSamples[z] = sumNavBitsSamples[z] + navBitsSamples[z*20+y];
				}
				if(sumNavBitsSamples[z] > 0)
					sumNavBitsSamples[z] = 1;
				else
					sumNavBitsSamples[z] = 0;

				printf("[%d] sumNavBitsSamples :  % d  navBitsSamples : % d \n", z, sumNavBitsSamples[z],navBitsSamples[z]);

			}

//		}



	}
}
int navPartyChck(int *aa)
{
	int *parity = calloc(6,sizeof(int));
	int counter=0;
	int status=0;
	if(aa[1]!=1)
		for(int q=2;q<26;q++)
			aa[q]=-1*aa[q];
	parity[0] = aa[0]  * aa[2]  * aa[3]  * aa[4]  * aa[6]  *
			aa[7]  * aa[11] * aa[12] * aa[13] * aa[14] *
			aa[15] * aa[18] * aa[19] * aa[21] * aa[24];

	parity[1] = aa[1]  * aa[3]  * aa[4]  * aa[5]  * aa[7]  *
			aa[8]  * aa[12] * aa[13] * aa[14] * aa[15] *
			aa[16] * aa[19] * aa[20] * aa[22] * aa[25];

	parity[2] = aa[0]  * aa[2]  * aa[4]  * aa[5]  * aa[6]  *
			aa[8]  * aa[9] * aa[13] * aa[14] * aa[15] *
			aa[16] * aa[17] * aa[20] * aa[21] * aa[23];

	parity[3] = aa[1]  * aa[3]  * aa[5]  * aa[6]  * aa[7]  *
			aa[9] * aa[10] * aa[14] * aa[15] * aa[16] *
			aa[17] * aa[18] * aa[21] * aa[22] * aa[24];

	parity[4] = aa[1]  * aa[2]  * aa[4]  * aa[6]  * aa[7]  *
			aa[8]  * aa[10] * aa[11] * aa[15] * aa[16] *
			aa[17] * aa[18] * aa[19] * aa[22] * aa[23] *
			aa[25];

	parity[5] = aa[0]  * aa[4]  * aa[6]  * aa[7]  * aa[9] *
			aa[10] * aa[11] * aa[12] * aa[14] * aa[16]*
			aa[20] * aa[23] * aa[24] * aa[25];
	for(int t=0;t<6;t++)
		if(parity[t]==aa[26+t])
			counter++;
	//	for(int z = 0; z < 6; z++)
	//	{
	//		printf("[%d] parity : % d \n", z, parity[z]);
	//	}


	if(counter==6)
		status=-1*aa[1];
	else
		status=0;
	free(parity);
	return status;
}
