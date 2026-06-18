// 定义两个LED引脚
const int ledPin1 = 2;   // GPIO 2
const int ledPin2 = 4;   // GPIO 4

void setup() {
  Serial.begin(115200);
  
  // 将两个LED引脚都设置为输出模式
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

void loop() {
  // LED1亮，LED2灭
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, LOW);
  Serial.println("LED1 ON, LED2 OFF");
  delay(1000);
  
  // LED1灭，LED2亮
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  Serial.println("LED1 OFF, LED2 ON");
  delay(1000);
}