#include "bsp_led.h"

void LED_GPIO_Config(void){
	GPIO_InitTypeDef GPIO_Init_Structure;
	RCC_APB2PeriphClockCmd( LED1_GPIO_CLK, ENABLE); 
	
	GPIO_Init_Structure.GPIO_Pin=LED1_GPIO_PIN;
	GPIO_Init_Structure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init_Structure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(LED1_GPIO_PORT, &GPIO_Init_Structure);
	
	GPIO_Init_Structure.GPIO_Pin=LED2_GPIO_PIN;
	GPIO_Init_Structure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init_Structure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(LED2_GPIO_PORT, &GPIO_Init_Structure);
	
	GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
	GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);
}


 
