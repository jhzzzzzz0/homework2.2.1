#include <WiFi.h>
#include <WebServer.h>

// ========== AP热点配置（连接ESP32自带WiFi）==========
const char* ap_ssid     = "ESP32-LAB017";
const char* ap_password = "12345678"; // 至少8位
// ====================================================

#define LED_PIN     2
#define TOUCH_PIN   4
#define TOUCH_THRESHOLD 550

WebServer server(80);

bool isArm = false;        // true=布防，false=撤防
bool isAlarm = false;      // 报警锁定标记
volatile bool touchTrigger = false; // 触摸中断标记

// 非阻塞闪烁定时器
unsigned long flashTimer = 0;
const unsigned long flashInterval = 80;
bool ledFlashState = false;

// 触摸中断服务函数
void touchISR() {
  touchTrigger = true;
}

// 网页页面生成
String buildHtml() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>物联网安防报警器</title>
<style>
*{margin:0;padding:0;box-sizing:border-box;font-family:Microsoft YaHei,Arial}
body{min-height:100vh;background:linear-gradient(135deg,#f0f3f8,#e1e8f5);display:flex;justify-content:center;align-items:center;padding:20px}
.card{background:#fff;width:100%;max-width:500px;padding:40px 30px;border-radius:20px;box-shadow:0 8px 28px rgba(30,60,130,0.12)}
h1{text-align:center;color:#223355;margin-bottom:30px;font-size:26px}
.state-box{text-align:center;font-size:19px;margin:20px 0;line-height:2}
#armStatus{font-weight:bold}
#alarmStatus{font-weight:bold}
.btn-group{display:flex;gap:16px;margin-top:35px}
button{flex:1;padding:14px 0;border:none;border-radius:14px;font-size:17px;cursor:pointer;transition:0.2s}
.arm-btn{background:#00aa44;color:white}
.disarm-btn{background:#dd3333;color:white}
button:hover{opacity:0.85;transform:translateY(-2px)}
</style>
</head>
<body>
<div class="card">
<h1>ESP32 安防报警主机</h1>
<div class="state-box">
系统布防状态：<span id="armStatus">加载中...</span><br>
警报状态：<span id="alarmStatus">加载中...</span>
</div>
<div class="btn-group">
<button class="arm-btn" onclick="setArm()">布防 Arm</button>
<button class="disarm-btn" onclick="setDisarm()">撤防 Disarm</button>
</div>
</div>

<script>
// 每秒自动刷新状态
function refreshStatus(){
  fetch("/getstate")
    .then(res=>res.json())
    .then(data=>{
      // 更新布防状态文字颜色
      if(data.isArm){
        document.getElementById("armStatus").innerText = "已布防";
        document.getElementById("armStatus").style.color = "#009933";
      }else{
        document.getElementById("armStatus").innerText = "未布防";
        document.getElementById("armStatus").style.color = "#cc2222";
      }
      // 更新警报状态文字颜色
      if(data.isAlarm){
        document.getElementById("alarmStatus").innerText = "【警报触发中】";
        document.getElementById("alarmStatus").style.color = "#ff2222";
      }else{
        document.getElementById("alarmStatus").innerText = "无警报";
        document.getElementById("alarmStatus").style.color = "#555555";
      }
    })
    .catch(err=>console.log("状态刷新失败",err));
}

// 布防接口
function setArm(){
  fetch("/arm");
}
// 撤防接口
function setDisarm(){
  fetch("/disarm");
}

// 页面加载立刻刷新 每1秒自动轮询
window.onload = refreshStatus;
setInterval(refreshStatus, 1000);
</script>
</body>
</html>
)rawliteral";
  return html;
}

// 网页路由处理 
// 首页：展示控制面板
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", buildHtml());
}

// 返回JSON状态接口，给网页实时读取
void handleGetState() {
  String json = "{\"isArm\":" + String(isArm ? 1 : 0) + ",\"isAlarm\":" + String(isAlarm ? 1 : 0) + "}";
  server.send(200, "application/json", json);
}

// 布防接口
void handleArm() {
  isArm = true;
  server.send(200, "text/plain", "ok");
}

// 撤防接口：关闭报警、重置所有状态
void handleDisarm() {
  isArm = false;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW);
  ledFlashState = false;
  server.send(200, "text/plain", "ok");
}

// 初始化
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 触摸中断绑定
  touchAttachInterrupt(TOUCH_PIN, touchISR, TOUCH_THRESHOLD);

  // ========== 开启AP热点（替换原来连路由器WiFi）==========
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("ESP32热点已开启");
  Serial.print("热点名称：");
  Serial.println(ap_ssid);
  Serial.print("热点密码：");
  Serial.println(ap_password);
  Serial.print("访问地址 http://");
  Serial.println(WiFi.softAPIP());
  // =======================================================

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/getstate", handleGetState); // 新增状态查询接口
  server.begin();
}

// 主循环 
void loop() {
  server.handleClient(); // 持续处理网页请求

  // 检测触摸触发，仅布防且无报警时锁定警报
  if (touchTrigger) {
    touchTrigger = false;
    if(isArm && !isAlarm){
      isAlarm = true;
      Serial.println("入侵触发，警报锁定！");
    }
  }

  // 非阻塞LED高频闪烁（不阻塞网页服务）
  if(isAlarm){
    unsigned long now = millis();
    if(now - flashTimer >= flashInterval){
      flashTimer = now;
      ledFlashState = !ledFlashState;
      digitalWrite(LED_PIN, ledFlashState);
    }
  }else{
    digitalWrite(LED_PIN, LOW);
  }
}