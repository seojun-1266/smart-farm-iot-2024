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

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  dht.begin();
  
  servoMotor.attach(motorPin); // 모터 핀에 서보 모터 연결
  pinMode(buzzerPin, OUTPUT); // 부저 핀을 출력으로 설정
  
  Serial.println("DHT Sensor and Keypad test!");
}

void loop() {
  // 키패드 입력 처리
  char key = keypad.getKey(); // 키패드 입력 가져오기
  
  if (key) {
    Serial.println("Key Pressed: " + String(key)); // 입력된 키 시리얼 모니터에 출력
    
    if (key == '5') { // 입력이 완료되면 비교
      if (checkPassword()) {
        moveMotor(); // 올바른 비밀번호 입력시 모터 움직이기
        btSerial.print("1");
        Serial.print("일");
      } else {
        buzz(); // 잘못된 비밀번호 입력시 부저 울리기
        btSerial.print("0");
        Serial.print("영");

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

    if (temp >= -100 && temp <= 60) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C, Humidity: ");
      Serial.print(humi);
      Serial.println(" %");

      //btSerial.print(temp);
      //btSerial.print(",");
      //btSerial.println(humi);
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