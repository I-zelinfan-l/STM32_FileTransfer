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
 

//文件按钮的长宽
#define FILE_BLOCK_WIDTH   60
#define FILE_BLOCK_HEIGHT   80

#define FILE_BUTTON_START_X     0
#define FILE_BUTTON_START_Y    0
#define FILE_BUTTON_END_X     LCD_X_LENGTH
#define FILE_BUTTON_END_Y    200


//功能函数按钮的长宽
#define FUNCTION_BUTTON_NUM    3  //功能模块数量
#define FUNCTION_BLOCK_WIDTH     80
#define FUNCTION_BLOCK_HEIGHT     30

#define FUNCTION_BUTTON_START_X     0
#define FUNCTION_BUTTON_START_Y    290
#define FUNCTION_BUTTON_END_X     LCD_X_LENGTH
#define FUNCTION_BUTTON_END_Y    LCD_Y_LENGTH

#define FILE_MAX_NUM_PAGE 4 // 一个目录下的文件页数
#define NUM_FILE_ONE_PAGE  12 //每页显示的文件数
#define MAX_NUM_FUNC   3  // 功能按键数，返回 + 上一页 + 下一页
#define BUTTON_NUM     (NUM_FILE_ONE_PAGE * FILE_MAX_NUM_PAGE + MAX_NUM_FUNC)     
                        // 按键总数 ：51 = 12 * 4 + 3 = 48 + 3，
                       

#define PALETTE_START_Y   0
#define PALETTE_END_Y     LCD_Y_LENGTH

#define FILE_NAME_LENGTH      255    //文件名的最长长度

#define FILE_PAGE_MAX_BTYE    255   //文件的一页最多显示字节数量
#define MAX_NUM_PAGE 8              // 一个文件下最多有8页

#if LCD_RGB_888
/*RGB888颜色转换*/
#define RGB(R,G,B)	( (R<< 16) | (G << 8) | (B))	/* 将8位R,G,B转化为 24位RGB888格式 */

#else 
/*RGB565 颜色转换*/
#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* 将8位R,G,B转化为 16位RGB565格式 */
#define RGB565_R(x)  ((x >> 8) & 0xF8)
#define RGB565_G(x)  ((x >> 3) & 0xFC)
#define RGB565_B(x)  ((x << 3) & 0xF8)

#endif


enum
{
	CL_WHITE    = RGB(255,255,255),	/* 白色 */
	CL_BLACK    = RGB(  0,  0,  0),	/* 黑色 */
	CL_RED      = RGB(255,	0,  0),	/* 红色 */
	CL_GREEN    = RGB(  0,255,  0),	/* 绿色 */
	CL_BLUE     = RGB(  0,	0,255),	/* 蓝色 */
	CL_YELLOW   = RGB(255,255,  0),	/* 黄色 */
	CL_ORANGE =RGB(255,128,64),  /*  橙色  */

	CL_GREY    = RGB( 98, 98, 98), 	/* 深灰色 */
	CL_GREY1		= RGB( 150, 150, 150), 	/* 浅灰色 */
	CL_GREY2		= RGB( 180, 180, 180), 	/* 浅灰色 */
	CL_GREY3		= RGB( 200, 200, 200), 	/* 最浅灰色 */
	CL_GREY4		= RGB( 230, 230, 230), 	/* 最浅灰色 */

	CL_BUTTON_GREY	= RGB( 195, 195, 195), /* WINDOWS 按钮表面灰色 */

	CL_MAGENTA      = RGB(255, 0, 255),	/* 红紫色，洋红色 */
	CL_CYAN         = RGB( 0, 255, 255),	/* 蓝绿色，青色 */

	CL_BLUE1        = RGB(  0,  0, 240),		/* 深蓝色 */
	CL_BLUE2        = RGB(  0,  0, 128),		/* 深蓝色 */
	CL_BLUE3        = RGB(  68, 68, 255),		/* 浅蓝色1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* 浅蓝色1 */

	/* UI 界面 Windows控件常用色 */
	CL_BTN_FACE		  = RGB(236, 233, 216),	/* 按钮表面颜色(灰) */
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* 分组框主线颜色 */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* 分组框阴影线颜色 */

	CL_MASK			    = 0x7FFF	/* RGB565颜色掩码，用于文字背景透明 */
};


typedef struct 
{
  uint16_t start_x;   //按键的x起始坐标  
  uint16_t start_y;   //按键的y起始坐标
  uint16_t end_x;     //按键的x结束坐标 
  uint16_t end_y;     //按键的y结束坐标
  uint32_t para;      //颜色 
  uint8_t touch_flag; //按键是否被选中 
	uint8_t  is_dir;   //文件是否是目录
	uint8_t  en;  // 按键是否使能
	
	char fname[FILE_NAME_LENGTH];
  
  void (*draw_btn)(void * btn);     //按键描绘函数
  void (*btn_command)(void * btn);  //按键功能执行函数，例如切换颜色、画刷
 
}Touch_Button;

typedef  struct{
	char *CurrentFileName;  //当前选中的文件名
	uint8_t  is_dir;  //是否是文件夹
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
