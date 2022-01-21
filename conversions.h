
#ifndef _CONVERSIONS_H
#define _CONVERSIONS_H

#include <stdint.h>

double 	readTempData	(uint8_t* raw_data);
double 	readHumData		(uint8_t* raw_data);
double 	readPresData	(uint8_t* raw_data);
double 	readLightData	(uint8_t* raw_data);
double	readBrightData	(uint8_t* raw_data);
double	readCO2Data		(uint8_t* raw_data);
//double	readFinePData	(uint8_t* raw_data);
double readPM25conc		(uint8_t* raw_data);
double readPM10conc		(uint8_t* raw_data);
double readPM25amount	(uint8_t* raw_data);
double readPM10amount	(uint8_t* raw_data);

#endif //_CONVERSIONS_H
