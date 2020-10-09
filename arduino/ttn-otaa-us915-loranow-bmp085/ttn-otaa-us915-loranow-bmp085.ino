//----------------------------------------//
//  ttn-otaa-us915-loranow-bmp085.ino
//
//  created 03 04 2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

#define VCC 8
#define VCC_ON pinMode(VCC, OUTPUT); digitalWrite(VCC, LOW);
#define VCC_OFF pinMode(VCC, OUTPUT); digitalWrite(VCC, HIGH);

void setup() {
  VCC_ON
  setup_lmic();
}

void loop() {
  loop_lmic();
  
  if (sleep_lmic()){
    delay(10);
    VCC_OFF
    system_sleep_x(sleep_time());
    VCC_ON;
    send_lmic();
  }
}
