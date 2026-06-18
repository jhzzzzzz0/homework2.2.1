// 定义LED引脚
const int ledPin = 2;

// 设置PWM属性
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

// 呼吸参数
const float PI_VAL = 3.14159265;
const int steps = 200;          // 一个完整呼吸周期的步数（越大越平滑）
const int delayTime = 10;       // 每步延时，控制呼吸速度

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
}

void loop() {
  // 一个完整的呼吸周期：0 → 2π
  for (int i = 0; i <= steps; i++) {
    // 将 i 映射到 0 ~ 2π 弧度
    float theta = (float)i / steps * 2 * PI_VAL;
    
    // 使用 (1 - cosθ) / 2 将正弦波映射到 0~1 范围
    // 这样亮度变化就是：慢→快→慢→快→慢（自然的呼吸节奏）
    float brightness = (1.0 - cos(theta)) / 2.0;
    
    // 映射到 PWM 占空比 0~255
    int dutyCycle = (int)(brightness * 255);
    
    ledcWrite(ledPin, dutyCycle);
    delay(delayTime);
  }
  
  Serial.println("Breathing cycle completed");
}
