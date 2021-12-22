/*
**#############################################################################
**      SVN Information
**
**      Checked In          : 2016-6-15
**
**      Revision            : 0.01
**
**      Release Information : Key cheak function
**   
**#############################################################################
*/
#include "Janus_Reg.h"
#include "App.h"
#define KEY1		PT1DAT_D0			//����һ�ε�ַ����һ����������������
#define KEY2		PT1DAT_D1			//����һ�ε�ַ�ݼ�һ�������������ݼ�
#define KEY3		PT1DAT_D2			//���º��ַ��λΪ0

/*
**#############################################################################
** Function prototype
**#############################################################################
*/
extern void Delay(uint32 howlong);
uint8 KeyScanf(void);

/*
**#############################################################################
** Variable declaration
**#############################################################################
*/
extern uint32 addresstemp;
static uint8 KeyScanfState = 1;		//����״̬��
static uint8 keydata = 0;
static uint32 longkeycount = 0;
static uint8 keyresult = 0;
static uint32 keydelaycount = 0;
enum
{
	KEY_FREE = 0x01,
	KEY_ONCE_DOWN,
	KEY_PRESS_DOWN,
	KEY_ONCE_LEAVE,
};

/*
**#############################################################################
** KeyCheak function
**#############################################################################
*/
void Key_Proc(void)
{
	keyresult = KeyScanf();
	switch(keyresult)
	{
		case 1:
			addresstemp++;
		break;
		case 2:
			addresstemp--;
		break;
		case 3:
			addresstemp = 0;
		break;
		case 4:
			if(TimerOutChkMsLong_pultus(&keydelaycount,150) == 1)		//������KEY1ʱ��ÿ��150ms��ַ����һ��
			{
				addresstemp++;
			}
		break;
		case 5:
			if(TimerOutChkMsLong_pultus(&keydelaycount,150) == 1)		//������KEY2ʱ��ÿ��150ms��ַ�ݼ�һ��
			{
				addresstemp--;
			}
		break;
		case 6:
			#ifdef USE_SPI_GET_REG
				Plutus_Reset_SPI();
			#endif
		
			#ifdef USE_UART_GET_REG
				Plutus_Reset_UART();
			#endif
		
			#ifdef USE_MCH_GET_REG
		
			#endif
		break;
	}
}


/*
**#############################################################################
** KeyScanf function
**#############################################################################
*/
uint8 KeyScanf(void)
{

	switch(KeyScanfState)
	{
		case KEY_FREE:
		if((KEY1&KEY2&KEY3) == 0)
			{
				KeyScanfState = KEY_ONCE_DOWN;				//״̬��ת��Ϊ������⵽һ�ΰ���
				if(KEY1 == 0)													//���水��������Ϣ
					keydata = 1;
				else if(KEY2 == 0)
					keydata = 2;
				else if(KEY3 == 0)
					keydata = 3;
			}
		break;
			
		case KEY_ONCE_DOWN:
			if(TimerOutChkMsLong_pultus(&keydelaycount,15) == 1)
			{
				if((KEY1&KEY2&KEY3) == 0)							//��������˶���ʱ�� ��⵽������Ȼ���£�״̬���л�������ȷ��������״̬
				{
					KeyScanfState = KEY_PRESS_DOWN;
					return keydata;											//���ذ�����Ϣ
				}
				else
				{
					KeyScanfState = KEY_FREE;						//��������Ѿ���̧����״̬���л��ᰴ������״̬
				}
			}
		break;
			
		case KEY_PRESS_DOWN:						
			if((KEY1&KEY2&KEY3) == 1)								//�����⵽����ȫ���뿪��״̬���л��������뿪һ��״̬
			{
				KeyScanfState = KEY_ONCE_LEAVE;
			}
			else
			{
				longkeycount++;
				if(longkeycount >= 40000)
				{
					if(KEY1 == 0)
						return 4;
					else if(KEY2 == 0)
						return 5;
					else if(KEY3 == 0)
						return 6;
				}
			}
		break;
			
		case KEY_ONCE_LEAVE:											//�����������������ʱ�仹�Ǽ�⵽����ȫ���뿪��״̬���л��������뿪״̬
			if(TimerOutChkMsLong_pultus(&keydelaycount,15) == 1)
			{
				if((KEY1&KEY2&KEY3) == 1)
				{
					KeyScanfState = KEY_FREE;
					longkeycount = 0;
					keydata = 0;
				}
			}
	}
	return 0;

}
