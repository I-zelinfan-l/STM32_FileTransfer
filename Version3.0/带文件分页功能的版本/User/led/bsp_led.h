#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define LED1_GPIO_PORT GPIOC
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOC
#define LED1_GPIO_PIN  GPIO_Pin_2

#define LED2_GPIO_PORT GPIOC
#define LED2_GPIO_CLK  RCC_APB2Periph_GPIOC
#define LED2_GPIO_PIN  GPIO_Pin_3

#define ON 1
#define OFF 0  
 
					
#define Led1(a) if(a)\
	        GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
          else  \
					GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN)

#define Led2(a) if(a)\
	        GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);\
          else  \
						GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN)				

#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) {p->ODR ^=i;} //输出反转状态


/* 定义控制IO的宏 */
#define LED1_TOGGLE		 digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_OFF		   digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_ON			   digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)

#define LED2_TOGGLE		 digitalToggle(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_OFF		   digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_ON			   digitalLo(LED2_GPIO_PORT,LED2_GPIO_PIN)

void LED_GPIO_Config(void);

#endif

