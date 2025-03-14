#include "main.h"
#include "mb.h"
#include "slave_mb_app.h"
#include "master_mb_app.h"

int main(void)
{
	/*Hardware Init*/
//	UART1_Init(115200);
//	UART2_Init(115200);
//	UART3_Init(115200);
//	UART4_Init(115200);
//	UART5_Init(115200);
//	MyRTC_Init();

	/*User TASK Init*/
	#if MODBUS_SLAVE_ENABLE
		MODBUS_INIT();
	#endif
	
	#if MODBUS_MASTER_ENABLE
		User_master_start();
	#endif

	while (1)
	{	
		rt_thread_mdelay(10);
	}
}
