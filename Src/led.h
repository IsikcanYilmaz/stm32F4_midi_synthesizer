

#include "main.h"
#include "tim.h"

#define PWM_CHANNEL_GREEN  TIM_CHANNEL_1
#define PWM_CHANNEL_BLUE   TIM_CHANNEL_2
#define PWM_CHANNEL_RED    TIM_CHANNEL_3
#define PWM_CHANNEL_ORANGE TIM_CHANNEL_4

#define LED_SIGNAL_SIZE 10000

#define LED_DEMO_MAX_PWM 8192 * 4
#define LED_DEMO_DELAY_THRESH 0

#define LED_CEIL (8192)
#define LED_SAMPLERATE 10000
#define LED_Fs ((float)(LED_SAMPLERATE))
#define LED_Ts (1.f/LED_Fs) // Period, step size

#define DEMO_ON 1

extern uint16_t led_signal[LED_SIGNAL_SIZE];
extern uint16_t led_lfo_signal[LED_SIGNAL_SIZE];
extern int16_t led_speed;

void led_init();
void led_isr();
void led_demo_animation();
