#include "mj_ht_sensor.h"
#include <BLEDevice.h>
#include <Arduino.h>
#include <cmath>

static BLEUUID SERVICE_UUID("226c0000-6476-4566-7562-66734470666d");
static BLEUUID CHAR_UUID("226caa55-6476-4566-7562-66734470666d");
static BLEUUID DESC_UUID("00002902-0000-1000-8000-00805f9b34fb");

MjHtSensor* MjHtSensor::instance_;

MjHtSensor::MjHtSensor(BLEAddress peerAddress)
  : peerAddress_(peerAddress),
    temperature_(INFINITY), humidity_(INFINITY),
    updateTime_(0)
{
  client_ = BLEDevice::createClient();
  instance_ = this;
}

bool MjHtSensor::available() const
{
  return std::isfinite(temperature_) && std::isfinite(humidity_);
}

bool MjHtSensor::requestUpdate()
{
  if (!client_->connect(peerAddress_)) {
    return false;
  }
  BLERemoteService* service = client_->getService(SERVICE_UUID);
  if (!service) return false;

  BLERemoteCharacteristic* characteristic = service->getCharacteristic(CHAR_UUID);
  if (!characteristic) return false;

  BLERemoteDescriptor* desc = characteristic->getDescriptor(DESC_UUID);
  if (!desc) return false;

  semaphore_.take();
  characteristic->registerForNotify(&MjHtSensor::notifyCallback);
  desc->writeValue((uint8_t*)"\x01\x00", 2, true);
  semaphore_.wait();

  client_->disconnect();
  return true;
}

void MjHtSensor::notifyCallback(
    BLERemoteCharacteristic* characteristic,
    uint8_t* rawData,
    size_t length,
    bool isNotify)
{
  if (!characteristic->getUUID().equals(CHAR_UUID)) return;
  if (length < 1) return;

  std::string data((char*)rawData, length-1);
  float temp, hum;
  if (sscanf(data.c_str(), "T=%f H=%f", &temp, &hum) == 2) {
    instance_->temperature_ = temp;
    instance_->humidity_ = hum;
    instance_->updateTime_ = millis();
    instance_->semaphore_.give();
  }
}
