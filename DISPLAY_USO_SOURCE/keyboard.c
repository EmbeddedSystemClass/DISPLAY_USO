
#include "keyboard.h"
#include "lcd.h"
#include <stdio.h>


unsigned char lastKey,prevKey;
unsigned char kf1,kf2,kf3;

volatile struct pt pt_keyboard;
volatile unsigned int key_code=0xFFFA;

sbit LED=P0^6;

//char KB_ReadPI();
//
//// Инициализация клавиатуры
//void KB_Initialize()
//{
//	KB_PI &= 0xE0; // 1110 0000
//
//}
//
//char KB_ReadPI()
//{
//	char pi=0xFF;
//	char i=0;
//	char count=0;
//	char tmp=0;
//	pi = KB_PI | 0xE0; // 1110 0000
//	for(i=0; i<5; i++)
//	{
//		tmp	= (pi >> i) | 0xFE;
//		if(tmp != (char)0xFF)
//			count++;
//	}
//	if(count > 1)
//		return 0xFF;
//	return pi;
//}
//
//// Чтение кода нажатой клавиши
//unsigned char KB_ReadKey()
//{
//	char i=0;
//	char po = 0x1E; // 0001 1110
//	char pi = 0xFF;
//	char ci = 0x0;
//	char co = 0x0;
//	char count=0;	
//	for(i=0; i<5; i++)
//	{
//		//KB_PI &= 0xE0; // 1110 0000
//		KB_PO |= 0x1F;
//		KB_PO &= (po | 0xE0); // 1110 0000
//		pi = KB_ReadPI();
//		if(pi != (char)0xFF) // если что-то нажато
//		{
//			if(pi != (char)0x55) // если не ошибка (нажата только одна клавиша строки)
//			{ 
//				co = /*(int)*/po; //
//				ci = /*(int)*/pi; // запоминаем код клавиши
//			//	co = co << 8;
//			//	ci = ci & 0x00FF; 
//				count++; // инкрементируем количество нажатых клавиш
//			}
//			else
//				return 0x55;
//		}
//		po = (po << 1) | 0x01;
//	}
//	if(count == 1) // если нажата только одна клавиша, возвращаем ее код
//	{
//		ci=(0xFF-ci)&0x7F;
//		co=(0xFF-co)&0x7F;
//		return (co<<1|ci);
//	}
//	return 0xFF;
//}
//
//unsigned char readKey(void)
// {
//	kf3=kf2;
//	kf2=kf1;
//	kf1=KB_ReadKey();/*getKeyCode(KBD_PIN & KBD_MASK)*/;
//	
//	if ((kf2==kf1) && (kf3==kf2)) {
//		prevKey = lastKey;
//		lastKey = kf1;
//	
//		if (prevKey != lastKey) 
//		{
//		//	sendMessage(MSG_KEY_PRESS, lastKey);
//
//			return lastKey;
//
//		}
//	}
//	return(0);
//}

PT_THREAD(KeyboardProcess(struct pt *pt))
 {

  static unsigned char i=0;
  static unsigned int  key_code_1=0xFFFF,key_code_2=0xFFFF, last_key_code=0xFFFF;
  

  PT_BEGIN(pt);

  while(1) 
  {  	
		PT_DELAY(pt,5);
		KB_PI&=~KB_MASK;
	///	KB_PI|=KB_MASK;
		key_code_1=0xFFFF;
		key_code_2=0xFFFF;

		for(i=0;i<KB_COLUMN;i++)
		{
			KB_PO|=KB_MASK;
			KB_PO&=(~(1<<i));

			if((KB_PI&KB_MASK)!=KB_MASK)
			{
				key_code_1=((KB_PO&KB_MASK)<<KB_COLUMN)|(KB_PI&KB_MASK);
				break;
			}
			KB_PO|=KB_MASK;				
		}
	
		PT_DELAY(pt,5);
			
	//	KB_PI|=KB_MASK;
		for(i=0;i<KB_COLUMN;i++)
		{
			KB_PO|=KB_MASK;
			KB_PO&=(~(1<<i));

			if((KB_PI&KB_MASK)!=KB_MASK)
			{
				key_code_2=((KB_PO&KB_MASK)<<KB_COLUMN)|(KB_PI&KB_MASK);
				break;
			}
			KB_PO|=KB_MASK;				
		}

		if((key_code_1==key_code_2)&&(key_code_1!=0xFFFF)&&(key_code_1!=last_key_code))
		{
			key_code=last_key_code=key_code_1;
			LED=~LED;				
		}
		else
		{
		//	key_code=0xFFFF; 
			if((key_code_1==0xFFFF)&&(key_code_2==0xFFFF))
			{
				last_key_code=0xFFFF;
			}
		}
  }

  PT_END(pt);
 }
