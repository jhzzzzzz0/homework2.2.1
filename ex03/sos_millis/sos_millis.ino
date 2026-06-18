const int ledPin = 2;

// 使用 unsigned long 防止时间溢出错误
const unsigned long shortOn = 200;    // S: 短闪时长
const unsigned long longOn = 600;     // O: 长闪时长 (通常是短的3倍)
const unsigned long offGap = 200;     // 字符内部间隔
const unsigned long pauseAfterSOS = 2000; // 单词间隔

unsigned long previousMillis = 0;
int step = 0; // 步骤计数器：0-17是闪烁，18是停顿
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  // 计算时间差，使用 unsigned long
  unsigned long elapsed = currentMillis - previousMillis;

  switch (step) {
    // --- S (短闪 x3) ---
    // 偶数步 (0, 2, 4) 代表灯亮
    case 0: case 2: case 4:
      ledState = HIGH;
      if (elapsed >= shortOn) {
        ledState = LOW; // 立即熄灭视觉更好，或者等到下一步
        previousMillis = currentMillis;
        step++;
      }
      break;

    // 奇数步 (1, 3, 5) 代表灯灭（间隔）
    case 1: case 3: case 5:
      ledState = LOW;
      if (elapsed >= offGap) {
        previousMillis = currentMillis;
        step++;
      }
      break;

    // --- O (长闪 x3) ---
    // 偶数步 (6, 8, 10) 代表灯亮
    case 6: case 8: case 10:
      ledState = HIGH;
      if (elapsed >= longOn) {
        ledState = LOW;
        previousMillis = currentMillis;
        step++;
      }
      break;

    // 奇数步 (7, 9, 11) 代表灯灭（间隔）
    case 7: case 9: case 11:
      ledState = LOW;
      if (elapsed >= offGap) {
        previousMillis = currentMillis;
        step++;
      }
      break;

    // --- S (短闪 x3) ---
    // 偶数步 (12, 14, 16) 代表灯亮
    case 12: case 14: case 16:
      ledState = HIGH;
      if (elapsed >= shortOn) {
        ledState = LOW;
        previousMillis = currentMillis;
        step++;
      }
      break;

    // 奇数步 (13, 15, 17) 代表灯灭（间隔）
    case 13: case 15: case 17:
      ledState = LOW;
      if (elapsed >= offGap) {
        previousMillis = currentMillis;
        step++;
      }
      break;

    // --- 结束停顿 ---
    case 18:
      ledState = LOW;
      if (elapsed >= pauseAfterSOS) {
        previousMillis = currentMillis;
        step = 0; // 重置回第一步
      }
      break;
  }

  digitalWrite(ledPin, ledState);
}