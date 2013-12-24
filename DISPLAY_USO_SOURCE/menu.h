#ifndef _MENU_H_
#define _MENU_H_

#include "rtos/core.h"
#include "lcd.h"
#include "mb_ascii/proto_ascii.h"
extern unsigned int xdata  controller_reg[REG_ADDR_MAX];  //ргистры хранения значений modbus

enum {
    MENU_CANCEL=1,
    MENU_RESET,
    MENU_MODE1,
    MENU_MODE2,
    MENU_MODE3,
	MENU_MODE4,
	MENU_MODE5,
    MENU_SENS1,
    MENU_SENS2,
    MENU_WARM,
    MENU_PROCESS
};

unsigned char startMenu(void);
void initMenu(void);
unsigned char dispMenu(msg_par par);
unsigned char* CopyToData(unsigned char code *mas); //копируем массив из code в data

void dispSetScroller(unsigned int num,unsigned int max);//установка значения с полосой
void DynamicDisplay(void);//динамическое отображение параметра

#endif
