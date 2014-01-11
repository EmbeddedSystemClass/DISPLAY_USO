//#include "globals.h"
#include "menu.h"
#include "keyboard.h"
#include <stdio.h>
#include <intrins.h>
#include "lcd.h"
#include "proto_uso/proto_uso.h"
#include "proto_uso/channels.h"
#include <string.h>

#define DISPLAY_WIDTH 	20
#define DISPLAY_HEIGHT	4

extern struct Channel xdata channels[CHANNEL_NUMBER];//обобщенна€ структура каналов

volatile struct pt pt_display;

sbit LED=P0^6;


enum
{
	DYN_NOT_DISPLAY=0,
	DYN_DISPALY_ON =1
//	DYN_DISPLAY_CH1=1,
//	DYN_DISPLAY_CH2=2,
//	DYN_DISPALY_CH3=3
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
MAKE_MENU(m_s0i1,  m_s0i2,	  NULL_ENTRY,  NULL_ENTRY, m_s1i1,       0, 	"DATA SCREEN1");
MAKE_MENU(m_s0i2,  m_s0i3,	  m_s0i1,      NULL_ENTRY, m_s1i1,       0, 	"DATA SCREEN2");
MAKE_MENU(m_s0i3,  m_s0i4,	  m_s0i2,      NULL_ENTRY, m_s1i1,       0, 	"DATA SCREEN3");
MAKE_MENU(m_s0i4,  NULL_ENTRY,m_s0i3,      NULL_ENTRY, m_s1i1,       0, 	"DATA SCREEN4");

MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  m_s0i1,     m_s2i1,       0, 		"Channel settings");
MAKE_MENU(m_s1i2,  m_s1i3,    m_s1i1,      m_s0i1,     m_s3i1,       0, 		"Channel calibr.");
MAKE_MENU(m_s1i3,  NULL_ENTRY,m_s1i2,      m_s0i1,     NULL_ENTRY,   MENU_DEV_SET,"Device settings");

// подменю Ќастройка каналов
MAKE_MENU(m_s2i1,  m_s2i2,    NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_CHN1_SET, 		"Channel 1 set.");
MAKE_MENU(m_s2i2,  m_s2i3,    m_s2i1,      m_s1i1,     NULL_ENTRY,   MENU_CHN2_SET, 		"Channel 2 set.");
MAKE_MENU(m_s2i3,  m_s2i4,	  m_s2i2,      m_s1i1,     NULL_ENTRY,   MENU_CHN3_SET, 		"Channel 3 set.");
MAKE_MENU(m_s2i4,  m_s2i5,	  m_s2i3,      m_s1i1,     NULL_ENTRY,   MENU_CHN4_SET, 		"Channel 4 set.");
MAKE_MENU(m_s2i5,  m_s2i6,	  m_s2i4,      m_s1i1,     NULL_ENTRY,   MENU_CHN5_SET, 		"Channel 5 set.");
MAKE_MENU(m_s2i6,  m_s2i7,	  m_s2i5,      m_s1i1,     NULL_ENTRY,   MENU_CHN6_SET, 		"Channel 6 set.");
MAKE_MENU(m_s2i7,  m_s2i8,	  m_s2i6,      m_s1i1,     NULL_ENTRY,   MENU_CHN7_SET, 		"Channel 7 set.");
MAKE_MENU(m_s2i8,  m_s2i9,	  m_s2i7,      m_s1i1,     NULL_ENTRY,   MENU_CHN8_SET, 		"Channel 8 set.");
MAKE_MENU(m_s2i9,  m_s2i10,	  m_s2i8,      m_s1i1,     NULL_ENTRY,   MENU_CHN_DOL_SET, 		"Channel dol set.");
MAKE_MENU(m_s2i10, m_s2i11,	  m_s2i9,      m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ1_SET, 	"Channel freq1 set.");
MAKE_MENU(m_s2i11, m_s2i12,	  m_s2i10,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ2_SET, 	"Channel freq2 set.");
MAKE_MENU(m_s2i12, m_s2i13,	  m_s2i11,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ3_SET, 	"Channel freq3 set.");
MAKE_MENU(m_s2i13, NULL_ENTRY,m_s2i12,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ_HI_SET, 	"Channel freqhi set.");

