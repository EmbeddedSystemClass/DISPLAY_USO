#include <ADuC845.h>


#include "timer3.h"
#include "proto_uso/proto_uso.h"
#include "preferences.h"
#include "eeprom/eeprom.h"
#include "timer1.h"

#include "watchdog.h"
#include "proto_uso/channels.h"
#include "frequency.h"

#include "pt/pt.h"
#include "lcd.h"
#include "menu.h"
#include "keyboard.h"
#include "calibrate/calibrate.h"

extern struct pt pt_proto, pt_wdt, pt_display, pt_keyboard;

sbit BIP=P0^7;
sbit LED=P0^6;

 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
		
	PLLCON&=PLLCON_VAL;//��������� ������� ����������
	CFG845=0x1;//enable xram

	
	ChannelsInit();//������������� �������� �������
	RestoreCalibrate();
	Protocol_Init();	
	Timer1_Initialize(); //������ �������� 200��	

	UART_Init();
	LCD_Initialize();


	WDT_Init(WDT_2000);//�������� ���������� ������
	LED=1;
	BIP=1;

	EA=1;

	while(1)
	{	
		ProtoProcess(&pt_proto);
		KeyboardProcess(&pt_keyboard);

		WDT_Process(&pt_wdt);	
		DisplayProcess(&pt_display);   
	}
}
//-----------------------------------------------------------------------------

void Timer1_Interrupt(void) interrupt 3  //������ ��������
{
	TH1	= TH1_VAL; ///200 Hz;
	TL1 = TL1_VAL;//

	pt_proto.pt_time++;
	pt_wdt.pt_time++;
	pt_display.pt_time++;
	pt_keyboard.pt_time++;
	return;	
}