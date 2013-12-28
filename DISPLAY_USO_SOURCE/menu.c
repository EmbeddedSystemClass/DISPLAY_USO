//#include "globals.h"
#include "menu.h"
#include "keyboard.h"
#include <stdio.h>
#include <intrins.h>
#include "lcd.h"
#include "proto_uso/proto_uso.h"
#include "proto_uso/channels.h"

#define DISPLAY_WIDTH 	20
#define DISPLAY_HEIGHT	4

extern struct Channel xdata channels[CHANNEL_NUMBER];//обобщенная структура каналов

volatile struct pt pt_display;

sbit LED=P0^6;
//char chr=0;
//unsigned int KEY=0;

enum
{
	DYN_NOT_DISPLAY=0,
	DYN_DISPLAY_CH1=1,
	DYN_DISPLAY_CH2=2,
	DYN_DISPALY_CH3=3
};


bit flag_menu_entry=0;//вошли в меню

typedef struct {
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Child;
	unsigned char     Select;
	char 		 *Text;
} code menuItem;

xdata menuItem code* selectedMenuItem; // текущий пункт меню

menuItem code* menuStack[10];
volatile unsigned char menuStackTop;

char buf[20];
unsigned char dynamic_disp=0;//номер отображаемого динамического экрана

#define MAKE_MENU(Name, Next, Previous, Parent, Child, Select, Text) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, (unsigned char)Select, { Text }}

#define PREVIOUS   (void*)(selectedMenuItem->Previous)
#define NEXT       (void*)(selectedMenuItem->Next)
#define PARENT     (void*)(selectedMenuItem->Parent)
#define CHILD      (void*)(selectedMenuItem->Child)
#define SELECT	   (void*)(selectedMenuItem->Select)

 char code strNULL[]  = "";

#define NULL_ENTRY Null_Menu
menuItem        Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, 0, {0x00}};

//                 NEXT,      PREVIOUS     PARENT,     CHILD
MAKE_MENU(m_s0i1,  NULL_ENTRY,NULL_ENTRY,  NULL_ENTRY, m_s1i1,       0, "DATA SCREEN");

MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  m_s0i1,     m_s2i1,       0, "Start");
MAKE_MENU(m_s1i2,  m_s1i3,    m_s1i1,      m_s0i1,     m_s3i1,       0, "Settings");
MAKE_MENU(m_s1i3,  NULL_ENTRY,m_s1i2,      m_s0i1,     NULL_ENTRY,   MENU_RESET, "Reset");

// подменю Запуск
MAKE_MENU(m_s2i1,  m_s2i2,    NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_MODE1, "Mode 1");
MAKE_MENU(m_s2i2,  m_s2i3,    m_s2i1,      m_s1i1,     NULL_ENTRY,   MENU_MODE2, "Mode 2");
MAKE_MENU(m_s2i3,  m_s2i4,	  m_s2i2,      m_s1i1,     NULL_ENTRY,   MENU_MODE3, "Mode 3");
MAKE_MENU(m_s2i4,  m_s2i5,	  m_s2i3,      m_s1i1,     NULL_ENTRY,   MENU_MODE4, "Mode 4");
MAKE_MENU(m_s2i5,  NULL_ENTRY,m_s2i4,      m_s1i1,     NULL_ENTRY,   MENU_MODE5, "Mode 5");

// подменю Настройка
MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY,  m_s1i2,     m_s4i1,       0, "Pressure");
MAKE_MENU(m_s3i2,  NULL_ENTRY,m_s3i1,      m_s1i2,     m_s5i1,       0, "Time");

// подменю Давление
MAKE_MENU(m_s4i1,  m_s4i2,    NULL_ENTRY,  m_s3i1,     NULL_ENTRY,   MENU_SENS1, "Sensor 1");
MAKE_MENU(m_s4i2,  NULL_ENTRY,m_s4i1,      m_s3i1,     NULL_ENTRY,   MENU_SENS2, "Sensor 2");

// подменю Время
MAKE_MENU(m_s5i1,  m_s5i2,    NULL_ENTRY,  m_s3i2,     NULL_ENTRY,   MENU_WARM, "Warm");
MAKE_MENU(m_s5i2,  NULL_ENTRY,m_s5i1,      m_s3i2,     NULL_ENTRY,   MENU_PROCESS, "Process");

