/*******************************************************************************
   Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
   Copyright (c) 2018 Terry Moore, MCCI

   Permission is hereby granted, free of charge, to anyone
   obtaining a copy of this document and accompanying files,
   to do whatever they want with them without any restriction,
   including, but not limited to, copying, modification and redistribution.
   NO WARRANTY OF ANY KIND IS PROVIDED.

   This example sends a valid LoRaWAN packet with payload "Hello,
   world!", using frequency and encryption settings matching those of
   the The Things Network. It's pre-configured for the Adafruit
   Feather M0 LoRa.

   This uses OTAA (Over-the-air activation), where where a DevEUI and
   application key is configured, which are used in an over-the-air
   activation procedure where a DevAddr and session keys are
   assigned/generated for use with all further communication.

   Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
   g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
   violated by this sketch when left running for longer)!

   To use this sketch, first register your application and device with
   the things network, to set or generate an AppEUI, DevEUI and AppKey.
   Multiple devices can use the same AppEUI, but each device has its own
   DevEUI and AppKey.

   Do not forget to define the radio type correctly in
   arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.

 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <ArduinoUniqueID.h>;

byte LORA_HTOI(char c) {
  return ((c <= '9') ? (c - '0') : ((c <= 'F') ? (c - 'A' + 10) : ((c <= 'f') ? (c - 'a' + 10) : (0))));
}

void _LORA_HEX_TO_DEVICE(char * address, char * hex, int len)
{
  for (int i = 0; i < len; i++)
    address[i] = ((LORA_HTOI(hex[2 * i]) << 4) + LORA_HTOI(hex[2 * i + 1]));
}

void os_getArtEui (u1_t* buf);
void os_getDevEui (u1_t* buf);
void os_getDevKey (u1_t* buf);
void do_send(osjob_t* j);
void onEvent (ev_t ev);

u1_t APPEUI[8];
u1_t APPKEY[16];

static boolean sleep = false;
static int _sleep_time = 60 * 8;
static byte send_confirm = 0;
#define SEND_CONFIRM 5

void os_getArtEui (u1_t* buf) {
  for (int i = 0; i < 8; i++) buf[i] = APPEUI[7 - i];
}

void os_getDevEui (u1_t* buf) {
  for (int i = 0; i < 8; i++) buf[i] = UniqueID8[7 - i];
}

void os_getDevKey (u1_t* buf) {
  memcpy(buf, APPKEY, 16);
}

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_UNUSED_PIN,
  .dio = {2, 6, 7}
};

static osjob_t sendjob;


void onEvent (ev_t ev) {
  sleep = false;

  if (LMIC_getSeqnoUp() == 1) LMIC_setAdrMode(0);

  switch (ev) {
    case EV_JOINED:
      u4_t netid = 0;
      devaddr_t devaddr = 0;
      u1_t nwkKey[16];
      u1_t artKey[16];
      LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
      LMIC_setLinkCheckMode(0);
      break;
    case EV_TXCOMPLETE:
      if (LMIC.txrxFlags & TXRX_ACK) {
        send_confirm = 0;
      }

      if (send_confirm >= SEND_CONFIRM) {
        lmic_rejoin();
      }

      if (LMIC.dataLen) {
        uint8_t port = 0;
        if (LMIC.txrxFlags & TXRX_PORT)
          port = LMIC.frame[LMIC.dataBeg - 1];

        message(LMIC.frame + LMIC.dataBeg, LMIC.dataLen , port);
      }
      sleep = true;
      break;
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    //Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    PayloadNow();
  }
}

void setup_lmic() {
  Serial.begin(115200);
  Serial.println(F("Starting"));
  UniqueIDdump(Serial);
  
  _LORA_HEX_TO_DEVICE(APPEUI, appEui, 8);
  _LORA_HEX_TO_DEVICE(APPKEY, appKey, 16);
  delay(1000);

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  LMIC_setClockError (MAX_CLOCK_ERROR * 10 / 100);

  LMIC_setAdrMode(0);

  LMIC_selectSubBand(1);
  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
}

void loop_lmic() {
  os_runloop_once();
}

boolean sleep_lmic() {
  if (sleep == true) {
    os_radio(RADIO_RST);
    SPI.end();
    sleep = false;
    return true;
  }
  return false;
}

void send_lmic() {
  os_setTimedCallback(&sendjob, os_getTime() + random(1000), do_send);
}

int sleep_time() {
  return _sleep_time;
}

void lmic_rejoin() {
  LMIC.devaddr = 0;
  LMIC_startJoining();
}


enum LoRa_Code {
  LORA_CODE_TIMER = 1,
  LORA_CODE_RESET = 90,
  LORA_CODE_RESET_ALL = 100
};

void(* resetFunc) (void) = 0;//declare reset function at address 0

void message(const uint8_t *payload, size_t size, uint8_t port)
{
  switch (port) {
    case LORA_CODE_TIMER:
      _sleep_time = payload[0];
      break;
    case LORA_CODE_RESET:
      LMIC_tryRejoin();
      break;
    case LORA_CODE_RESET_ALL:
      resetFunc();
      break;
  }
}
