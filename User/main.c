#include "main.h"

int main(void)
{
	/*Hardware Init*/
	LOG_I("version:%s  System Init..." , Version);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*User TASK Init*/

	//IWDG_Init(IWDG_Prescaler_64, 0x0FFF);
	while (1)
	{
		//IWDG_Feed();
		rt_thread_mdelay(1000);
	}
}
