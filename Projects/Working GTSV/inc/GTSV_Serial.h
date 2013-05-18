/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GTSV_SERIAL_H_INCLUDED
#define __GTSV_SERIAL_H_INCLUDED

#define SERIAL_COM_PORT	USART1
#define SERIAL_BAUDRATE	9600

#define SERIAL_PACKET_SOF	0x3A
#define SERIAL_PACKET_EOF1	0x0D
#define SERIAL_PACKET_EOF2	0x0A

#define SERIAL_PACKET_IDX_SOF			0
#define SERIAL_PACKET_IDX_CMD			1
#define SERIAL_PACKET_IDX_DATA_LEN		2
#define SERIAL_PACKET_MIN_BYTES		7



#define SERIAL_BUFF_MAX_CMD	18
#define SERIAL_BUFF_CMD_MAX_BYTES	38

#define SERIAL_RX_PACKET_TIMEOUT_MS	500

enum Serial_Rx_State_t {
	SERIAL_RX_STATE_IDLE,
	SERIAL_RX_STATE_RECEIVING
};


struct Serial_Tx_t{
	uint8_t *pdata;
	uint8_t bytes_left;
	unsigned tx_completed:1;
};
struct Serial_Rx_t {
	enum Serial_Rx_State_t state;
	uint8_t buff[SERIAL_BUFF_MAX_CMD][SERIAL_BUFF_CMD_MAX_BYTES];
	uint8_t nbr_of_cmd_in_buff;
	uint8_t head_cmd_idx;
	uint8_t tail_cmd_idx;
		
	uint8_t bytes_cnt;
	uint16_t time_out_cnt;
	uint8_t* current;
	uint8_t* last;
	uint8_t data_len;
	unsigned time_out:1;
};

extern struct Serial_Rx_t rxBuff;


void Serial_rx_ISR(void);
void Serial_tx_ISR(void);

void Usart_to_default_config(void);
void Serial_time_tick50ms(void);

void Serial_tx_send_bytes_blocking(uint8_t *p_tx, uint8_t nbr_of_bytes);
void Serial_tx_send_bytes_non_blocking(uint8_t *p_tx, uint8_t nbr_of_bytes);

bool Serial_rx_check_buff_not_empty(void);
uint8_t* Serial_rx_get_1cmd_from_buff(void);


#endif
