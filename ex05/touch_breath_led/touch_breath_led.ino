#define TOUCH_PIN 4
#define LED_PIN 2
#define TOUCH_THRESHOLD 550//阈值

// PWM参数
const int pwmFreq = 5000;//频率
const int pwmRes = 8;//分辨率

int speedGear = 1;    // 速度档位 
bool touchFlag = false;

// 触摸中断服务函数
void touchISR() {
  touchFlag = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // 绑定触摸中断
  touchAttachInterrupt(TOUCH_PIN, touchISR, TOUCH_THRESHOLD);
  // LEDC PWM绑定引脚
  ledcAttach(LED_PIN, pwmFreq, pwmRes);
  Serial.println("1");
}

void loop() {
  // 检测触摸事件，循环切换档位
  if (touchFlag) {
    touchFlag = false;
    speedGear++;
    if (speedGear > 3) speedGear = 1;
    Serial.println(speedGear);
  }

  // 延时，控制呼吸速度
  int stepDelay;
  switch (speedGear) {
    case 1: stepDelay = 7; break;
    case 2: stepDelay = 3; break;
    case 3: stepDelay = 1; break;
    default: stepDelay = 10;
  }

  // 呼吸灯：渐亮
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_PIN, duty);
    delay(stepDelay);
  }
  // 呼吸灯：渐暗
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED_PIN, duty);
    delay(stepDelay);
  }
}