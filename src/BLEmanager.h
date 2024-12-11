#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID "ce329a3c-b829-42a8-9739-b2c160c3497b"
#define CHARACTERISTIC_UUID "d769bc0a-00f3-481c-8fb7-93dd37d0c3c4"

extern BLEServer* pServer;
extern BLECharacteristic* pCharacteristic;
extern bool deviceConnected;

void setup_ble();
void sendAppState(bool unlocked, int unlockDuration, unsigned long timestamp);

#endif