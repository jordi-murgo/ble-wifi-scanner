#ifndef APP_PREFERENCES_H
#define APP_PREFERENCES_H

#include <Preferences.h>
#include <Arduino.h>

// Estructura para almacenar las preferencias
struct AppPreferencesData {
    // General
    char device_name[32];
    uint8_t operation_mode;             // 0-OFF, 1-SCAN_MODE, 2-DETECTION_MODE
    uint32_t autosave_interval;
    int8_t minimal_rssi;
    bool passive_scan;
    bool stealth_mode;
    // Wifi
    bool only_management_frames;
    uint32_t loop_delay;
    // BLE
    uint32_t ble_scan_period;
    bool ignore_random_ble_addresses;  
    uint32_t ble_scan_duration;        
};

const int8_t OPERATION_MODE_OFF = 0;
const int8_t OPERATION_MODE_SCAN = 1;
const int8_t OPERATION_MODE_DETECTION = 2;

// Declaraciones de funciones
void initAppPreferences();
void loadAppPreferences();
void saveAppPreferences();

// Variables externas
extern AppPreferencesData appPrefs;
extern Preferences preferences;

#endif // APP_PREFERENCES_H
