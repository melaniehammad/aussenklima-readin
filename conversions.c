#include <stdint.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "conversions.h"

//SHT-31
double readHumData(uint8_t* raw_data){
	uint16_t data = (raw_data[0]<<8)|raw_data[1];
#ifdef DEBUG
	printf("Raw Humidity: 0x%x\n", data);
	fflush(stdout);
#endif
	double hum = data/65535.0;
	return 100*hum;
}

//SHT-31
double readTempData(uint8_t* raw_data){
	uint16_t data = (raw_data[0]<<8)|raw_data[1];
#ifdef DEBUG
	printf("Raw Temperature: 0x%x\n", data);
	fflush(stdout);
#endif
	double tem = data/65535.0;
	return -45+175*tem;
}

//double 	readPresData	(uint8_t* raw_data);
//double 	readLightData	(uint8_t* raw_data);
//double	readBrightData	(uint8_t* raw_data);
//double	readCO2Data		(uint8_t* raw_data);
//double	readFinePData	(uint8_t* raw_data);
