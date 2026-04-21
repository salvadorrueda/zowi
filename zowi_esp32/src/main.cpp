#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>
#include "config.h"
#include "web_ui.h"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

BluetoothSerial BT;
AsyncWebServer  server(HTTP_PORT);
AsyncWebSocket  ws("/ws");

volatile bool btConnected = false;

// BT receive buffer (collects chars until a complete &&...%% frame arrives)
static String btBuf;

// Dance mode state (ESP32-side autonomous movement)
static bool     danceMode    = false;
static uint32_t danceNext    = 0;
static const uint8_t DANCE_MOVES[] = {1,2,3,4,6,7,8,9,10,12,13,19,20};
static const uint8_t DANCE_COUNT   = sizeof(DANCE_MOVES);

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

void onWsEvent(AsyncWebSocket*, AsyncWebSocketClient*, AwsEventType,
               void*, uint8_t*, size_t);
void handleWsMessage(const char* payload);
void btSend(const char* cmd);
void broadcastStatus();
void broadcastJson(const char* json);
void btReadTask(void*);
void startDance();
void stopDance();

// ---------------------------------------------------------------------------
// Bluetooth helpers
// ---------------------------------------------------------------------------

// Send a command using the Zowi serial protocol: &&CMD ARGS%%\n
void btSend(const char* cmd) {
    if (!btConnected) return;
    BT.print("&&");
    BT.print(cmd);
    BT.println("%%");
}

// ---------------------------------------------------------------------------
// WebSocket helpers
// ---------------------------------------------------------------------------

void broadcastJson(const char* json) {
    ws.textAll(json);
}

void broadcastStatus() {
    char buf[128];
    snprintf(buf, sizeof(buf),
        "{\"type\":\"status\",\"bt\":%s,\"ip\":\"%s\",\"dance\":%s}",
        btConnected ? "true" : "false",
        WiFi.localIP().toString().c_str(),
        danceMode ? "true" : "false");
    broadcastJson(buf);
}

// ---------------------------------------------------------------------------
// WebSocket command dispatcher (JSON → Zowi protocol)
// ---------------------------------------------------------------------------

void handleWsMessage(const char* payload) {
    JsonDocument doc;
    if (deserializeJson(doc, payload) != DeserializationError::Ok) return;

    const char* cmd = doc["cmd"] | "";
    char buf[64];

    if (strcmp(cmd, "S") == 0) {
        stopDance();
        btSend("S");

    } else if (strcmp(cmd, "M") == 0) {
        stopDance();
        int id   = doc["args"][0] | 1;
        int t    = doc["args"][1] | 1000;
        int sz   = doc["args"][2] | 15;
        snprintf(buf, sizeof(buf), "M %d %d %d", id, t, sz);
        btSend(buf);

    } else if (strcmp(cmd, "H") == 0) {
        stopDance();
        snprintf(buf, sizeof(buf), "H %d", (int)(doc["args"][0] | 1));
        btSend(buf);

    } else if (strcmp(cmd, "K") == 0) {
        snprintf(buf, sizeof(buf), "K %d", (int)(doc["args"][0] | 1));
        btSend(buf);

    } else if (strcmp(cmd, "L") == 0) {
        snprintf(buf, sizeof(buf), "L %lu", (unsigned long)(doc["args"][0] | 0));
        btSend(buf);

    } else if (strcmp(cmd, "T") == 0) {
        int freq = doc["args"][0] | 440;
        int dur  = doc["args"][1] | 500;
        snprintf(buf, sizeof(buf), "T %d %d", freq, dur);
        btSend(buf);

    } else if (strcmp(cmd, "D") == 0 || strcmp(cmd, "N") == 0 || strcmp(cmd, "B") == 0) {
        btSend(cmd);   // single-letter sensor requests

    } else if (strcmp(cmd, "DANCE") == 0) {
        bool enable = doc["args"][0] | false;
        enable ? startDance() : stopDance();
        broadcastStatus();
    }
}

// ---------------------------------------------------------------------------
// WebSocket event handler
// ---------------------------------------------------------------------------

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {

    if (type == WS_EVT_CONNECT) {
        broadcastStatus();
        return;
    }

    if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->opcode == WS_TEXT && len > 0) {
            data[len] = '\0';
            handleWsMessage((char*)data);
        }
    }
}

// ---------------------------------------------------------------------------
// FreeRTOS task: read BT responses from the Zowi and forward to WebSocket
// ---------------------------------------------------------------------------

