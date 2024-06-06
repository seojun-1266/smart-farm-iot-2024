int hum_pin = 7;  // 가습기 핀
int sensor_pin = A0;  // 센서 핀
int output_value;

void setup() {
  Serial.begin(9600);
  pinMode(hum_pin, OUTPUT);  // 가습기 핀을 출력 모드로 설정
}

void loop() {
  output_value = analogRead(sensor_pin);  // 센서 값 읽기
  output_value = map(output_value, 550, 0, 0, 100);  // 센서 값을 0에서 100 사이로 매핑
  
  Serial.print("센서 값: ");
  Serial.println(output_value);  // 센서 값을 시리얼 모니터에 출력
  
  if (output_value < -80) {  // 센서 값이 40보다 크면
    digitalWrite(hum_pin, HIGH);  // 가습기 켜기
  } else {
    digitalWrite(hum_pin, LOW);  // 그렇지 않으면 가습기 끄기
  }
  
  delay(1000);  // 1초 지연하여 시리얼 출력 읽기 쉽게 하기
}
