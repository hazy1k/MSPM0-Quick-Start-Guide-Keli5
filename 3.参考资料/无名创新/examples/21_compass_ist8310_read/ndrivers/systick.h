#ifndef SYS_TICK_H
#define SYS_TICK_H

#include <stdint.h>


typedef struct
{
  volatile float last_time;
  volatile float current_time;
  volatile float period;
  volatile uint16_t period_int;//??¦Ëms
}systime;




uint32_t micros(void);
uint32_t millis(void);
void Delay_Ms(uint32_t x);
void delay_ms(uint32_t x);
void delay_us(uint32_t x);
void Delay_Us(uint32_t x);


void 	get_systime(systime *sys);
float get_systime_ms(void);
uint32_t get_systick_ms(void);



#endif /* SYS_TICK_H */







