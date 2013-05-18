
#include "main.h"
#include "GTSV_serial.h"


struct Serial_Rx_t rxBuff = {
	.nbr_of_cmd_in_buff = 0,
	.head_cmd_idx = 0,
	.tail_cmd_idx = 0,
};

struct Serial_Tx_t txBuff = {
	.pdata = NULL,
	.bytes_left = 0,
	.tx_completed = TRUE,
};

void Serial_time_out_begin(void);
bool Serial_check_timeout(void);
void Serial_rx_reset_to_idle(void);
void Serial_rx_complete_receiving_packet(void);


void Usart_to_default_config(void)
{
	USART_InitTypeDef USART_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_USART1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}

void Serial_tx_send_bytes_blocking(uint8_t *p_tx, uint8_t nbr_of_bytes)
{
	uint8_t i;
	uint8_t * tmp = p_tx;
	//tmp = p_tx;

	for(i=0; i<nbr_of_bytes; i++){
		while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET){}
		USART1->DR = *tmp;
		tmp++;
	}
}

void Serial_tx_send_bytes_non_blocking(uint8_t *p_tx, uint8_t nbr_of_bytes)
{
	txBuff.pdata = p_tx;
	txBuff.bytes_left = nbr_of_bytes;
	txBuff.tx_completed = FALSE;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void Serial_tx_ISR(void)
{
	if(!txBuff.tx_completed){
		USART1->DR = *txBuff.pdata;
		if(txBuff.bytes_left>0){
			txBuff.pdata++;
			txBuff.bytes_left--;
		}else{
			txBuff.tx_completed = TRUE;
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}	
}

void Serial_rx_ISR(void)
{
	static uint8_t * p_last;
	static uint8_t * p_current;

	p_last = p_current;
	p_current = &(rxBuff.buff[rxBuff.head_cmd_idx][rxBuff.bytes_cnt]);
	*p_current = (uint8_t)(SERIAL_COM_PORT->DR & 0x00FF);

	switch(rxBuff.state){
	case SERIAL_RX_STATE_IDLE:
		//wait until SOF
		if(*p_current == SERIAL_PACKET_SOF){
			rxBuff.state = SERIAL_RX_STATE_RECEIVING;
			rxBuff.bytes_cnt++;
			Serial_time_out_begin();
		}else{
			rxBuff.bytes_cnt = 0;
		}

		break;
	case SERIAL_RX_STATE_RECEIVING:
		//keep receiving until EOF
		if((*p_current==SERIAL_PACKET_EOF2) && (*p_last==SERIAL_PACKET_EOF1)){
			rxBuff.data_len = rxBuff.buff[rxBuff.head_cmd_idx][SERIAL_PACKET_IDX_DATA_LEN];
			if(rxBuff.data_len == (rxBuff.bytes_cnt-6)){								
				Serial_rx_complete_receiving_packet();
			}else{
				Serial_rx_reset_to_idle();
			}
		}else{
			rxBuff.bytes_cnt++;
		}

		//& check for  time out
		if(Serial_check_timeout()){
			Serial_rx_reset_to_idle();
		}
		break;
	default:
		Serial_rx_reset_to_idle();
		break;
	}
}



void Serial_time_out_begin(void)
{
	rxBuff.time_out_cnt 	= 0;
	rxBuff.time_out 		= FALSE;
}
void Serial_time_tick50ms(void)
{
	if(rxBuff.time_out_cnt > (SERIAL_RX_PACKET_TIMEOUT_MS/50))
		rxBuff.time_out = TRUE;
	else
		rxBuff.time_out_cnt++;
}
bool Serial_check_timeout(void)
{
	return rxBuff.time_out;
	//return FALSE;
}

void Serial_rx_reset_to_idle(void)
{
	rxBuff.state = SERIAL_RX_STATE_IDLE;
	rxBuff.bytes_cnt = 0;
}
void Serial_rx_complete_receiving_packet(void)
{	
	//make first byte = buff len
	rxBuff.buff[rxBuff.head_cmd_idx][0] = rxBuff.bytes_cnt;
	if(rxBuff.head_cmd_idx < SERIAL_BUFF_MAX_CMD-1)
		rxBuff.head_cmd_idx++;
	else
		rxBuff.head_cmd_idx = 0;
	
	rxBuff.nbr_of_cmd_in_buff++;
	
	Serial_rx_reset_to_idle();
}

bool Serial_rx_check_buff_not_empty(void)
{
	if(rxBuff.nbr_of_cmd_in_buff>0)
		return TRUE;
	else
		return FALSE;
}
uint8_t* Serial_rx_get_1cmd_from_buff(void)
{
	uint8_t *pdata;
	if(rxBuff.nbr_of_cmd_in_buff>0){
		pdata = rxBuff.buff[rxBuff.tail_cmd_idx];
		rxBuff.nbr_of_cmd_in_buff--;					
		if(rxBuff.tail_cmd_idx<SERIAL_BUFF_MAX_CMD-1)
			rxBuff.tail_cmd_idx++;
		else
			rxBuff.tail_cmd_idx=0;
		return pdata;
	}else{
		return NULL;
	}

}

