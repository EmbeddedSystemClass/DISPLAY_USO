#include "device.h"
#include "eeprom/eeprom.h"
extern volatile unsigned char xdata ADRESS_DEV;
void Device_Save_Address(unsigned char address)
{
	ADRESS_DEV=address;
	EEPROM_Write(&address,1,DEVICE_ADDR_EEPROM);
}

unsigned char Device_Restore_Address(void)
{
}