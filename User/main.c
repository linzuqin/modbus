#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "mb.h"
#include "slave_mb_app.h"
#include "master_mb_app.h"
#include "AT_Device.h"
#include "iwdog.h"

ota_t ota;
int main(void)
{
	/*Hardware Init*/
	LOG_I("System Init...");
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	MyRTC_Init();
	
	/*User TASK Init*/
	#if MODBUS_SLAVE_ENABLE
		MODBUS_INIT();
	#endif
	
	#if MODBUS_MASTER_ENABLE
		User_master_start();
	#endif

	#if USE_AT_DEVICE
		AT_START();
	#endif

	IWDG_Init(IWDG_Prescaler_64, 0x0FFF);
	while (1)
	{	
		
		IWDG_Feed();

		rt_thread_mdelay(1000);
	}
}
