#include "RCSwitch.h"

RCSwitch mySwitch = RCSwitch();

/*
 *           ---------
 * _________|  4680   |__________| DATA
 *   8640                 1534
 *     0         1         2         3
*/
void setup() {
  Serial.begin(74880);
  
  mySwitch.enableReceive(14);
  //mySwitch.enableTransmit(5);
}


void loop() {
  capture();
  //test();
}

/*
 * Ch 9 Up  : 202761677073 
 * Ch 9 Stop: 202761677141
 * Ch 9 Down: 202761677107 
 * 
   Ch 14 Down 1: 202761678387 / 0010111100110101100010011010111000110011
   Ch 14 Down 2: 202761678396 / 0010111100110101100010011010111000111100
   Ch 14 Stop:   202761678421 / 0010111100110101100010011010111001010101

   Ch 15 Up   202761678609, 202761678622
   Ch 15 Stop 202761678677
   Ch 15 Down 202761678643, 202761678652
 */

static const uint64_t CH_15_UP_1 = 202761678609ull;
static const uint64_t CH_15_UP_2 = 202761678622ull;

void test() {
  uint32_t time = millis();
  Serial.print("sending...");
  
  mySwitch.send(CH_15_UP_1, 40);
  mySwitch.send(CH_15_UP_2, 40);
  mySwitch.send(CH_15_UP_2, 40);
  
  Serial.println("done.");
  while(true) {
    yield();
  }
}

void capture() {
  if (mySwitch.available()) {
    
    Serial.print("Received ");
    uint64_t xx = mySwitch.getReceivedValue()/1000000000ULL;
    if (xx >0) Serial.print((long)xx);
    Serial.print((long)(mySwitch.getReceivedValue() - xx*1000000000));
    Serial.print(" / ");
    char bits[41];
    memset(bits, 0, 41);
    to40Bit(bits, mySwitch.getReceivedValue());
    Serial.println(bits);
    /*Serial.println("Raw data");
    unsigned int* raw = mySwitch.getReceivedRawdata();
    for(int i = 0; i < RCSWITCH_MAX_CHANGES; i++) {
      Serial.printf("%d, ", raw[i]);
    }
    Serial.println("\n");*/
    mySwitch.resetAvailable();
  }
}

void to40Bit(char* buf, uint64_t n) {
  int i = 39;
  while(n > 0) {
    buf[i] = '0' + (n % 2);
    n /= 2;
    i--;
  }
  while(i >= 0) {
    buf[i] = '0';
    i--;
  }
}
