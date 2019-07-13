/* **************** OSW LABS ****************** */
/**
 * @license
 * Copyright OSW Labs. All Rights Reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 */

/* *************** AgroLab IoT **************** 
 * Authors:  
  - Sergio Morcuende (https://github.com/smorcuend)
  - Rabindranath Andujar (https://github.com/ndujar)
 * Source Code: https://github.com/OSWHackerspace/AgrolabIoT 
 * Community: https://github.com/osweekends
*/

/* Parameters */
/* 
HUM -> Humidity
TMP -> Tempertaure
SM -> SoilMoisture
*/
#define _DEBUG_ true

#include "config.h"

#include <ThingerESP8266.h>

// Sensors lib imports
#include <Adafruit_Sensor.h>
#include <DHT.h>

/* Thinger */
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

#define DHTTYPE DHT11 // DHT 11
#define DHTPIN D6     // Pin which is connected to the DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

#define SMSensorDigitalPin D7 
#define SMSensor A0

volatile float temperature = 0;
volatile float humidity = 0;

// Soil Moisture
volatile int SMSensorValue = 0;

/* Interrupts handlers */
const int SMSensorInterruptPin = digitalPinToInterrupt(SMSensorDigitalPin);

/* Sensors Readings */
void dht11Read() {
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    temperature = 0;
    humidity = 0;
    return;
  }
}

void smRead() {
  int rawValue = analogRead(SMSensor); // [0-1024]
  SMSensorValue = map(rawValue, 0, 1024, 100, 0);
}

void print()  {
  Serial.println("NEXT MEASUREMENT");

  Serial.print("TMP%");
  Serial.println(temperature);
  Serial.print("HUM%");
  Serial.println(humidity);

  Serial.print("SM%");
  Serial.println(SMSensorValue);

  Serial.println();
}

// publish data sensors
void publishThinger()
{
  thing["data"] >> [](pson &out) {

    // DHT11 value
    dht11Read();
      
    // SMSensor value
    smRead();

    out["TMP"] = temperature;
    out["HUM"] = humidity;
    out["SM"] = SMSensorValue;

    if (_DEBUG_) print();
  };
}

void setup() {
  Serial.begin(115200);

  pinMode(SMSensor, INPUT);
  pinMode(SMSensorDigitalPin, INPUT);

  dht.begin();

  thing.add_wifi(SSID_1, SSID_PASSWORD_1);

  publishThinger();
}

void loop() {
  thing.handle();
}
