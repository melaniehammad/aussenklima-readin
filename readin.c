//author: Tarik Hammad, 1311028

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <threads.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "readin.h"
#include "conversions.h"

#ifdef DEBUG
void verboseOut(uint8_t* string, size_t length){
	for(int i=0; i<length; i++){
		uint8_t current = string[i];
		for (int j=7; j>=0; j--){	//going through every bit of current byte
			printf("%d", current&(1<<j) ? 1 : 0);
		}
		printf(" %02x %03u %c\n", string[i], string[i], string[i]);
	}
}
#endif

thrd_t thread[AMOUNT_OF_DATABOXES];
//mtx_t mutex;

int evaluate(void*);

int main(int argc, char **argv){
	//mtx_init(&mutex, mtx_plain);
	struct termios tty;
	int serial_port = -1;
	//if no args, try ttyUSB0 first, then ttyUSB1
	if(argc==1){
		serial_port = open("/dev/ttyUSB0", O_RDONLY);
		if(serial_port < 0)
			serial_port = open("/dev/ttyUSB1", O_RDONLY);
	} else if (argc==2){
		serial_port = open(argv[1], O_RDONLY);
	}
	
	if(serial_port < 0){
		printf("Error %d from open: %s\n", errno, strerror(errno));
		exit(-1);	//no point in going any further...
	}
	
	if(tcgetattr(serial_port, &tty) != 0)
		printf("Error %d from tcgetattr: %s\n", errno, strerror(errno));
	
	tty.c_cflag &= ~(PARENB|CSTOPB|CSIZE|CRTSCTS);
	tty.c_cflag |= CS8 | CREAD | CLOCAL;		//byte size = 8,, turn on read and ignore control lines
	tty.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHONL|ISIG);
	tty.c_iflag &= ~(IXON|IXOFF|IXANY);
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	tty.c_oflag &= ~(OPOST|ONLCR);
	
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = sizeof(databox);
	
	//set baud rate
	cfsetispeed(&tty, B38400);
	cfsetospeed(&tty, B38400);
	
	//saving tty settings
	if(tcsetattr(serial_port, TCSANOW, &tty))
		printf("Error %d from tcsetattr: %s\n", errno, strerror(errno));
		
	//printf("CONFIG DONE\n");

	while(1){	//application loop
	
		uint8_t readbuffer[AMOUNT_OF_DATABOXES*sizeof(databox)];
		databox databoxes[AMOUNT_OF_DATABOXES];
		
		size_t n = read(serial_port, &readbuffer, sizeof(readbuffer));
#ifdef DEBUG
		verboseOut(readbuffer, n);
#endif
		unsigned int offset = 0;	
		
		while(readbuffer[offset]==0)	//ignore possible 0-bytes at the start of devfile
			offset++;
		
		while((offset+sizeof(databox))<=n){	//there are bytes to be read
#ifdef DEBUG
			printf("reading on offset=%d\n", offset);
			
			verboseOut(&readbuffer[offset], sizeof(databox));
#endif			
			uint8_t box_id = offset/sizeof(databox);
			if(box_id >= AMOUNT_OF_DATABOXES){	//no more databoxes
#ifdef DEBUG
				printf("Databox buffer exeeded.\n");
#endif
				break;
			}
#ifdef DEBUG
			printf("Using databox Nr.%d\n", box_id);
			fflush(stdout);
#endif
			databoxes[box_id].sensor_id = readbuffer[0+offset];
			databoxes[box_id].information_id = readbuffer[1+offset];
			memcpy(databoxes[box_id].bytes, &readbuffer[2+offset], 4);
			
			int r = thrd_create(&thread[box_id], evaluate, &databoxes[box_id]);
#ifdef DEBUG	
			if(r==thrd_success)
				printf("Thread created sucessfully!\n");
			else
				printf("Error creation thread.\n");
#endif			
			offset += sizeof(databox);
		}
		
	}
	//mtx_destroy(&mutex);
	close(serial_port);
	
	return 0;
}

int evaluate(void* _arg){
	databox *arg = (databox*)_arg;
	char information_string[10];
	double data = -1.0;
#ifdef DEBUG
	printf("EVAL: databytes are %02x %02x %02x %02x\n", arg->bytes[0], arg->bytes[1], arg->bytes[2], arg->bytes[3]);
#endif	
	switch(arg->information_id){
		case DUMMY_DATA_ID:
#ifdef DEBUG
			printf("Dummy Data recieved\n");
#endif
			thrd_exit(1);
			break;
		case TEMPERATURE_DATA_ID:
#ifdef DEBUG
			printf("Temparature Data recieved\n");
#endif			
			data = readTempData(arg->bytes);
			strcpy(information_string, "temp");
			break;
		case REL_HUM_DATA_ID:
#ifdef DEBUG
			printf("Humidity Data recieved\n");
#endif			
			data = readHumData(arg->bytes);
			strcpy(information_string, "humidity");
			break;
		case AIR_PRESSURE_DATA_ID:
#ifdef DEBUG
			printf("Air Pressure Data recieved\n");
#endif			
			//data = readPresData(arg->bytes);
			strcpy(information_string, "pressure");
			break;
		case BRIGHTNESS_DATA_ID:
#ifdef DEBUG
			printf("Brightness Data recieved\n");
#endif			
			//data = readBrightData(arg->bytes);
			strcpy(information_string, "bright");
			break;
		case CO2_DATA_ID:
#ifdef DEBUG
			printf("CO2 Data recieved\n");
#endif
			//data = readCO2Data(arg->bytes);
			strcpy(information_string, "co2");
			break;
		case PARTICULATES_DATA_ID:
#ifdef DEBUG
			printf("Fine Particle Data recieved\n");
#endif
			//data = readFinePData(arg->bytes);
			strcpy(information_string, "fine_part");
			break;
		default:
#ifdef DEBUG
			printf("Unknown Information (%d), aborting...\n", arg->information_id);
#endif
			thrd_exit(1);
			break;
	}
		
	pid_t pid = fork();
	if(pid == -1){
#ifdef DEBUG
		fprintf(stderr, "Error while forking process.\n");
#endif		
		exit(-1);
	}
	else if(pid == 0){
		char http_body[100];
		sprintf(http_body, 
			"%s,station=%d,sensor=%d data=%lf %ld",	//for some reason there mustn't be a space after the comma (InfluxDB sytax)
			information_string,
			STATION_ID,
			arg->sensor_id,
			data,
			time(NULL));
#ifdef DEBUG
		printf("http_body: %s\n", http_body);
#endif		
		execlp("curl", 
				"curl", 
#ifdef DEBUG
				"-i", 		//prints header of response
#endif
				"-XPOST", 
				"-u", AUTH_U, 
				DATABASE, 
				"--data-binary", http_body,
				 NULL);
		return 1;
	}
	else{
		//wait for child to terminate, prevent zombie
		int curl_return;
		waitpid(pid, &curl_return, 0);
#ifdef DEBUG
		printf("curl call returned %d\n", WEXITSTATUS(curl_return));
#endif
	}
	thrd_exit(0);
}
