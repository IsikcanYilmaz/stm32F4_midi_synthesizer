

#include "main.h"
#include "tim.h"

#define PWM_CHANNEL_GREEN  TIM_CHANNEL_1
#define PWM_CHANNEL_ORANGE TIM_CHANNEL_2
#define PWM_CHANNEL_RED    TIM_CHANNEL_3
#define PWM_CHANNEL_BLUE   TIM_CHANNEL_4
#define LED_SET_CHANNEL(x,y) __HAL_TIM_SET_COMPARE(&htim4, x, y);

#define NUM_LEDS 4
#define LED_SIGNAL_SIZE 10000

#define LED_DEMO_MAX_PWM 8192 * 4
#define LED_DEMO_DELAY_THRESH 0

#define LED_CEIL (8192)
#define LED_SAMPLERATE 10000
#define LED_Fs ((float)(LED_SAMPLERATE))
#define LED_Ts (1.f/LED_Fs) // Period, step size

#define LED_DEMO_ON 1
#define LED_DEMO_BLINK  0 // blink
#define LED_DEMO_SINE   1 // normal sine
#define LED_DEMO_ALTERN 0 // alternating leds sine
#define LED_DEMO_LATEST 0 // alternating leds that modulate faster when button is held down
#define LED_DEMO_VOICES 0 // leds to reflect adsr outputs of (at most) 4 voices

extern uint16_t led_signal[LED_SIGNAL_SIZE];
extern uint16_t led_lfo_signal[LED_SIGNAL_SIZE];
extern int16_t led_speed;

extern uint32_t led_cursor;

void led_init();
void led_isr();


