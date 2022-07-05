#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"

extern uint32_t HH,MM,SS,T,LS;

void RTC_Configuration(void);
void Time_Current(void);
void RTC_init(void);
