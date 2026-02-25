#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_SLIDER";
const char* password = "12345678";

WebServer server(80);

volatile float kp = 0.0;
volatile float ki = 0.0;
volatile float kd = 0.0;

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family: Arial; text-align: center; margin-top: 40px; }
input { width: 90%; }
</style>
</head>
<body>

<h3>Kp</h3>
<input type="range" min="-5" max="5" step="0.01" value="0"
oninput="send('kp', this.value)">
<p id="kpv">0</p>

<h3>Ki</h3>
<input type="range" min="-5" max="5" step="0.01" value="0"
oninput="send('ki', this.value)">
<p id="kiv">0</p>

<h3>Kd</h3>
<input type="range" min="-5" max="5" step="0.01" value="0"
oninput="send('kd', this.value)">
<p id="kdv">0</p>

<script>
function send(name, val){
  document.getElementById(name + "v").innerHTML = val;
  fetch("/set?" + name + "=" + val);
}
</script>

</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", PAGE);
}

void handleSet() {
  if (server.hasArg("kp")) kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) ki = server.arg("ki").toFloat();
  if (server.hasArg("kd")) kd = server.arg("kd").toFloat();
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();   // non-blocking

  Serial.print("KP: "); Serial.print(kp, 2);
  Serial.print(" | KI: "); Serial.print(ki, 2);
  Serial.print(" | KD: "); Serial.println(kd, 2);

  // your real-time control code runs here uninterrupted
}