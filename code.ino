define BLYNK_TEMPLATE_ID           "TMPLcg6Aie7A"
#define BLYNK_TEMPLATE_NAME         "Yellow Farm"
#define BLYNK_AUTH_TOKEN            "ss2-7XzIVBWV8xU4ZWuk8GLTag64pvzH"

#define BLYNK_PRINT Serial

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "SparkFun_SCD30_Arduino_Library.h"
SCD30 airSensor;

char ssid[] = "Mushroom";
char pass[] = "1234567890";

BlynkTimer timer;

const int analogInPin = 34; // analog input pin for multiplexer signal
const int s0 = 19; // multiplexer control pin s0
const int s1 = 18; // multiplexer control pin s1
const int s2 = 5; // multiplexer control pin s2
const int s3 = 17; // multiplexer control pin s3

const int dry = 4095; // value for dry sensor
const int wet = 2350; // value for wet sensor

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }
}

void loop()
{
  if (airSensor.dataAvailable())
  {
    float h = airSensor.getHumidity();
    float t = airSensor.getTemperature();
    float c = airSensor.getCO2();
    Serial.print("CO2(ppm): ");
    Serial.print(c);

    Serial.print(", temp(C): ");
    Serial.print(t);

    Serial.print(", humidity(%): ");
    Serial.print(h);

    Serial.println();
    Blynk.virtualWrite(V4, c);
    Blynk.virtualWrite(V5, h);
    Blynk.virtualWrite(V6, t);
  }
  int sum = 0;
  for (int i = 0; i < 16; i ++) {
    sum += readMux(i);
    delay(50);
  }
  Serial.print("moisture: ");
  int averageSum = (int)sum / 16;
  Serial.println(averageSum);
  Blynk.virtualWrite(V0, averageSum);
  Blynk.run();
  delay(1000);
}

float readMux(int channel)
{
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4] = {
    {0, 0, 0, 0}, //channel 0
    {1, 0, 0, 0}, //channel 1
    {0, 1, 0, 0}, //channel 2
    {1, 1, 0, 0}, //channel 3
    {0, 0, 1, 0}, //channel 4
    {1, 0, 1, 0}, //channel 5
    {0, 1, 1, 0}, //channel 6
    {1, 1, 1, 0}, //channel 7
    {0, 0, 0, 1}, //channel 8
    {1, 0, 0, 1}, //channel 9
    {0, 1, 0, 1}, //channel 10
    {1, 1, 0, 1}, //channel 11
    {0, 0, 1, 1}, //channel 12
    {1, 0, 1, 1}, //channel 13
    {0, 1, 1, 1}, //channel 14
    {1, 1, 1, 1} //channel 15
  };

  //loop through the 4 sig
  for (int i = 0; i < 4; i ++)
  {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(analogInPin);
  int moisture = map(val, wet, dry, 100, 0);
  return moisture;
}
