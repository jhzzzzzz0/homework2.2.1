const int ledPin = 2;

// 定义动作序列数组
const unsigned long sequenceDurations[] = {
  200, 200,   // S: 亮200ms, 灭200ms
  200, 200,   // S: 亮200ms, 灭200ms
  200, 200,   // S: 亮200ms, 灭200ms (S结束)

  600, 200,   // O: 亮600ms, 灭200ms
  600, 200,   // O: 亮600ms, 灭200ms
  600, 200,   // O: 亮600ms, 灭200ms (O结束)

  200, 200,   // S: 亮200ms, 灭200ms
  200, 200,   // S: 亮200ms, 灭200ms
  200, 2000   // S: 亮200ms, 灭2000ms (一轮结束的大停顿)
};

// 获取序列的总步数
const int totalSteps = sizeof(sequenceDurations) / sizeof(sequenceDurations[0]);

unsigned long previousMillis = 0;
int currentStep = 0; // 当前执行到数组的第几个元素

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - previousMillis;

  // 判断当前步的时间是否到了
  if (elapsed >= sequenceDurations[currentStep]) {
    // 时间到了，进入下一步
    previousMillis = currentMillis;
    currentStep++;

    // 如果超出了数组范围，说明一轮播放完毕，重置回第0步
    if (currentStep >= totalSteps) {
      currentStep = 0;
    }
  }

  // 根据当前步数的奇偶性决定灯亮还是灭(偶亮奇灭)
  if (currentStep % 2 == 0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}