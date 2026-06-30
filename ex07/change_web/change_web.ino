#include <WiFi.h>
#include <WebServer.h>

// WiFi配置
const char* ssid = "xyz";
const char* password = "zzzzzz111";
// 硬件与PWM参数
const int LED_PIN = 2;
const int PWM_FREQ = 5000;
const int PWM_RES = 8; // 0~255亮度范围

WebServer server(80);
int currentBright = 0; // 存储当前LED亮度

// 生成美化版网页
String buildHtml() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 网页无极调光器</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: "Microsoft YaHei", Arial, sans-serif;
    }
    body {
      min-height: 100vh;
      background: linear-gradient(135deg, #f0f4fd, #e4ecff);
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 20px;
    }
    .card {
      background: #ffffff;
      width: 100%;
      max-width: 520px;
      padding: 45px 35px;
      border-radius: 22px;
      box-shadow: 0 8px 30px rgba(40, 80, 180, 0.15);
    }
    h1 {
      color: #254b9c;
      font-size: 28px;
      text-align: center;
      margin-bottom: 25px;
      letter-spacing: 1px;
    }
    .text-box {
      text-align: center;
      font-size: 18px;
      color: #555;
      margin-bottom: 30px;
    }
    #valText {
      font-size: 36px;
      font-weight: bold;
      color: #2970e0;
      padding: 6px 16px;
      background: #f0f6ff;
      border-radius: 12px;
      margin-left: 10px;
    }
    .slider-wrap {
      margin: 35px 0;
    }
    /* 美化滑动条样式 */
    input[type="range"] {
      width: 100%;
      height: 14px;
      border-radius: 10px;
      background: #dde6f8;
      outline: none;
      -webkit-appearance: none;
    }
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 28px;
      height: 28px;
      border-radius: 50%;
      background: #2970e0;
      cursor: pointer;
      box-shadow: 0 2px 10px rgba(41, 112, 224, 0.35);
      transition: all 0.2s ease;
    }
    input[type="range"]::-webkit-slider-thumb:hover {
      transform: scale(1.12);
      background: #1d5fcb;
    }
    .btn-group {
      display: flex;
      gap: 14px;
      margin-top: 15px;
    }
    button {
      flex: 1;
      padding: 13px 0;
      border: none;
      border-radius: 12px;
      font-size: 16px;
      cursor: pointer;
      transition: 0.25s ease;
    }
    .btn-off {
      background: #e8edf7;
      color: #254b9c;
    }
    .btn-full {
      background: #2970e0;
      color: white;
    }
    button:hover {
      opacity: 0.85;
      transform: translateY(-2px);
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>ESP32 网页无极调光</h1>
    <div class="text-box">
      当前亮度数值：<span id="valText">)rawliteral" + String(currentBright) + R"rawliteral(</span>
    </div>
    <div class="slider-wrap">
      <input type="range" id="brightSlider" min="0" max="255" value=")rawliteral" + String(currentBright) + R"rawliteral(">
    </div>
    <div class="btn-group">
      <button class="btn-off" onclick="setBright(0)">一键熄灭</button>
      <button class="btn-full" onclick="setBright(255)">最大亮度</button>
    </div>
  </div>

  <script>
    const slider = document.getElementById("brightSlider");
    const valShow = document.getElementById("valText");

    // 通用发送亮度请求函数
    function setBright(val) {
      slider.value = val;
      valShow.innerText = val;
      fetch("/set?b="+val).catch(err => console.log("通信异常", err));
    }

    // 拖动滑块实时更新
    slider.addEventListener("input", function() {
      let val = this.value;
      valShow.innerText = val;
      fetch("/set?b="+val).catch(err => console.log("通信异常", err));
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// 首页路由：展示调光页面
void handleIndex() {
  server.send(200, "text/html; charset=utf-8", buildHtml());
}

// 处理滑块亮度设置请求 /set?b=xxx
void handleSetBright() {
  if(server.hasArg("b")){
    currentBright = server.arg("b").toInt();
    // 边界限制
    if(currentBright < 0) currentBright = 0;
    if(currentBright > 255) currentBright = 255;
    ledcWrite(LED_PIN, currentBright);
  }
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);

  // WiFi连接
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("网页访问地址 http://");
  Serial.println(WiFi.localIP());

  // 路由绑定
  server.on("/", handleIndex);
  server.on("/set", handleSetBright);
  server.begin();
}

void loop() {
  server.handleClient();
}