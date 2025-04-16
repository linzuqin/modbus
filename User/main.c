#include "stm32f10x.h"                  // Device header
#include "main.h"
#include "mb.h"
#include "slave_mb_app.h"
#include "master_mb_app.h"
#include "AT_Device.h"

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

	AT_START();
	while (1)
	{	
		LOG_I("System run...");

		rt_thread_mdelay(1000);
	}
}
