#include "lcd.h"

void LCD_Strobe();
void LCD_WaitWhileBusy();
bit LCD_IsBusy();



// Инициализация LDC
void LCD_Initialize()
{
	//Timer0_Initialize(TIMER0_MODE_16BIT);
//	ET0 = 1;
		
	// ждем >= 15 мс
//	flag_next = 0;
//	tmr0_init();
//	Timer0_Run(15);
	
	delay(15);
//	while(!flag_next)
//	{}

	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();

	// ждем >= 4.1 мс
//	flag_next = 0;
//	Timer0_Run(5);
//	 delay(15);
//	while(!flag_next)
//	{}

	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();

	// ждем >= 100 мкс
//	flag_next = 0;
//	Timer0_Run(1);
//	delay(15);
	//while(!flag_next)
	//{}

	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();
//	delay(15);
	LCD_WriteCommand(LCD_CMD_CLEAR);
	LCD_WriteCommand(LCD_CMD_ON);
	LCD_WriteCommand(LCD_CMD_ON);
	LCD_WriteCommand(LCD_4_STR);
}

// Запись команды
void LCD_WriteCommand(char cmd)
{
	LCD_WaitWhileBusy();
	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = cmd;
	LCD_Strobe();
//	delay(15);
}

// Запись символа
void LCD_WriteData(char ch)
{
	LCD_WaitWhileBusy();
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

	// ждем >= 230 нс
//	flag_next = 0;
//	Timer0_Run(1);
	delay(15);
//	while(!flag_next)
//	{}

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

// Ожидание, пока LCD занят
void LCD_WaitWhileBusy()
{
//	while(LCD_IsBusy())
	{}

	delay(20);
}

// Чтение значения счетчика адреса
char LCD_ReadAC()
{
	LCD_RS = RS_IR;
	LCD_RW = RW_RD;
	LCD_Strobe();
	return LCD_DATA & 0x7F; // 0111 1111
}

// Установка значения счетчика адреса
void LCD_WriteAC(unsigned char value)
{
	unsigned char cmd = 0x80; // 1000 0000
	cmd |= value;
	LCD_WriteCommand(cmd);
}


void delay(int length)
{
 	int counter=0;
	while(counter!=4*length/* && !TF0*/)
	{
	 counter++;
	 //TF0=0;
	}
}