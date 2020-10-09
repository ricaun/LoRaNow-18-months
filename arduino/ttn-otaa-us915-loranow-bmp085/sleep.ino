//----------------------------------------//
//  sleep.ino
//
//  created 10 02 2019
//  by Luiz Henrique Cassettari
//----------------------------------------//


#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

// Utility macros
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

void system_sleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  adc_disable();
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  sleep_disable();
  adc_enable();
  sei();
}

void system_sleep_x(int x) {
  while(x > 0) {
    if (x > 8) {
      system_sleep_8s();
      x-=8;
    }
    else if (x >= 1) {
      system_sleep_1s();
      x-=1;
    }
  }
}

void system_sleep_8s(){
  wdt_enable(WDTO_8S);
  WDT_on();
  system_sleep();
}

void system_sleep_1s(){
  wdt_enable(WDTO_1S);
  WDT_on();
  system_sleep();
}

void system_sleep_500ms(){
  wdt_enable(WDTO_500MS);
  WDT_on();
  system_sleep();
}

void system_sleep_250ms(){
  wdt_enable(WDTO_250MS);
  WDT_on();
  system_sleep();
}

void WDT_on() {
  WDTCSR |= (1 << WDIE);
}

void WDT_off(void)
{
  //MCUSR = 0x00;
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0x00;
}

ISR (WDT_vect)
{
  // WDIE & WDIF is cleared in hardware upon entering this ISR
  wdt_disable();
  WDT_off();
}
