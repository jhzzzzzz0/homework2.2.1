#include <WiFi.h>
#include <WebServer.h>

const char* ap_ssid = "ESP32-LAB017";
const char* ap_pass = "12345678";

const int LED1 = 2;
const int LED2 = 4;
const int LED3 = 16;

WebServer server(80);

int bri1 = 0;
int bri2 = 0;
int bri3 = 0;

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>三路LED滑条调光-低延迟</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{
  font-family:Arial;
  background:linear-gradient(135deg,#4158D0,#C850C0);
  min-height:100vh;
  display:flex;justify-content:center;align-items:center;
}
.box{
  background:#fff;
  width:92%;max-width:500px;
  padding:35px;
  border-radius:20px;
  box-shadow:0 15px 50px rgba(0,0,0,0.25);
}
h1{text-align:center;color:#333;margin-bottom:8px;font-size:24px}
.sub{text-align:center;color:#888;margin-bottom:35px;font-size:14px}
.led-item{
  margin:25px 0;
  padding:20px;
  background:#f7f8fc;
  border-radius:14px;
}
.led-title{
  font-size:17px;font-weight:bold;color:#222;margin-bottom:12px;
  display:flex;justify-content:space-between;
}
.val-text{color:#666;font-size:18px}
input[type="range"]{
  width:100%;height:10px;
  cursor:pointer;
}
.footer{
  text-align:center;margin-top:40px;color:#aaa;font-size:12px
}
</style>
</head>
<body>
<div class="box">
  <h1>三路LED亮度调节</h1>
  <p class="sub">滑动滑块实时控制PWM亮度 0~255</p>

  <div class="led-item">
    <div class="led-title">LED1 (GPIO2) <span class="val-text" id="v1">)rawliteral"+String(bri1)+R"rawliteral(</span></div>
    <input type="range" min="0" max="255" value=")rawliteral"+String(bri1)+R"rawliteral(" id="s1">
  </div>

  <div class="led-item">
    <div class="led-title">LED2 (GPIO4) <span class="val-text" id="v2">)rawliteral"+String(bri2)+R"rawliteral(</span></div>
    <input type="range" min="0" max="255" value=")rawliteral"+String(bri2)+R"rawliteral(" id="s2">
  </div>

  <div class="led-item">
    <div class="led-title">LED3 (GPIO16) <span class="val-text" id="v3">)rawliteral"+String(bri3)+R"rawliteral(</span></div>
    <input type="range" min="0" max="255" value=")rawliteral"+String(bri3)+R"rawliteral(" id="s3">
  </div>

  <div class="footer">ESP32 三路PWM滑条控制 WebServer</div>
</div>

<script>
// 防抖计时器
let timer1 = null;
let timer2 = null;
let timer3 = null;
const delay = 120; // 防抖间隔，数值越小反应越快，建议150~220ms

// LED1
document.getElementById("s1").addEventListener("input", function(){
  let val = this.value;
  document.getElementById("v1").innerText = val;
  clearTimeout(timer1);
  timer1 = setTimeout(()=>send(1, val), delay);
})
// LED2
document.getElementById("s2").addEventListener("input", function(){
  let val = this.value;
  document.getElementById("v2").innerText = val;
  clearTimeout(timer2);
  timer2 = setTimeout(()=>send(2, val), delay);
})
// LED3
document.getElementById("s3").addEventListener("input", function(){
  let val = this.value;
  document.getElementById("v3").innerText = val;
  clearTimeout(timer3);
  timer3 = setTimeout(()=>send(3, val), delay);
})

// 发送请求
async function send(ledNum, val){
  await fetch("/set?led="+ledNum+"&bri="+val, {cache:"no-cache"});
}
</script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", makePage());
}

void handleSet() {
  int led = server.arg("led").toInt();
  int val = server.arg("bri").toInt();
  if(val < 0) val = 0;
  if(val > 255) val = 255;

  switch(led){
    case 1: bri1 = val; analogWrite(LED1, bri1); break;
    case 2: bri2 = val; analogWrite(LED2, bri2); break;
    case 3: bri3 = val; analogWrite(LED3, bri3); break;
  }
  // 最短返回，减少传输耗时
  server.send(200, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  analogWrite(LED1, bri1);
  analogWrite(LED2, bri2);
  analogWrite(LED3, bri3);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}