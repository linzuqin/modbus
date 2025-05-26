#ifndef __MPU6050_H
#define __MPU6050_H
#include "MPU6050_Reg.h"
#include "sys.h"

typedef struct mpu6050
{
	/* data */
	uint8_t ID;
	int16_t AX;
	int16_t AY;
	int16_t AZ;
	int16_t GX;
	int16_t GY;
	int16_t GZ;
	float temp;

}mpu6050_data_t;
extern mpu6050_data_t mpu6050;


#endif
