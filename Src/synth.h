
#define NUM_OSCILLATORS 4
#define PI 3.14159265358979323846
#define TAU (2.0 * PI)
#define BUF_SIZE 2048
#include "main.h"

extern uint16_t counters[NUM_OSCILLATORS];
extern int16_t signal[256];
extern int nsamples;
extern uint16_t i2s_buffer[BUF_SIZE];

void synth_init();
void play_note(uint8_t note, uint8_t velocity);
void synth_output();
void note_on(uint8_t key, uint8_t vel);
void note_off(uint8_t key);
void test_tone();
void make_sound();
void update_lfos();

void mixer();
