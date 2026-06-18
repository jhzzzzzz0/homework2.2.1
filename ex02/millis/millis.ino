const int ledPin = 2; // 板载LED引脚
unsigned long previousMillis = 0;
const long interval = 500; // 1Hz对应半周期500ms
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}