// подменю  алибровка каналов
MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_CHN1_CAL, 		"Channel 1 cal.");
MAKE_MENU(m_s3i2,  m_s3i3,    m_s3i1,      m_s1i1,     NULL_ENTRY,   MENU_CHN2_CAL, 		"Channel 2 cal.");
MAKE_MENU(m_s3i3,  m_s3i4,	  m_s3i2,      m_s1i1,     NULL_ENTRY,   MENU_CHN3_CAL, 		"Channel 3 cal.");
MAKE_MENU(m_s3i4,  m_s3i5,	  m_s3i3,      m_s1i1,     NULL_ENTRY,   MENU_CHN4_CAL, 		"Channel 4 cal.");
MAKE_MENU(m_s3i5,  m_s3i6,	  m_s3i4,      m_s1i1,     NULL_ENTRY,   MENU_CHN5_CAL, 		"Channel 5 cal.");
MAKE_MENU(m_s3i6,  m_s3i7,	  m_s3i5,      m_s1i1,     NULL_ENTRY,   MENU_CHN6_CAL, 		"Channel 6 cal.");
MAKE_MENU(m_s3i7,  m_s3i8,	  m_s3i6,      m_s1i1,     NULL_ENTRY,   MENU_CHN7_CAL, 		"Channel 7 cal.");
MAKE_MENU(m_s3i8,  m_s3i9,	  m_s3i7,      m_s1i1,     NULL_ENTRY,   MENU_CHN8_CAL, 		"Channel 8 cal.");
MAKE_MENU(m_s3i9,  m_s3i10,	  m_s3i8,      m_s1i1,     NULL_ENTRY,   MENU_CHN_DOL_CAL, 		"Channel dol cal.");
MAKE_MENU(m_s3i10, m_s3i11,	  m_s3i9,      m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ1_CAL, 	"Channel freq1 cal.");
MAKE_MENU(m_s3i11, m_s3i12,	  m_s3i10,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ2_CAL, 	"Channel freq2 cal.");
MAKE_MENU(m_s3i12, m_s3i13,	  m_s3i11,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ3_CAL, 	"Channel freq3 cal.");
MAKE_MENU(m_s3i13, NULL_ENTRY,m_s3i12,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ_HI_CAL, 	"Channel freqhi cal.");


unsigned char string_buf[32];
unsigned char input_char_count;
#define INPUT_CHAR_BUF_LEN	16
unsigned char input_char_buf[INPUT_CHAR_BUF_LEN];

unsigned char menuHandler(menuItem* currentMenuItem,unsigned int key);	 //обработка меню
void CalibrationScreen(unsigned char channel);//экран калибровки канала

