#ifndef __MYRTC_H
#define __MYRTC_H

extern uint16_t MyRTC_Time[];

int MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);
void BKP_SAVE_DATA(void);

#endif
