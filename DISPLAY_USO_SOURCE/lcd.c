#include "lcd.h"
#include "stdio.h"

void LCD_Strobe();
void LCD_WaitWhileBusy();
bit  LCD_IsBusy();

// ������������� LDC
void LCD_Initialize()
{		
	// ���� >= 15 ��
	
	delay(50000);

	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();

	// ���� >= 4.1 ��

	delay(10000);

	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = 0x30;
	LCD_Strobe();

	// ���� >= 100 ���

	delay(1000);

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


	startMenu();
}

// ������ �������
void LCD_WriteCommand(char cmd)
{
	LCD_RS = RS_IR;
	LCD_RW = RW_WR;
	LCD_DATA = cmd;
	LCD_Strobe();
	delay(100);
}

// ������ �������
void LCD_WriteData(char ch)
{
	LCD_RS = RS_DR;
	LCD_RW = RW_WR;
	LCD_DATA = ch;
	LCD_Strobe();
	delay(100);
}

// ������ ������ ��������
void LCD_WriteString(char *buf)
{
	int len=0;
	int i=0;
	len = strlen(buf);
	for(i=0; i<len; i++)
	{
		LCD_WriteData(buf[i]);
	}
}

// ��������� ������
void LCD_Strobe()
{
	LCD_E = 1;
	delay(500);
	LCD_E = 0;
}

// �������� ����� ��������� LCD
bit LCD_IsBusy()
{
	LCD_RS = RS_IR;
	LCD_RW = RW_RD;
	LCD_Strobe();
	return LCD_BF;
}


// ��������� �������� �������� ������
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
