#include "bsp_key.h"

void Key_GPIO_Config(void){
	
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	KEY1_GPIO_CLK |= (1<<2);
	KEY1_GPIO_CLK |= (1<<4);
	
	GPIO_Init_Structure.GPIO_Pin=KEY1_GPIO_PIN;
	GPIO_Init_Structure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEY1_GPIO_PORT,&GPIO_Init_Structure);
	
	GPIO_Init_Structure.GPIO_Pin=KEY2_GPIO_PIN;
	GPIO_Init_Structure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(KEY2_GPIO_PORT,&GPIO_Init_Structure);
	
	/*GPIOA->CRL &= ~((0x0f)<<(4*0));
	GPIOA->CRL |= ((1)<<(2+4*0));
	
	GPIOC->CRL &= ~((0x0f)<<(4*2));
	GPIOC->CRL |= ((1)<<(2+4*2));
	*/
	
	
}
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin){
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin)==KEY_ON)
	{
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin)==KEY_ON);
		return KEY_ON;
		
	}
	else
		return KEY_OFF;
}
