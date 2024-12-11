#include "BLEManager.h"

#include "main.h"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

int stateDebounceDelay = 500;
unsigned long lastStateUpdate = 0;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected");
    deviceConnected = true;
    BLEDevice::startAdvertising();
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("Received Command:");
      Serial.println(value.c_str());  // Print the raw JSON string

      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, value);

      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
      }

      if (!unlocked) {
        Serial.println("Terminal needs to be unlocked to change configuration");
        return;
      }

      if (doc["unlock_duration"].is<int>()) {
        unsigned long newUnlockDuration =
            doc["unlock_duration"].as<unsigned long>();
        unlockDuration = newUnlockDuration;
        Serial.print("Updated unlock_duration to: ");
        Serial.println(unlockDuration);
      }

      if (doc["password"].is<const char*>()) {
        String newPassword = doc["password"].as<String>();
        changePassword(newPassword);
        Serial.print("Updated password to: ");
        Serial.println(newPassword);
      }
    }
  }
};

void setup_ble() {
  BLEDevice::init("ESP32 BLE Control");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                               BLECharacteristic::PROPERTY_WRITE |
                               BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  // start the BLE service
  pService->start();

  BLEDevice::startAdvertising();

  Serial.println("Bluetooth device is ready to pair");
}

void sendAppState(bool unlocked, int unlockDuration, unsigned long timestamp) {
  if (timestamp - stateDebounceDelay < lastStateUpdate) {
    return;
  }
  JsonDocument doc;
  doc["unlocked"] = unlocked;
  doc["unlock_duration"] = unlockDuration;
  doc["timestamp"] = timestamp;

  String jsonStr;
  serializeJson(doc, jsonStr);

  pCharacteristic->setValue(jsonStr.c_str());
  pCharacteristic->notify();

  lastStateUpdate = timestamp;
}