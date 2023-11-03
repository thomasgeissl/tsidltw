#include "config.h"
#include <Wire.h>
#include <WiFi.h>
#if HAS_MIDI
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#endif
#include <OSCMessage.h>

#if HAS_VL53L
#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#endif


#if HAS_MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
#endif


#if HAS_MIDI
Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, usbMIDI);
#endif

const char *Apssid = "nantlab-wlan";
const char *Appassword = "nantlab-wlan-pw";
WiFiUDP Udp;
const IPAddress outIp(255, 255, 255, 255);

int touchValues[NUMBER_OF_TOUCHES];

long _timestamp;
long _frameCounter = 0;
int _distance;

void readSensors () {
  for (auto i = 0; i < NUMBER_OF_TOUCHES; i++) {
    touchValues[i] = touchRead(touchPins[i]);
  }

#if HAS_MPU6050
  mpu.getEvent(&a, &g, &temp);
#endif
#if HAS_VL53L
  if (lox.isRangeComplete()) {
    _distance = lox.readRange();
    Serial.println(_distance);
  }
#endif

}

void sendValues() {
  OSCMessage touchMsg("/esp/touch");
  touchMsg.add(ID);
  for (auto i = 0; i < NUMBER_OF_TOUCHES; i++) {
    touchMsg.add(touchValues[i]);
  }
  Udp.beginPacket(outIp, REMOTE_OSC_PORT);
  touchMsg.send(Udp);
  Udp.endPacket();
  touchMsg.empty();

#if HAS_MPU6050
  OSCMessage accelerometerMsg("/esp/motion");
  accelerometerMsg.add(ID);
  accelerometerMsg.add(g.gyro.x);
  accelerometerMsg.add(g.gyro.y);
  accelerometerMsg.add(g.gyro.z);
  accelerometerMsg.add(a.acceleration.x);
  accelerometerMsg.add(a.acceleration.y);
  accelerometerMsg.add(a.acceleration.z);
  Udp.beginPacket(outIp, REMOTE_OSC_PORT);
  accelerometerMsg.send(Udp);
  Udp.endPacket();
  accelerometerMsg.empty();

  OSCMessage temperatureMsg("/esp/temperature");
  temperatureMsg.add(ID);
  temperatureMsg.add(temp.temperature);

  Udp.beginPacket(outIp, REMOTE_OSC_PORT);
  temperatureMsg.send(Udp);
  Udp.endPacket();
  temperatureMsg.empty();
#endif

#if HAS_VL53L
  OSCMessage distanceMessage("/esp/distance");
  distanceMessage.add(ID);
  distanceMessage.add(_distance);

  Udp.beginPacket(outIp, REMOTE_OSC_PORT);
  distanceMessage.send(Udp);
  Udp.endPacket();
  distanceMessage.empty();
#endif

#if HAS_MIDI
  usbMIDI.sendControlChange(1, 1, ID);
  usbMIDI.sendNoteOn(60, 127, ID);
  delay(50);
  usbMIDI.sendNoteOff(60, 0, ID);
#endif
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(Apssid, Appassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(myIP);
  Udp.begin(LOCAL_OSC_PORT);

#if HAS_MIDI
  usbMIDI.begin();
#endif

#if HAS_MPU6050
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
#endif

#if HAS_VL53L
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  lox.startRangeContinuous();

#endif

  Serial.println("");
  delay(100);
}

void loop() {
  auto timestamp = millis();
  _frameCounter++;
  readSensors();
  if (_frameCounter % 2 == 0) {
    sendValues();
  }
}
