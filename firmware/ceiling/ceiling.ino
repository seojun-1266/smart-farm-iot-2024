#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h> // 서보 모터 라이브러리 추가
#include <Adafruit_NeoPixel.h>

// LCD 설정 (주소 0x27, 16x2 크기)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoMotor; // 서보 모터 객체 생성

#define TRIG_PIN 7
#define ECHO_PIN 8
#define BUZZER_PIN 13
#define LIGHT_SENSOR_PIN A0
#define LED_PIN 6
#define LED_COUNT 60
#define BRIGHTNESS 50
#define FAN_PIN 11
#define BUTTON_PIN 10

long duration; // 초음파 센서의 펄스 지속 시간
int distance; // 물체와의 거리
unsigned long startMillis; // 수확 시점 타이머 시작 시간
const unsigned long interval = 5000; // 5초 (수확 시간 판별)
const unsigned long lightCheckInterval = 3000; // 3초 (조도 센서 판별)
bool isHarvestTime = false; // 수확 시간 여부 플래그
bool buzzerActive = false; // 버저 활성화 상태 플래그
String currentMessage = ""; // LCD에 표시할 현재 메시지
bool lightBelowThreshold = false; // 조도 센서 기준치 이하 여부 플래그
unsigned long lightStartMillis; // 조도 센서 타이머 시작 시간

bool fanState = false; // 팬 모터 상태 (false: 꺼짐, true: 켜짐)
bool lastButtonState = LOW; // 마지막 버튼 상태

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  pinMode(TRIG_PIN, OUTPUT); // 트리거 핀을 출력으로 설정
  pinMode(ECHO_PIN, INPUT); // 에코 핀을 입력으로 설정
  pinMode(BUZZER_PIN, OUTPUT); // 버저 핀을 출력으로 설정
  pinMode(FAN_PIN, OUTPUT); // 팬 모터 핀을 출력으로 설정
  pinMode(BUTTON_PIN, INPUT_PULLUP); // 버튼 핀을 입력 풀업으로 설정

  lcd.init(); // LCD 초기화
  lcd.backlight(); // LCD 백라이트 켬
  lcd.setCursor(0, 0); // 커서를 첫 줄 첫 칸에 설정
  lcd.print("Initializing..."); // 초기화 메시지 출력

  servoMotor.attach(9); // 서보 모터를 아두이노 9번 핀에 연결

  strip.begin();                    // 네오픽셀 초기화(필수)
  strip.setBrightness(BRIGHTNESS);  // 네오픽셀 밝기 설정 *RGBW만
  strip.show();

  Serial.begin(9600); // 시리얼 통신 시작
}

void loop() {
  // 초음파 센서를 사용하여 거리 측정
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 초음파 신호의 왕복 시간을 계산
  duration = pulseIn(ECHO_PIN, HIGH);
  // 거리를 cm 단위로 변환
  distance = (duration / 2) * 0.0343;

  // 조도 센서 값을 읽음
  int lightLevel = analogRead(LIGHT_SENSOR_PIN);

  // 거리와 조도 센서 값을 시리얼 모니터에 출력
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Light Level: ");
  Serial.println(lightLevel);

  // 수확 시간 여부 확인
  if (distance <= 4) {
    if (!isHarvestTime) {
      // 수확 시간 시작 타이머 초기화
      startMillis = millis();
      isHarvestTime = true;
      buzzerActive = false;
    }

    // 수확 시간이 5초 이상 지속되면 버저를 울림
    if (!buzzerActive && millis() - startMillis >= interval) {
      digitalWrite(BUZZER_PIN, HIGH);
      buzzerActive = true;
      displayMessage("Harvest time!");
      delay(2000); // 2초 동안 버저를 울림
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else {
    isHarvestTime = false;
    buzzerActive = false;
    displayMessage("Not harvest time!");
  }

  // 조도 센서 값이 100보다 크면 LED 끄기
  if (lightLevel > 100) {
    digitalWrite(LED_PIN, LOW); // LED 끔
    if (!lightBelowThreshold) {
      // 조도 센서 기준치 초과 시간 기록 시작
      lightStartMillis = millis();
      lightBelowThreshold = true;
    }

    // 조도 센서 기준치 초과 시간이 3초 이상 지속되면
    if (millis() - lightStartMillis >= lightCheckInterval) {
      // 서보 모터를 130도로 회전
      servoMotor.write(130);
    }
  } else {
    digitalWrite(LED_PIN, HIGH); // LED 켬
    if (lightBelowThreshold) {
      // 조도 센서 기준치 이하 시간 기록
      lightStartMillis = millis();
      lightBelowThreshold = false;
    }

    // 조도 센서 기준치 이하 시간이 3초 이상 지속되면
    if (millis() - lightStartMillis >= lightCheckInterval) {
      // 서보 모터를 0도로 회전
      servoMotor.write(0);
    }
  }

  // 네오픽셀 LED 제어
  if (lightLevel > 100) {
    // 네오픽셀 끄기
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 0, 0, 0, 0); // RGBW
    }
    strip.show();
  } else {
    // 네오픽셀 켜기 (흰색)
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 255, 255, 255, 255); // RGBW
    }
    strip.show();
  }

  // 버튼 상태 읽기
  bool buttonState = digitalRead(BUTTON_PIN);

  // 버튼이 눌렸을 때 (상태가 LOW에서 HIGH로 변경될 때)
  if (buttonState == LOW && lastButtonState == HIGH) {
    fanState = !fanState; // 팬 모터 상태를 토글
    digitalWrite(FAN_PIN, fanState ? HIGH : LOW); // 팬 모터 상태 변경
    Serial.print("Fan State: ");
    Serial.println(fanState ? "ON" : "OFF");
  }

  lastButtonState = buttonState; // 마지막 버튼 상태 업데이트

  delay(100); // 100ms 딜레이
}

void displayMessage(String message) {
  // 메시지가 변경될 때만 LCD 업데이트
  if (currentMessage != message) {
    Serial.print("Updating LCD: ");
    Serial.println(message);
    lcd.clear(); // LCD 화면 초기화
    lcd.setCursor(0, 0); // 커서를 첫 줄 첫 칸에 설정
    lcd.print(message); // 메시지 출력
    currentMessage = message; // 현재 메시지를 업데이트
  }
}
