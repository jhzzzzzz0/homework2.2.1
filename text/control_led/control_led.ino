#include <WiFi.h>
#include <WebServer.h>

// AP热点配置
const char* ap_ssid = "ESP32-LAB017";
const char* ap_pass = "12345678";

const int LED_PIN = 2; // GPIO2 PWM引脚
WebServer server(80);

// 全局变量存储当前亮度 0~255
int ledBrightness = 0;

String makePage() {
  // 根据当前亮度计算状态显示
  String stateTip = (ledBrightness > 0) ? "已点亮" : "已熄灭";
  String ledColor = (ledBrightness > 0) ? "#4CAF50" : "#f44336";
  String shadowStr = (ledBrightness > 0) ? ("0 0 " + String(ledBrightness/8) + "px " + ledColor) : "none";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED 亮度控制面板</title>
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
      max-width: 450px;
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
      margin: 20px 0 20px;
      padding: 15px 30px;
      background: #f8f9fa;
      border-radius: 50px;
    }
    
    .led-dot {
      width: 16px;
      height: 16px;
      border-radius: 50%;
      background-color: )rawliteral" + ledColor + R"rawliteral(;
      box-shadow: )rawliteral" + shadowStr + R"rawliteral(;
      transition: all 0.3s ease;
    }
    
    .led-text {
      font-size: 18px;
      font-weight: 500;
      color: #333;
    }

    .brightness-info{
      margin:10px 0 30px;
      font-size:16px;
      color:#555;
    }
    
    .input-box{
      margin:30px 0;
    }
    input[type="number"]{
      width:180px;
      padding:14px 16px;
      font-size:16px;
      border:2px solid #ddd;
      border-radius:10px;
      outline:none;
      transition:0.3s;
    }
    input[type="number"]:focus{
      border-color:#667eea;
    }
    .tip-text{
      font-size:13px;
      color:#999;
      margin-top:8px;
    }
    
    .btn-group {
      display: flex;
      gap: 20px;
      justify-content: center;
      margin-top:20px;
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
    
    .btn-submit {
      background: linear-gradient(135deg, #2196F3, #1976D2);
      color: white;
      box-shadow: 0 4px 15px rgba(33, 150, 243, 0.4);
    }
    
    .btn-submit:hover {
      transform: translateY(-3px);
      box-shadow: 0 6px 20px rgba(33, 150, 243, 0.5);
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
    <h1>💡 LED 亮度控制面板</h1>
    
    
    <div class="led-indicator">
      <div class="led-dot"></div>
      <span class="led-text">当前状态：<b>)rawliteral" + stateTip + R"rawliteral(</b></span>
    </div>
    <div class="brightness-info">当前亮度值：<b>)rawliteral" + String(ledBrightness) + R"rawliteral(</b></div>

    <!-- 亮度输入表单，提交到 /set -->
    <div class="input-box">
      <form action="/set" method="GET">
        <input type="number" name="val" min="0" max="255" value=")rawliteral" + String(ledBrightness) + R"rawliteral(" required>
        <div class="tip-text">输入范围：0 ~ 255（0熄灭，255最亮）</div>
        <div class="btn-group" style="margin-top:20px">
          <button class="btn btn-submit" type="submit">提交设置亮度</button>
          <a href="/off" class="btn btn-off">一键熄灭</a>
        </div>
      </form>
    </div>
    
    <div class="footer">
      ESP32 PWM Brightness Web Control
    </div>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

// 主页页面
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 一键熄灭LED
void handleOff() {
  ledBrightness = 0;
  analogWrite(LED_PIN, ledBrightness);
  server.sendHeader("Location", "/");
  server.send(303);
}

// 接收输入的亮度数值
void handleSetBright() {
  // 获取表单传入的val参数
  if (server.hasArg("val")) {
    int inputVal = server.arg("val").toInt();
    // 限制数值范围 0~255
    if(inputVal >= 0 && inputVal <= 255){
      ledBrightness = inputVal;
    }else{
      ledBrightness = 0; // 非法数值默认熄灭
    }
    analogWrite(LED_PIN, ledBrightness);
  }
  // 跳转回主页
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  ledBrightness = 0;
  analogWrite(LED_PIN, ledBrightness);
  
  // 开启AP热点
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("AP热点已开启");
  Serial.print("热点名称：");
  Serial.println(ap_ssid);
  Serial.print("热点IP地址：");
  Serial.println(WiFi.softAPIP());

  // 绑定网页路由
  server.on("/", handleRoot);
  server.on("/off", handleOff);
  server.on("/set", handleSetBright); // 调光提交接口
  server.begin();
}

void loop() {
  server.handleClient();
}
