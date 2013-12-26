#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <ADuC845.h>
#include "pt/pt.h"



#define KEY_UP				0x24
#define KEY_DOWN			0xA
#define KEY_RIGHT			0x14
#define KEY_LEFT			0x21
#define KEY_OK				0x12
#define KEY_0				0xC3
#define KEY_1				0x85
#define KEY_2				0x86
#define KEY_3				0x84
#define KEY_4				0x9//(0x9)
#define KEY_5				0xA//(A)
#define KEY_6				0xC//(C0)
#define KEY_7				0x11
#define KEY_8				0x12
#define KEY_9				0x14
#define KEY_POINT			0xC2


//#define KB_MASK 0xE0 // 1110 0000

sfr KB_PO = 0x80; // P0
sfr KB_PI = 0x90; // P1

#define KB_ROW		5
#define	KB_COLUMN	5

#define KB_MASK		0x1F

sbit KB_Out0 = KB_PO^0;
sbit KB_Out1 = KB_PO^1;
sbit KB_Out2 = KB_PO^2;
sbit KB_Out3 = KB_PO^3;
sbit KB_Out4 = KB_PO^4;

sbit KB_In0 = KB_PI^0;
sbit KB_In1 = KB_PI^1;
sbit KB_In2 = KB_PI^2;
sbit KB_In3 = KB_PI^3;
sbit KB_In4 = KB_PI^4;

void KB_Initialize();
unsigned char KB_ReadKey();

void  KBD_init();
PT_THREAD(KeyboardProcess(struct pt *pt));


#endif
