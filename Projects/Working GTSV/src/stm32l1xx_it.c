/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Template/stm32l1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_it.h"
#include "GTSV_RH_RTC_config.h"
#include "GTSV_BlackControl_board.h"
#include "IRremote.h"
#include "main.h"


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1);
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{  
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
 /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}



/**
  * @brief  This function handles SysTick interrupts.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	/* TS handler */
  	TSL_Timer_ISR();
	if(++gSystemFlags.msMainTick == MAIN_TICK_MS){
		gSystemFlags.msMainTick = 0;
		gSystemFlags.msMain_flag=1;
	}
     msTicks++;
	if((msTicks%10) ==0){
		gSystemFlags.ms10_flag =1;
		if((msTicks%50) == 0){
			gSystemFlags.ms50_flag = 1;
			Buzzer_off_timing_tick50ms();
			if((msTicks%100) == 0){
				Serial_time_out_tick();
				gSystemFlags.ms100_flag = 1;
				if((msTicks%200)==0)
					gSystemFlags.ms200_flag =1;					
				if((msTicks%300) == 0)
					gSystemFlags.ms300_flag = 1;
				if((msTicks%500) == 0)
					gSystemFlags.ms500_flag = 1;
				
				//Tsense_key_detect_tick50ms();
				
			}
		}
	}

	//buzzer on/off timing
	//Buzzer_systickISR_timing_ms();  //<--put this to main loop
	//if(gSystemFlags.sys_state == SYS_STATE_BLOWING_APO)
		//auto_power_off_check_time();
}

bool Systick_check_delay50ms(void)
{
	if(gSystemFlags.ms50_flag){
		gSystemFlags.ms50_flag=0;
		return TRUE;
	}else{
		return FALSE;
	}
}

#define IRR_GAP_TICKS 	500
/**
  * @brief  TIMER 6 used for IR receiver decode, interupt every 50us
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void)
{ 
	//TIME_DEBUG_SET(6);
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);		
		uint8_t ir_data = *irrparams.recv_pin;
		irrparams.tick_cnt++; //one more 50us tick
		
		//check buffer overflow
		if(irrparams.rawbuff_len >= 88){
			irrparams.recv_state = IRR_STATE_STOP;
		}
		switch(irrparams.recv_state){
		case IRR_STATE_IDLE: 
			//wait until mark, increasing tick_cnt => tick_cnt is measuring Space.
			//if we got a mark, check if previous space big enoug.
			//if the previous space is not big enough 
			//then zero tick & wait for another bigger space.
			if(ir_data == IRR_MARK){ 
				if(irrparams.tick_cnt < IRR_GAP_TICKS){ //not big enough to be a gap
					irrparams.tick_cnt = 0; //reset counter & wait for another mark
				} else { //we get here until we got a tick_cnt>500 (5ms)
					irrparams.rawbuff_len = 0;
					irrparams.rawbuff[irrparams.rawbuff_len++] = irrparams.tick_cnt;
					irrparams.tick_cnt = 0;
					irrparams.recv_state = IRR_STATE_MARK;
				}
			} else {  //we are counting the space gap
				if(irrparams.tick_cnt==65534)
					irrparams.tick_cnt = 65533;
			}
			break;
		case IRR_STATE_MARK: //timing MARK
			if(ir_data == IRR_SPACE){
				irrparams.rawbuff[irrparams.rawbuff_len++] = irrparams.tick_cnt;
				irrparams.tick_cnt = 0;
				irrparams.recv_state = IRR_STATE_SPACE;
			} else {
				//mark state too long --> do something! (normally the mark is not too long)
			}
			break;
		case IRR_STATE_SPACE: //timing SPACE
			if(ir_data == IRR_MARK){
				irrparams.rawbuff[irrparams.rawbuff_len++] = irrparams.tick_cnt;
				irrparams.tick_cnt = 0;
				irrparams.recv_state = IRR_STATE_MARK;
			} else {
				if(irrparams.tick_cnt> IRR_GAP_TICKS){
					// big SPACE, indicates gap between codes
        			// Mark current code as ready for processing
        			// Switch to STOP
        			// Don't reset timer; keep counting space width
        			irrparams.recv_state = IRR_STATE_STOP;
				}
			}
			break;
		case IRR_STATE_STOP:
			if(ir_data == IRR_MARK){
				irrparams.tick_cnt=0;
			}
			break;

		}
		
		
	}
	//TIME_DEBUG_RESET(6);
	
}



/**
  * @brief  This function handles TIM7 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(void)
{	
	Buzzer_timerISR_make_sound();	
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
}

void EXTI0_IRQHandler(void)
{
  /* Disable general interrupts */
  //disableInterrupts();
  	  //RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  //RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
  EXTI_ClearITPendingBit(EXTI_Line0);
  //enableInterrupts();
}

