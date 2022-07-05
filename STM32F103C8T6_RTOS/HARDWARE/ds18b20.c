#include "ds18b20.h"

float ds18b20_read(void)
{
 uint8_t busy=0, temp1, temp2;
 uint16_t temp3,TimeOut=0;
 float result;
 OneWire_Reset();
 OneWire_Write(0xCC);
 OneWire_Write(0x44);
 while ((busy==0)&&TimeOut<2000)
 {
		busy = OneWire_Read();
 }
 OneWire_Reset();
 OneWire_Write(0xCC);
 OneWire_Write(0xBE);
 temp1 = OneWire_Read();
 temp2 = OneWire_Read();
 temp3 = temp2;
 temp3<<=8;
 temp3|=temp1;
 result = (float)(temp3 / 16.0);
 delay_ms(200);
 return(result);
} 
/*******************************************************************************
Noi Dung      :   Ham thiet lap ket noi 1 day
Tham Bien     :   Khong.
Tra Ve        :   Khong.
*******************************************************************************/
void OneWire_Reset()
{
 GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_Out_OD);//Thiet lap dau ra
 GPIO_ResetBits(GPIOB,GPIO_Pin_9);
 delay_us(500);
 GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_IN_FLOATING);//Thiet lap dau vao
 delay_us(500); 
 GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_IN_FLOATING);//Thiet lap dau vao
}
/*******************************************************************************
Noi Dung      :   Ham ghi du lieu 1 day
Tham Bien     :   data -> du lieu can goi
Tra Ve        :   Khong.
*******************************************************************************/
void OneWire_Write(uint8_t data)
{
 uint8_t count;
 for (count=0; count<8; ++count)
 {
  GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_Out_OD);//Thiet lap dau ra
  GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  delay_us( 2 ); 
  if(data&0x01)
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
  else 
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  data>>=1;
  delay_us( 60 ); 
  GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_IN_FLOATING);//Thiet lap dau vao
  delay_us(2);
 }
}
/*******************************************************************************
Noi Dung      :   ham doc du lieu 1 day
Tham Bien     :   Khong.
Tra Ve        :   Gia tri doc duoc
*******************************************************************************/
uint8_t OneWire_Read(void)
{
 uint8_t count, data=0;
 for (count=0; count<8; ++count)
 {
  GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_Out_OD);//Thiet lap dau ra
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  delay_us( 2 );                      
  GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_IN_FLOATING);//Thiet lap dau vao         
  delay_us( 8 );                
  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9))
  data|=0x01<<count;
  delay_us( 120 ); 
 }
 return( data );
}

