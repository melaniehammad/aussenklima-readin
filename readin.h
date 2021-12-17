
#ifndef _READIN_H
#define _READIN_H

#include <stdint.h>

#define DATABASE "localhost:8086/write?db=testdb&precision=s"
#define AUTH_HEADER "Authorization: Token writer:scribe"
#define AUTH_U "writer:scribe"
#define STATION_ID 0
#define AMOUNT_OF_DATABOXES 10

#define DUMMY_DATA_ID 			00
#define TEMPERATURE_DATA_ID		01
#define REL_HUM_DATA_ID			02
#define AIR_PRESSURE_DATA_ID	03
#define BRIGHTNESS_DATA_ID		04//depricated
#define CO2_DATA_ID				05
#define PARTICULATES_DATA_ID	06//likely to change

typedef struct {
	uint8_t sensor_id;
	uint8_t	information_id;
	uint8_t bytes[4];
} databox;

#endif //_READIN_H
