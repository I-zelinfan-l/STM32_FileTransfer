#ifndef SD_FILE_H
#define SD_FILE_H

#include "stm32f10x.h"
#include <stdio.h>

#include "ff.h"
#include "diskio.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_usart_dma.h"
#include "./font/fonts.h"
#include "bsp_led.h"
#include "bsp_key.h"
 

//�ļ���ť�ĳ���
#define FILE_BLOCK_WIDTH   60
#define FILE_BLOCK_HEIGHT   80

#define FILE_BUTTON_START_X     0
#define FILE_BUTTON_START_Y    0
#define FILE_BUTTON_END_X     LCD_X_LENGTH
#define FILE_BUTTON_END_Y    200


//���ܺ�����ť�ĳ���
#define FUNCTION_BUTTON_NUM    3  //����ģ������
#define FUNCTION_BLOCK_WIDTH     80
#define FUNCTION_BLOCK_HEIGHT     30

#define FUNCTION_BUTTON_START_X     0
#define FUNCTION_BUTTON_START_Y    290
#define FUNCTION_BUTTON_END_X     LCD_X_LENGTH
#define FUNCTION_BUTTON_END_Y    LCD_Y_LENGTH

#define FILE_MAX_NUM_PAGE 4 // һ��Ŀ¼�µ��ļ�ҳ��
#define NUM_FILE_ONE_PAGE  12 //ÿҳ��ʾ���ļ���
#define MAX_NUM_FUNC   3  // ���ܰ����������� + ��һҳ + ��һҳ
#define BUTTON_NUM     (NUM_FILE_ONE_PAGE * FILE_MAX_NUM_PAGE + MAX_NUM_FUNC)     
                        // �������� ��51 = 12 * 4 + 3 = 48 + 3��
                       

#define PALETTE_START_Y   0
#define PALETTE_END_Y     LCD_Y_LENGTH

#define FILE_NAME_LENGTH      255    //�ļ����������

#define FILE_PAGE_MAX_BTYE    255   //�ļ���һҳ�����ʾ�ֽ�����
#define MAX_NUM_PAGE 8              // һ���ļ��������8ҳ

#if LCD_RGB_888
/*RGB888��ɫת��*/
#define RGB(R,G,B)	( (R<< 16) | (G << 8) | (B))	/* ��8λR,G,Bת��Ϊ 24λRGB888��ʽ */

#else 
/*RGB565 ��ɫת��*/
#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
#define RGB565_R(x)  ((x >> 8) & 0xF8)
#define RGB565_G(x)  ((x >> 3) & 0xFC)
#define RGB565_B(x)  ((x << 3) & 0xF8)

#endif


enum
{
	CL_WHITE    = RGB(255,255,255),	/* ��ɫ */
	CL_BLACK    = RGB(  0,  0,  0),	/* ��ɫ */
	CL_RED      = RGB(255,	0,  0),	/* ��ɫ */
	CL_GREEN    = RGB(  0,255,  0),	/* ��ɫ */
	CL_BLUE     = RGB(  0,	0,255),	/* ��ɫ */
	CL_YELLOW   = RGB(255,255,  0),	/* ��ɫ */
	CL_ORANGE =RGB(255,128,64),  /*  ��ɫ  */

	CL_GREY    = RGB( 98, 98, 98), 	/* ���ɫ */
	CL_GREY1		= RGB( 150, 150, 150), 	/* ǳ��ɫ */
	CL_GREY2		= RGB( 180, 180, 180), 	/* ǳ��ɫ */
	CL_GREY3		= RGB( 200, 200, 200), 	/* ��ǳ��ɫ */
	CL_GREY4		= RGB( 230, 230, 230), 	/* ��ǳ��ɫ */

	CL_BUTTON_GREY	= RGB( 195, 195, 195), /* WINDOWS ��ť�����ɫ */

	CL_MAGENTA      = RGB(255, 0, 255),	/* ����ɫ�����ɫ */
	CL_CYAN         = RGB( 0, 255, 255),	/* ����ɫ����ɫ */

	CL_BLUE1        = RGB(  0,  0, 240),		/* ����ɫ */
	CL_BLUE2        = RGB(  0,  0, 128),		/* ����ɫ */
	CL_BLUE3        = RGB(  68, 68, 255),		/* ǳ��ɫ1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* ǳ��ɫ1 */

	/* UI ���� Windows�ؼ�����ɫ */
	CL_BTN_FACE		  = RGB(236, 233, 216),	/* ��ť������ɫ(��) */
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* �����������ɫ */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* �������Ӱ����ɫ */

	CL_MASK			    = 0x7FFF	/* RGB565��ɫ���룬�������ֱ���͸�� */
};


typedef struct 
{
  uint16_t start_x;   //������x��ʼ����  
  uint16_t start_y;   //������y��ʼ����
  uint16_t end_x;     //������x�������� 
  uint16_t end_y;     //������y��������
  uint32_t para;      //��ɫ 
  uint8_t touch_flag; //�����Ƿ�ѡ�� 
	uint8_t  is_dir;   //�ļ��Ƿ���Ŀ¼
	uint8_t  en;  // �����Ƿ�ʹ��
	
	char fname[FILE_NAME_LENGTH];
  
  void (*draw_btn)(void * btn);     //������溯��
  void (*btn_command)(void * btn);  //��������ִ�к����������л���ɫ����ˢ
 
}Touch_Button;

typedef  struct{
	char *CurrentFileName;  //��ǰѡ�е��ļ���
	uint8_t  is_dir;  //�Ƿ����ļ���
}CUR_FILE;




void find_sd_file(char *path);
void Touch_Button_Init(void);
void Button_Fresh(char *path );
void File_Name_Init(void);
void Touch_Button_Down(uint16_t x,uint16_t y);
void Touch_Button_Up(uint16_t x,uint16_t y);
void  KeyTrace(void);
void  USARTx_ReceiveFile(void);
void  USARTx_SendFile(void);
#endif
