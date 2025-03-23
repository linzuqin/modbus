#include "main.h"
#include "mb.h"
#include "slave_mb_app.h"
#include "master_mb_app.h"
#include "ymodem.h"

ota_t ota;
int main(void)
{
	/*Hardware Init*/

	MyRTC_Init();
	ymodem_init();
	/*User TASK Init*/
	#if MODBUS_SLAVE_ENABLE
		MODBUS_INIT();
	#endif
	
	#if MODBUS_MASTER_ENABLE
		User_master_start();
	#endif

	while (1)
	{	
		ymodem_start();
		rt_thread_mdelay(10);
	}
}
