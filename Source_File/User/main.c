/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶显示汉字实验（字库在外部FLASH）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart_dma.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "bsp_spi_sdcard.h"
#include "ff.h"
#include "sd_file.h"
#include "bsp_xpt2046_lcd.h"
 
 FATFS fs;			
FRESULT res_sd;
char path[]="0:";

int main(void)
{	
	 LED_GPIO_Config();
	Key_GPIO_Config();
	//LCD 初始化
	ILI9341_Init ();         
	
	//触摸屏初始化
	XPT2046_Init();
 
	Calibrate_or_Get_TouchParaWithFlash(6,0);
  
	/* USART config */
	SPI_FLASH_Init();
	ILI9341_GramScan ( 6 );
    
    LCD_SetBackColor(CL_WHITE);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	
 
	USART_Config();  
	USARTx_DMA_Config();
    USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, DISABLE);
	
	XPT2046_Touch_Calibrate(6);
	printf("1");
	
	res_sd=f_mount(&fs,"0:",1);
	 
	 if(res_sd == FR_NO_FILESYSTEM)
	{
		//printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			//printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
 
			//printf("《《格式化失败。》》res_sd =%d\r\n",res_sd);
			while(1);
		}
	}
  else if(res_sd!=FR_OK)
  {
   // printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
   // printf("！！可能原因：SD卡初始化不成功。\r\n");
		while(1);
  }
  else
  {
   // printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
	
	 
	Button_Fresh(path);
	
	while(1){
		KeyTrace();
		XPT2046_TouchEvenHandler();
	}
	
	//f_mount(NULL,"0:",1);
	

   
  
	
	
}
