
#define NUM_OSCILLATORS 4
#include "main.h"

extern uint16_t counters[NUM_OSCILLATORS];

void synth_init();
void play_note(uint8_t note, uint8_t velocity);
void synth_output();
void note_on(uint8_t key, uint8_t vel);
void note_off(uint8_t key);
void test_tone();
void make_sound();
void update_lfos();