//125ms tick
void RTC_WKUP_IRQHandler (void)
{
	static uint8_t ms125Tick = 0;
	static uint16_t sec1Tick = 0;
	/* Toggle LED1 */
	//GPIO_TOGGLE(LD_GPIO_PORT,LD_GREEN_GPIO_PIN);
	//BITBAND_POINTER_AT(GPIOB_BASE + ODR_REG_OFFSET, 6) ^= 1;
	//Lcd_icon_toggle(LCD_CLOCK_ICON);
	//LCD_UpdateDisplayRequest();
	gSystemFlags.ms125_flag=1;

	
	ms125Tick++;
	if(ms125Tick == 8){ //1s tick here!
		ms125Tick=0;
		sec1Tick++;
		//check time out in Blowing Auto Power Off
		if(gSystemFlags.sys_state == SYS_STATE_BLOWING_APO){
			if(gSystemFlags.blower_apo_remaining_sec == 0){
				gSystemFlags.blower_apo_time_out =1;
			}else{
				gSystemFlags.blower_apo_remaining_sec--;
			}
		}
	}

	Tsense_key_hold_detect_tick125ms();
	Lcd_blink_tick125ms();  //tick 125 for LCD blink cursor

	PWR_RTCAccessCmd(ENABLE);
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line20);
	PWR_RTCAccessCmd(DISABLE);

}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
		Serial_rx_ISR();
	}
	/*
	if(USART_GetITStatus(USART1, USART_IT_TXE) == SET){
		Serial_tx_ISR();

	}

	*/

}

#if 0
void USARTx_IRQHandler(void)
{
  //* USART in mode Tramitter -------------------------------------------------*/
  if (USART_GetITStatus(USARTx, USART_IT_TXE) == SET)
  { //* When Joystick Pressed send the command then send the data */
    if (UsartMode == USART_MODE_TRANSMITTER)
    { //* Send the command */
      if (UsartTransactionType == USART_TRANSACTIONTYPE_CMD)
      {
        USART_SendData(USARTx, CmdBuffer[TxIndex++]);
        if (TxIndex == 0x02)
        {
          /* Disable the USARTx transmit data register empty interrupt */
          USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
        }
      }
      /* Send the data */
      else
      {
        USART_SendData(USARTx, TxBuffer[TxIndex++]);
        if (TxIndex == GetVar_NbrOfData())
        {
          /* Disable the USARTx transmit data register empty interrupt */
          USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
        }
      }
    }
    /*If Data Received send the ACK*/
    else
    {
      USART_SendData(USARTx, AckBuffer[TxIndex++]);
      if (TxIndex == 0x02)
      {
          /* Disable the USARTx transmit data register empty interrupt */
          USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
      }
    }
  }
  
  /* USART in mode Receiver --------------------------------------------------*/
  if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET)
  {
    if (UsartMode == USART_MODE_TRANSMITTER)
    {
      AckBuffer[RxIndex++] = USART_ReceiveData(USARTx);
    }
    else
    {
      /* Receive the command */
      if (UsartTransactionType == USART_TRANSACTIONTYPE_CMD)
      {
        CmdBuffer[RxIndex++] = USART_ReceiveData(USARTx);
      }
      /* Receive the USART data */
      else
      {
        RxBuffer[RxIndex++] = USART_ReceiveData(USARTx);
      }
    }
  }     
}

#endif

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_md.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
