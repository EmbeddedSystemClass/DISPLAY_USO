#include "lcd.h"
#include "stdio.h"
#include "proto_uso/proto_uso.h"
#include "channels.h"


volatile struct pt pt_display;
extern unsigned int frame_receieved;
extern struct Channel xdata channels[CHANNEL_NUMBER];//обобщенная структура каналов

void LCD_Strobe();
void LCD_WaitWhileBusy();
bit LCD_IsBusy();



// Инициализация LDC
void LCD_Initialize()
{

		
	// ждем >= 15 мс

	
	delay(50);


	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();

	// ждем >= 4.1 мс

	 delay(1000);


	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();

	// ждем >= 100 мкс

	delay(100);


	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();
	delay(100);
	LCD_WriteCommand(LCD_CMD_CLEAR);
	delay(1000);
	LCD_WriteCommand(LCD_CMD_ON);
	delay(1000);
	LCD_WriteCommand(LCD_CMD_ON);
	delay(1000);
	LCD_WriteCommand(LCD_4_STR);

	PT_INIT(&pt_display);
}

// Запись команды
void LCD_WriteCommand(char cmd)
{
	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = cmd;
	LCD_Strobe();
//	delay(15);
}

// Запись символа
void LCD_WriteData(char ch)
{
	LCD_RS = RS_DR;
	LCD_RW = RW_WR;
	LCD_DATA = ch;
	LCD_Strobe();
//	delay(15);
}

// Запись строки символов
void LCD_WriteString(char *buf)
{
	int len=0;
	int i=0;
	len = strlen(buf);
	for(i=0; i<len; i++)
		LCD_WriteData(buf[i]);
}

// Генерация строба
void LCD_Strobe()
{
	LCD_E = 1;
	delay(280);
	LCD_E = 0;
}

// Проверка флага занятости LCD
bit LCD_IsBusy()
{
	LCD_RS = RS_IR;
	LCD_RW = RW_RD;
	LCD_Strobe();
	return LCD_BF;
}



// Установка значения счетчика адреса
void LCD_WriteAC(unsigned char value)
{
	unsigned char cmd = 0x80; // 1000 0000
	cmd |= value;
	LCD_WriteCommand(cmd);
}


void delay(unsigned int time)
{
	while(time--);
}

#define LCD_1_STR_ADDR	0x00
#define LCD_2_STR_ADDR	0x40
#define LCD_3_STR_ADDR	0x14
#define LCD_4_STR_ADDR	0x54

PT_THREAD(DisplayProcess(struct pt *pt))
 {
static unsigned char string_buf[32];
static unsigned int P=200,F=32	;
static unsigned int U_ch2=756,U_ch3=375;
static float I_ch4=45.6;

//static float channel_1_val=100.55;

  PT_BEGIN(pt);

  while(1) 
  {
  	PT_DELAY(pt,50);
	Channel_All_Get_Data_Request();
	PT_DELAY(pt,50);

	sprintf(&string_buf,"P=%3dkg/cm F=%4dkgs",P,F);
	LCD_WriteAC(LCD_1_STR_ADDR);
	LCD_WriteString(&string_buf);


	sprintf(&string_buf,"2=%4d  mV",U_ch2);
	LCD_WriteAC(LCD_2_STR_ADDR);
	LCD_WriteString(&string_buf);

	sprintf(&string_buf,"3=%4d  mV",U_ch3);
	LCD_WriteAC(LCD_3_STR_ADDR);
	LCD_WriteString(&string_buf);

	sprintf(&string_buf,"4=%5.1f mA",I_ch4);
	LCD_WriteAC(LCD_4_STR_ADDR);
	LCD_WriteString(&string_buf);

//
//	sprintf(&string_buf,"Received=%5d",frame_receieved);
//	LCD_WriteAC(LCD_3_STR_ADDR);
//	LCD_WriteString(&string_buf);

	
	
  }

  PT_END(pt);
 }