/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Template/main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    13-September-2011
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "GTSV_BlackControl_board.h"
#include "GTSV_BlackControl_lcd.h"
#include "GTSV_TSense.h"
#include "buzzer.h"
#include "GTSV_serial.h"
#include <stdint.h>
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
struct SystemConfig {
	uint8_t sys_clk_src; //0x00: MSI | 0x04:HSI | 0x08:HSE | 0x0C: PLL --> used as sysclk source
	RCC_ClocksTypeDef RCC_Clk;
	unsigned RCC_FLAG_HSIRDY1:1;
	unsigned RCC_FLAG_MSIRDY1:1;
	unsigned RCC_FLAG_HSERDY1:1;
	unsigned RCC_FLAG_PLLRDY1:1;
	unsigned RCC_FLAG_LSERDY1:1;
	unsigned RCC_FLAG_LSIRDY1:1; //LSI oscillator clock ready
    unsigned RCC_FLAG_OBLRST1:1;  // Option Byte Loader (OBL) reset 
	unsigned RCC_FLAG_PINRST1:1;// Pin reset
	unsigned RCC_FLAG_PORRST1:1;// POR/PDR reset
	unsigned RCC_FLAG_SFTRST1:1;// Software reset
	unsigned RCC_FLAG_IWDGRST1:1;// Independent Watchdog reset
	unsigned RCC_FLAG_WWDGRST1:1;// Window Watchdog reset
	unsigned RCC_FLAG_LPWRRST1:1;// Low Power reset

};

enum System_state_enum_t {
	SYS_STATE_OFF,
	SYS_STATE_AUTO,
	SYS_STATE_CLK_ADJ,
	SYS_STATE_BLOWING,
	SYS_STATE_BLOWING_APO_ADJ,
	SYS_STATE_BLOWING_APO	
};

enum System_working_mode_enum_t {
	WORKING_INPUT_SLAVE,
	WORKING_OUTPUT_MASTER
};

struct SystemFlags {
	unsigned ms10_flag:1;
	unsigned ms50_flag:1;
	unsigned ms100_flag:1;
	unsigned ms125_flag:1;
	unsigned ms200_flag:1;
	unsigned ms500_flag:1;
	unsigned ms300_flag:1;
	unsigned msMain_flag:1;
	unsigned fanRotate:2;

	unsigned light_state:1;
	unsigned led_backlight:1;
	unsigned time_adj_stage:1;
	unsigned blower_apo_time_out:1;

//when start up, default working mode to INPUT_SLAVE
//any touch button --> send cmd --> Wait util Ack --> working_mode = OUTPUT_MASTER
//any correct command from serials --> INPUT_SLAVE --> Ack
	enum System_working_mode_enum_t working_mode;
	
	uint8_t  msMainTick;
	uint8_t  time_adj_delay;
	uint8_t  tmp_hour;
	uint8_t  tmp_min;
	uint8_t  blower_apo_mins_tmp;
	uint8_t  blower_apo_mins;
	uint8_t  blower_fan_speed;
	enum System_state_enum_t sys_state;
	RTC_TimeTypeDef blower_apo_begin;
	RTC_TimeTypeDef blower_apo_end;
	uint16_t blower_apo_remaining_sec;

	uint8_t system_uid[12];
	
	
};


extern uint16_t msTicks;
extern struct SystemFlags gSystemFlags;
extern uint32_t tmp_ir_cmd;

/* Exported constants --------------------------------------------------------*/
#define DEBUG
#define INT_PRIORITY_WKUP		((1 << __NVIC_PRIO_BITS) -2)
#define INT_PRIORITY_SYSTICK		((1 << __NVIC_PRIO_BITS) -3)
#define INT_PRIORITY_TIM7		((1 << __NVIC_PRIO_BITS) -5)
#define INT_PRIORITY_TIM6		((1 << __NVIC_PRIO_BITS) -6)
#define INT_PRIORITY_USART1		((1 << __NVIC_PRIO_BITS) -2) //-->make sure no lost

#define MAIN_TICK_MS			25
#define TIME_ADJ_DELAY_DEFAULT	88


