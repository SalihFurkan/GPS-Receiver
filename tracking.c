/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: probeData.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 13-Nov-2018 00:34:03
 */

/* Include Files */
#include <stdio.h>
#include "initSettings.h"
#include "probeData.h"
#include "acquisition.h"
#include "rtwtypes.h"
#include "tracking.h"
#include "navigation.h"
#include <math.h>

extern void probeData8bit(int8_t *data, int msTake, int cursor);
extern void probeDataSampleBySample_8bit(int8_t *data, int sampleSizeToTake, int cursor);
extern void generateCaCode(int PRN, short caCode[]);
extern void navigation(int *IP,int **acqResults);

void tracking(int **acqResults)
{
	double *acqResulted[5];
		for (int q=0;q<5;q++)
		{
			acqResulted[q] = (double *)calloc(4,sizeof(int));
		}
	printf("__LINE__");
	for(int g = 0; g < 5; g++)
	{

		printf("%d",acqResulted[g][0]);
		printf("\n");
		printf("%d" ,acqResulted[g][1]);
		printf("\n");
		printf("%d",acqResulted[g][2]);
		printf("\n");
	}
	acqResulted[0][0] = 9549921.298980713;
	acqResulted[0][1] = 36321;
	acqResulted[0][2] = 14;
	int tot=0;
	printf("Tracking started");
	printf("\n");

	short counter2[1023];
	generateCaCode(acqResulted[0][2], counter2);


	short caCode[1025];
	caCode[0]=counter2[1022];
	//printf("[0]: caCode = %d ,  \n", caCode[0] );

	for (int i=1;i<1024;i++)
	{
		caCode[i]=counter2[i-1];
		//printf("[%d]: caCode = %d ,  \n",i, caCode[i] );

	}

	caCode[1024]=counter2[0];
	//printf("[1024]: caCode = %d ,  \n", caCode[1024] );


	int counter=0;
	double *absoluteSample[5];
	for (int q=0;q<5;q++)
	{
		absoluteSample[q] = (double *)malloc(sizeof(double)*MS_TO_PROCESS);
	}
	double *codeFreq[5];
	for (int q=0;q<5;q++)
	{
		codeFreq[q]  = malloc(sizeof(double)*MS_TO_PROCESS);
	}
	double *carrFreq = malloc(sizeof(double)*MS_TO_PROCESS);

	double *dllDiscr[5];
	for (int q=0;q<5;q++)
	{
		dllDiscr[q]  = malloc(sizeof(double)*MS_TO_PROCESS);
	}

	double *dllDiscrFilt[5];
	for (int q=0;q<5;q++)
	{
		dllDiscrFilt[q]  = malloc(sizeof(double)*MS_TO_PROCESS);
	}
	double *pllDiscr[5];
	for (int q=0;q<5;q++)
	{
		pllDiscr[q]  = malloc(sizeof(double)*MS_TO_PROCESS);
	}
	double *pllDiscrFilt[5];
	for (int q=0;q<5;q++)
	{
		pllDiscrFilt[q]  = malloc(sizeof(double)*MS_TO_PROCESS);
	}

	for (int i=0;i<5;i++){
		for (int j=0;j<MS_TO_PROCESS;j++){

			codeFreq[i][j]=2^32-3;
			dllDiscr[i][j]=2^32-3;
			dllDiscrFilt[i][j]=2^32-3;
			pllDiscr[i][j]=2^32-3;
			pllDiscrFilt[i][j]=2^32-3;
		}
	}

	//OUTPUTS FROM CORRELATORS

	int *IP = malloc(sizeof(int)*MS_TO_PROCESS);

	int *IE = malloc(sizeof(int)*MS_TO_PROCESS);

	int *IL = malloc(sizeof(int)*MS_TO_PROCESS);
	//QUADRATURE PHASE

	int *QE = malloc(sizeof(int)*MS_TO_PROCESS);

	int *QP = malloc(sizeof(int)*MS_TO_PROCESS);

	int *QL = malloc(sizeof(int)*MS_TO_PROCESS);
	int codePeriods=MS_TO_PROCESS;
	double earlyLateSpc=0.5;
	double PDIcode=0.001;
	//işlem önceliği!!!!!!!!!
	double wn=(2*8*0.7)/(4*0.7*0.7+1);
	double tau1code=1/(wn*wn);
	double tau2code=(2*0.7)/wn;

	double wn2=(25*8*0.7)/(4*0.7*0.7+1);
	double tau1carr=0.25/(wn2*wn2);
	double tau2carr=(2*0.7)/wn2;

	double PDIcarr=0.001;


	for (int i=0;i<32;i++){

		if(acqResults[i][0]!=0){
			acqResulted[counter][0] = acqResults[i][0];
			acqResulted[counter][1] = acqResults[i][1];
			acqResulted[counter][2] = acqResults[i][2];
			counter++;
		}

	}


	printf("__LINE__");

	printf("__LINE__");
	double remCodePhase = 0.0;
	double carrFreqTemp = 0.0 ;
	double carrFreqBasis ;
	double remCarrPhase = 0.0;
	double oldCodeNco = 0.0;
	double oldCodeError = 0.0;
	double oldCarrNco = 0.0;
	double oldCarrError = 0.0;
	int codeFreqKurs;
	double codePhaseStep = 0.0267857142857143;
	int blksize = 0;
	int8_t *rawSignal = malloc(sizeof(int8_t)*SAMPLES_PER_CODE);
	double *tcode = malloc(sizeof(double)*SAMPLES_PER_CODE);
	int *tcode2 = malloc(sizeof(int)*SAMPLES_PER_CODE);
	int8_t *earlyCode = malloc(sizeof(int8_t)*SAMPLES_PER_CODE);
	int8_t *lateCode = malloc(sizeof(int8_t)*SAMPLES_PER_CODE);
	int8_t *promptCode = malloc(sizeof(int8_t)*SAMPLES_PER_CODE);
	double *time = malloc(sizeof(double)*(SAMPLES_PER_CODE+1));
	double *trigarg = malloc(sizeof(double)*(SAMPLES_PER_CODE+1));
	double *carrCos = malloc(sizeof(double)*(SAMPLES_PER_CODE));
	double *carrSin = malloc(sizeof(double)*(SAMPLES_PER_CODE));
	double *qBasebandSignal =  malloc(sizeof(double)*(SAMPLES_PER_CODE));
	double *iBasebandSignal =  malloc(sizeof(double)*(SAMPLES_PER_CODE));
	double I_E = 0;
	double Q_E = 0;
	double I_P = 0;
	double Q_P = 0;
	double I_L = 0;
	double Q_L = 0;
	double carrError = 0.0;
	double carrNco = 0;
	double codeError = 0.0;
	double codeNco = 0.0;
	int cursor = 0;
	int masterCursor = acqResulted[0][1] -1;

	for (int i=0;i<1;i++)
	{
		carrFreqTemp = acqResulted[i][0];
		carrFreqBasis = acqResulted[i][0];
		remCarrPhase = 0.0;
		oldCodeNco = 0.0;
		oldCodeError = 0.0;
		oldCarrNco = 0.0;
		oldCarrError = 0.0;
		codeFreqKurs = 0;
		remCodePhase = 0.0;

		cursor = 0;
		for(int q = 0; q<MS_TO_PROCESS;q++)
		{
			if(q > 0)
				codePhaseStep = codeFreq[i][q-1]/SAMPLING_FREQ;
			blksize = ceil((CODE_LENGTH-remCodePhase)/codePhaseStep);
			printf("\n\n\n\n\n\n\n\n codePhaseStep= %0.5f, blksize= %d \n", codePhaseStep, blksize);

			rawSignal = realloc(rawSignal,sizeof(int8_t)*blksize);
			memset(rawSignal,0,blksize);

			tcode = realloc(tcode,sizeof(double)*blksize);
			memset(tcode,0,blksize);

			tcode2 = realloc(tcode2,sizeof(int)*blksize);
			memset(tcode2,0,blksize);

			earlyCode = realloc(earlyCode,sizeof(int8_t)*blksize);
			memset(earlyCode,0,blksize);

			lateCode = realloc(lateCode,sizeof(int8_t)*blksize);
			memset(lateCode,0,blksize);

			promptCode = realloc(promptCode,sizeof(int8_t)*blksize);
			memset(promptCode,0,blksize);

			time = realloc(time,sizeof(double)*(blksize+1));
			memset(time,0,blksize);

			trigarg = realloc(trigarg,sizeof(double)*(blksize+1));
			memset(trigarg,0,blksize);

			carrCos = realloc(carrCos,sizeof(double)*(blksize));
			memset(carrCos,0,blksize);

			carrSin = realloc(carrSin,sizeof(double)*(blksize));
			memset(carrSin,0,blksize);

			qBasebandSignal =  realloc(qBasebandSignal,sizeof(double)*(blksize));
			memset(qBasebandSignal,0,blksize);

			iBasebandSignal =  realloc(iBasebandSignal,sizeof(double)*(blksize));
			memset(iBasebandSignal,0,blksize);
			//printf("  masterCursor = %d   blksize = %d\n", masterCursor, blksize);

			I_E = 0;
			Q_E = 0;
			I_P = 0;
			Q_P = 0;
			I_L = 0;
			Q_L = 0;
			probeDataSampleBySample_8bit(rawSignal, blksize, masterCursor);

			for(int j=0; j<blksize; j++)
			{

				//rawSignal[j]=data[cursor +j+(int)acqResulted[i][1]-1];

				//if(q==2)
				//	printf("[%d]: rawSignal = %d \n",j, rawSignal[j]);
				tcode[j] = remCodePhase-earlyLateSpc + j*codePhaseStep;
				tcode2[j] = ceil(tcode[j]) + 1;
				if(tcode[j]-((int)tcode[j])>0.000001 || tcode[j]<=0)
					tcode2[j] = ceil(tcode[j]) + 1;
				else
					tcode2[j] = ceil(tcode[j]);

				earlyCode[j] = caCode[tcode2[j]-1];

				//				printf("[%d]: tCode2 = %d , earlyCode-earlycodeControl = %d \n",j, tcode2[j], earlyCode[j]-earlycodeControl[j]);

				tcode[j] = remCodePhase + earlyLateSpc + j*codePhaseStep;
				if(tcode[j]-((int)tcode[j])>0.000001 || tcode[j]<=0)
					tcode2[j] = ceil(tcode[j]) + 1;
				else
					tcode2[j] = ceil(tcode[j]);
				lateCode[j] = caCode[tcode2[j]-1];
				//printf("[%d]: tCode = %0.05f , tcode2 = %d \n",j, tcode[j], tcode2[j]);


				tcode[j] = remCodePhase + j*codePhaseStep;
				if(tcode[j]-((int)tcode[j])>0.000001 || tcode[j]<=0)
					tcode2[j] = ceil(tcode[j]) + 1;
				else
					tcode2[j] = ceil(tcode[j]);
				promptCode[j] = caCode[tcode2[j]-1];

				//printf("[%d]: promptCode = %d tCode = %0.05f , tcode2 = %d \n",j, promptCode[j], tcode[j], tcode2[j]);



				time[j] = j*TIME_DIVIDER;
				trigarg[j] = (carrFreqTemp*2*M_PI*time[j])+remCarrPhase;
				if(j == blksize-1)
				{
					j++;
					time[j] = (j)*TIME_DIVIDER;
					trigarg[j] = (carrFreqTemp*2*M_PI*time[j])+remCarrPhase;
					j--;
				}

				carrCos[j]=cos(trigarg[j]);
				carrSin[j]=sin(trigarg[j]);
				//printf("[%d]: trigarg: %0.10f , time[j] = %0.10f  \n", j, trigarg[j], time[j]);

				qBasebandSignal[j] = carrCos[j]*rawSignal[j];
				iBasebandSignal[j] = carrSin[j]*rawSignal[j];

				I_E = I_E + earlyCode[j]*iBasebandSignal[j];
				Q_E = Q_E + earlyCode[j]*qBasebandSignal[j];
				I_P = I_P + promptCode[j]*iBasebandSignal[j];
				Q_P = Q_P + promptCode[j]*qBasebandSignal[j];
				I_L = I_L + lateCode[j]*iBasebandSignal[j];
				Q_L = Q_L + lateCode[j]*qBasebandSignal[j];
				//printf("[%d]: , earlyCode[j] = %d  \n", j, earlyCode[j]);

			}

			printf("trigarg[blksize] = %0.5f \n", trigarg[blksize]);
			remCodePhase = (tcode[blksize-1]+codePhaseStep)-1023.0;
			printf("remCodePhase: %0.15f \n", remCodePhase);
			printf("I_E: %0.05f, Q_E: %0.05f, I_P: %0.05f, Q_P: %0.05f, I_L: %0.05f, Q_L: %0.05f \n ",I_E, Q_E, I_P, Q_P, I_L, Q_L );

			printf("trigarg: %0.05f\n", trigarg[blksize-1]);
			//printf("2 * M_PI: %0.05f\n", 2 * M_PI);
			for(int k=0;k<1000000;k++)
			{
				if(trigarg[blksize]<k*(2 * M_PI))
				{
					remCarrPhase = trigarg[blksize]-(k-1)*(2 * M_PI);
					printf("remCarrPhase: %0.05f\n", remCarrPhase);
					break;

				}

			}

			carrError = atan(Q_P/I_P)/(2*M_PI);
			printf("carrError: %0.05f\n", carrError);
			carrNco = oldCarrNco + (tau2carr/tau1carr)*(carrError-oldCarrError)+carrError*(PDIcarr/tau1carr);
			printf("carrNco: %0.09f\n", carrNco);
			oldCarrNco = carrNco;
			oldCarrError = carrError;

			carrFreqTemp = carrFreqBasis + carrNco;
			printf("carrFreqBasis: %0.09f\n", carrFreqBasis);
			printf("carrFreqTemp: %0.09f\n", carrFreqTemp);

			carrFreq[i] = carrFreqTemp;

			codeError = ((sqrt(I_E*I_E + Q_E*Q_E))-(sqrt(I_L*I_L + Q_L*Q_L)))/((sqrt(I_E*I_E + Q_E*Q_E))+(sqrt(I_L*I_L + Q_L*Q_L)));
			printf("codeError: %0.05f\n", codeError);
			codeNco = oldCodeNco + (tau2code/tau1code)*(codeError-oldCodeError)+codeError*(PDIcode/tau1code);
			//codeError 2 katı geliyor.
			printf("codeNco: %0.05f\n", codeNco);
			oldCodeNco = codeNco;
			oldCodeError = codeError;
			codeFreq[i][q] = CODE_FREQ_BASIS-codeNco;


			//RECORDS
			absoluteSample[i][q] = acqResulted[i][1] + (q+1)*SAMPLES_PER_CODE;
			printf("[%d]: absoluteSample[%d][%d]: %0.05f\n",q,i,q, absoluteSample[i][q]);
			dllDiscr[i][q] = codeError;
			printf("[%d]:dllDiscr[%d][%d]: %0.05f\n",q,i,q, dllDiscr[i][q]);
			dllDiscrFilt[i][q]= codeNco;
			printf("[%d]:dllDiscrFilt[%d][%d]: %0.05f\n",q,i,q, dllDiscrFilt[i][q]);
			pllDiscr[i][q] = carrError;
			printf("[%d]:pllDiscr[%d][%d]: %0.05f\n",q,i,q, pllDiscr[i][q]);
			pllDiscrFilt[i][q]= carrNco;
			printf("[%d]:pllDiscrFilt[%d][%d]: %0.05f\n",q,i,q, pllDiscrFilt[i][q]);

			IE[q] = (int) I_E;
			QE[q] = (int) Q_E;
			IP[q] = (int) I_P;
			QP[q] = (int) Q_P;
			IL[q] = (int) I_L;
			QL[q] = (int) Q_L;

			cursor = cursor + blksize;
			masterCursor = masterCursor + blksize;
		}
		printf("Before navigation");
		navigation(IP,acqResulted);
	}
}


