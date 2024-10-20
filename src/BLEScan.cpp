#include "BLEScan.h"
#include "BLEDeviceList.h"
#include "MACAddress.h"
#include "AppPreferences.h"

extern BLEDeviceList bleDeviceList;
extern AppPreferencesData appPrefs;

BLEScanClass BLEScanner;

void BLEScanClass::setup() {
    Serial.println("Setting up BLE Scanner");
    pBLEScan = BLEDevice::getScan();
    callbacks = new BLEScanAdvertisedDeviceCallbacks();
    start();
    Serial.println("BLE Scanner setup complete");
}

void BLEScanClass::start() {
    Serial.println("Starting BLE Scan task");
    pBLEScan->setAdvertisedDeviceCallbacks(callbacks);
    pBLEScan->setActiveScan(true);
    if (!isScanning) {
        isScanning = true;
        xTaskCreatePinnedToCore(
            [](void* parameter) { static_cast<BLEScanClass*>(parameter)->scan_loop(); },
            "BLE_Scan_Task", 4096, this, 1, &scanTaskHandle, 0);
    }
    Serial.println("BLE Scan task started");
}

void BLEScanClass::stop() {
    Serial.println("Stopping BLE Scan task");
    pBLEScan->stop();
    pBLEScan->setAdvertisedDeviceCallbacks(nullptr);
    if (isScanning) {
        isScanning = false;
        vTaskDelete(scanTaskHandle);
        scanTaskHandle = nullptr;
        pBLEScan->stop();
    }
    Serial.println("BLE Scan task stopped");
}

void BLEScanClass::scan_loop() {
    Serial.println("ScanLoop started");
    while (true) {
        delay(appPrefs.ble_scan_period);
        Serial.println("Starting BLE Scan");
        BLEScanResults foundDevices = pBLEScan->start(appPrefs.ble_scan_duration, true); // Escanea durante x segundos
        Serial.printf("BLE Scan complete. %d devices found.\n", foundDevices.getCount());
    }
}

void BLEScanClass::BLEScanAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    try {
        int rssi = advertisedDevice.getRSSI();

        if (rssi >= appPrefs.minimal_rssi) {
            std::string addressStr = advertisedDevice.getAddress().toString();
            std::string nameStr = advertisedDevice.haveName() ? advertisedDevice.getName() : "";
            std::string appearanceStr = std::to_string(advertisedDevice.getAppearance());
            std::string uuidStr = advertisedDevice.getServiceUUID().toString();
            boolean isPublic = (advertisedDevice.getAddressType() == BLE_ADDR_TYPE_PUBLIC);

            esp_bd_addr_t bleaddr;
            memcpy(bleaddr, advertisedDevice.getAddress().getNative(), sizeof(esp_bd_addr_t));

            Serial.printf("Address: %s, isPublic: %d, Name: '%s', Appearance: %s, Service UUID: %s\n",
                          addressStr.c_str(), isPublic, nameStr.c_str(), appearanceStr.c_str(), uuidStr.c_str());

            if (!appPrefs.ignore_random_ble_addresses || isPublic) {
                bleDeviceList.updateOrAddDevice(MacAddress(bleaddr), rssi, String(nameStr.c_str()), isPublic);
            }
        }
    } catch (const std::exception &e) {
        Serial.printf("Exception in onResult: %s\n", e.what());
    } catch (...) {
        Serial.println("Unknown exception in onResult");
    }
}
