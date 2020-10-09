//----------------------------------------//
//  Payload.ino
//
//  created 10 02 2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

#include <CayenneLPP.h>

void PayloadNow()//(Stream &out)
{
  CayenneLPP lpp(64);
  lpp.reset();
  lpp.addAnalogInput(4, readVcc() / 1000.0);

  if (bmp180_setup()) {
    lpp.addTemperature(1, bmpTemp());
    lpp.addBarometricPressure(2, bmpPres());
  }

  boolean confirm = (random(100) == 1);
  // Confirm auto rejoin
  if (confirm) send_confirm++;

  LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), confirm);
}

//----------------------------------------//
// readVcc
//----------------------------------------//

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
