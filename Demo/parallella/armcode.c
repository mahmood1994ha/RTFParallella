#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <e-hal.h> //hardware abstraction library
#include <time.h>   /* Needed for struct timespec */

//get the data offset for core
#include "debugFlags.h"


#define READ_PRECISION_US 1000

int nsleep(long miliseconds){
   struct timespec req, rem;
   if(miliseconds > 999){
        req.tv_sec = (int)(miliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (miliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   } else {
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = miliseconds * 999000;    /* Must be in range of 0 to 999999999 */
   }
   return nanosleep(&req , &rem);
}

int main()
{
	//counters for row and colum, cored id and loop counter
	unsigned   row_loop,col_loop;
	// this will contain the epiphany platform configuration
	e_platform_t epiphany;
	e_epiphany_t dev;
	e_return_stat_t result;
	e_return_stat_t resultCore2;
	unsigned int message[9];
	long  message2;
	int loop;
	int addr;
	e_init(NULL); // initialise the system establish connection to the Device
	e_reset_system(); // reset the epiphnay chip
	e_get_platform_info(&epiphany);//gets the configuration info for the parallella platofrm
	//debug flag
	//------------------------------------------------
	//one core within the parallella work group is 1 x 1 i.e single core
	e_open(&dev,0,0,1,1);
	//reset the group
	e_reset_group(&dev);
	//load the group
	result =  e_load("main.elf",&dev,0,0,E_FALSE);
	resultCore2 = e_load("main.elf",&dev,0,1,E_FALSE);

	if (result != E_OK){
		fprintf(stderr,"Error Loading the Epiphany Application 1 %i\n", result);
	}
	e_start_group(&dev);
	fprintf(stderr,"FreeRTOS started \n");
	addr = cnt_address & 0xffff;//0x7000;
	int pollLoopCounter = 0;
	int taskMessage;
	int prevtaskMessage;
	int prevpollLoopCounter = 0;
	//int tickMissed=0;
	//int tickMessage=0;
	for (pollLoopCounter=0;pollLoopCounter<30;pollLoopCounter++){
		//int compensate = 0;
		message[3] = 0;
		e_read(&dev,0,0,addr, &message, sizeof(message));
		//taskMessage = message[6];

		//skip printing if a a tick is missed
		/*if (message[8]!= pollLoopCounter && tickMissed==0){
			fprintf(stderr,"missed tick %3u||\n", message[8]);
			tickMissed=1;
			//continue;
			//compensate = 1;
		}*/
		fprintf(stderr,"task 1 %3u||", message[2]);
		fprintf(stderr,"task 2 %3u||", message[0]);
		fprintf(stderr,"task 3 %3u||", message[4]);
		fprintf(stderr,"task holding core %2u||", message[6]);
		fprintf(stderr,"debug flag %4u||", message[7]);
		fprintf(stderr, "tick %3d",message[8]+1);
		fprintf(stderr,"\n");
		//nsleep(1);
		usleep(READ_PRECISION_US);
	}
	fprintf(stderr,"----------------------------------------------\n");
	e_close(&dev);
	e_finalize();
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	fprintf(stderr,"demo complete \n ");
	return 0;
}
