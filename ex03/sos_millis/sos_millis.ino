const int ledPin = 2;

// 时序参数（单位：ms）
const int shortOn = 200;
const int longOn = 500;
const int offGap = 200;
const int pauseAfterSOS = 2000;

unsigned long previousMillis = 0;
int step = 0; // 0-8: SOS序列, 9: 停顿
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - previousMillis;

  switch (step) {
    // 3次短闪
    case 0:
    case 2:
    case 4:
      ledState = HIGH;
      if (elapsed >= shortOn) {
        ledState = LOW;
        previousMillis = currentMillis;
        step++;
      }
      break;
    case 1:
    case 3:
    case 5:
      ledState = LOW;
      if (elapsed >= offGap) {
        previousMillis = currentMillis;
        step++;
      }
      break;

    // 3次长闪
    case 6:
    case 8:
    case 10:
      ledState = HIGH;
      if (elapsed >= longOn) {
        ledState = LOW;
        previousMillis = currentMillis;
        step++;
      }
      break;
    case 7:
    case 9:
    case 11:
      ledState = LOW;
      if (elapsed >= offGap) {
        previousMillis = currentMillis;
        step++;
      }
      break;

    // 后3次短闪
    case 12:
    case 14:
    case 16:
      ledState = HIGH;
      if (elapsed >= shortOn) {
        ledState = LOW;
        previousMillis = currentMillis;
        step++;
      }
      break;
    case 13:
    case 15:
    case 17:
      ledState = LOW;
      if (elapsed >= offGap) {
        previousMillis = currentMillis;
        step++;
      }
      break;

    // 结束停顿
    case 18:
      ledState = LOW;
      if (elapsed >= pauseAfterSOS) {
        previousMillis = currentMillis;
        step = 0; // 循环
      }
      break;
  }

  digitalWrite(ledPin, ledState);
}