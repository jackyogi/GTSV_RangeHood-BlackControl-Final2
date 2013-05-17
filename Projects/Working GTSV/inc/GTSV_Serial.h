/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GTSV_SERIAL_H_INCLUDED
#define __GTSV_SERIAL_H_INCLUDED

/* Includes ------------------------------------------------------------------*/

#define SERIAL_COM_PORT		USART1


enum Serial_Cmd_Enum_t {
	SERIAL_CMD_LIGHT=0,
	SERIAL_CMD_PLUS=1,
	SERIAL_CMD_MINUS=2,
	SERIAL_CMD_TIMER=3,
	SERIAL_CMD_AUTO=4,
	SERIAL_CMD_UID=5,
	SERIAL_CMD_REQUEST_UID=6,
	SERIAL_CMD_ACK=7,
	SERIAL_CMD_TMP_TIME,
	SERIAL_CMD_RESET_TIME_ADJ_DELAY,
	SERIAL_TOTAL_CMD
};

#define SERIAL_RX_FRAME_SOF	0xAA
#define SERIAL_RX_FRAME_EOF	0xCC

#define SERIAL_RX_STATE_IDLE		0
#define SERIAL_RX_STATE_RECEIVING 	1



#define SERIAL_RX_CMD_SIZE		(88)
#define SERIAL_RX_NUM_OF_CMD		(8)

//#define SERIAL_TX_BUFF_SIZE		(88)


struct Serial_Parrams_t {
	uint8_t buff[SERIAL_RX_NUM_OF_CMD][SERIAL_RX_CMD_SIZE];
	uint8_t nbr_of_cmd_in_buff;  //number of unprocessed cmd in buff
	uint8_t next_cmd_idx;
	uint8_t receiving_cmd_idx;
	
	uint8_t other_uid[12];  //96bits uid
	
	uint8_t tmp_mins;
	uint8_t tmp_hrs;
	unsigned other_uid_valid:1;
	uint8_t light_state;
	
};


struct Serial_Cmd_Result_t {
	enum Serial_Cmd_Enum_t cmd;
	uint8_t data[SERIAL_RX_CMD_SIZE - 5];
	uint8_t data_len;   //num of bytes in data
	volatile uint8_t *raw_buff;
	uint8_t raw_buff_len;  //num of bytes in raw buff
	
};

extern struct Serial_Parrams_t _serial_parrams;
extern struct Serial_Cmd_Result_t _serial_cmd_results;
extern uint8_t serial_rx_state;
/* Exported constants --------------------------------------------------------*/

void Usart_to_default_config(void);
void Serial_rx_ISR(void);
void Serial_tx_ISR(void);
void Serial_cmd_detect(void);
bool Serial_check_cmd(enum Serial_Cmd_Enum_t cmd);
void Serial_cmd_detect(void);

void Serial_send_my_uid(void);
void Serial_send_cmd(enum Serial_Cmd_Enum_t cmd);
uint8_t * Serial_get_other_uid(void);
void Serial_time_out_tick(void);
void Serial_send_tmp_time(void);
void Serial_send_cmd_light(void);


#endif

