#include "main.h"
#include "buzzer.h"




#define BUZZER_BIT\
		*((volatile unsigned char *)(BITBAND_PERI(BUZZER_PORT_BASE + ODR_REG_OFFSET, BUZZER_PIN_NUM)))
#define BUZZER_TOGGLE()\
		BUZZER_BIT ^= 1;

//buzzer private global vars
uint16_t	_gBuzzer_bip_lenght = 5;
uint8_t		_gBuzzer_50msTick = 0;
uint8_t		_gBuzzer_bip_cnt = 0;

int Buzzer_bip(void)
{

	if(_gBuzzer_50msTick == 0){
		_gBuzzer_50msTick = 1;
		_gBuzzer_bip_cnt = 1;
		return 0;
	} else {
		return -1;
	}

}
int Buzzer_2bips(void)
{

	if(_gBuzzer_50msTick == 0){
		_gBuzzer_50msTick = 1;
		_gBuzzer_bip_cnt = 2;
		return 0;
	} else {
		return -1;
	}

}

int Buzzer_bip1(void)
{
	if(gSystemFlags.working_mode == WORKING_OUTPUT_MASTER){
		if(_gBuzzer_50msTick == 0){
			_gBuzzer_50msTick = 1;
			_gBuzzer_bip_cnt = 1;
			return 0;
		} else {
			return -1;
		}
	}
	return -1;
}
int Buzzer_2bips1(void)
{
	if(gSystemFlags.working_mode == WORKING_OUTPUT_MASTER){
		if(_gBuzzer_50msTick == 0){
			_gBuzzer_50msTick = 1;
			_gBuzzer_bip_cnt = 2;
			return 0;
		} else {
			return -1;
		}
	}
	return -1;
}
int Buzzer_3bips(void)
{
	
	if(_gBuzzer_50msTick == 0){
		_gBuzzer_50msTick = 1;
		_gBuzzer_bip_cnt = 3;
		return 0;
	} else {
		return -1;
	}
}

/*
//call in systick ISR every ms to timing on/off buzzer
void Buzzer_systickISR_timing_ms(void)
{
	if(_gBuzzer_msTick){
		if(_gBuzzer_msTick == 1){
			_gBuzzer_msTick++;
			TIM_Cmd(BUZZER_TIMER, ENABLE);
		} else if(_gBuzzer_msTick < _gBuzzer_bip_lenght){
			_gBuzzer_msTick++;
		} else {
			_gBuzzer_msTick = 0;
			TIM_Cmd(BUZZER_TIMER, DISABLE);
		}
	}
}
*/
void Buzzer_off_timing_tick50ms(void)
{
	if(_gBuzzer_50msTick){
		if(_gBuzzer_50msTick < _gBuzzer_bip_lenght){
			if(_gBuzzer_50msTick == 1)
				TIM_Cmd(BUZZER_TIMER, ENABLE);
			_gBuzzer_50msTick++;
		}else{
			TIM_Cmd(BUZZER_TIMER, DISABLE);
			_gBuzzer_bip_cnt--;
			if(_gBuzzer_bip_cnt == 0)
				_gBuzzer_50msTick = 0;
			else
				_gBuzzer_50msTick = 1;
		}
	}

	
}
//call in Timer ISR to generate sound
void Buzzer_timerISR_make_sound(void)
{
	BUZZER_TOGGLE();
}


void Buzzer_timer_to_default_state(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	// Init timer 7 to generate buzzer sound & auto off
	//clock to TIME7
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	//TIM7 TimeBase init
	TIM_TimeBaseStructure.TIM_Period = ((SystemCoreClock / (2*BUZZER_DEFAULT_SOUND_FREQ_HZ)) - 1);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(BUZZER_TIMER, &TIM_TimeBaseStructure);

	//enable local timer interupt
	TIM_ClearITPendingBit(BUZZER_TIMER,TIM_IT_Update);
	TIM_ITConfig(BUZZER_TIMER,TIM_IT_Update,ENABLE);

	//Enable Global TIM6 INT
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_TIM7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