void menuChange(menuItem code* NewMenu)
{
	if (NewMenu == &NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

unsigned char dispMenu(void)
{
xdata menuItem code * tempMenu, *tempMenu2;

	// перва€ строка - заголовок. »ли пункт меню верхнего уровн€


	tempMenu = selectedMenuItem;//->Parent;


    LCD_WriteCommand(LCD_CMD_CLEAR);

	if ((tempMenu == &m_s0i1)||(tempMenu == &m_s0i2)||(tempMenu == &m_s0i3)||(tempMenu == &m_s0i4)) 
	{ // мы на верхнем уровне
		dynamic_disp= DYN_DISPALY_ON;
	} 
	else 
	{
		dynamic_disp=DYN_NOT_DISPLAY;

		LCD_WriteCommand(LCD_SET_ADDR|0x1);
		LCD_WriteCommand(LCD_SET_ADDR|0x1);

		tempMenu2=tempMenu->Previous;
		if(tempMenu2!=&NULL_ENTRY)
		{
			LCD_WriteString(tempMenu2->Text);
		}
		
		LCD_WriteCommand(LCD_SET_ADDR|0x40);
		LCD_WriteData(0xC9);

		LCD_WriteCommand(LCD_SET_ADDR|0x41);
		LCD_WriteString(tempMenu->Text);


		LCD_WriteCommand(LCD_SET_ADDR|0x15);

		tempMenu2=tempMenu->Next;
		if(tempMenu2!=&NULL_ENTRY)
		{
			LCD_WriteString(tempMenu2->Text);

			LCD_WriteCommand(LCD_SET_ADDR|0x55);
	
			tempMenu2=tempMenu2->Next;
			if(tempMenu2!=&NULL_ENTRY)
		    LCD_WriteString(tempMenu2->Text);
		}
	}
	return (1);
}

unsigned char menuKey(unsigned char key) 
{
	menuItem* sel;


	if(!flag_menu_entry)
	{
		switch (key) 
		{
			case 0: 
			{
				return 1;
			}
			break;
		
			case '[': 
			{
				menuChange(PREVIOUS);
	
			}
			break;
		
			case ']': 
			{
				menuChange(NEXT);			
			}
			break;
		
			case 'x':
			{
			  _nop_();
			}
			break;
		
			case 'F':
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
	
			case 'C': 
			{ // отмена выбора (возврат)
				menuChange(PARENT);
			}
			break;
	
			default:
			{
	
			}		
		} 
	
		if(key!=0)
	   	{
			LCD_WriteCommand(LCD_CMD_CLEAR);
			delay(10);
			dispMenu(); 
		}
	}
	else
	{
		if(key== 'C') 
		{ // отмена выбора (возврат)
	
			flag_menu_entry=0;
			dynamic_disp=DYN_NOT_DISPLAY;
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
		case MENU_DEV_SET:
		{	
//			#define MAX 100
//			static  char res_chr=0;
//			LCD_WriteCommand(LCD_CMD_CLEAR);
//		//	LED=!LED;
//		//	delay(3);
//			LCD_WriteCommand(LCD_SET_ADDR|0x0);
//			LCD_WriteCommand(LCD_SET_ADDR|0x0);
//		//	delay(10);
//			LCD_WriteString("RESET:");
//			
//			switch(key)
//			{
//				case KEY_9:
//				{
//					res_chr++;
//				}
//				break;
//
//				case KEY_7:
//				{
//					res_chr--;
//				}
//				break;
//
//				case KEY_0:
//				{
//					res_chr=0;
//				}
//				break;
//			}
//			
//			if(res_chr>MAX)
//				res_chr=MAX;
//			if(res_chr<0)
//				res_chr=0;
//
//			dispSetScroller(res_chr,MAX);	
		}
		break; 
	//----------------------------------------------
//		 case  MENU_MODE1:
//		 {
//		 	static  char on_off=0;
//			
//			LCD_WriteCommand(LCD_CMD_CLEAR);
//
//			delay(3);
//			LCD_WriteCommand(LCD_SET_ADDR|0x0);
//			LCD_WriteCommand(LCD_SET_ADDR|0x0);
//		//	delay(10);
//			LCD_WriteString("MODE 1:");
//			
//			switch(key)
//			{
//				case KEY_8:
//				{
//					LED=~LED;
//				}
//				break;
//			}
//		//	flag_menu_entry=0;
//
//			LCD_WriteCommand(LCD_SET_ADDR|0x19);
//		//	delay(10);
//			if(LED==0)
//			{
//				LCD_WriteString("ON");
//			}
//			else
//			{
//				LCD_WriteString("OFF");	
//			}
//		 }
//		 break;
//	//----------------------------------------------
//		 case  MENU_MODE2:
//		 {
//		 	//flag_menu_entry=0;
//				#define  CUR_BASE 			0x19 //базовый	 адрес  дл€ курсора 
//				#define  CUR_MAX_OFFSET 	3//длина вводимого значени€
//				static unsigned char cur_addr_offset=0;//адрес смещени€ положени€ курсора
//				unsigned char buf[CUR_MAX_OFFSET+1];
//
//				static unsigned int num=0;
//
//				LCD_WriteCommand(LCD_CMD_CLEAR);
//	
//				delay(3);
//				LCD_WriteCommand(LCD_SET_ADDR|0x0);
//				LCD_WriteCommand(LCD_SET_ADDR|0x0);
//				delay(2);
//				LCD_WriteString("MODE 2:");
//				
//				switch(key)
//				{
//					case KEY_9:
//					{
//						 cur_addr_offset++;
//						 if(cur_addr_offset>(CUR_MAX_OFFSET-1))
//						 {
//						 	  cur_addr_offset=(CUR_MAX_OFFSET-1);
//						 }
//					}
//					break;
//
//					case KEY_7:
//					{
//						  if(cur_addr_offset>0)
//						  {
//						  	cur_addr_offset--;
//						  }
//					}
//					break;
//
//					case /*KEY_UP*/KEY_8:
//					{
//						switch(cur_addr_offset)
//						{
//							case 0:
//							{
//								num+=100;	
//							}
//							break;
//
//							case 1:
//							{
//								num+=10;
//							}
//							break;
//
//							case 2:
//							{
//								num++;
//							}
//							break;
//						}
//
//						if(num>999)
//							num=999; 
//					}
//					break;
//
//					case /*KEY_DOWN*/KEY_2:
//					{
//						if(num>0)
//						{
//							switch(cur_addr_offset)
//							{
//								case 0:
//								{
//									num-=100;
//								}
//								break;
//	
//								case 1:
//								{
//									num-=10;
//								}
//								break;
//	
//								case 2:
//								{
//									num--;
//								}
//								break;
//							}
//						}
//
//
//					}
//					break;
//				}
//
//				LCD_WriteCommand(LCD_SET_ADDR|0x18);
//				LCD_WriteString("[   ]");
//
//				LCD_WriteCommand(LCD_SET_ADDR|0x19);
//			//	sprintf(buf,"%i",num);
//				buf[0]=(unsigned char)(num/100)+0x30;
//				buf[1]=(unsigned char)((num%100)/10)+0x30;
//				buf[2]=(unsigned char)(num%10)+0x30;
//				buf[3]=0;
//				LCD_WriteString(buf);
//
//				LCD_WriteCommand(LCD_SET_ADDR|(CUR_BASE+cur_addr_offset));
//				LCD_WriteCommand(LCD_CMD_CURSOR);
//		 }
//		 break;
//	//---------------------------------------------
//		 case  MENU_MODE3:
//		 {
//		   	LCD_WriteCommand(LCD_CMD_CLEAR);
//	
//			delay(3);
//			LCD_WriteCommand(LCD_SET_ADDR|0x0);
//			LCD_WriteCommand(LCD_SET_ADDR|0x0);
//			delay(2);
//			LCD_WriteString("MODE 3:");
//		 	LCD_WriteCommand(LCD_SET_ADDR|0x18);
//
////			buf[0]=(unsigned char)(controller_reg[0]/100)+0x30;
////			buf[1]=(unsigned char)((controller_reg[0]%100)/10)+0x30;
////			buf[2]=(unsigned char)(controller_reg[0]%10)+0x30;
//			buf[3]=0;
//			LCD_WriteString(buf);
//		//	flag_menu_entry=0;
//		 }
//		 break;
//	//----------------------------------------------
//		 case  MENU_MODE4:
//		 {
//		 	//flag_menu_entry=0;
//			LCD_WriteCommand(LCD_CMD_CLEAR);
//			delay(3);
//			dynamic_disp=1;
//
//				switch(key)
//				{
//					case KEY_9:
//					{
//					   _nop_();
//					}
//					break;
//
//					case KEY_7:
//					{
//					   _nop_();
//					}
//					break;
//
//					case KEY_UP:
//					{
//						_nop_();	
//					}
//					break;
//
//					case KEY_DOWN:
//					{
//			
//					  _nop_();
//					}
//					break;
//				}
//		 }
//		 break;
//	//----------------------------------------------
//		 case  MENU_MODE5:
//		 {
//		 	flag_menu_entry=0;
//		 }
//		 break;
	//----------------------------------------------

	case MENU_CHN1_CAL:
	{
		static unsigned char has_point;
		switch(key)
		{
			case 'F':
			{
		 		LCD_WriteCommand(LCD_CMD_CLEAR);
//				CalibrationScreen(2);
				dynamic_disp= DYN_DISPALY_ON;
				input_char_count=0;
				has_point=0;
				memset (input_char_buf,0,INPUT_CHAR_BUF_LEN);
			}
			break;

			case '1':
			{
			   input_char_buf[input_char_count]='1';
			   input_char_count++;
			}
			break;

			case '2':
			{
			   input_char_buf[input_char_count]='2';
			   input_char_count++;
			}
			break;

			case '3':
			{
			   input_char_buf[input_char_count]='3';
			   input_char_count++;
			}
			break;

			case '4':
			{
			   input_char_buf[input_char_count]='4';
			   input_char_count++;
			}
			break;

			case '5':
			{
			   input_char_buf[input_char_count]='5';
			   input_char_count++;
			}
			break;

			case '6':
			{
			   input_char_buf[input_char_count]='6';
			   input_char_count++;
			}
			break;

			case '7':
			{
			   input_char_buf[input_char_count]='7';
			   input_char_count++;
			}
			break;

			case '8':
			{
			   input_char_buf[input_char_count]='8';
			   input_char_count++;
			}
			break;

			case '9':
			{
			   input_char_buf[input_char_count]='9';
			   input_char_count++;
			}
			break;

			case '0':
			{
			   input_char_buf[input_char_count]='0';
			   input_char_count++;
			}
			break;

			case '-':
			{
			   if(input_char_count==0)
			   {
				   input_char_buf[input_char_count]='-';
				   input_char_count++;
			   }
			}
			break;

			case '.':
			{
			   if((has_point==0)&&(input_char_count!=0))
			   {
				   input_char_buf[input_char_count]='.';
				   input_char_count++;
				   has_point=1;
			   }
			}
			break;
		}
		
		if(input_char_count>=INPUT_CHAR_BUF_LEN)
		{
			 input_char_count=INPUT_CHAR_BUF_LEN-1;
		}	
	}
	break;
		 
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
//-------------------------------------------------------
//void dispSetScroller(unsigned int num,unsigned int max)//установка значени€ с полосой
//{
//	unsigned char buf[21];//буфер дл€ полосы прокрутки
//	unsigned char i=0,n=0;
//	memset(buf,0x20,20);
//	buf[20]=0;
//	
//	n=(unsigned char)(DISPLAY_WIDTH*num/max);
//	
//	if(n>DISPLAY_WIDTH)
//	{
//		n=DISPLAY_WIDTH;
//	}
//
//	for(i=0;i<n;i++)
//	{
//		buf[i]=0xFF;
//	}
//	LCD_WriteCommand(LCD_SET_ADDR|0x54);
//	LCD_WriteString(buf);
//
// 	LCD_WriteCommand(LCD_SET_ADDR|0x1D);
//	sprintf(buf,"%u     ",num);
//	LCD_WriteString(buf);
//
//	return;
//}

enum
{
	CAL_FLOAT_LO=0,
	CAL_FLOAT_HI
};

void CalibrationScreen(unsigned char channel)//экран калибровки канала
{
	static 	unsigned char cal_float;//верхнее или нижнее значение

   sprintf(&string_buf,"Chn. value: %d",(unsigned int)channels[channel].channel_data);
   LCD_WriteAC(LCD_1_STR_ADDR);
   LCD_WriteString(&string_buf);

   LCD_WriteAC(LCD_2_STR_ADDR);
   LCD_WriteString(&input_char_buf);

	switch(cal_float)
	{
		case CAL_FLOAT_LO:
		{
		}
		break;

		case CAL_FLOAT_HI:
		{
		}
		break;

		default:
		{
		}
	}
}


PT_THREAD(DisplayProcess(struct pt *pt))
 {

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

	
	if(selectedMenuItem == &m_s0i1)
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
		PT_YIELD(pt);//дадим другим процессам врем€
		LCD_WriteString(&string_buf);
	   
	
		U_ch2=(unsigned int)(channels[1].channel_data*10000/0xFFFF);
		if(U_ch2>U_MAX)
		{
			U_ch2=U_MAX;
		}
	
		sprintf(&string_buf,"2=%4d  mV",U_ch2);
		LCD_WriteAC(LCD_2_STR_ADDR);
		PT_YIELD(pt);//дадим другим процессам врем€
		LCD_WriteString(&string_buf);
		
		U_ch3=(unsigned int)(channels[2].channel_data*10000/0xFFFF);
		if(U_ch3>U_MAX)
		{
			U_ch3=U_MAX;
		}
	
		sprintf(&string_buf,"3=%4d  mV",U_ch3);
		LCD_WriteAC(LCD_3_STR_ADDR);
		PT_YIELD(pt);//дадим другим процессам врем€
		LCD_WriteString(&string_buf);
	
		
	 	I_ch4=((float)channels[3].channel_data*20.0/0xFFFF);
		if(I_ch4>I_MAX)
		{
			I_ch4=I_MAX;
		}
		
		sprintf(&string_buf,"4=%4.1f  mA",I_ch4);
		LCD_WriteAC(LCD_4_STR_ADDR);
		PT_YIELD(pt);//дадим другим процессам врем€
		LCD_WriteString(&string_buf);	
	}

	if(selectedMenuItem == &m_s0i2)
	{
		sprintf(&string_buf,"SCREEN 2");
		LCD_WriteAC(LCD_4_STR_ADDR);
		LCD_WriteString(&string_buf);
	}

	if(selectedMenuItem == &m_s0i3)
	{
		sprintf(&string_buf,"SCREEN 3");
		LCD_WriteAC(LCD_4_STR_ADDR);
		LCD_WriteString(&string_buf);
	}

	if(selectedMenuItem == &m_s0i4)
	{
		sprintf(&string_buf,"SCREEN 4");
		LCD_WriteAC(LCD_4_STR_ADDR);
		LCD_WriteString(&string_buf);
	}
	
	if((selectedMenuItem == &m_s3i1) && (flag_menu_entry==1))	 //calibr 1 channel
	{
		CalibrationScreen(2);	
	}
  }

  PT_END(pt);
 }
