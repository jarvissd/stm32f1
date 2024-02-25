#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdio.h"
#include "delayac.h"
#include "L2C.h"
#include "string.h"

SemaphoreHandle_t xMutex;
volatile  int duty_cycle =0;
volatile  uint8_t ispress1=0,ispress2=0;
volatile  unsigned int count=0,tong=0, tanso=72000000/(1000*7200),tocdo=0;

//void hardwareInit(void);
 void fn_GPIO_Init(void);
 void fn_PWM_Init(void);
void fn_EXTI_Init(void);
void  fn_USART_Init(void);

void vTaskNutbam(void *p);
void vTaskPWM(void *p);
void vTaskEXTI1(void *p);
void vTaskUART(void *p);
//void vTaskLCD(void *p);
void vTaskTocdo(void*p);
int main(void){
	SystemInit();
	SystemCoreClockUpdate();
	
	xMutex=xSemaphoreCreateMutex();

	fn_GPIO_Init();
	fn_PWM_Init();
	fn_EXTI_Init();
	fn_USART_Init();
	// I2CLCD_Init();
	
    xTaskCreate(vTaskNutbam, "NutbamTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
   xTaskCreate(vTaskPWM, "PWMTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTaskEXTI1, "EXTI1Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(vTaskUART, "UARTTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, NULL);
  //  xTaskCreate(vTaskLCD, "LCDTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 0, NULL);
		xTaskCreate(vTaskTocdo,"TocdoTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 0, NULL);
	
	vTaskStartScheduler();
	
	return 0;
}


 
 void fn_GPIO_Init(void)
 {
		GPIO_InitTypeDef GPIO_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
	 
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
 
 }

 void fn_PWM_Init(void){
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef timerInit;
		TIM_OCInitTypeDef PWM_Init;  
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
		
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	 
		timerInit.TIM_CounterMode=TIM_CounterMode_Up;
		timerInit.TIM_Period=1000;
		timerInit.TIM_Prescaler=100;
		TIM_TimeBaseInit(TIM2,&timerInit);
		TIM_Cmd(TIM2,ENABLE);
	 
		PWM_Init.TIM_OCMode=TIM_OCMode_PWM1;
		PWM_Init.TIM_OCPolarity=TIM_OCPolarity_High;
		PWM_Init.TIM_Pulse=(int)(duty_cycle*10);
		PWM_Init.TIM_OutputState=TIM_OutputState_Enable;
		TIM_OC1Init(TIM2,&PWM_Init);
		
	}
 void fn_EXTI_Init(void){
		EXTI_InitTypeDef EXTI_InitStructure;

	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
		EXTI_ClearITPendingBit(EXTI_Line4);
		
		EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_Line=EXTI_Line4;
		EXTI_InitStructure.EXTI_LineCmd=ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		
 }
void fn_USART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
 
    GPIO_InitStructure.GPIO_Pin =    GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx  ;
		USART_Init(USART1, &USART_InitStructure);
 
    USART_Cmd(USART1, ENABLE);
 
	}

void vTaskNutbam(void *p){
	while (1) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            delay_us(20);
            if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) && (ispress1 == 0)) {
                xSemaphoreTake(xMutex, portMAX_DELAY);
                duty_cycle += 10;
 
                if (duty_cycle > 100) {
                    duty_cycle = 100;
                }
                xSemaphoreGive(xMutex);
                ispress1 = 1;
            }
        } else {
            ispress1 = 0;
        }
				
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            delay_us(20);
            if ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) && (ispress2 == 0)) {
                xSemaphoreTake(xMutex, portMAX_DELAY);
								ispress2 = 1;
								duty_cycle -= 10;
                if (duty_cycle < 0) {
                    duty_cycle = 0;
                }
                xSemaphoreGive(xMutex);
               
            }
        } else {
            ispress2 = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));  
    }
	}
void vTaskPWM(void *p)
{
	while(1)
		{
		xSemaphoreTake(xMutex, portMAX_DELAY);
        TIM_SetCompare1(TIM2, (unsigned int)(duty_cycle * 10));
        xSemaphoreGive(xMutex);
        vTaskDelay(pdMS_TO_TICKS(5));  
    }
}
		
void vTaskEXTI1(void *p){
	while(1)
	{	
		if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
            EXTI_ClearITPendingBit(EXTI_Line4);  
        count++;
		}
			vTaskDelay(pdMS_TO_TICKS(1)); 
    }
}
void vTaskUART(void *p)
{
	char receivedChar;
	while(1)
	{
		receivedChar = USART_ReceiveData(USART1);

        if (receivedChar == '1') {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            duty_cycle += 50;
            if (duty_cycle > 300) {
                duty_cycle = 300;
            }
            xSemaphoreGive(xMutex);
        } else if (receivedChar == '2') {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            duty_cycle -= 50;
            if (duty_cycle < 0) {
                duty_cycle = 0;
            }
            xSemaphoreGive(xMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  
    }
}
void vTaskTocdo(void *p){
	while(1)
	{
 
		tocdo=(int)((count)/(60)) ;
		count=0;
		vTaskDelay(700/portTICK_RATE_MS);
	}
}
//void vTaskLCD(void *p)
//{
//	char str[16];
//	char str1[16];
//	char str2[16];
//	while (1) {
//        xSemaphoreTake(xMutex, portMAX_DELAY);

//        I2CLCD_Clear();
//        LCD_Gotoxy(0, 0);
//        snprintf(str, sizeof(str), "Duty:%d%%", duty_cycle/3);
//        LCD_Puts(str);
//        LCD_Gotoxy(9, 0);
//        snprintf(str2,sizeof(str2)," %d Hz",tanso);
//				LCD_Puts(str2);
//        LCD_Gotoxy(0, 1);
//        snprintf(str1, sizeof(str1), "Toc do:%d /sec", tocdo);
//        LCD_Puts(str1);

//        xSemaphoreGive(xMutex);

//        vTaskDelay(pdMS_TO_TICKS(10));  
//    }
//}
