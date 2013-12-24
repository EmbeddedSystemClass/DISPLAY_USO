
#include "keyboard.h"
#include "lcd.h"
#include <stdio.h>


unsigned char lastKey,prevKey;
unsigned char kf1,kf2,kf3;


char KB_ReadPI();

// Инициализация клавиатуры
void KB_Initialize()
{
	KB_PI &= 0xE0; // 1110 0000
	setHandler(MSG_KEY_SCAN, &readKey);


	setTimer(MSG_KEY_SCAN, 0, 1);
}

char KB_ReadPI()
{
	char pi=0xFF;
	char i=0;
	char count=0;
	char tmp=0;
	pi = KB_PI | 0xE0; // 1110 0000
	for(i=0; i<5; i++)
	{
		tmp	= (pi >> i) | 0xFE;
		if(tmp != (char)0xFF)
			count++;
	}
	if(count > 1)
		return 0xFF;
	return pi;
}

// Чтение кода нажатой клавиши
unsigned char KB_ReadKey()
{
	char i=0;
	char po = 0x1E; // 0001 1110
	char pi = 0xFF;
	char ci = 0x0;
	char co = 0x0;
	char count=0;	
	for(i=0; i<5; i++)
	{
		//KB_PI &= 0xE0; // 1110 0000
		KB_PO |= 0x1F;
		KB_PO &= (po | 0xE0); // 1110 0000
		pi = KB_ReadPI();
		if(pi != (char)0xFF) // если что-то нажато
		{
			if(pi != (char)0x55) // если не ошибка (нажата только одна клавиша строки)
			{ 
				co = /*(int)*/po; //
				ci = /*(int)*/pi; // запоминаем код клавиши
			//	co = co << 8;
			//	ci = ci & 0x00FF; 
				count++; // инкрементируем количество нажатых клавиш
			}
			else
				return 0x55;
		}
		po = (po << 1) | 0x01;
	}
	if(count == 1) // если нажата только одна клавиша, возвращаем ее код
	{
		ci=(0xFF-ci)&0x7F;
		co=(0xFF-co)&0x7F;
		return (co<<1|ci);
	}
	return 0xFF;
}

unsigned char readKey(msg_par par)
 {
	kf3=kf2;
	kf2=kf1;
	kf1=KB_ReadKey();/*getKeyCode(KBD_PIN & KBD_MASK)*/;
	
	if ((kf2==kf1) && (kf3==kf2)) {
		prevKey = lastKey;
		lastKey = kf1;
	
		if (prevKey != lastKey) 
		{
			sendMessage(MSG_KEY_PRESS, lastKey);



		}
	}
	return(0);
}

/*unsigned char repeatKey(msg_par par) {
	if (prevKey == lastKey) {
		sendMessage(MSG_KEY_PRESS, lastKey);
		if (par>5)
			setTimer(MSG_KEY_REPEAT,par-1,par);
		else
			setTimer(MSG_KEY_REPEAT,5,5);
	}
	return(0);
}



void  KBD_init() {
//	KBD_PORT |= KBD_MASK;
//	KBD_DDR &= ~ KBD_MASK;

//	lastKey = 0;
//	prevKey = 0;

	setHandler(MSG_KEY_SCAN, &readKey);
//	setHandler(MSG_KEY_REPEAT, &repeatKey);

	setTimer(MSG_KEY_SCAN, 1, 1);
}*/
