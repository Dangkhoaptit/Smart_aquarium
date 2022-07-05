#include "ds18b20_mh.h"

void GPIO_SetState(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin,GPIOMode_TypeDef GPIO_Mode)
{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
/*-----------------------------------------------------------------------------
Noi Dung : Doc 1 byte du lieu tu DS18B20 ra ngoai.
Tham Bien : Khong.
Tra Ve : Byte du lieu can doc.
-----------------------------------------------------------------------------*/
uint8_t DS18B20_ReadByte(void)
{
	uint8_t i=0;
	uint8_t data=0;
	for(i=8;i>0;i--)
	{
		GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_Out_PP); // Cau hinh chan DQ la OUPUT
		GPIO_ResetBits(GPIOB, GPIO_Pin_9);// Keo chan DQ xuong muc '0'
		data>>=1;
		GPIO_SetBits(GPIOB, GPIO_Pin_9); // Keo chan DQ len muc '1'
		GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_IPU); // Cau hinh chan DQ la INPUT
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9))// neu co tin hieu tai chan 9
		data|=0x80; // Nhan du lieu tra ve tu DS18B20delay_us(120);
	}
	return(data);
}
/*-----------------------------------------------------------------------------
Noi Dung : Viet 1 byte du lieu vao DS18B20.
Tham Bien : data: Byte du lieu can viet vao.
Tra Ve : Khong.
-----------------------------------------------------------------------------*/
void DS18B20_WriteByte(uint8_t data)
{
	uint8_t i=0;
	GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_Out_PP); // Cau hinh chan DQ la OUTPUT
	for (i=8;i>0;i--)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_9);// Keo chan DQ xuong muc '0'DS18B20_PORT_DQ=0;
		GPIO_WriteBit (GPIOB,GPIO_Pin_9,(BitAction) (data&0x01)) ; //DS18B20_PORT_DQ=(data&0x01) // Viet du lieu vao DS18B20
		delay_us(60);
		GPIO_SetBits(GPIOB, GPIO_Pin_9); // Keo chan DQ len muc '1'DS18B20_PORT_DQ=1;
		data>>=1;
	}
}
/*-----------------------------------------------------------------------------
Noi Dung : Khoi tao DS18B20.
Tham Bien : Khong.
Tra Ve : Gia tri tra ve khi khoi tao xong cua DS18B20 (bit).
-----------------------------------------------------------------------------*/
void DS18B20_Init(void)
{
	GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_Out_PP); // Cau hinh chan DQ la OUTPUTGPIO_ResetBits(GPIOB, GPIO_Pin_9);// Keo chan DQ xuong muc '0'DS18B20_PORT_DQ=0; trong khoang 480us
	delay_us(500);
	GPIO_SetState(GPIOB,GPIO_Pin_9,GPIO_Mode_IPU); // Cau hinh chan DQ la INPUT trong khoang 480us
	delay_us(500);
}
/*-----------------------------------------------------------------------------
Noi Dung : Ghi 1 byte lenh chuc nang vao DS18B20.
Tham Bien : byte_func: byte lenh chuc nang cua DS18B20.
Tra Ve : Khong.
-----------------------------------------------------------------------------*/
void DS18B20_WriteFunc(uint8_t byte_func)
{
	DS18B20_Init(); // Khoi tao DS18B20
	DS18B20_WriteByte(DS1820_CMD_SKIPROM); // Truy cap thang den cac lenh chuc nang bo nho cua DS18B20
	DS18B20_WriteByte(byte_func); // Viet ma lenh chuc nang
}
/*-----------------------------------------------------------------------------
Noi Dung : Cau hinh cho DS18B20.
Tham Bien : temp_low: Gia tri nhiet do thap nhat de dua ra canh bao.
temp_high: Gia tri nhiet do cao nhat de dua ra canh bao.
resolution: Do phan giai cho DS18B20.(1|R1|R0|1|1|1|1|1)
Tra Ve : Khong.
-----------------------------------------------------------------------------*/
void DS18B20_Config(uint8_t temp_low, uint8_t temp_high, uint8_t resolution)
{
	resolution=(resolution<<5)|0x1f;
	DS18B20_WriteFunc(DS1820_CMD_WRITESCRPAD); // Cho phep ghi 3 byte vao bo nho nhap:
	DS18B20_WriteByte(temp_high); // byte 2: ThDS18B20_WriteByte(temp_low); // byte 3: Tl
	DS18B20_WriteByte(resolution); // byte 4: configuration register
	DS18B20_WriteFunc(DS1820_CMD_COPYSCRPAD); // Ghi vao EEPROM
}
/*-----------------------------------------------------------------------------
Noi Dung : Doc gia tri nhiet do do duoc cua DS18B20.
Tham Bien : Khong.
Tra Ve : Gia tri nhiet do do duoc.
-----------------------------------------------------------------------------*/
float DS18B20_ReadTemp(void)
{
	float temp;
	uint8_t buff_temp1,buff_temp2;
	DS18B20_WriteFunc(DS1820_CMD_CONVERTTEMP); // Khoi dong qua trinh do va chuyen doi nhiet do ra so nhi phan
	delay_us(200);
	DS18B20_WriteFunc(DS1820_CMD_READSCRPAD); // Doc du lieu tu bo nho DS18b20
	buff_temp1 = DS18B20_ReadByte();
	temp=((float)(buff_temp1&0x0f))/16; // Lay phan thuc cua gia tri nhiet do

	buff_temp2 = DS18B20_ReadByte();
	buff_temp1 =((buff_temp1&0xf0)>>4)|((buff_temp2&0x0f)<<4) ; // Lay phan nguyen cua gia tri nhiet do
	
	temp=temp+buff_temp1;
	return temp;
}