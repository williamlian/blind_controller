#include "signal.h"

bool sendCode(int chan, const char* t) {
  if(chan < 0 || chan > 15 || t == 0) {
    return false;
  }
  signal_t sig = SIGNALS[chan];
  int start = millis();
  if(strcmp(t, "up") == 0) {
      while(millis() - start < BTN_PRESS_TIME) {
        send(sig.up);
        yield();
      }
      return true;
  } else if(strcmp(t, "stop") == 0) {
      while(millis() - start < BTN_PRESS_TIME) {
        send(sig.stop);
        yield();
      }
      return true;
  } else if(strcmp(t, "down") == 0) {
      while(millis() - start < BTN_PRESS_TIME) {
        send(sig.down);
        yield();
      }
      return true;
  } else if(strcmp(t, "close") == 0) {
      while(millis() - start < 3000) {
        send(sig.stop);
        yield();
      }
      return true;
  } else {
    return false;
  }
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
