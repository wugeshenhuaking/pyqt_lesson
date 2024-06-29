#ifndef __APP_RELAY_H__
#define __APP_RELAY_H__

#include <stdint.h>
#include <stdio.h>

typedef union{
  uint16_t channel0:1;
  uint16_t channel1:1;
  uint16_t channel2:1;
  uint16_t channel3:1;
  uint16_t channel4:1;
  uint16_t channel5:1;
  uint16_t channel6:1;
  uint16_t channel7:1;
  uint16_t channel8:1;
  uint16_t channel9:1;
  uint16_t channel10:1;
  uint16_t channel11:1;
  uint16_t channel12:1;
  uint16_t channel13:1;
  uint16_t channel14:1;
  uint16_t channel15:1;
}relay_state_u;

typedef struct{
  uint8_t device_channel;
  relay_state_u relay_state;
}app_relay_t;

#endif
