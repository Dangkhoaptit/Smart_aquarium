//#include "sys.h"
//Do an he thong nhung
//smart aquarium
#include "sys.h"
#include "LED.h"
#include "delay.h"
#include "usart.h"
#include "i2c_lcd.h"
#include "includes.h"

#include "ds18b20.h"

#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "RTC_BTL.h"

#include "servo_drive.h"

/*======================================== START TASK============================================*/
#define START_TASK_PRIO      			10 
#define START_STK_SIZE  			  	64
OS_STK START_TASK_STK[START_STK_SIZE];

void start_task(void *pdata);	
 			   
				 
/*==================================== READ TEMPERATURE TASK =====================================*/
#define READ_TEMPERATURE_PRIO       					3
#define READ_TEMPERATURE_STK_SIZE  		    		64
OS_STK READ_TEMPERATURE_STK[READ_TEMPERATURE_STK_SIZE];

void READ_TEMPERATURE(void *pdata);


/*=================================== CONTROL SERVO MOTOR TASK====================================*/
#define SERVO_TASK_PRIO    			5
#define SERVO_STK_SIZE  		 		128
OS_STK SERVO_TASK_STK[SERVO_STK_SIZE];

void servo_task(void *pdata);


/*====================================== DISPLAY LCD18X2 TASK =====================================*/
#define SHOW_TASK_PRIO    		4
#define SHOW_STK_SIZE  		 		64
OS_STK SHOW_TASK_STK[SHOW_STK_SIZE];

void show_task(void *pdata);


/*====================================== CONTROL LIGHT TASK ========================================*/
#define LIGH_CTR_PRIO    			2
#define LIGH_CTR_SIZE  		 		32
OS_STK LIGH_CTR_STK[LIGH_CTR_SIZE];

void LIGH_CTR_task(void *pdata);


/*==================================== CONTROL FAN AND HEATER TASK =================================*/
#define TEMPERATURE_CTR_PRIO    			1
#define TEMPERATURE_CTR_SIZE  		 		64
OS_STK TEMPERATURE_CTR_STK[LIGH_CTR_SIZE];

void TEMPERATURE_CTR_task(void *pdata);


/*=========================== RECEIVE DATA FROM ESP32 AND PROCESS RCT TASK =========================*/
#define RTC_TASK_PRIO       			6
#define RTC_STK_SIZE  					128
OS_STK RTC_TASK_STK[RTC_STK_SIZE];

void main_task(void *pdata);

OS_EVENT * msg_key;	

int deg =0;
int speed =0;

uint32_t i=0,n=1,h=0,m=0,t=0,k=0;
uint32_t  hh,mm,ss;

char a[16],des[16];

float nhietdo = 0;
char str[20];

int main(void)
{	
	 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		delay_init();	
	 
		LED_Init();
		USARTx_Init(USART1, Pins_PA9PA10, 115200);	 
	 
		RTC_init();
		LS=0;
	 
		HEATER_CTR = 0;
		FAN_CTR = 0;
		ALARM = 0;
		delay_ms(50);
		 
		I2C_LCD_Configuration();
		lcd_init ();                         
		lcd_send_string ("Smart Aquarium");
		lcd_Control_Write(0xC0);
		lcd_send_string("   GROUP 5  ");
		delay_ms(1000);
		
		 
		OSInit();   
		OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );
		OSStart();
  
}
 