/* Exported macro ------------------------------------------------------------*/

#ifndef BITBAND_PERI
#define BITBAND_PERI(a,b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))
#endif
#ifndef BITBAND_SRAM
#define BITBAND_SRAM(a,b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))
#endif

#define U_ID_0 (*(uint32_t*) 0x1FF80050)
#define U_ID_1 (*(uint32_t*) 0x1FF80054)
#define U_ID_2 (*(uint32_t*) 0x1FF80058)

#define MY_UID_0_ADDR	((uint8_t*) 0x1FF80050)

#define BITBAND_POINTER_AT(a,b)\
			*((volatile unsigned char *)(BITBAND_PERI(a,b)))



//this is for toggle pin
#ifndef ODR_REG_OFFSET
#define ODR_REG_OFFSET		0x14
#endif
#ifndef BSRRL_REG_OFFSET	
#define BSRRL_REG_OFFSET		0x18
#endif
#ifndef BSRRH_REG_OFFSET	
#define BSRRH_REG_OFFSET		0x1A
#endif

#define LED_BACKLIGHT	BITBAND_POINTER_AT(GPIOB_BASE + ODR_REG_OFFSET, 12)
#define LED_BACKLIGHT_ON	BITBAND_POINTER_AT(GPIOB_BASE + BSRRL_REG_OFFSET, 12) = 1
#define LED_BACKLIGHT_OFF BITBAND_POINTER_AT(GPIOB_BASE + BSRRH_REG_OFFSET, 12) = 1

#define LED_LIGHT_BT	BITBAND_POINTER_AT(GPIOB_BASE + ODR_REG_OFFSET, 13)
#define LED_LIGHT_BT_ON	BITBAND_POINTER_AT(GPIOB_BASE + BSRRL_REG_OFFSET, 13)=1
#define LED_LIGHT_BT_OFF BITBAND_POINTER_AT(GPIOB_BASE + BSRRH_REG_OFFSET, 13)=1

#define LED_TIMER_BT	BITBAND_POINTER_AT(GPIOB_BASE + ODR_REG_OFFSET, 14)
#define LED_PLUS_BT		BITBAND_POINTER_AT(GPIOA_BASE + ODR_REG_OFFSET, 2)
#define LED_MINUS_BT	BITBAND_POINTER_AT(GPIOA_BASE + ODR_REG_OFFSET, 4)
#define LED_AUTO_BT		BITBAND_POINTER_AT(GPIOA_BASE + ODR_REG_OFFSET, 1)

#define MAIN_LAMP		BITBAND_POINTER_AT(GPIOB_BASE + ODR_REG_OFFSET, 7)
#define MAIN_LAMP_ON	BITBAND_POINTER_AT(GPIOB_BASE + BSRRL_REG_OFFSET, 7) = 1
#define MAIN_LAMP_OFF	BITBAND_POINTER_AT(GPIOB_BASE + BSRRH_REG_OFFSET, 7) = 1

#define BLOWER_FAN1		BITBAND_POINTER_AT(GPIOC_BASE + ODR_REG_OFFSET, 2)
#define BLOWER_FAN2		BITBAND_POINTER_AT(GPIOC_BASE + ODR_REG_OFFSET, 3)
#define BLOWER_FAN3		BITBAND_POINTER_AT(GPIOC_BASE + ODR_REG_OFFSET, 1)
#define BLOWER_FAN4		BITBAND_POINTER_AT(GPIOC_BASE + ODR_REG_OFFSET, 0)






/* Exported functions ------------------------------------------------------- */

//void TimingDelay_Decrement(void);
//void Delay(__IO uint32_t nTime);


void Cpu_to_default_config(void);
void Get_system_clk_config(void);
void Ports_to_default_config(void);
void Ports_to_input_slave_config(void);
void Ports_to_output_master_config(void);

void Timers_to_default_config(void);


void Irr_main_loop(void);

void Blower_set_speed(uint8_t spd);
void auto_power_off_check_time(void);
bool Systick_check_delay50ms(void);

void main_big_switch(void);
void main_tick(void);


#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
