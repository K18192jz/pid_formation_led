#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_SLIDER";
const char* password = "12345678";

WebServer server(80);



#define LDR_PIN 34 
int command = 0;
#define LED_PIN 23
int ldrValue;
volatile float kp,ki,kd;
int a=255; // just for the serial ploter
int b=0; // just for the serial ploter
int error;
int last_error;
float led_brightness;



int auoto=0;
int autotime;
void handleRoot() {
  String page = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<style>body{font-family:Arial;text-align:center;margin-top:20px;}input{width:90%;}"
                "button{padding:10px 20px;font-size:16px;margin:10px;}</style></head><body>";

  page += "<h3>Kp</h3><input type='range' min='-5' max='5' step='0.01' value='" + String(kp) + "' oninput='send(\"kp\", this.value)'>";
  page += "<p id='kpv'>" + String(kp) + "</p>";

  page += "<h3>Ki</h3><input type='range' min='-5' max='5' step='0.01' value='" + String(ki) + "' oninput='send(\"ki\", this.value)'>";
  page += "<p id='kiv'>" + String(ki) + "</p>";

  page += "<h3>Kd</h3><input type='range' min='-5' max='5' step='0.01' value='" + String(kd) + "' oninput='send(\"kd\", this.value)'>";
  page += "<p id='kdv'>" + String(kd) + "</p>";

  page += "<hr><h3>Command</h3><input type='range' min='0' max='200' step='1' value='" + String(command) + "' oninput='send(\"command\", this.value)'>";
  page += "<p id='commandv'>" + String(command) + "</p>";

  page += "<h3>Autotime (ms)</h3><input type='range' min='0' max='5000' step='500' value='" + String(autotime) + "' oninput='send(\"autotime\", this.value)'>";
  page += "<p id='autotimev'>" + String(autotime) + "</p>";

  page += "<hr><h3>Auto Mode</h3><button onclick='toggleAuto()'>Toggle Auto</button>";
  page += "<p id='autov'>" + String(auoto) + "</p>";

  page += "<script>"
          "function send(name,val){document.getElementById(name+'v').innerHTML=val;fetch('/set?'+name+'='+val);}"
          "function toggleAuto(){let current=parseInt(document.getElementById('autov').innerHTML);"
          "let newVal=current===0?1:0;document.getElementById('autov').innerHTML=newVal;"
          "fetch('/set?auoto='+newVal);}"
          "</script></body></html>";

  server.send(200, "text/html", page);
}



void handleSet() {
  if (server.hasArg("kp")) kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) ki = server.arg("ki").toFloat();
  if (server.hasArg("kd")) kd = server.arg("kd").toFloat();

  if (server.hasArg("command")) command = server.arg("command").toInt();
  if (server.hasArg("auoto")) auoto = server.arg("auoto").toInt();
  if (server.hasArg("autotime")) autotime = server.arg("autotime").toInt();

  server.send(200, "text/plain", "OK");
}



void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();


  kp=0.6;
  ki=0.35;
  kd=0.01;
}
unsigned long tim;
int vz=0;

void loop() {
  if (auoto == 1){
  if (millis() - tim > autotime && vz==0){
  command=0;
  tim = millis();
  vz=1;
  }
  if (millis() - tim > autotime && vz==1){
  command=100;
  tim = millis();
  vz=2;
  }
  if (millis() - tim > autotime && vz==2){
  command=200;
  tim = millis();
  vz=0;
  }}

  readSerialCommand();   
  server.handleClient();   // non-blocking



  
  int ldrValue1 = analogRead(LDR_PIN);
  delay(1);
  int ldrValue2 = analogRead(LDR_PIN);
  delay(1);
  int ldrValue3 = analogRead(LDR_PIN);
  delay(1);
  int ldrValue4 = analogRead(LDR_PIN);
  delay(1);
  int ldrValue5 = analogRead(LDR_PIN);
  ldrValue = (ldrValue1 + ldrValue2 + ldrValue3 + ldrValue4 + ldrValue5)/5; 
  delay(21);

 // calculate_pid();
  
  analogWrite(LED_PIN, command);  
  
  
  Serial.print(ldrValue);
  Serial.print(",");

  last_error=error;
}





int I;
void calculate_pid(){
  error = command - ldrValue;
  int P = error;
  I=I+error;
  int D = error - last_error;
  led_brightness = (float) (P*kp) + (I*ki) + (D*kd);

  if (led_brightness < 10) led_brightness=0;
  if (led_brightness > 255) led_brightness=255;

  analogWrite(LED_PIN, led_brightness);  
}




void readSerialCommand() {
  static String buffer = "";

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {   // ENTER
      buffer.trim();

      if (buffer.length() > 0) {

        char type = buffer.charAt(0);

        // PID commands: p, i, d
        if (type == 'p' || type == 'i' || type == 'd') {
          float value = buffer.substring(1).toFloat();

          if (type == 'p') kp = value;
          if (type == 'i') ki = value;
          if (type == 'd') kd = value;
        }
        // Otherwise → normal command
        else {
          command = buffer.toInt();
        }
      }

      buffer = "";   // clear buffer
    }
    else {
      buffer += c;   // collect characters
    }
  }
}

/*
#include <stdio.h>

int main() {
    printf("Hello, world!\n");
    return 0;
}
    */