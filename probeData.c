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
#include "probeData.h"
#include <stdio.h>
#define MAXCHAR 10

void probeData4bit(int8_t *data,  int msTake)
{

	FILE *fid;
	fid = fopen(FILE_NAME, "rb");

	int8_t buffer[msTake*SAMPLES_PER_CODE/2];

	if(fid == NULL)
	{
		printf("Error!\n");
	}
	else
	{
		if(fread(buffer,1,msTake*SAMPLES_PER_CODE/2,fid) == NULL)
		{
			printf("Can't read! \n");
		}
		else
		{
			for(int i = 0; i<msTake*SAMPLES_PER_CODE; i = i + 2)
			{
				data[i] = (buffer[i/2]&(-16))/16;
				if(((buffer[i/2]&15)) >= 8)
				{
					data[i+1] = ((buffer[i/2]&15)<<4)/16 - 16;
				}
				else
				{
					data[i+1] = ((buffer[i/2]&15)<<4)/16;
				}

			}

		}
		fclose(fid);
	}


}

void probeData8bit(int8_t *data, int msTake, int cursor)
{

	FILE *fid;
	fid = fopen(FILE_NAME, "rb");
	int fseekSuccess;


	if(fid == NULL)
	{
		printf("Error!\n");
	}
	else
	{
		fseekSuccess = fseek(fid, cursor, SEEK_SET);
		printf("fseek = %d \n", fseekSuccess);

		if(fread(data,1,msTake*SAMPLES_PER_CODE,fid) == NULL)
		{
			printf("Can't read! \n");
		}
		fclose(fid);
	}


}

void probeDataSampleBySample_8bit(int8_t *data, int sampleSizeToTake, int cursor)
{

	FILE *fid;
	fid = fopen(FILE_NAME, "rb");
	int fseekSuccess;


	if(fid == NULL)
	{
		printf("Error!\n");
	}
	else
	{
		fseekSuccess = fseek(fid, cursor, SEEK_SET);
		printf("fseek = %d \n", fseekSuccess);

		if(fread(data,1,sampleSizeToTake,fid) == NULL)
		{
			printf("Can't read! \n");
		}
		fclose(fid);
	}


}
void probeData(int *data, int msTake)
{

	FILE *fid;
	fid = fopen(FILE_NAME, "rb");

	if(fid == NULL)
	{
		printf("Error!\n");
	}
	else
	{

		if(fread(data,sizeof(int),msTake,fid) == NULL)
		{
			printf("Can't read! \n");
		}
		fclose(fid);
	}


}

void probeBits(int8_t *data, int msTake, int cursor)
{

	FILE *fid;
	fid = fopen(FILE_NAME, "rb");
	int fseekSuccess;


	if(fid == NULL)
	{
		printf("Error!\n");
	}
	else
	{
		fseekSuccess = fseek(fid, cursor, SEEK_SET);
		printf("fseek = %d \n", fseekSuccess);

		if(fread(data,1,msTake,fid) == NULL)
		{
			printf("Can't read! \n");
		}
		fclose(fid);
	}


}