void btReadTask(void*) {
    for (;;) {
        while (BT.available()) {
            char c = BT.read();
            btBuf += c;

            // Detect complete frame: &&...%%
            int start = btBuf.indexOf("&&");
            int end   = btBuf.indexOf("%%");
            if (start >= 0 && end > start) {
                // Extract content between && and %%
                String frame = btBuf.substring(start + 2, end);
                btBuf = btBuf.substring(end + 2);  // consume the frame

                // Ignore ACK frames (A, F) – only forward sensor data
                if (frame.length() >= 2 && frame[0] != 'A' && frame[0] != 'F') {
                    char type_ch = frame[0];
                    String value = (frame.length() > 2) ? frame.substring(2) : "";
                    value.trim();

                    char json[96];
                    if (type_ch == 'D') {
                        snprintf(json, sizeof(json),
                            "{\"type\":\"sensor\",\"key\":\"dist\",\"val\":%.1f}",
                            value.toFloat());
                    } else if (type_ch == 'N') {
                        snprintf(json, sizeof(json),
                            "{\"type\":\"sensor\",\"key\":\"noise\",\"val\":%d}",
                            value.toInt());
                    } else if (type_ch == 'B') {
                        snprintf(json, sizeof(json),
                            "{\"type\":\"sensor\",\"key\":\"bat\",\"val\":%.1f}",
                            value.toFloat());
                    } else if (type_ch == 'E') {
                        // Robot name response
                        snprintf(json, sizeof(json),
                            "{\"type\":\"name\",\"val\":\"%s\"}", value.c_str());
                    } else {
                        continue;  // unknown frame type, skip
                    }
                    broadcastJson(json);
                }

                // Guard against runaway buffer
                if (btBuf.length() > 256) btBuf = "";
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// ---------------------------------------------------------------------------
// Autonomous dance mode (runs in loop())
// ---------------------------------------------------------------------------

void startDance() {
    danceMode = true;
    danceNext = millis();
}

void stopDance() {
    danceMode = false;
    btSend("S");
}

void tickDance() {
    if (!danceMode || !btConnected) return;
    uint32_t now = millis();
    if (now < danceNext) return;

    uint8_t moveId = DANCE_MOVES[random(DANCE_COUNT)];
    int     t      = random(800, 1600);
    int     sz     = random(15, 35);

    char buf[32];
    snprintf(buf, sizeof(buf), "M %d %d %d", moveId, t, sz);
    btSend(buf);

    // Schedule next move after current one finishes (approx 1 step = T ms)
    danceNext = now + t + 200;
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    Serial.println("\n[Zowi Bridge] Booting...");

    // --- WiFi (STA mode) ---
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("[WiFi] Connecting to ");
    Serial.print(WIFI_SSID);
    uint8_t retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 30) {
        delay(500);
        Serial.print('.');
        retries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connected. IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\n[WiFi] Failed – check SSID/password in config.h");
    }

    // --- Bluetooth (master mode) ---
    if (!BT.begin("ZowiBridge", true)) {
        Serial.println("[BT] Failed to initialise Bluetooth");
    } else {
        BT.setPin(ZOWI_BT_PIN);
        Serial.println("[BT] Connecting to Zowi...");
        btConnected = BT.connect(ZOWI_BT_MAC);
        Serial.println(btConnected ? "[BT] Connected!" : "[BT] Not found, will retry in loop.");
    }

    // --- WebSocket ---
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // --- HTTP routes ---
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", HTML_PAGE);
    });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        char buf[128];
        snprintf(buf, sizeof(buf),
            "{\"bt\":%s,\"ip\":\"%s\",\"dance\":%s}",
            btConnected ? "true" : "false",
            WiFi.localIP().toString().c_str(),
            danceMode ? "true" : "false");
        req->send(200, "application/json", buf);
    });

    server.begin();
    Serial.println("[HTTP] Server started on port " + String(HTTP_PORT));

    // --- BT reader task (Core 0) ---
    xTaskCreatePinnedToCore(btReadTask, "BTRead", 4096, nullptr, 1, nullptr, 0);

    Serial.println("[Zowi Bridge] Ready.");
}

// ---------------------------------------------------------------------------
// Loop
// ---------------------------------------------------------------------------

void loop() {
    ws.cleanupClients();

    // Reconnect BT if lost
    if (!BT.connected()) {
        if (btConnected) {
            btConnected = false;
            danceMode   = false;
            broadcastStatus();
            Serial.println("[BT] Connection lost, retrying in 5s...");
        }
        delay(5000);
        btConnected = BT.connect(ZOWI_BT_MAC);
        if (btConnected) {
            Serial.println("[BT] Reconnected.");
            broadcastStatus();
        }
        return;
    }

    tickDance();
}
