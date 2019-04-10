#include "mj_ht_sensor.h"
#include "telegram_bot.h"

#include <TinyGsmClientSIM800.h>
#include <BLEDevice.h>
#include <BLEAddress.h>
#include <HardwareSerial.h>
#include <StreamString.h>

static const BLEAddress MJHT_ADDRESS("4c:65:a8:d3:e5:bc");
MjHtSensor mjHtSensor(MJHT_ADDRESS);

static const uint8_t MODEM_TX_PIN = 27;
static const uint8_t MODEM_RX_PIN = 25;
HardwareSerial modemSerial(1);
TinyGsmSim800 modem(modemSerial);
TinyGsmSim800::GsmClient gsmClient(modem);

TelegramBot bot(gsmClient);
long lastSendTime = 0;

void checkConnection()
{
  if (!modem.isNetworkConnected()) {
    Serial.println("Initializing modem");
    if (!modem.restart()) {
      Serial.println("Failed to initialize modem");
      ESP.restart();
    }
    Serial.println("Connecting to mobile network");
    if (!modem.waitForNetwork()) {
      Serial.println("Failed to connect to mobile network");
      ESP.restart();
    }
  }

  if (!modem.isGprsConnected()) {
    Serial.println("Connecting to Internet");
    if (!modem.gprsConnect("internet", nullptr, nullptr)) {
      Serial.println("Failed to connect to Internet");
      return;
    }
  }
}

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

  modemSerial.begin(57600, SERIAL_8N1, MODEM_TX_PIN, MODEM_RX_PIN);
  bot.begin();

  BLEDevice::init("ESP32Demo");
  BLEDevice::setPower(ESP_PWR_LVL_P7);

  xTaskCreatePinnedToCore(bleTask, "bleTask", 4096,
    nullptr, 0, nullptr, 1);
}

void loop()
{
  checkConnection();
  bot.processIncomingMessages();

  if (mjHtSensor.available() && mjHtSensor.updateTime() > lastSendTime) {
    StreamString text;
    text.printf("T=%0.2fºC H=%0.2f%%", mjHtSensor.temperature(), mjHtSensor.humidity());
    bot.publish(text);
    lastSendTime = mjHtSensor.updateTime();
  }

  if (millis() - mjHtSensor.updateTime() > 300000) {
    ESP.restart();
  }

  delay(1000);
}
