// 定义RGB LED引脚
const int redPin   = 2;
const int greenPin = 4;
const int bluePin  = 16;

// 设置PWM属性
const int freq = 5000;          // 频率 5000Hz
const int resolution = 8;       // 分辨率 8位 (0-255)

// 彩虹参数
const float PI_VAL = 3.14159265;
const int steps = 200;          // 一个完整周期的步数
const int delayTime = 15;       // 每步延时，控制渐变速度

void setup() {
  Serial.begin(115200);
  
  // 绑定三个引脚到PWM
  ledcAttach(redPin,   freq, resolution);
  ledcAttach(greenPin, freq, resolution);
  ledcAttach(bluePin,  freq, resolution);
}

void loop() {
  // 一个完整的彩虹周期：0 → 2π
  for (int i = 0; i <= steps; i++) {
    // 将 i 映射到 0 ~ 2π 弧度
    float theta = (float)i / steps * 2 * PI_VAL;
    
    // 三个通道分别偏移 0°、120°、240°
    float r = (1.0 + sin(theta)) / 2.0;
    float g = (1.0 + sin(theta + 2 * PI_VAL / 3)) / 2.0;
    float b = (1.0 + sin(theta + 4 * PI_VAL / 3)) / 2.0;
    
    // 映射到 PWM 占空比 0~255 并写入
    ledcWrite(redPin,   (int)(r * 255));
    ledcWrite(greenPin, (int)(g * 255));
    ledcWrite(bluePin,  (int)(b * 255));
    
    delay(delayTime);
  }
  
  Serial.println("Rainbow cycle completed");
}