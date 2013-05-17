/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUZZER_H_INCLUDED
#define __BUZZER_H_INCLUDED

#define BUZZER_DEFAULT_SOUND_FREQ_HZ	2000
#define BUZZER_DEFAULT_BIP_LENGHT_MS	200

#define BUZZER_TIMER_NUM				7
#define BUZZER_TIMER				TIM7


#define BUZZER_PORT_BASE				GPIOA_BASE
#define BUZZER_PIN_NUM				12

//call this to initialize timer ISR to make sound
void Buzzer_timer_to_default_state(void);

//put this in the Timer ISR to generate sound
void Buzzer_timerISR_make_sound(void);

//call this as 50ms tick to timing buzzer off.
void Buzzer_off_timing_tick50ms(void);


int Buzzer_bip(void);  
int Buzzer_2bips(void);
int Buzzer_3bips(void);



#endif
