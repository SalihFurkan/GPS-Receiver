
#define MS_TO_PROCESS      			 37000       //ms
#define NUM_OF_CHANNELS   			 8
#define SKIP_NUMBER_OF_BYTES    	 0
#define FILE_NAME   				 "corr4.bin"
#define DATA_TYPE 					 uint8

#define M_PI 				3.14159265358979323846

#define IF                 			 9548000      //Hz
#define SAMPLING_FREQ      			 38192000     //Hz
#define CODE_FREQ_BASIS      		 1023000     //Hz
#define CODE_LENGTH         		 1023
#define SAMPLES_PER_CODE 			 38192
#define SAMPLES_PER_CODE_CHIP 		 37

#define LIMIT 						 38400
#define SKIP_ACQUISITION    		 0
#define ACQ_SATELLITE_MAX   		 32         //PRN numbers
#define ACQ_SEARCH_BAND      		 14           //kHz
#define ACQ_THRESHOLD       		 2.5
#define NUMBER_OF_FRQ_BINS			 ACQ_SEARCH_BAND*2 + 1

#define DLL_DAMPING_RATIO         	 0.7
#define DLL_NOISE_BANDWIDTH       	 2       //Hz
#define DLL_CORRELATOR_SPACING    	 0.5     //chips

#define PLL_DAMPING_RATIO         	 0.7
#define PLL_NOISE_BANDWIDTH       	 25      //Hz

#define NAV_SOL_PERIOD       		 500          //ms
#define ELEVATION_MASK      		 10           //degrees 0 - 90
#define USE_TROP_CORR        		 1            // 0 - Off                                           // 1 - O
#define PLOT_TRACKING       		 1            //0 - Off
#define SPEED_OF_LIGHT               299792458    // The speed of light, m/s
#define START_OFFSET        		 68.802       //ms Initial sign. travel time
#define TIME_DIVIDER				 2.618349392542940930037704231252618349392542940930037704231e-8 // 1/38192