unsigned char menuHandler(menuItem* currentMenuItem,unsigned int key);	 //обработка меню

void menuChange(menuItem code* NewMenu)
{
	if (NewMenu == &NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

unsigned char dispMenu(void)
{
xdata menuItem code * tempMenu, *tempMenu2;

	// первая строка - заголовок. Или пункт меню верхнего уровня


	tempMenu = selectedMenuItem;//->Parent;


    LCD_WriteCommand(LCD_CMD_CLEAR);

	
	LCD_WriteCommand(LCD_SET_ADDR|0x5);
	if (tempMenu == &m_s0i1) 
	{ // мы на верхнем уровне
		//LCD_WriteString("MENU:");
		dynamic_disp= DYN_DISPLAY_CH1;
	} 
	else 
	{
		dynamic_disp=DYN_NOT_DISPLAY;
	//	LCD_WriteCommand(LCD_SET_ADDR|0x5);
	//	LCD_WriteString("              ");
		LCD_WriteCommand(LCD_SET_ADDR|0x5);

		tempMenu2=tempMenu->Previous;
		if(tempMenu2!=&NULL_ENTRY)
			LCD_WriteString(/*CopyToData*/(tempMenu2->Text));
//-----------------------------------------------------------		
		LCD_WriteCommand(LCD_SET_ADDR|0x44);
		LCD_WriteData(0xC9);
	//	LCD_WriteCommand(LCD_SET_ADDR|0x45);
	//	LCD_WriteString("              ");
		LCD_WriteCommand(LCD_SET_ADDR|0x45);
		LCD_WriteString(/*CopyToData*/(tempMenu->Text));
//-----------------------------------------------------------

	//	LCD_WriteCommand(LCD_SET_ADDR|0x19);
	//	LCD_WriteString("              ");
		LCD_WriteCommand(LCD_SET_ADDR|0x19);

		tempMenu2=tempMenu->Next;
		if(tempMenu2!=&NULL_ENTRY)
		{
			LCD_WriteString(/*CopyToData*/(tempMenu2->Text));

			LCD_WriteCommand(LCD_SET_ADDR|0x59);
	
			tempMenu2=tempMenu2->Next;
			if(tempMenu2!=&NULL_ENTRY)
		    LCD_WriteString(/*CopyToData*/(tempMenu2->Text));
		}
	}

	return (1);
}

unsigned char menuKey(unsigned int key) 
{

menuItem* sel;

//KEY=key;
if(!flag_menu_entry)
{
	switch (key) 
	{
		case 0: 
		{
			return 1;
		}
		break;
	
		case /*KEY_UP*/KEY_8: 
		{
			menuChange(PREVIOUS);

		}
		break;
	
		case /*KEY_DOWN*/KEY_2: 
		{
			menuChange(NEXT);			
		}
		break;
	
		case /*KEY_RIGHT*/KEY_6:
		{
		  _nop_();
		}
		break;
	
		case /*KEY_OK*/KEY_5:
		{ // выбор пункта					
				sel = selectedMenuItem->Select;//SELECT;
				if (selectedMenuItem->Select != 0) 
				{
					menuHandler(selectedMenuItem,key);	
					return (1);
				} 
				else 
				{
					menuChange(CHILD);
				}
		}
		break;

		case /*KEY_LEFT*/KEY_4: 
		{ // отмена выбора (возврат)
			menuChange(PARENT);
		}
		break;

		default:
		{

		}		
	} 

	if(key!=0xFFFF)
   	{
		LCD_WriteCommand(LCD_CMD_CLEAR);
		delay(10);
		dispMenu(); 
	}
}
else
{
	if(key== /*KEY_LEFT*/KEY_4) 
	{ // отмена выбора (возврат)

		flag_menu_entry=0;
		dynamic_disp=0;
		LCD_WriteCommand(LCD_CMD_CLEAR);
		delay(3);
		LCD_WriteCommand(LCD_CMD_ON);
		delay(3);
		dispMenu(); 
	}
	else
	{
		sel = SELECT;
		if (sel != 0) 
		{
			menuHandler(selectedMenuItem,key);
		}
		 
	}
}

	return (1);
}
//-----------------------------------------------------

unsigned char menuHandler(menuItem* currentMenuItem,unsigned int key)	 //обработка меню
{
	flag_menu_entry=1;
	
	switch (currentMenuItem->Select) 
	{
		case MENU_RESET:
		{	
			#define MAX 100
			static  char res_chr=0;
			LCD_WriteCommand(LCD_CMD_CLEAR);
		//	LED=!LED;
		//	delay(3);
			LCD_WriteCommand(LCD_SET_ADDR|0x0);
			LCD_WriteCommand(LCD_SET_ADDR|0x0);
		//	delay(10);
			LCD_WriteString("RESET:");
			
			switch(key)
			{
				case KEY_9:
				{
					res_chr++;
				}
				break;

				case KEY_7:
				{
					res_chr--;
				}
				break;

				case KEY_0:
				{
					res_chr=0;
				}
				break;
			}
			
			if(res_chr>MAX)
				res_chr=MAX;
			if(res_chr<0)
				res_chr=0;

			dispSetScroller(res_chr,MAX);	
		}
		break; 
	//----------------------------------------------
		 case  MENU_MODE1:
		 {
		 	static  char on_off=0;
			
			LCD_WriteCommand(LCD_CMD_CLEAR);

			delay(3);
			LCD_WriteCommand(LCD_SET_ADDR|0x0);
			LCD_WriteCommand(LCD_SET_ADDR|0x0);
		//	delay(10);
			LCD_WriteString("MODE 1:");
			
			switch(key)
			{
				case KEY_8:
				{
					LED=~LED;
				}
				break;
			}
		//	flag_menu_entry=0;

			LCD_WriteCommand(LCD_SET_ADDR|0x19);
		//	delay(10);
			if(LED==0)
			{
				LCD_WriteString("ON");
			}
			else
			{
				LCD_WriteString("OFF");	
			}
		 }
		 break;
	//----------------------------------------------
		 case  MENU_MODE2:
		 {
		 	//flag_menu_entry=0;
				#define  CUR_BASE 			0x19 //базовый	 адрес  для курсора 
				#define  CUR_MAX_OFFSET 	3//длина вводимого значения
				static unsigned char cur_addr_offset=0;//адрес смещения положения курсора
				unsigned char buf[CUR_MAX_OFFSET+1];

				static unsigned int num=0;

				LCD_WriteCommand(LCD_CMD_CLEAR);
	
				delay(3);
				LCD_WriteCommand(LCD_SET_ADDR|0x0);
				LCD_WriteCommand(LCD_SET_ADDR|0x0);
				delay(2);
				LCD_WriteString("MODE 2:");
				
				switch(key)
				{
					case KEY_9:
					{
						 cur_addr_offset++;
						 if(cur_addr_offset>(CUR_MAX_OFFSET-1))
						 {
						 	  cur_addr_offset=(CUR_MAX_OFFSET-1);
						 }
					}
					break;

					case KEY_7:
					{
						  if(cur_addr_offset>0)
						  {
						  	cur_addr_offset--;
						  }
					}
					break;

					case /*KEY_UP*/KEY_8:
					{
						switch(cur_addr_offset)
						{
							case 0:
							{
								num+=100;	
							}
							break;

							case 1:
							{
								num+=10;
							}
							break;

							case 2:
							{
								num++;
							}
							break;
						}

						if(num>999)
							num=999; 
					}
					break;

					case /*KEY_DOWN*/KEY_2:
					{
						if(num>0)
						{
							switch(cur_addr_offset)
							{
								case 0:
								{
									num-=100;
								}
								break;
	
								case 1:
								{
									num-=10;
								}
								break;
	
								case 2:
								{
									num--;
								}
								break;
							}
						}


					}
					break;
				}

				LCD_WriteCommand(LCD_SET_ADDR|0x18);
				LCD_WriteString("[   ]");

				LCD_WriteCommand(LCD_SET_ADDR|0x19);
			//	sprintf(buf,"%i",num);
				buf[0]=(unsigned char)(num/100)+0x30;
				buf[1]=(unsigned char)((num%100)/10)+0x30;
				buf[2]=(unsigned char)(num%10)+0x30;
				buf[3]=0;
				LCD_WriteString(buf);

				LCD_WriteCommand(LCD_SET_ADDR|(CUR_BASE+cur_addr_offset));
				LCD_WriteCommand(LCD_CMD_CURSOR);
		 }
		 break;
	//---------------------------------------------
		 case  MENU_MODE3:
		 {
		   	LCD_WriteCommand(LCD_CMD_CLEAR);
	
			delay(3);
			LCD_WriteCommand(LCD_SET_ADDR|0x0);
			LCD_WriteCommand(LCD_SET_ADDR|0x0);
			delay(2);
			LCD_WriteString("MODE 3:");
		 	LCD_WriteCommand(LCD_SET_ADDR|0x18);

//			buf[0]=(unsigned char)(controller_reg[0]/100)+0x30;
//			buf[1]=(unsigned char)((controller_reg[0]%100)/10)+0x30;
//			buf[2]=(unsigned char)(controller_reg[0]%10)+0x30;
			buf[3]=0;
			LCD_WriteString(buf);
		//	flag_menu_entry=0;
		 }
		 break;
	//----------------------------------------------
		 case  MENU_MODE4:
		 {
		 	//flag_menu_entry=0;
			LCD_WriteCommand(LCD_CMD_CLEAR);
			delay(3);
			dynamic_disp=1;

				switch(key)
				{
					case KEY_9:
					{
					   _nop_();
					}
					break;

					case KEY_7:
					{
					   _nop_();
					}
					break;

					case KEY_UP:
					{
						_nop_();	
					}
					break;

					case KEY_DOWN:
					{
			
					  _nop_();
					}
					break;
				}
		 }
		 break;
	//----------------------------------------------
		 case  MENU_MODE5:
		 {
		 	flag_menu_entry=0;
		 }
		 break;
	//----------------------------------------------
	}	
	return 0;
}
//-----------------------------------------------------
unsigned char startMenu(void)
 {
	selectedMenuItem = &m_s0i1;

	dispMenu();
	PT_INIT(&pt_display);
	return 0;
}



//unsigned char* CopyToData( unsigned char code *mas) //копируем массив из code в data
//{
//	static xdata unsigned char data_mas[40];
//	unsigned char i=0;
//
//	while(mas[i]!=0)
//	{
//		data_mas[i]=mas[i];
//		i++;
//	}
//	data_mas[i]=0;
//	return &data_mas;
//}
//-------------------------------------------------------
void dispSetScroller(unsigned int num,unsigned int max)//установка значения с полосой
{
	unsigned char buf[21];//буфер для полосы прокрутки
	unsigned char i=0,n=0;
	memset(buf,0x20,20);
	buf[20]=0;
	
	n=(unsigned char)(DISPLAY_WIDTH*num/max);
	
	if(n>DISPLAY_WIDTH)
	{
		n=DISPLAY_WIDTH;
	}

	for(i=0;i<n;i++)
	{
		buf[i]=0xFF;
	}
	LCD_WriteCommand(LCD_SET_ADDR|0x54);
	LCD_WriteString(buf);

 	LCD_WriteCommand(LCD_SET_ADDR|0x1D);
	sprintf(buf,"%u     ",num);
	LCD_WriteString(buf);

	return;
}
//-------------------------------------------------------
//void DynamicDisplay(void)//динамическое отображение параметра
//{
////	static volatile unsigned int k=0;
//	static volatile unsigned int last=0;
//	switch(dynamic_disp)
//	{
//		case 0:
//		{
//			_nop_();
//		}
//		break;
//
//		case 1:
//		{
//		//	k++;
//		//	LCD_WriteCommand(LCD_CMD_CLEAR);
//		//	delay(3);
//		//	if(last!=controller_reg[0])
//			{
//				LCD_WriteCommand(LCD_SET_ADDR|0x5);
//				LCD_WriteString("MODBUS REG");
//				LCD_WriteCommand(LCD_SET_ADDR|0x1A);
//				delay(3);
////				sprintf(buf,"Register: %u     ",controller_reg[0]);
//				LCD_WriteString(buf);
////				last=controller_reg[0];
//			}
//		}
//		break;
//
//		case 2:
//		{
//			_nop_();
//		}
//		break;
//
//		default:
//		{
//			_nop_();
//		}
//	}	
//	
//	return;
//}

#define LCD_1_STR_ADDR	0x00
#define LCD_2_STR_ADDR	0x40
#define LCD_3_STR_ADDR	0x14
#define LCD_4_STR_ADDR	0x54


#define P_MAX 999
#define F_MAX 9999
#define U_MAX 9999
#define I_MAX 40.0	

#define K_P	18.413	//коэффициенты F=K*P+B
#define B_P	-5.674

PT_THREAD(DisplayProcess(struct pt *pt))
 {
static unsigned char string_buf[32];
static unsigned int P=200;
static int F=32;
static unsigned int U_ch2=756,U_ch3=375;
static float I_ch4=18.6;

//static float channel_1_val=100.55;

  PT_BEGIN(pt);

  while(1) 
  {
  
 	PT_YIELD_UNTIL(pt,dynamic_disp); //ждем команды на старт	 
  
  	PT_DELAY(pt,20);
	Channel_All_Get_Data_Request();
	PT_DELAY(pt,20);

//	dispMenu();

//	P= (unsigned int)channels[0].channel_data;
//	if(P>P_MAX)
//	{
//		P=P_MAX;
//	}
//
//
//	F=(unsigned int)((float)P*K_P+(B_P));
//
//	if(F<0)
//	{
//		F=0;
//	}
//
//	if(F>F_MAX)
//	{
//		F=F_MAX;	
//	}
//
//	sprintf(&string_buf,"P=%3dkg/cm F=%4dkgs",P,F);
//	LCD_WriteAC(LCD_1_STR_ADDR);
//	LCD_WriteString(&string_buf);
//
//
//	U_ch2=(unsigned int)(channels[1].channel_data*10000/0xFFFF);
//	if(U_ch2>U_MAX)
//	{
//		U_ch2=U_MAX;
//	}
//
//	sprintf(&string_buf,"2=%4d  mV",U_ch2);
//	LCD_WriteAC(LCD_2_STR_ADDR);
//	LCD_WriteString(&string_buf);
//
//	U_ch3=(unsigned int)(channels[2].channel_data*10000/0xFFFF);
//	if(U_ch3>U_MAX)
//	{
//		U_ch3=U_MAX;
//	}
//
//	sprintf(&string_buf,"3=%4d  mV",U_ch3);
//	LCD_WriteAC(LCD_3_STR_ADDR);
//	LCD_WriteString(&string_buf);
//
//
// 	I_ch4=((float)channels[3].channel_data*20.0/0xFFFF);
//	if(I_ch4>I_MAX)
//	{
//		I_ch4=I_MAX;
//	}
//
//	sprintf(&string_buf,"4=%4.1f  mA KEY=%X",I_ch4,key_code);
//	LCD_WriteAC(LCD_4_STR_ADDR);
//	LCD_WriteString(&string_buf);


		switch(dynamic_disp)
		{
			case DYN_NOT_DISPLAY:
			{
				_nop_();
			}
			break;
	
			case DYN_DISPLAY_CH1:
			{
				P= (unsigned int)channels[0].channel_data;
				if(P>P_MAX)
				{
					P=P_MAX;
				}
			
			
				F=(unsigned int)((float)P*K_P+(B_P));
			
				if(F<0)
				{
					F=0;
				}
			
				if(F>F_MAX)
				{
					F=F_MAX;	
				}
			
				sprintf(&string_buf,"P=%3dkg/cm F=%4dkgs",P,F);
				LCD_WriteAC(LCD_1_STR_ADDR);
				LCD_WriteString(&string_buf);
			
			
				U_ch2=(unsigned int)(channels[1].channel_data*10000/0xFFFF);
				if(U_ch2>U_MAX)
				{
					U_ch2=U_MAX;
				}
			
				sprintf(&string_buf,"2=%4d  mV",U_ch2);
				LCD_WriteAC(LCD_2_STR_ADDR);
				LCD_WriteString(&string_buf);
			
				U_ch3=(unsigned int)(channels[2].channel_data*10000/0xFFFF);
				if(U_ch3>U_MAX)
				{
					U_ch3=U_MAX;
				}
			
				sprintf(&string_buf,"3=%4d  mV",U_ch3);
				LCD_WriteAC(LCD_3_STR_ADDR);
				LCD_WriteString(&string_buf);
			
			
			 	I_ch4=((float)channels[3].channel_data*20.0/0xFFFF);
				if(I_ch4>I_MAX)
				{
					I_ch4=I_MAX;
				}
			
				sprintf(&string_buf,"4=%4.1f  mA",I_ch4);
				LCD_WriteAC(LCD_4_STR_ADDR);
				LCD_WriteString(&string_buf);	
			}
			break;
	
			case DYN_DISPLAY_CH2:
			{
				
			}
			break;
	
			default:
			{
				_nop_();
			}
	   }	
	
  }

  PT_END(pt);
 }
