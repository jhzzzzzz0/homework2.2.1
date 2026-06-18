// 定义LED引脚
const int ledPin = 2;

// 记录上一次LED状态改变的时间
unsigned long previousMillis = 0;

const unsigned long interval = 500; // 1Hz = 1000ms周期，半周期为500ms

// LED当前状态
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 获取当前系统运行时间（毫秒）
  unsigned long currentMillis = millis();

  // 检查时间差是否达到设定间隔
  if (currentMillis - previousMillis >= interval) {
    // 更新时间戳
    previousMillis = currentMillis;

    // 翻转LED状态
    ledState = !ledState;

    // 写入新状态
    digitalWrite(ledPin, ledState);
  }

}