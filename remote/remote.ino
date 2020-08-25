/* [ALDOMO] Blind Controller
 * 
 * PULSE LENGTH (L) 360us
 * 
 * SYNC
 *           ---------
 * _________|  4680   |__________| DATA
 *   8640                 1534
 *     0         1         2         3
 *     
 * HIGH
 *  __________  1L
 * |  2L      |____
 * 
 * LOW
 *  ______    2L
 * |  1L  |_________
*/
#include <ESP8266WiFi.h>


#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15
#define D9  3
#define D10 1

#define DATA_PIN     5      // D1
#define CODE_LENGTH  40     // 40 bits code
#define PULSE_LENGTH 360    // 360 us
#define TX_REPEAT    3     // transmit 3 times per send

#define UP_PIN    D2 
#define STOP_PIN  D5 
#define DOWN_PIN  D3
#define CLOSE_PIN D6

#define CHANNEL  15

enum button_t {
  UP, STOP, DOWN, CLOSE
};

struct signal_t {
  uint64_t up;
  uint64_t upClear;
  uint64_t stop;
  uint64_t down;
  uint64_t downClear;
}

static const SIGNALS[16] PROGMEM = {
  // CH 0
  {202761674769, 202761674782, 202761674837, 202761674803, 202761674812},
  // CH 1
  {202761675025, 202761675038, 202761675093, 202761675059, 202761675068},
  // CH 2
  {202761675281, 202761675294, 202761675349, 202761675315, 202761675324},
  // CH 3
  {202761675537, 202761675550, 202761675605, 202761675571, 202761675580},
  // CH 4
  {202761675793, 202761675806, 202761675861, 202761675827, 202761675836},
  // CH 5
  {202761676049, 202761676062, 202761676117, 202761676083, 202761676092},
  // CH 6
  {202761676305, 202761676318, 202761676373, 202761676339, 202761676348},
  // CH 7
  {202761676561, 202761676574, 202761676629, 202761676595, 202761676604},
  // CH 8
  {202761676817, 202761676830, 202761676885, 202761676851, 202761676860},
  // CH 9
  {202761677073, 202761677086, 202761677141, 202761677107, 202761677116},
  // CH 10
  {202761677329, 202761677342, 202761677397, 202761677363, 202761677372},
  // CH 11
  {202761677585, 202761677598, 202761677653, 202761677619, 202761677628},
  // CH 12
  {202761677841, 202761677854, 202761677909, 202761677875, 202761677884},
  // CH 13
  {202761678097, 202761678110, 202761678165, 202761678131, 202761678140},
  // CH 14
  {202761678353, 202761678366, 202761678421, 202761678387, 202761678396},
  // CH 15
  {202761678609, 202761678622, 202761678677, 202761678643, 202761678652},
};

bool isUp, isDown, isStop, isClose;

void preinit() {
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup() {
  Serial.begin(74880);
  Serial.println();
  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(DATA_PIN, LOW);

  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(CLOSE_PIN, INPUT_PULLUP);
}

void loop() {
  if(isUp) {
    Serial.println("UP");
    sendCode(button_t::UP);
    isUp = false;
  } else if(isStop) {
    Serial.println("STOP");
    sendCode(button_t::STOP);
    isStop = false;
  } else if(isDown) {
    Serial.println("DOWN");
    sendCode(button_t::DOWN);
    isDown = false;
  } else if(isClose) {
    Serial.println("CLOSE");
    sendCode(button_t::CLOSE);
    isClose = false;
  }
  Serial.println("Starting light sleep");
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(UP_PIN), GPIO_PIN_INTR_LOLEVEL);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(DOWN_PIN), GPIO_PIN_INTR_LOLEVEL);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(STOP_PIN), GPIO_PIN_INTR_LOLEVEL);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(CLOSE_PIN), GPIO_PIN_INTR_LOLEVEL);
  wifi_fpm_set_wakeup_cb(wakeupCallback);
  wifi_fpm_open();
  Serial.flush();
  wifi_fpm_do_sleep(0xFFFFFFF);
  delay(100);
}

void sendCode(button_t t) {
  signal_t sig = SIGNALS[CHANNEL];
  switch(t) {
    case button_t::UP:
      while(digitalRead(UP_PIN) == LOW) {
        send(sig.up);
      }
      //send(sig.upClear);
      break;
    case button_t::STOP:
      while(digitalRead(STOP_PIN) == LOW) {
        send(sig.stop);
      }
      break;
    case button_t::DOWN:
      while(digitalRead(DOWN_PIN) == LOW) {
        send(sig.down);
      }
      //send(sig.downClear);
      break;
    case button_t::CLOSE:
      int start = millis();
      while(millis() - start < 3000) {
        send(sig.stop);
      }
      break;
  }
}

void wakeupCallback() {
  wifi_fpm_close();
  isUp = digitalRead(UP_PIN) == LOW;
  isStop = digitalRead(STOP_PIN) == LOW;
  isDown = digitalRead(DOWN_PIN) == LOW;
  isClose = digitalRead(CLOSE_PIN) == LOW;
}

void send(uint64_t code) {
  for (int nRepeat = 0; nRepeat < TX_REPEAT; nRepeat++) {
    for (int i = CODE_LENGTH - 1; i >= 0; i--) {
      code & (1ull << i) ? transmit(HIGH) : transmit(LOW);
    }
    transmitSync();
  }
  digitalWrite(DATA_PIN, LOW);
}

void transmit(int level) {
  digitalWrite(DATA_PIN, HIGH);
  delayMicroseconds(level == HIGH ? 2 * PULSE_LENGTH : PULSE_LENGTH);
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(level == HIGH ? PULSE_LENGTH : 2 * PULSE_LENGTH);
}

void transmitSync() {
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(8640);
  digitalWrite(DATA_PIN, HIGH);
  delayMicroseconds(4680);
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(1530);
}
