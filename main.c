
#include <stdio.h>
#include "initSettings.h"
#include "probeData.h"
#include "acquisition.h"
#include "tracking.h"
#include "rtwtypes.h"
#include "gnsstk/gps.h"


extern void probeData8bit(int8_t *data, int msTake, int cursor);
extern void acquisition(int8_t *data,int** acqResults);
extern void tracking(int **acqResults);
extern void probeData(int *data, int msTake);
extern void probeBits(int8_t *data, int msTake, int cursor);

int main()
{

//	int *acqResult[32];
//
//	for (int q=0;q<32;q++)
//	{
//		acqResult[q] = (int *)calloc(4,sizeof(int));
//	}

	//int8_t *data11ms = malloc(sizeof(int8_t)*MS_TO_PROCESS*SAMPLES_PER_CODE);

	//probeData8bit(data11ms, MS_TO_PROCESS, 0);

//	printf("After probeData()\n");

	//acquisition(data11ms, acqResult);
//	printf("After Acquisition\n");
	//free(data11ms);

	//int *IP = malloc(sizeof(int)*MS_TO_PROCESS);

	//probeData8bit(data11ms, MS_TO_PROCESS);
//	printf("Before tracking\n");
//	tracking(acqResult);
//	printf("After tracking\n");
//	probeData(IP, MS_TO_PROCESS);
	//	printf("%d \n", IP[36999]);
//	navigation(IP,acqResult);
	unsigned char *subframe1 = malloc(sizeof(int8_t)*240);
	unsigned char *subframe2 = malloc(sizeof(int8_t)*240);
	unsigned char *subframe3 = malloc(sizeof(int8_t)*240);
	probeBits(subframe1, 240, 0);
	printf("\n\n\n");
//	for(int i = 0; i < 300; i++)
//	{
//		printf("subframe1[%d] = %d \n", i,subframe1[i] );
//	}

	probeBits(subframe2, 240, 300);
	printf("\n\n\n");
	for(int i = 0; i < 300; i++)
	{
		printf("subframe2[%d] - subframe1[%d] = %d  \n", i,subframe2[i]-subframe1[i] );
	}
	probeBits(subframe3, 240, 600);
	printf("\n\n\n");
	for(int i = 0; i < 300; i++)
	{
		printf("subframe3[%d] = %d  \n", i,subframe3[i] );
	}
	GPS_structEphemeris st_gpsEph;

	  unsigned*       tow;                //!< time of week in subframe1; the time of the leading bit edge of subframe 2     [s]
	  unsigned short* iodc;               //!< 10 bit issue of data (clock); 8 LSB bits will match the iode                  []
	  unsigned char*  iode;               //!< 8 bit  issue of data (ephemeris)                                              []
	  unsigned*       toe;                //!< reference time ephemeris (0-604800)                                           [s]
	  unsigned*       toc;                //!< reference time (clock)   (0-604800)                                           [s]
	  unsigned short* week;               //!< 10 bit gps week 0-1023 (user must account for week rollover )                 [week]
	  unsigned char*  health;             //!< 6 bit health parameter; 0 if healthy; unhealth othersize                      [0=healthy]
	  unsigned char*  alert_flag;         //!< 1 = URA may be worse than indicated                                           [0;1]
	  unsigned char*  anti_spoof;         //!< anti-spoof flag from 0=off; 1=on                                              [0;1]
	  unsigned char*  code_on_L2;         //!< 0=reserved; 1=P code on L2; 2=C/A on L2                                       [0;1;2]
	  unsigned char*  ura;                //!< User Range Accuracy lookup code; 0 is excellent; 15 is use at own risk        [0-15]; see p. 83 GPSICD200C
	  unsigned char*  L2_P_data_flag;     //!< flag indicating if P is on L2 1=true                                          [0;1]
	  unsigned char*  fit_interval_flag;  //!< fit interval flag (four hour interval or longer) 0=4 fours; 1=greater         [0;1]
	  unsigned short* age_of_data_offset; //!< age of data offset                                                            [s]
	  double* tgd;                //!< group delay                                                                   [s]
	  double* af2;                //!< polynomial clock correction coefficient (rate of clock drift)                 [s/s^2]
	  double* af1;                //!< polynomial clock correction coefficient (clock drift)                         [s/s]
	  double* af0;                //!< polynomial clock correction coefficient (clock bias)                          [s]
	  double* m0;                 //!< mean anomaly at reference time                                                [rad]
	  double* delta_n;            //!< mean motion difference from computed value                                    [rad/s]
	  double* ecc;                //!< eccentricity                                                                  []
	  double* sqrta;              //!< square root of the semi-major axis                                            [m^(1/2)]
	  double* omega0;             //!< longitude of ascending node of orbit plane at weekly epoch                    [rad]
	  double* i0;                 //!< inclination angle at reference time                                           [rad]
	  double* w;                  //!< argument of perigee                                                           [rad]
	  double* omegadot;           //!< rate of right ascension                                                       [rad/s]
	  double* idot;               //!< rate of inclination angle                                                     [rad/s]
	  double* cuc;                //!< amplitude of the cosine harmonic correction term to the argument of latitude  [rad]
	  double* cus;                //!< amplitude of the sine harmonic correction term to the argument of latitude    [rad]
	  double* crc;                //!< amplitude of the cosine harmonic correction term to the orbit radius          [m]
	  double* crs;                //!< amplitude of the sine harmonic correction term to the orbit radius            [m]
	  double* cic;                //!< amplitude of the cosine harmonic correction term to the angle of inclination  [rad]
	  double* cis;        //!< amplitude of the sine harmonic correction term to the angle of inclination    [rad]

	  GPS_DecodeRawGPSEphemeris(
			subframe1,
			subframe2,
			subframe3,
			18,
			tow,
			iodc,
			iode,
			toe,
			toc,
			week,
			health,
			alert_flag,
			anti_spoof,
			code_on_L2,
			ura,
			L2_P_data_flag,
			fit_interval_flag,
			age_of_data_offset,
			tgd,
			af2,
			af1,
			af0,
			m0,
			delta_n,
			ecc,
			sqrta,
			omega0,
			i0,
			w,
			omegadot,
			idot,
			cuc,
			cus,
			crc,
			crs,
			cic,
			cis);

	return 0;
}

