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

double readPM25conc		(uint8_t* raw_data){
	if(crc8(raw_data, 2) != raw_data[3])
		return -1.0;
	uint16_t data = (raw_data[0]<<8)|raw_data[1];
#ifdef DEBUG
	printf("Raw PM2.5 concentration: 0x%x\n", data);
	fflush(stdout);
#endif
	return (double)data;
}

double readPM10conc		(uint8_t* raw_data){
	if(crc8(raw_data, 2) != raw_data[3])
		return -1.0;
	uint16_t data = (raw_data[0]<<8)|raw_data[1];
#ifdef DEBUG
	printf("Raw PM10 concentration: 0x%x\n", data);
	fflush(stdout);
#endif
	return (double)data;
}

double readPM25amount	(uint8_t* raw_data){
	if(crc8(raw_data, 2) != raw_data[3])
		return -1.0;
	uint16_t data = (raw_data[0]<<8)|raw_data[1];
#ifdef DEBUG
	printf("Raw PM2.5 amount: 0x%x\n", data);
	fflush(stdout);
#endif
	return (double)data;
}

double readPM10amount	(uint8_t* raw_data){
	if(crc8(raw_data, 2) != raw_data[3])
		return -1.0;
	uint16_t data = (raw_data[0]<<8)|raw_data[1];
#ifdef DEBUG
	printf("Raw PM10 amount: 0x%x\n", data);
	fflush(stdout);
#endif
	return (double)data;
}

//double 	readPresData	(uint8_t* raw_data);
//double 	readLightData	(uint8_t* raw_data);
//double	readBrightData	(uint8_t* raw_data);
//double	readCO2Data		(uint8_t* raw_data);

uint8_t crc8(uint8_t *data, size_t size) {
	uint8_t crc = 0xFF;
	for(int i = 0; i < size; i++) {
		crc ^= data[i];
		for(uint8_t j = 0; bit < 8; j++) {
			if(crc & 0x80) 
				crc = (crc << 1) ^ 0x31u;
			else 
				crc = (crc << 1);
		}
	}
	return crc;
}