
#include "main.h"
#include "GTSV_serial.h"



struct Serial_Parrams_t _serial_parrams = {
	.nbr_of_cmd_in_buff = 0,
	.next_cmd_idx = 0,
	.receiving_cmd_idx = 0,
	.other_uid = {0,0,0},
	.other_uid_valid = 0,
};

struct Serial_Cmd_Result_t _serial_cmd_results;

struct Serial_Cmd_Detect_t {
	unsigned active:1;
	unsigned hold:1;
};
volatile struct Serial_Cmd_Detect_t _serial_cmd_detect[SERIAL_TOTAL_CMD];

volatile uint16_t _serial_time_out_cnt;

void Serial_time_out_begin(void);




bool Serial_cmd_do_decode(struct Serial_Cmd_Result_t *results)
{
	uint8_t i;
	static uint8_t tmp_cmd;
	tmp_cmd = results->cmd;
	switch(tmp_cmd){
	case SERIAL_CMD_LIGHT:
	case SERIAL_CMD_PLUS:
	case SERIAL_CMD_MINUS:
	case SERIAL_CMD_TIMER:
	case SERIAL_CMD_AUTO:
	case SERIAL_CMD_REQUEST_UID:
		results->data_len=0;
		return TRUE;
		break;
	case SERIAL_CMD_UID:
		if(results->raw_buff_len==15){
			for(i=0; i<15; i++){
			//save uid
			//make uid valid
				if((i>1) &&(i<14))
					_serial_parrams.other_uid[i-2] = *results->raw_buff;
				results->raw_buff++;
			}
			_serial_parrams.other_uid_valid=1;
			return TRUE;
		}else{
			return FALSE;
		}

		break;
	default:
		return FALSE;
		break;
	}

}


//first byte is len, 2nd byte is cmd
bool Serial_cmd_decode(struct Serial_Cmd_Result_t *results)
{
	if(_serial_parrams.nbr_of_cmd_in_buff>0){  //if has at least one cmd in buff
		results->raw_buff_len = _serial_parrams.buff[_serial_parrams.next_cmd_idx][0];  //data len
		results->raw_buff = &(_serial_parrams.buff[_serial_parrams.next_cmd_idx][1]);
		results->cmd = (enum Serial_Cmd_Enum_t)_serial_parrams.buff[_serial_parrams.next_cmd_idx][1];
		_serial_parrams.nbr_of_cmd_in_buff--;
		_serial_parrams.next_cmd_idx++;
		if(_serial_parrams.next_cmd_idx == SERIAL_RX_NUM_OF_CMD)
			_serial_parrams.next_cmd_idx = 0;

		if(Serial_cmd_do_decode(results)){
			return TRUE;
		}
		return FALSE;
	}else{
		return FALSE;
	}
}




void Serial_cmd_detect(void)
{
	uint8_t i;
	uint8_t tmp_cmd;
	uint8_t data_len;
	uint8_t *pdata;
	pdata = &(_serial_parrams.buff[_serial_parrams.next_cmd_idx][3]);
	data_len = _serial_parrams.buff[_serial_parrams.next_cmd_idx][2];

	if(_serial_parrams.nbr_of_cmd_in_buff>0){
		tmp_cmd = (enum Serial_Cmd_Enum_t)_serial_parrams.buff[_serial_parrams.next_cmd_idx][1];

		switch(tmp_cmd){
		case SERIAL_CMD_LIGHT:
			if(data_len == 1){
				_serial_cmd_detect[SERIAL_CMD_LIGHT].active = 1;
				_serial_parrams.light_state = *pdata;
			}
			break;
		case SERIAL_CMD_PLUS:
			_serial_cmd_detect[SERIAL_CMD_PLUS].active = 1;
			//Buzzer_bip();
			break;
		case SERIAL_CMD_MINUS:
			_serial_cmd_detect[SERIAL_CMD_MINUS].active = 1;
			//Buzzer_bip();
			break;
		case SERIAL_CMD_TIMER:
			_serial_cmd_detect[SERIAL_CMD_TIMER].active = 1;
			//Buzzer_bip();
			break;
		case SERIAL_CMD_AUTO:
			_serial_cmd_detect[SERIAL_CMD_AUTO].active = 1;
			//Buzzer_bip();
			break;
		case SERIAL_CMD_REQUEST_UID:
			_serial_cmd_detect[SERIAL_CMD_REQUEST_UID].active = 1;
			//Buzzer_bip();
			break;
		case SERIAL_CMD_UID:
			if(data_len==12){
				for(i=0; i<12; i++){
					_serial_parrams.other_uid[i] = *pdata;
					pdata++;
				}
				_serial_parrams.other_uid_valid=1;
			}
			break;
		case SERIAL_CMD_TMP_TIME:
			if(data_len == 2){
				_serial_parrams.tmp_hrs = *(pdata+1);
				_serial_parrams.tmp_mins = *(pdata);
				_serial_cmd_detect[SERIAL_CMD_TMP_TIME].active = 1;
			}
			break;
		case SERIAL_CMD_RESET_TIME_ADJ_DELAY:
			_serial_cmd_detect[SERIAL_CMD_RESET_TIME_ADJ_DELAY].active = 1;
			break;
		default:
			//while(1);
			break;
		}

		_serial_parrams.nbr_of_cmd_in_buff--;
		_serial_parrams.next_cmd_idx++;
		if(_serial_parrams.next_cmd_idx == SERIAL_RX_NUM_OF_CMD)
			_serial_parrams.next_cmd_idx = 0;


	}else{
		for(i=0; i < SERIAL_TOTAL_CMD; i++){
			_serial_cmd_detect[i].active = 0;
		}
	}

}


