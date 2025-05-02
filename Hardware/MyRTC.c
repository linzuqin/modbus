#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "main.h"

uint16_t MyRTC_Time[] = {2025, 3, 11, 18, 22, 30};
uint32_t Unix_Time=1717077100;

static struct rt_thread rtc_thread;
static char rtc_thread_stack[1024];


void MyRTC_SetTime(void);

//如果LSE无法起振导致程序卡死在初始化函数中
//可将初始化函数替换为下述代码，使用LSI当作RTCCLK
//LSI无法由备用电源供电，故主电源掉电时，RTC走时会暂停
 
int MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5B5)
	{
		BKP_DeInit();

		RCC_LSICmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(40000 - 1);
		RTC_WaitForLastTask();
		
		MyRTC_SetTime();
		
		BKP_WriteBackupRegister(BKP_DR1, 0xA5B5);
	}
	else
	{
		RCC_LSICmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
	return 1;
}

void MyRTC_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year = MyRTC_Time[0] - 1900;
	time_date.tm_mon = MyRTC_Time[1] - 1;
	time_date.tm_mday = MyRTC_Time[2];
	time_date.tm_hour = MyRTC_Time[3];
	time_date.tm_min = MyRTC_Time[4];
	time_date.tm_sec = MyRTC_Time[5];
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;
	
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

void MyRTC_ReadTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;
	
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time[0] = time_date.tm_year + 1900;
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
	
	time_cnt=mktime(&time_date);
	Unix_Time=time_cnt;
}

void sys_time(void)
{
	MyRTC_ReadTime();
	rt_kprintf("system version:%s\n", Version);
	rt_kprintf("system_time:%d-%02d-%02d %02d:%02d:%02d\n", MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2], MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);
	rt_kprintf("Unix_Time:%d\n", Unix_Time);
}

static void rtc_time_thread_entry(void *parameter)
{
	MyRTC_Init();
	while (1)
	{
		MyRTC_ReadTime();
		rt_thread_mdelay(1000); // Delay for 1 second
	}
}

static int create_rtc_time_thread(void)
{
	rt_err_t result = rt_thread_init(&rtc_thread, 
									 "rtc_time", 
									 rtc_time_thread_entry, 
									 RT_NULL, 
									 rtc_thread_stack, 
									 sizeof(rtc_thread_stack), 
									 25, 
									 10);
	if (result == RT_EOK)
	{
		rt_thread_startup(&rtc_thread);
	}
	else
	{
		rt_kprintf("Failed to initialize RTC time thread\n");
		return -1; // Return error code if thread initialization fails
	}
	return 0; // Return success code
}

INIT_APP_EXPORT(create_rtc_time_thread);
MSH_CMD_EXPORT(sys_time, system time);
