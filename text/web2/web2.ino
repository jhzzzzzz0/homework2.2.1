#include <WiFi.h>
#include <WebServer.h>

/*const char* ssid = "xyz";
const char* password = "zzzzzz111";*/
const char* ap_ssid = "ESP32-LAB017";
const char* ap_pass = "12345678"; // 至少8位

const int LED_PIN = 2; // D2 常见为 GPIO2

WebServer server(80);

String makePage() {
  String state = digitalRead(LED_PIN) ? "ON" : "OFF";
  String ledColor = digitalRead(LED_PIN) ? "#4CAF50" : "#f44336";
  String ledText = digitalRead(LED_PIN) ? "已点亮" : "已熄灭";
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED 控制面板</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
    }
    
    .container {
      background: white;
      border-radius: 20px;
      padding: 40px 50px;
      box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
      text-align: center;
      max-width: 400px;
      width: 90%;
    }
    
    h1 {
      color: #333;
      margin-bottom: 10px;
      font-size: 24px;
    }
    
    .subtitle {
      color: #888;
      font-size: 14px;
      margin-bottom: 30px;
    }
    
    .led-indicator {
      display: inline-flex;
      align-items: center;
      gap: 10px;
      margin: 20px 0 40px;
      padding: 15px 30px;
      background: #f8f9fa;
      border-radius: 50px;
    }
    
    .led-dot {
      width: 16px;
      height: 16px;
      border-radius: 50%;
      background-color: )rawliteral" + ledColor + R"rawliteral(;
      box-shadow: 0 0 10px )rawliteral" + ledColor + R"rawliteral(;
      transition: all 0.3s ease;
    }
    
    .led-text {
      font-size: 18px;
      font-weight: 500;
      color: #333;
    }
    
    .btn-group {
      display: flex;
      gap: 20px;
      justify-content: center;
    }
    
    .btn {
      padding: 15px 30px;
      border: none;
      border-radius: 10px;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s ease;
      text-decoration: none;
      display: inline-block;
      min-width: 120px;
    }
    
    .btn-on {
      background: linear-gradient(135deg, #4CAF50, #45a049);
      color: white;
      box-shadow: 0 4px 15px rgba(76, 175, 80, 0.4);
    }
    
    .btn-on:hover {
      transform: translateY(-3px);
      box-shadow: 0 6px 20px rgba(76, 175, 80, 0.5);
    }
    
    .btn-off {
      background: linear-gradient(135deg, #f44336, #e53935);
      color: white;
      box-shadow: 0 4px 15px rgba(244, 67, 54, 0.4);
    }
    
    .btn-off:hover {
      transform: translateY(-3px);
      box-shadow: 0 6px 20px rgba(244, 67, 54, 0.5);
    }
    
    .footer {
      margin-top: 40px;
      color: #aaa;
      font-size: 12px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>💡 LED 控制面板</h1>
    <p class="subtitle">实验二 · 按钮控制 LED</p>
    
    <div class="led-indicator">
      <div class="led-dot"></div>
      <span class="led-text">当前状态：<b>)rawliteral" + ledText + R"rawliteral(</b></span>
    </div>
    
    <div class="btn-group">
      <a href="/on" class="btn btn-on">点亮 LED</a>
      <a href="/off" class="btn btn-off">熄灭 LED</a>
    </div>
    
    <div class="footer">
      ESP32 Web Server Control
    </div>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleOn() {
  digitalWrite(LED_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  WiFi.mode(WIFI_AP);
WiFi.softAP(ap_ssid, ap_pass);
Serial.println("AP已开启");
Serial.print("AP IP: ");
Serial.println(WiFi.softAPIP());
  /*WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());*/

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
}

void loop() {
  server.handleClient();
}