void Serial_cmd_detect1(void)
{
	static uint8_t tmp_cmd;
	int i;

	if(Serial_cmd_decode(&_serial_cmd_results)){
		tmp_cmd = _serial_cmd_results.cmd;

		switch(tmp_cmd){
		case SERIAL_CMD_LIGHT:
			_serial_cmd_detect[SERIAL_CMD_LIGHT].active = 1;
			Buzzer_bip();
			break;
		case SERIAL_CMD_PLUS:
			_serial_cmd_detect[SERIAL_CMD_PLUS].active = 1;
			Buzzer_bip();
			break;
		case SERIAL_CMD_MINUS:
			_serial_cmd_detect[SERIAL_CMD_MINUS].active = 1;
			Buzzer_bip();
			break;
		case SERIAL_CMD_TIMER:
			_serial_cmd_detect[SERIAL_CMD_TIMER].active = 1;
			Buzzer_bip();
			break;
		case SERIAL_CMD_AUTO:
			_serial_cmd_detect[SERIAL_CMD_AUTO].active = 1;
			Buzzer_bip();
			break;
		case SERIAL_CMD_UID:
			_serial_cmd_detect[SERIAL_CMD_UID].active = 1;
			Buzzer_bip();
			break;
		case SERIAL_CMD_REQUEST_UID:
			_serial_cmd_detect[SERIAL_CMD_REQUEST_UID].active = 1;
			Buzzer_bip();
			break;
		default:
			tmp_cmd = 0;
			break;
		}

	}else{
		tmp_cmd = 0;
		for(i=0; i < SERIAL_TOTAL_CMD; i++){
			_serial_cmd_detect[i].active = 0;
		}
	}

}

bool Serial_check_cmd(enum Serial_Cmd_Enum_t cmd)
{
	return _serial_cmd_detect[cmd].active;

}


void Serial_tx_ISR(void)
{

}





uint8_t serial_rx_state = SERIAL_RX_STATE_IDLE;
void Serial_rx_ISR(void)
{

	static uint8_t bytes_cnt=0;
	static uint8_t nbr_data_bytes;
	_serial_parrams.buff[_serial_parrams.receiving_cmd_idx][bytes_cnt] =
					(uint8_t)(USART_ReceiveData(SERIAL_COM_PORT) & 0x00FF);

	//if idle state, wait for start of frame
	if(serial_rx_state == SERIAL_RX_STATE_IDLE){
		if(_serial_parrams.buff[_serial_parrams.receiving_cmd_idx][bytes_cnt]
									== SERIAL_RX_FRAME_SOF){  //wait until sof
			serial_rx_state = SERIAL_RX_STATE_RECEIVING;
			bytes_cnt++;
			Serial_time_out_begin();
		}else{
			bytes_cnt = 0;
		}
	} else if (serial_rx_state == SERIAL_RX_STATE_RECEIVING){
		if(bytes_cnt<3){
			bytes_cnt++;
		}else{
			nbr_data_bytes = _serial_parrams.buff[_serial_parrams.receiving_cmd_idx][2];
			if(nbr_data_bytes==0){
				//check for correct EOF
				if(_serial_parrams.buff[_serial_parrams.receiving_cmd_idx][3]==SERIAL_RX_FRAME_EOF){
					_serial_parrams.buff[_serial_parrams.receiving_cmd_idx][0] = 3;
					serial_rx_state = SERIAL_RX_STATE_IDLE;
					bytes_cnt = 0;
					_serial_parrams.receiving_cmd_idx++;
					if(_serial_parrams.receiving_cmd_idx==SERIAL_RX_NUM_OF_CMD)
						_serial_parrams.receiving_cmd_idx=0;
					_serial_parrams.nbr_of_cmd_in_buff++;
				}else{
					serial_rx_state = SERIAL_RX_STATE_IDLE;
					bytes_cnt = 0;
				}
			}else{
				if(bytes_cnt == nbr_data_bytes+3){
					//check for correct EOF
					if(_serial_parrams.buff[_serial_parrams.receiving_cmd_idx][bytes_cnt]==SERIAL_RX_FRAME_EOF){
						_serial_parrams.buff[_serial_parrams.receiving_cmd_idx][0] = nbr_data_bytes+3;
						_serial_parrams.receiving_cmd_idx++;
						if(_serial_parrams.receiving_cmd_idx==SERIAL_RX_NUM_OF_CMD)
							_serial_parrams.receiving_cmd_idx=0;
						_serial_parrams.nbr_of_cmd_in_buff++;
					}else{ //if not correct EOF discard
						serial_rx_state = SERIAL_RX_STATE_IDLE;
						bytes_cnt = 0;
					}
				}else{
					bytes_cnt++;
				}

			}

		}

	}
}



