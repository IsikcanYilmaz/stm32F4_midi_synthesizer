
#include "midi.h"
#include "gpio.h"
#include "led.h"
#include "usart.h"
#include "synth.h"
#include "dma.h"
#include "cmd_shell.h"

// https://www.nyu.edu/classes/bello/FMT_files/9_MIDI_code.pdf // 
// https://users.cs.cf.ac.uk/Dave.Marshall/Multimedia/node158.html //
//

MIDIPacket_t midi_packet_buffer[MIDI_BUFFER_SIZE]; // TODO make this better // implement general purpose fifo
volatile uint16_t midi_packet_buffer_head = 0;
volatile uint16_t midi_packet_buffer_tail = 0;
volatile uint16_t midi_usart_buffer_index = 0;

volatile uint8_t midi_dma_buffer[MIDI_DMA_BUFFER_SIZE_BYTES];

void midi_init(){
#if !CNFG_MIDI_OVER_USB
  HAL_UART_Receive_DMA(&huart3, (uint8_t *) &midi_dma_buffer, MIDI_DMA_BUFFER_SIZE_BYTES);
#endif
}

// puts packet in fifo. increments tail
void enqueue_midi_packet(MIDIPacket_t *p){
  midi_packet_buffer[midi_packet_buffer_tail] = *p;
  midi_packet_buffer_tail++;
  midi_packet_buffer_tail = midi_packet_buffer_tail % MIDI_BUFFER_SIZE;
}

// returns address to packet at head of the buffer. increments head.
MIDIPacket_t* dequeue_midi_packet(){
  MIDIPacket_t *ret = &midi_packet_buffer[midi_packet_buffer_head];
  midi_packet_buffer_head++;
  midi_packet_buffer_head = midi_packet_buffer_head % MIDI_BUFFER_SIZE;
  return ret;
}

// receive_midi_packet() is called on USART3 global interrupt. 
// handles the reception of midi packet. enqueues it.
void receive_midi_packet(uint8_t data){
}

// This is to get called by the loop in main.c. 
void midi_update(){
#if MIDI_CNFG_USER_BUTTON_DEMO // user button acts as a midi button press
  static bool pressed = false;
  if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){
    if (pressed == false){ // BUTTON PRESS EVENT
      pressed = true;
      inject_midi_packet(61, true); 
    }
  } else {
    if (pressed == true){ // BUTTON RELEASE EVENT
      pressed = false;
      inject_midi_packet(61, false); 
    }
  }
  while(midi_packet_buffer_head != midi_packet_buffer_tail){
    process_midi_packet(dequeue_midi_packet());
  }
#endif

  static uint16_t lastIndex = 0;
  uint16_t bytesSinceLastIndex = MIDI_DMA_BUFFER_SIZE_BYTES - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);

  // Special case where DMA Buffer rolls over. In that case, get byte by byte from where we left off,
  // until where the dma buffer counter is at. I expect to handle only one packet in this block. if 
  // there are more, that may be a problem, i.e. line 92
  if (bytesSinceLastIndex < lastIndex){ // DMA BUFFER ROLLED OVER
    print("DMA BUFFER ROLLED OVER. bytesSinceLastIndex=%d lastIndex=%d/%d\n", bytesSinceLastIndex, lastIndex, MIDI_DMA_BUFFER_SIZE_BYTES);
    MIDIPacket_t p;
    uint8_t i = 0;
    uint16_t lastByteIndex = lastIndex;
    do {
      uint8_t *p_bytes = (uint8_t *) &p;
      if (i == 3){
        print("DMA BUFFER ROLL OVER TOO LARGE\n");
        process_midi_packet(&p);
        i = 0;
      }
      p_bytes[i] = midi_dma_buffer[lastByteIndex];
      i++;
      lastByteIndex++;
      if (lastByteIndex == MIDI_DMA_BUFFER_SIZE_BYTES){
        lastByteIndex = 0;
      }
    } while(lastByteIndex != bytesSinceLastIndex);
    lastIndex = lastByteIndex;
    process_midi_packet(&p);
    return;
  } // DMA BUFFER ROLL OVER HANDLING

  // Process queued packets every time this function gets called.
  int queuedPackets = abs(lastIndex - bytesSinceLastIndex) / MIDI_PACKET_SIZE;
  if (queuedPackets){ 
    // While last index is not the head of the circular buff (minus all bytes of an incomplete midi packet)
    while(lastIndex != (bytesSinceLastIndex - (bytesSinceLastIndex % MIDI_PACKET_SIZE))){
      MIDIPacket_t *p = (MIDIPacket_t *) (&midi_dma_buffer[lastIndex]);
      process_midi_packet(p);
      lastIndex += MIDI_PACKET_SIZE;
      if (lastIndex >= MIDI_DMA_BUFFER_SIZE_BYTES){
        lastIndex = 0;
        print("%s:%d lastIndex >= DMA_BUFFER_SIZE_BYTES\n", __FILE__, __LINE__);
        //break;
      }
    }
  }
}

void process_midi_packet(MIDIPacket_t *p){
  uint8_t key, vel;
  switch(p->status_byte & 0xf0){
    case NOTE_OFF:
      key = p->data_byte1;
      vel = p->data_byte2;
      note_off(key);
      break;
    case NOTE_ON:
      key = p->data_byte1;
      vel = p->data_byte2;
      note_on(key, vel);
      break;
    case POLY_KEY_PRESSURE:
      return;
    case PITCH_BEND:
      return;
    default:
      return;
  }
  print("%s PROCESSING MIDI PACKET %x %x %x\nkey:%d vel:%d", __FUNCTION__, p->status_byte, p->data_byte1, p->data_byte2, key, vel);
}

void inject_midi_packet(uint16_t midiNum, bool noteOn){
  MIDIPacket_t p;
  uint8_t key = 49; // THIS MAY BE WRONG. MAY HAVE TO -20 FROM KEY VALUES
  uint8_t vel = 100;
  if (noteOn){
    p.data_byte1 = key;
    p.data_byte2 = vel;
    p.status_byte = NOTE_ON;
  } else {
    p.data_byte2 = key;
    p.data_byte1 = vel;
    p.status_byte = NOTE_OFF;
  }
  enqueue_midi_packet(&p);
}


