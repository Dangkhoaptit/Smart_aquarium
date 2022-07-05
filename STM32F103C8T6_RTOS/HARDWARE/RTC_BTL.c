#include "RTC_BTL.h"

uint32_t HH,MM,SS,T,LS;

void RTC_Configuration(void){
		// cau hinh RTC
		RCC->APB1ENR |= (1<<28) | (1<<27);
		PWR->CR |= (1<<8);
	
		// bat clock 32,768khz
		RCC->BDCR |=(1<<0);
		// bat cho clock on dinh
		while (!(RCC->BDCR & ( 1<<1))){
		}
	
		RCC->BDCR &= ~(3<<8);
		RCC->BDCR |= (1<<8) ;
	
		RCC->BDCR |=(1<<15);
	
		// cau hinh RTC
		
		while(!(RTC->CRL & (1<<5))){
		}
		RTC->CRL |= (1<<4);
	
		RTC->PRLH = 0X0000;
		RTC->PRLL = 0X7fff;
	
		RTC->CRL &= ~(1<<4);
		while(!(RTC->CRL & (1<<5))){
		}
}

void RTC_init(void){
		HH=0; MM=0; SS=0;
	// Kiem tra RTC da duoc cau hinh chua bang cach kiem tra thanh ghi backup data
  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    // RTC Configuration
    RTC_Configuration();
		// Time current
		Time_Current();
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
		// cho bo RTC on dinh;
    RTC_WaitForSynchro();
    // cho phep ngat moi 1s
    //RTC_ITConfig(RTC_IT_SEC, ENABLE);
    // cho ghi xong data cho bo RTC
    RTC_WaitForLastTask();
		// xoa co reset cho bo RTC
		RCC_ClearFlag();
  }
	RTC_Configuration();
	RCC_ClearFlag();
	Time_Current();
	
}

void Time_Current(void)
{
  /* cho ghi xong data cho bo RTC */
  RTC_WaitForLastTask();
  /* Thay doi gia tri thoi gian hien tai */
	RTC_SetCounter(HH*3600 + MM*60 + SS);
  /* cho ghi xong data cho bo RTC */
  RTC_WaitForLastTask();
}