void Serial_send_bytes(uint8_t *p_tx, uint8_t nbr_of_bytes)
{
	uint16_t checksum=0;
	
	uint8_t i;
	uint8_t * tmp;
	uint8_t remaining_bytes;
	tmp = p_tx;
	remaining_bytes = nbr_of_bytes;

	for(i=0;i<(nbr_of_bytes-1); i++){
		checksum += *(p_tx+i);
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE)){}
		//USART1->DR = *tmp;
		USART_SendData(USART1, *tmp);
	}
	
	while(remaining_bytes!=0){
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE)){}
		//USART1->DR = *tmp;
		USART_SendData(USART1, *tmp);
		tmp++;
		remaining_bytes--;
	}

}

void Serial_send_cmd_light()
{
	uint8_t tx_tmp[5];
	tx_tmp[0] = SERIAL_RX_FRAME_SOF;
	tx_tmp[1] = SERIAL_CMD_LIGHT;
	tx_tmp[2] = 1;
	tx_tmp[3] = gSystemFlags.light_state;
	tx_tmp[4] =  SERIAL_RX_FRAME_EOF;
	Serial_send_bytes(tx_tmp, 5);


}
void Serial_send_tmp_time(void)
{
	uint8_t tx_tmp[6];
	tx_tmp[0] = SERIAL_RX_FRAME_SOF;
	tx_tmp[1] = SERIAL_CMD_TMP_TIME;
	tx_tmp[2] = 2;
	tx_tmp[3] = gSystemFlags.tmp_min;
	tx_tmp[4] = gSystemFlags.tmp_hour;
	tx_tmp[5] = SERIAL_RX_FRAME_EOF;
	Serial_send_bytes(tx_tmp, 6);

}

void Serial_send_my_uid(void){
	uint8_t i;
	uint8_t tx_tmp[16];
	tx_tmp[0]	= SERIAL_RX_FRAME_SOF;
	tx_tmp[15]= SERIAL_RX_FRAME_EOF;
	tx_tmp[1] = SERIAL_CMD_UID;
	tx_tmp[2] = 12;
	for (i=0; i<12; i++){
		tx_tmp[i+3] = gSystemFlags.system_uid[i];
	}
	Serial_send_bytes(tx_tmp, 16);
}

void Serial_send_cmd(enum Serial_Cmd_Enum_t cmd)
{
	uint8_t tx_tmp[4];
	tx_tmp[0] = SERIAL_RX_FRAME_SOF;
	tx_tmp[1] = (uint8_t)cmd;
	tx_tmp[2] = 0;
	tx_tmp[3] = SERIAL_RX_FRAME_EOF;

	Serial_send_bytes(tx_tmp, 4);

}

bool Serial_check_other_uid_valid(void)
{
	return _serial_parrams.other_uid_valid;
}

uint8_t* Serial_get_other_uid(void)
{
	if(_serial_parrams.other_uid_valid){

		return _serial_parrams.other_uid;
	}else{
		return NULL;
	}
}


void Serial_time_out_begin(void)
{
	_serial_time_out_cnt=0;
}
void Serial_time_out_tick(void)
{
	if(_serial_time_out_cnt>5)
		serial_rx_state = SERIAL_RX_STATE_IDLE;
	else
		_serial_time_out_cnt++;

}

void Usart_to_default_config(void)
{
	USART_InitTypeDef USART_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* USARTx configuration ----------------------------------------------------*/
	/* USARTx configured as follow:
	- BaudRate = 57600 baud
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
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

