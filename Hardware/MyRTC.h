#ifndef __MYRTC_H
#define __MYRTC_H

extern uint16_t MyRTC_Time[];
extern uint32_t Unix_Time;
int MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

#endif
