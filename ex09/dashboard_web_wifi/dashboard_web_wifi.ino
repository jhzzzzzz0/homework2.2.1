#include <WiFi.h>
#include <WebServer.h>

// AP热点配置（手机连接ESP32的WiFi）
const char* ap_ssid = "ESP32-LAB017";
const char* ap_pass = "12345678"; // 密码至少8位

// 硬件定义
#define TOUCH_PIN 4
WebServer server(80);

// 存储触摸原始读数
uint16_t touchValue = 0;

// 生成仪表盘网页
String buildDashboard() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>触摸传感器实时仪表盘</title>
<style>
*{margin:0;padding:0;box-sizing:border-box;font-family:Microsoft YaHei,Arial}
body{
  min-height: 100vh;
  background: linear-gradient(145deg, #1a2332, #2c3b52);
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 20px;
}
.dashboard-card{
  background: #0f1620;
  width: 100%;
  max-width: 550px;
  padding: 50px 30px;
  border-radius: 24px;
  box-shadow: 0 0 40px rgba(0, 170, 255, 0.2);
  border: 1px solid #2a4060;
  text-align: center;
}
h1{
  color: #e8f2ff;
  font-size: 26px;
  margin-bottom: 40px;
  letter-spacing: 1px;
}
.value-box{
  background: linear-gradient(135deg, #002b4d, #004073);
  border-radius: 20px;
  padding: 40px 20px;
  margin-bottom: 30px;
}
#touchNum{
  font-size: 72px;
  font-weight: bold;
  color: #00ccff;
  text-shadow: 0 0 15px #00ccff80;
}
.tip-text{
  color: #a0b8d8;
  font-size: 16px;
  margin-top: 12px;
}
.desc{
  color: #7890b0;
  font-size: 15px;
  line-height: 1.8;
}
</style>
</head>
<body>
<div class="dashboard-card">
  <h1>触摸传感器实时监测仪表盘</h1>
  <div class="value-box">
    <div id="touchNum">--</div>
    <div class="tip-text">触摸原始采集值</div>
  </div>
  <div class="desc">
    手指靠近触摸引脚 → 数值降低<br>
    手指离开触摸引脚 → 数值回升
  </div>
</div>

<script>
let reqLock = false; // 请求锁，防止请求堆积延迟
// 高频拉取传感器数据
async function updateSensorData(){
  if(reqLock) return;
  reqLock = true;
  try{
    const res = await fetch("/data", {cache:"no-cache"});
    const data = await res.json();
    document.getElementById("touchNum").innerText = data.touchVal;
  }catch(err){}
  reqLock = false;
}

// 页面加载立刻刷新，循环轮询100ms一次
window.onload = updateSensorData;
setInterval(updateSensorData, 100);
</script>
</body>
</html>
)rawliteral";
  return html;
}

// 首页：展示仪表盘页面
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", buildDashboard());
}

// 数据接口：返回实时触摸数值JSON（sprintf优化，减少卡顿）
void handleGetData() {
  char buf[32];
  sprintf(buf, "{\"touchVal\":%d}", touchValue);
  server.send(200, "application/json", buf);
}

void setup() {
  Serial.begin(115200);

  // 开启AP热点，不连接路由器WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("======== ESP32热点已启动 ========");
  Serial.print("热点名称：");
  Serial.println(ap_ssid);
  Serial.print("热点密码：");
  Serial.println(ap_pass);
  Serial.print("访问地址 http://");
  Serial.println(WiFi.softAPIP());

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/data", handleGetData);
  server.begin();
}

void loop() {
  // 持续处理网页请求
  server.handleClient();

  // 实时读取触摸引脚模拟量
  touchValue = touchRead(TOUCH_PIN);
  delay(100);
}