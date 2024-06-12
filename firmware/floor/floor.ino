#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include "DHT.h"

// Keypad 설정
const byte ROWS = 4; // 행의 수
const byte COLS = 4; // 열의 수

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {5, 4}; // 행 핀들
byte colPins[COLS] = {3, 2}; // 열 핀들

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// DHT 설정
SoftwareSerial btSerial(9, 10);

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long currTime = 0;
unsigned long prevTime = 0;

// 서보 모터 및 부저 설정
const int motorPin = 12; // 모터 핀
const int buzzerPin = 13; // 부저 핀
Servo servoMotor; // 서보 모터 객체 생성
bool motorMoved = false; // 모터가 움직였는지 여부
int hum_pin = 7;  // 가습기 핀
int led_pin = 8;  // LED 핀
int fan_pin = 11;  // 팬 모터 핀

int soil_moisture_pin = A0;  // 토양 습도 센서 핀
int light_sensor_pin = A1;  // 조도 센서 핀
int flame_sensor_pin = A2;  // 불꽃 감지 센서 핀

int soil_moisture_value;
int light_sensor_value;
int flame_sensor_value;

int count;
void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  dht.begin();
  
  servoMotor.attach(motorPin); // 모터 핀에 서보 모터 연결
  pinMode(buzzerPin, OUTPUT); // 부저 핀을 출력으로 설정
  
  Serial.println("DHT Sensor and Keypad test!");

  pinMode(hum_pin, OUTPUT);  // 가습기 핀을 출력 모드로 설정
  pinMode(led_pin, OUTPUT);  // LED 핀을 출력 모드로 설정
  pinMode(fan_pin, OUTPUT);  // 팬 모터 핀을 출력 모드로 설정
}

void loop() {
  // 토양 습도 값 읽기
  soil_moisture_value = analogRead(soil_moisture_pin);  
  soil_moisture_value = map(soil_moisture_value, 550, 0, 0, 100);  // 0에서 100 사이로 매핑
  
  // 조도 센서 값 읽기
  light_sensor_value = analogRead(light_sensor_pin);  
  light_sensor_value = map(light_sensor_value, 1023, 0, 0, 100);  // 0에서 100 사이로 매핑
  
  // 불꽃 감지 센서 값 읽기
  flame_sensor_value = analogRead(flame_sensor_pin);
  flame_sensor_value = map(flame_sensor_value, 0, 1023, 0, 100);  // 0에서 100 사이로 매핑
  // 키패드 입력 처리
  char key = keypad.getKey(); // 키패드 입력 가져오기
  
  if (key) {
    Serial.println("Key Pressed: " + String(key)); // 입력된 키 시리얼 모니터에 출력
    
    if (key == '5') { // 입력이 완료되면 비교
      if (checkPassword()) {
        moveMotor(); // 올바른 비밀번호 입력시 모터 움직이기
        count = 0;
        if(count == 0){
          btSerial.print("0");
          Serial.print("영");
        }
      } else {
        
        buzz(); // 잘못된 비밀번호 입력시 부저 울리기
        //if(count2 == 1)
        count = 1;
        if(count == 1){
        btSerial.print("1");
        Serial.print("일");
        }
      }
    }
  }
  
  // DHT 센서 데이터 처리
  currTime = millis();
  if (currTime - prevTime >= 5000) {
    prevTime = currTime;
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(humi) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.print("토양 습도 값: ");
    Serial.print(soil_moisture_value);
    Serial.print(" | 조도 센서 값: ");
    Serial.print(light_sensor_value);
    Serial.print(" | 불꽃 감지 센서 값: ");
    Serial.println(flame_sensor_value);

    // 토양 습도가 일정 수준 이하이면 가습기 켜기
  if (soil_moisture_value < -1) {
    digitalWrite(hum_pin, HIGH);
  } else {
    digitalWrite(hum_pin, LOW);
  }
  
  // 조도 센서가 일정 수준 이하이면 LED 켜기
  if (light_sensor_value < 30) {
    digitalWrite(led_pin, HIGH);
  } else {
    digitalWrite(led_pin, LOW);
  }
  
  // 불꽃 감지 센서가 일정 수준 이상이면 팬 모터 동작
  if (flame_sensor_value <73) {
    digitalWrite(fan_pin, HIGH);
  } else {
    digitalWrite(fan_pin, LOW);
  }
  
  delay(1000);  // 1초 지연

    if (temp >= -100 && temp <= 60) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C, Humidity: ");
      Serial.print(humi);
      Serial.println(" %");

      btSerial.print("습도 : ");
      btSerial.print(humi);
      btSerial.println(" %");
      btSerial.print("온도 : ");
      btSerial.print(temp);
      btSerial.print("°C");
    } else {
      Serial.print("Temperature out of range: ");
      Serial.println(temp);
    }
  }
}

bool checkPassword() {
  // 비밀번호를 여기에 추가하세요
  char password[] = "1111";
  char enteredPassword[5]; // 입력된 비밀번호 저장용
  
  Serial.println("Enter Password:");
  
  for (int i = 0; i < 4; ++i) {
    char key = keypad.getKey(); // 입력된 키 가져오기
    
    while (!key) {
      key = keypad.getKey(); // 키가 입력될 때까지 대기
    }
    
    enteredPassword[i] = key; // 입력된 키를 비밀번호에 추가
    Serial.print("*"); // 비밀번호 마스킹
  }
  
  enteredPassword[4] = '\0'; // 문자열 끝 지정
  
  Serial.println(); // 새 줄
  
  // 입력된 비밀번호와 저장된 비밀번호 비교
  if (strcmp(password, enteredPassword) == 0) {
    return true; // 비밀번호가 일치함
  } else {
    return false; // 비밀번호가 일치하지 않음
  }
}

void moveMotor() {
  servoMotor.write(90); // 모터를 90도로 회전
  motorMoved = true; // 모터 움직임 표시
  delay(2000); // 2초 대기
  servoMotor.write(0); // 모터를 초기 위치로 되돌림
  motorMoved = false; // 모터 움직임 표시 해제
}

void buzz() {
  if (!motorMoved) {
    // 부저를 2초 동안 울림
    tone(buzzerPin, 1000);
    delay(2000);
    noTone(buzzerPin);
  }
}