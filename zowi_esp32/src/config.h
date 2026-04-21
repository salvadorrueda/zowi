#pragma once

// ---- WiFi ---------------------------------------------------------------
// El ESP32 se conecta a esta red como cliente (modo STA).
// La IP asignada por el router aparece en el monitor serie al arrancar.
#define WIFI_SSID     "MiRedWiFi"
#define WIFI_PASSWORD "MiPassword"

// ---- Bluetooth del Zowi -------------------------------------------------
// MAC del módulo BT del Zowi (ver etiqueta del módulo o usar el escáner BT).
// Formato: 6 bytes en orden big-endian.
// Ejemplo para 98:D3:31:B2:DB:76:
const uint8_t ZOWI_BT_MAC[6] = {0x98, 0xD3, 0x31, 0xB2, 0xDB, 0x76};

// PIN Bluetooth del módulo HC-05 (por defecto "1234" o "0000")
#define ZOWI_BT_PIN   "1234"

// Puerto del servidor web
#define HTTP_PORT 80
