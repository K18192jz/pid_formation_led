uint8_t LDR_PIN=2;   // ADC pin
int command = 0;
#define LED_PIN 23
int ldrValue;
float kp,ki,kd;
int a=255;
int b=0;
int error;
int last_error;
float led_brightness;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  kp=0.6;
  ki=0.35;
  kd=0.01;
}

void loop() {

  readSerialCommand();   



  
  ldrValue = analogRead(LDR_PIN);


  calculate_pid();
  
  Serial.print(a);
  Serial.print("     ");
  Serial.print(b);
  Serial.print("     ");
  Serial.print(command);
  Serial.print("     ");
  Serial.print(led_brightness);
  Serial.print("     ");
  Serial.print(kp);
  Serial.print("     ");
  Serial.print(ki);
  Serial.print("     ");
  Serial.print(kd);
  Serial.print("     ");
  Serial.println(ldrValue);
  delay(5);

  last_error=error;
}





int I;
void calculate_pid(){
  error = command - ldrValue;
  int P = error;
  I=I+error;
  int D = error - last_error;
  led_brightness = (float) (P*kp) + (I*ki) + (D*kd);

  if (led_brightness < 70) led_brightness=0;
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