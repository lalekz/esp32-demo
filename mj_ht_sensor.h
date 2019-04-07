#ifndef MJ_HT_SENSOR_H
#define MJ_HT_SENSOR_H

#include <BLEAddress.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>

class MjHtSensor {
public:
  MjHtSensor(BLEAddress peerAddress);

  bool requestUpdate();
  float temperature() const { return temperature_; }
  float humidity() const { return humidity_; }

private:
  static void notifyCallback(
    BLERemoteCharacteristic* characteristic,
    uint8_t* data,
    size_t length,
    bool isNotify);

private:
  static MjHtSensor* instance_;
  BLEAddress peerAddress_;
  BLEClient* client_;

  FreeRTOS::Semaphore semaphore_;
  float temperature_;
  float humidity_;
};

#endif
