#include "sys.h"

int main(void)
{
	/*Hardware Init*/
	LOG_I("version:%s  System Init..." , Version);
	
	/*User TASK Init*/
	
	//IWDG_Init(IWDG_Prescaler_64, 0x0FFF);

	while (1)
	{
		//IWDG_Feed();
	LOG_I("time:%04d-%02d-%02d %02d:%02d:%02d\r\nTemp:%.1f  %.1f\r\nGX:%d GY:%d GZ:%d", MyRTC_Time[0] , MyRTC_Time[1] , MyRTC_Time[2], MyRTC_Time[3] , MyRTC_Time[4] , MyRTC_Time[5], temperature , mpu6050.temp, mpu6050.GX , mpu6050.GY, mpu6050.GZ);
		rt_thread_mdelay(1000);
	}
}

