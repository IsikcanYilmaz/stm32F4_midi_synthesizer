

#include "led.h"
#include "math.h"
#include "gpio.h"
#include "oscillator.h"
#include "tim.h"
#include "vca.h"
#include "config.h"
//#include "synth.h"

// tim4 is the timer with the pwm controlling the leds.
// tim5 is the sampling timer (i guess itd be called that?)
// we want the sampling timer to interrupt with frequency of 500Hz? 
// thie initialization of the timer will be done in tim.c

extern VCA_t voices[NUM_VOICES];

Oscillator_t led_osc;
Oscillator_t led_lfo;
uint16_t led_signal[LED_SIGNAL_SIZE];
uint16_t led_lfo_signal[LED_SIGNAL_SIZE];
uint32_t led_cursor; 

int16_t led_speed;

static float led_update_oscillator(Oscillator_t *osc){
  /* Sine
  */
  float z;

  while (osc->phase >= _2PI)
    osc->phase -= _2PI;

  z = sin(osc->phase);
  osc->out = osc->amp * z;

  osc->phase += _2PI * LED_Ts * osc->freq; // increment phase
  return osc->out;
}

void led_init(){
  led_cursor = 0;

  led_tim_init(LED_SAMPLERATE);
  led_osc.amp = 1.0f;
  led_osc.last_amp = 0.9f;
  led_osc.freq = 1;
  led_osc.phase = 3*_PI/2; // Start with amplitude 0
  led_osc.out = 0;
  led_osc.modInd = 0;

  led_lfo.amp = 1.0f;
  led_lfo.last_amp = 0.9f;
  led_lfo.freq = 0.2;
  led_lfo.phase = 0;
  led_lfo.out = 0;
  led_lfo.modInd = 0;

  uint16_t pos;
  volatile float y = 0;

  for (pos = 0; pos < LED_SIGNAL_SIZE; pos++){
    // lets not deal with the oscillator abstraction yet
    y = led_update_oscillator(&led_osc);
    //y = sin((pos * LED_Ts) * _2PI); 
    led_signal[pos] = (uint16_t) (LED_CEIL/2 * (y + 1));
  }
  for (pos = 0; pos < LED_SIGNAL_SIZE; pos++){
    // lets not deal with the oscillator abstraction yet
    y = led_update_oscillator(&led_lfo); // TODO deal with the oscillator abstraction
    //y = sin((pos * LED_Ts) * _2PI); 
    led_lfo_signal[pos] = (uint16_t) (LED_CEIL/2 * (y + 1));
  }
  if (LED_DEMO_ON) HAL_TIM_Base_Start_IT(&htim5);

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void led_isr(){
#if LED_DEMO_BLINK
  /* BLINK */
     static bool on = false;
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, on * 4096);
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, on * 4096);
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, on * 4096);
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, on * 4096);
     on = !on;
#endif
#if LED_DEMO_SINE
  /* SINE */
  static uint32_t led_cursor = 0;
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, led_signal[led_cursor]);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, led_signal[led_cursor]);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, led_signal[led_cursor]);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, led_signal[led_cursor]);
  led_cursor++;
  if (led_cursor >= LED_SIGNAL_SIZE){
    led_cursor = 0;
  }
#endif
#if LED_DEMO_ALTERN
  /* ALTERNATING LED SIN */
  static uint32_t led_cursor = 0;
  static uint8_t curr_channel = TIM_CHANNEL_1;
  __HAL_TIM_SET_COMPARE(&htim4, curr_channel, led_signal[led_cursor]);
  led_cursor++;
  if (led_cursor >= LED_SIGNAL_SIZE){
    led_cursor = 0;
    curr_channel += 4;
    if (curr_channel > TIM_CHANNEL_4){
      curr_channel = TIM_CHANNEL_1;
    }   
  }
#endif
#if 0 && LED_DEMO_LATEST
  /* ALTERNATING LED SIN WITH SPEEDING UP ALTERNATION ON BUTTON PRESS
   */
  static uint8_t curr_channel = TIM_CHANNEL_1;
  static int16_t offset = 5;
  static bool offset_up = false;
  __HAL_TIM_SET_COMPARE(&htim4, curr_channel, led_signal[led_cursor]);
  led_cursor += offset;
  if (led_cursor >= LED_SIGNAL_SIZE){
    if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) || voices[0].amp > 0){
      offset_up = true;
      offset += (offset_up) ? 10 : -10;
    } else {
      offset_up = false;
      offset += (offset_up) ? 10 : -10;
    }

    if (offset >= 500){
      offset = 500;
    } 
    if (offset <= 5){
      offset = 5;
    }
  }

  if (led_cursor >= LED_SIGNAL_SIZE){
    led_cursor = 0;
    curr_channel += 4;
    if (curr_channel > TIM_CHANNEL_4){
      curr_channel = TIM_CHANNEL_1;
    }

  }

  led_speed = offset;
#endif
#if LED_DEMO_VOICES
  /* ADSR -> LEDs
   */
  int ch = PWM_CHANNEL_GREEN;
  for (int i = 0; i < NUM_VOICES; i++){
    VCA_t *v = &(voices[i]);
    float amp = v->amp;
    __HAL_TIM_SET_COMPARE(&htim4, ch, 999 * amp);
    ch += 4;
  }
#endif
}

