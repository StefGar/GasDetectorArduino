#include "thingProperties.h"
#include <DHT.h>
#include <Servo.h>

#define DHTPIN 3
#define DHTTYPE DHT11
#define MQ4PIN A0
#define FAN_PIN 2
#define BUZZER_PIN 12
#define SERVO_PIN 9

DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

const unsigned long delayPeriod = 1000;
unsigned long previousMillis = 0;

const int TEMP_UMBRAL = 40;
const int GAS_UMBRAL = 200;

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  noTone(BUZZER_PIN);  // Asegura que buzzer esté apagado al iniciar

  dht.begin();
  myservo.attach(SERVO_PIN);
}

int readMQ4() {
  int sum = 0;
  const int samples = 10;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(MQ4PIN);
    delay(10);
  }
  return sum / samples;
}

void loop() {
  ArduinoCloud.update();

  if (millis() - previousMillis >= delayPeriod) {
    previousMillis = millis();

    // Leer temperatura
    float t = dht.readTemperature();
    if (!isnan(t)) {
      dHT11 = t;
      Serial.print("Temperatura (°C): ");
      Serial.println(dHT11);

      if (dHT11 >= TEMP_UMBRAL) {
        digitalWrite(FAN_PIN, HIGH);
      } else {
        digitalWrite(FAN_PIN, LOW);
      }
    }

    // Leer sensor de gas con promedio
    mQ4 = readMQ4();
    Serial.print("MQ4 (gas): ");
    Serial.println(mQ4);

    // Control del buzzer con tone() para buzzer pasivo
    if (mQ4 >= GAS_UMBRAL) {
      tone(BUZZER_PIN, 4000); // Tono a 1000 Hz
    } else {
      if (bUZZER) {
        tone(BUZZER_PIN, 4000); // Control manual: prender buzzer
      } else {
        noTone(BUZZER_PIN);     // Apagar buzzer
      }
    }
  }
}

void onBUZZERChange() {
  // Solo cambia el buzzer manual si no hay gas alto
  if (mQ4 < GAS_UMBRAL) {
    if (bUZZER) {
      tone(BUZZER_PIN, 4000);
    } else {
      noTone(BUZZER_PIN);
    }
  }
}
