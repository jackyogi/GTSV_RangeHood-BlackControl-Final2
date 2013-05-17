 /**
  ******************************************************************************
  * @file    stm32l_discovery_lcd.h
  * @author  Microcontroller Division
  * @version V1.0.0
  * @date    Apri-2011
  * @brief   This file contains all the functions prototypes for the glass LCD
  *          firmware driver.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GTSV_BlackControl_lcd
#define __GTSV_BlackControl_lcd

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"   

extern volatile uint8_t _lcd_blink_cursor;
/*  =========================================================================
                                 LCD MAPPING
    =========================================================================
     LCD COM0: 

*/
enum Lcd_Icons {
	LCD_CLOCK_ICON=0,
	LCD_LIGHTBULB_ICON,
	LCD_LIGHTRAY_ICON,
	LCD_FAN1_ICON,
	LCD_FAN2_ICON,
	LCD_FAN3_ICON,
	LCD_ROTATE_ICON,
	LCD_COLON_ICON,
	LCD_ALL_ICON
};


void Lcd_configure_GPIO(void);
void Lcd_to_default_config(void);

void Lcd_clear(void);

void Lcd_fill_num_to_position(uint8_t num, uint8_t pos);
void Lcd_fill_hours(uint8_t num);
void Lcd_fill_mins(uint8_t num);
void Lcd_icon_on(enum Lcd_Icons icon);
void Lcd_icon_off(enum Lcd_Icons icon);
void Lcd_icon_toggle(enum Lcd_Icons icon);
void Lcd_icon_fan(uint8_t num);
//void Lcd_blink_systicISR_ms(void);
void Lcd_icon_buff_flush(void);
uint8_t Lcd_get_blink_cursor(void);
uint8_t Lcd_get_fan_cursor_slow(void);
uint8_t Lcd_get_fan_cursor_fast(void);


void Lcd_fill_pos_with_blank(uint8_t pos);
void Lcd_fill_pos_with_num(uint8_t pos, uint8_t num);
void Lcd_blink_tick125ms(void);

#endif /* stm32l_discovery_lcd*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
