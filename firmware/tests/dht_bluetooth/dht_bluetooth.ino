#include <SoftwareSerial.h>
#include "DHT.h"

SoftwareSerial btSerial(9, 10);

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long currTime = 0;
unsigned long prevTime = 0;

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  dht.begin();
}

void loop() {
  currTime = millis();
  if (currTime - prevTime >= 5000) {
    prevTime = currTime;
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(humi) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    if (temp >= 0 && temp <= 60) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C, Humidity: ");
      Serial.print(humi);
      Serial.println(" %");

      btSerial.print(temp);
      btSerial.print(",");
      btSerial.println(humi);
    } else {
      Serial.print("Temperature out of range: ");
      Serial.println(temp);
    }
  }
}
