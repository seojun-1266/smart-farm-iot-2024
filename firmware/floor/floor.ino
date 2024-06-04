int hum_pin = 7;  // 가습기 핀
int led_pin = 8;  // LED 핀
int fan_pin = 11;  // 팬 모터 핀

int soil_moisture_pin = A0;  // 토양 습도 센서 핀
int light_sensor_pin = A1;  // 조도 센서 핀
int flame_sensor_pin = A2;  // 불꽃 감지 센서 핀

int soil_moisture_value;
int light_sensor_value;
int flame_sensor_value;

void setup() {
  Serial.begin(9600);
  
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
  
  // 시리얼 모니터에 값 출력
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
  if (light_sensor_value < 100) {
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
}
