/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   Һ����ʾ����ʵ�飨�ֿ����ⲿFLASH��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
	//LCD ��ʼ��
	ILI9341_Init ();         
	
	//��������ʼ��
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
		//printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			//printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_sd = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
 
			//printf("������ʽ��ʧ�ܡ�����res_sd =%d\r\n",res_sd);
			while(1);
		}
	}
  else if(res_sd!=FR_OK)
  {
   // printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
   // printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
   // printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
	
	 
	Button_Fresh(path);
	
	while(1){
		KeyTrace();
		XPT2046_TouchEvenHandler();
	}
	
	//f_mount(NULL,"0:",1);
	

   
  
	
	
}
