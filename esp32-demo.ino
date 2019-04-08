#include "mj_ht_sensor.h"
#include <BLEDevice.h>
#include <BLEAddress.h>
#include <HardwareSerial.h>

static const BLEAddress MJHT_ADDRESS("4c:65:a8:d3:e5:bc");
MjHtSensor mjHtSensor(MJHT_ADDRESS);

void bleTask(void*)
{
  for (;;) {
    Serial.println("Requesting values from BLE sensor");
    if (mjHtSensor.requestUpdate()) {
      Serial.printf("T=%0.2fºC H=%0.2f%%", mjHtSensor.temperature(), mjHtSensor.humidity());
      Serial.println();
      delay(30000);
    }
    else {
      Serial.println("Failed to request values from BLE sensor");
      delay(1000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting ESP32 Demo");

  BLEDevice::init("ESP32Demo");
  BLEDevice::setPower(ESP_PWR_LVL_P7);

  xTaskCreatePinnedToCore(bleTask, "bleTask", 4096,
    nullptr, 0, nullptr, 1);
}

void loop()
{
  if (millis() - mjHtSensor.updateTime() > 300000) {
    ESP.restart();
  }

  delay(1000);
}
