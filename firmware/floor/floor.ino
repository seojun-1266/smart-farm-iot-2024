#include <Keypad.h>

#include <Servo.h>
#include <SoftwareSerial.h>
#include "DHT.h"

// Keypad 설정
const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; // 행 핀들
byte colPins[COLS] = {A3, A2, A1, A0}; // 열 핀들

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// DHT 설정
SoftwareSerial btSerial(9, 10);

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long currTime = 0;
unsigned long prevTime = 0;

// 서보 모터 및 부저 설정

const int motorPin = 12; 
const int buzzerPin = 13; 
Servo servoMotor; 
bool motorMoved = false; 

int hum_pin = 7;  
int led_pin = 8;  
int fan_pin = 11;  

int soil_moisture_pin = A0;  
int light_sensor_pin = A1;  
int flame_sensor_pin = A2;  

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  dht.begin();
  
  servoMotor.attach(motorPin); 
  pinMode(buzzerPin, OUTPUT); 
  pinMode(hum_pin, OUTPUT);  
  pinMode(led_pin, OUTPUT);  
  pinMode(fan_pin, OUTPUT);  

  Serial.println("DHT Sensor and Keypad test!");
}

void loop() {

  int soil_moisture_value = map(analogRead(soil_moisture_pin), 550, 0, 0, 100);  
  int light_sensor_value = map(analogRead(light_sensor_pin), 1023, 0, 0, 100);  
  int flame_sensor_value = map(analogRead(flame_sensor_pin), 0, 1023, 0, 100);  

  char key = keypad.getKey(); 

  if (key == '5') { 
    if (checkPassword()) {
      moveMotor(); 
      btSerial.print("0");
      Serial.print("올바른 비밀번호 ");
    } else {
      buzz(); 
      btSerial.print("1");
      Serial.print("침입자 발생 ");
    }
  }
  
  currTime = millis();
  if (currTime - prevTime >= 5000) {
    prevTime = currTime;
    float h = dht.readHumidity();// 습도를 측정합니다.
    float t = dht.readTemperature();// 온도를 측정합니다.
    float f = dht.readTemperature(true);// 화씨 온도를 측정합니다.

    t = t+2;

    // 값 읽기에 오류가 있으면 오류를 출력합니다.
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.print("토양 습도 값: ");
    Serial.print(soil_moisture_value);
    Serial.print(" | 조도 센서 값: ");
    Serial.print(light_sensor_value);
    Serial.print(" | 불꽃 감지 센서 값: ");
    Serial.println(flame_sensor_value);

    digitalWrite(hum_pin, soil_moisture_value < 10 ? HIGH : LOW);
    digitalWrite(led_pin, light_sensor_value < 30 ? HIGH : LOW);
    digitalWrite(fan_pin, flame_sensor_value < 73 ? HIGH : LOW);

    String data1 = "Temperature: " + String(t) + " °C, Humidity: " + String(h) + " %";

    Serial.println(data1);
    //Serial.println(t);

    btSerial.println(t);
    //btSerial.println(t);
  }
}

bool checkPassword() {
  char password[] = "1111";
  char enteredPassword[5];
  
  Serial.println("Enter Password:");
  
  for (int i = 0; i < 4; ++i) {
    char key;
    do {
      key = keypad.getKey(); 
    } while (!key);
    
    enteredPassword[i] = key; 
    Serial.print("*"); 
  }
  
  enteredPassword[4] = '\0'; 
  
  Serial.println(); 
  
  return strcmp(password, enteredPassword) == 0;
}

void moveMotor() {
  servoMotor.write(90); 
  motorMoved = true; 
  delay(2000); 
  servoMotor.write(0); 
  motorMoved = false; 
}

void buzz() {
  if (!motorMoved) {
    tone(buzzerPin, 1000);
    delay(2000);
    noTone(buzzerPin);
  }
}
