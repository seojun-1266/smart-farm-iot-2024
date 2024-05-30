#include <SoftwareSerial.h>

SoftwareSerial btSerial = SoftwareSerial(9, 10);

int echo =6;
int trig =7;

void setup(){
  Serial.begin(9600);
  btSerial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop(){
  float distance = ultraSonic();
  Serial.print(distance);
  Serial.println("cm");


  if(distance <= 20)
  {
    Serial.print("danger");
    btSerial.println("danger");
    delay(2000);
  }
  else
  {
    Serial.print("good");
    btSerial.println("good");
  }
delay(100);
}

float ultraSonic()
{
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  unsigned long duration = pulseIn(echo, HIGH);

  float distanceCM = ((34000*duration) / 1000000)/2;
  return distanceCM;
}
