#ifndef _LDC_H_
#define _LCD_H_

#include <string.h>
#include <ADuC845.h>

#include "pt/pt.h"

#define RS_IR 0
#define RS_DR 1

#define RW_WR 0
#define RW_RD 1

#define LCD_CMD_CLEAR    	0x01
#define LCD_CMD_OFF      	0x08
#define LCD_CMD_ON       	0x0C
#define LCD_CMD_BLINK    	0x0D
#define LCD_CMD_CURSOR   	0x0E
#define LCD_CMD_CURBLINK 	0x0F
#define LCD_CMD_RIGHT	 	0x1C
#define LCD_CMD_LEFT	 	0x18
#define LCD_CMD_CUR_HOME	0x02
#define LCD_4_STR 			0x38

#define LCD_SET_ADDR		0x80

sfr  LCD_DATA=0xA0; // P2

sbit LCD_BF=LCD_DATA^7;
sbit LCD_RS=P3^3; // RS=0 - выбран IR (регистр команд), RS=1 - выбран DR (регистр данных)
sbit LCD_RW=P3^4;
sbit LCD_E =P3^5;

void LCD_Initialize();
void LCD_WriteCommand(char cmd);
void LCD_WriteData(char ch);
void LCD_WriteString(char *buf);
char LCD_ReadAC();
void LCD_WriteAC(unsigned char value);

//PT_THREAD(DisplayProcess(struct pt *pt));

void delay(unsigned int);

#endif