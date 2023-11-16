#include <Blynk.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6ifO0MF2w"
#define BLYNK_TEMPLATE_NAME "t3 do nhip tim"
LiquidCrystal lcd(D0, D7, D6, D5, D4, D3);

PulseOximeter pox;
uint32_t tsLastReport = 0;
byte lastState = LOW;  // trạng thái cuối
byte currentState;     // trạng thái hiện tại
byte flagStateChange = HIGH; // cờ thay đổi trạng thái

unsigned long lastDebounceTime = 0;
char auth[] = "_Z7LlSWsV20-RGrC-Ji35-D6cyagedIc";             // Authentication Token Sent by Blynk
char ssid[] = "Hieunv";        //WiFi SSID
char pass[] = "0356976299";        //WiFi Password
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);
    Serial.print("Dang tinh toan");
    lcd.begin(16,2);
    Blynk.begin(auth, ssid, pass);
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
    pinMode(3,INPUT);
    pinMode(1,INPUT);
}
void kiemTra(){
  if(digitalRead(3)==LOW){
      delay(100);
      if(digitalRead(3)==LOW){
        while(true){
           currentState = digitalRead(1);
        
          // kiểm tra nút có nhấn không
          if (currentState != flagStateChange) {// nếu nút nhấn (currentState=LOW)
        
            lastDebounceTime = millis();// lấy thời điểm nhấn nút
        
            flagStateChange = currentState;// đặc lại giá trị cho cờ
          }
        
          //sau 50ms từ lúc nhấn nút thì chạy vào đây
          if ((millis() - lastDebounceTime) > 10000) { // cái này để bỏ qua khoảng nhiễu nhấn nút
            {
              if (lastState == HIGH && currentState == LOW) {// cái này chạy khi nhấn xuống
                break;
              }
              
              lastState = currentState;//đặt lại trạng thái cuối
            }
          }
        }
      }
      
    }
    
}
void loop()
{
      kiemTra();
      pox.update();
      Blynk.run();
      if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("HR:");
      lcd.print(pox.getHeartRate());
      lcd.print("BPM");
    
      lcd.setCursor(0, 1);
      lcd.print("SpO2:");
      lcd.print(pox.getSpO2());
      lcd.print("%");

      Blynk.virtualWrite(V1, pox.getHeartRate());
      Blynk.virtualWrite(V2, pox.getSpO2());
      tsLastReport = millis();
    }
}
