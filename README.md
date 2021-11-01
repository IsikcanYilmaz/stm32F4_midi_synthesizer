# STM32F4_midi_synthesizer
messing around with audio synthesis on an STM32F4 disco board. 

so far it can:
- provide a little command shell to talk to the device.
- animate onboard LEDs.
- take in midi signals from a 5pin midi connector.
- wavetable synthesis, sin, square, triangle waves.
- ADSR envelope.
- 4 sounds sound okay. needs optimizing.

todo:
- optimization. too many float ops in the code. not necessary.
- midi knobs.
- audio effects.