void start_task(void *pdata){
	 
	 OS_CPU_SR cpu_sr=0;
	  pdata = pdata; 
	 
	 msg_key=OSMboxCreate((void*)0);	
	 OS_ENTER_CRITICAL();
	 
	 OSTaskCreate(READ_TEMPERATURE,(void *)0,(OS_STK *)&READ_TEMPERATURE_STK[READ_TEMPERATURE_STK_SIZE-1],READ_TEMPERATURE_PRIO );
	 OSTaskCreate(servo_task,(void *)0,(OS_STK *)&SERVO_TASK_STK[SERVO_STK_SIZE-1],SERVO_TASK_PRIO );
	 OSTaskCreate(show_task,(void *)0,(OS_STK*)&SHOW_TASK_STK[SHOW_STK_SIZE-1],SHOW_TASK_PRIO);
	 OSTaskCreate(main_task,(void *)0,(OS_STK*)&RTC_TASK_STK[RTC_STK_SIZE-1],RTC_TASK_PRIO);
	 OSTaskCreate(LIGH_CTR_task,(void *)0,(OS_STK*)&LIGH_CTR_STK[LIGH_CTR_SIZE-1],LIGH_CTR_PRIO);
	 OSTaskCreate(TEMPERATURE_CTR_task,(void *)0,(OS_STK*)&TEMPERATURE_CTR_STK[TEMPERATURE_CTR_SIZE-1],TEMPERATURE_CTR_PRIO);
	 
	 OSTaskSuspend(START_TASK_PRIO);	
	 OS_EXIT_CRITICAL();
	 
 }
 
void READ_TEMPERATURE(void *pdata)
{	  
	while(1)
	{
			nhietdo = ds18b20_read();
			sprintf(str,"Temp: %3.2f oC",(float)nhietdo);
			delay_ms(2000);
	};
}

void TEMPERATURE_CTR_task(void *pdata)
{
	while(1){
		
		if(nhietdo > 32)
		{
			FAN_CTR = 1;
			delay_ms(500);
		}
		else if (nhietdo < 30)
		{
			HEATER_CTR = 1;
			delay_ms(500);
		}
		else
		{
			HEATER_CTR = 0;
			FAN_CTR = 0 ;
			delay_ms(5);
		}
		
	}
	
}

void LIGH_CTR_task(void *pdata)
{
	while(1)
	{
	if(SS == 15){
			ALARM = 1;
			delay_ms(50);
		}
		
		else if(SS == 20){
			ALARM = 0;
			delay_ms(50);
		}
		else
			delay_ms(100);
	}
}

void servo_task(void *pdata)
{	  
	while(1)
	{

		if(SS == 5 || SS == 15 || SS ==25|| SS == 35 || SS ==45 ){	
			servo_180_deg(PA,11, 0);
			delay_ms(500);
			servo_180_deg(PA,11, 180);
			delay_ms(500);
			
		}
		
		else
		{
			delay_ms(500);
			
		}
		
	}
}

void main_task(void *pdata)
{	
	char buffer[100];
	
 	while(1)
	{		
		
		if (USART_Gets(USART1, buffer, sizeof(buffer))) 	
		{
				
		//printf("%s",buffer);		
		h=0;m=0;t=0;i=0;
			
			if(buffer[i+1]==':'){
				h=buffer[i]-48;
				i=i+2;
			}
			else 
			{h=(buffer[i]-48)*10+ buffer[i+1] -48;i=i+3;}		
			
				if(buffer[i+1]==':'){
						m=buffer[i]-48;
								i=i+2;
							}
				else 
				{m=(buffer[i]-48)*10+ buffer[i+1] -48;i=i+3;}		
				
				if(buffer[i+1]==':'){
					t=buffer[i]-48;
					i=i+2;
				}
				else 
				{t=(buffer[i]-48)*10+ buffer[i+1] -48;i=i+3;}		

				HH = h;
				MM = m;
				SS = t;
				Time_Current();
				
				sprintf(a,"Time: %d:%d:%d",h,m,t);
				sprintf(des, "Update: ");
				printf("dangkhoa_ %d:%d:%d",HH,MM,SS);
				
				delay_ms(200);

			}
			else{
				T = RTC_GetCounter()%86400;
				HH=T/3600;
				MM=(T%3600)/60;
				SS=(T%3600)%60;

				if(SS!=LS)
				{
					LS=SS;		
					sprintf(a, "Time: %d:%d:%d",HH,MM,SS);
					sprintf(des, "");
				}
						
			}
	}
}	

void show_task(void *pdata)
{
//	char buffer[100];
	while(1)
	{
		Delete_LCD();	
		lcd_send_string (a);
		lcd_Control_Write(0xc0);
		lcd_send_string (str);
		delay_ms(200);

	}									 
}




