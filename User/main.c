#include "main.h"

int main(void)
{
	/*Hardware Init*/
	LOG_I("version:%s  System Init..." , Version);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
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

	//IWDG_Init(IWDG_Prescaler_64, 0x0FFF);
	while (1)
	{
		//IWDG_Feed();
		rt_thread_mdelay(1000);
	}
}
