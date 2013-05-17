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
#ifndef __IRREMOTE_H_INCLUDED
#define __IRREMOTE_H_INCLUDED

/* Includes ------------------------------------------------------------------*/
#include "main.h"



/* Exported constants --------------------------------------------------------*/

#define IRR_RAWBUF			100 	// Lenght of raw duration buffer
#define IRR_SENS_PORT_BASE	GPIOB_BASE	//address of the port base register
#define IRR_SENS_PIN		15			//pin number	0..15

// Decoded value for NEC when a repeat code is received
#define IRR_NEC_REPEAT		0xffffffff

// IR detector output is active low
#define IRR_MARK			0
#define IRR_SPACE			1

#define IRR_NEC_CMD_SPEEDUP		0x04FB8877
#define IRR_NEC_CMD_SPEEDDOWN	0x04FB50AF
#define IRR_NEC_CMD_ONOFF		0x04FB40BF
#define IRR_NEC_CMD_LIGHT		0x04FB58A7
#define IRR_NEC_CMD_AUTO		0x04FB906F
#define IRR_NEC_CMD_TIMER		0x04FB9867
#define IRR_NEC_CMD_REPEAT		0xffffffff

#define IRR_NUM_OF_KEYS			5
/* Exported types ------------------------------------------------------------*/
enum Irr_Key_Enum_t{
	IRR_KEY_PLUS=0,
	IRR_KEY_MINUS=1,
	IRR_KEY_TIMER=2,
	IRR_KEY_AUTO=3,
	IRR_KEY_LIGHT=4
};

//ir recv states
enum irr_state_t {
	IRR_STATE_IDLE,
	IRR_STATE_MARK,
	IRR_STATE_SPACE,
	IRR_STATE_STOP
};

enum irr_decode_type_t {
	IRR_DECODE_NEC,
	IRR_DECODE_SONY,
	IRR_DECODE_RC5
};

struct irrparams_t {
	volatile unsigned char * recv_pin; 		//the address of the IDR bit of the port
	enum irr_state_t recv_state;		//state machine
	uint16_t	tick_cnt;			//counts 50us ticks
	uint16_t	rawbuff[IRR_RAWBUF];//raw data
	uint8_t		rawbuff_len;		//counter of entries in rawbuff

};

struct irr_decode_results_t {
	enum irr_decode_type_t decode_type;		//NEC, SONY, RC5, UNKNOWN
	uint8_t panasonicAddress; //this is only used for decoding Panasonic data
	uint32_t value;			//decoded value
	uint8_t  value_bit_len;	//number of bit in decoded value
	volatile uint16_t * rawbuff;  //pointer to rawbuff
	int	rawbuff_len;				//number of records in rawbuff
};


/* Exported vars ------------------------------------------------------------*/

extern struct irrparams_t irrparams;
extern struct irr_decode_results_t irr_decode_results;

/* Exported functions ------------------------------------------------------- */
void Irr_init(void);
int Irr_decode(struct irr_decode_results_t *results);
void Irr_resume(void);
void Irr_key_detect(void);
void Irr_key_detect1(void);



bool Irr_check_key(enum Irr_Key_Enum_t key);



#endif /* __IRREMOTE_H_INCLUDED */

/******************* (C) COPYRIGHT 2013 GTSV*****END OF FILE****/