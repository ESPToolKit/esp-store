#include <ESPJsonDB.h>
#include <ESPStore.h>

ESPJsonDB db;
ESPStore netConf;

void setup() {
    Serial.begin(115200);

    JsonDocument defaults;
    defaults["ssid"] = "";
    defaults["password"] = "";
    defaults["hostname"] = "ESP_DEVICE";
    defaults["autoReconnect"] = true;
    netConf.setDefault(defaults.as<JsonVariantConst>());

    if (!db.init("/db").ok()) {
        Serial.println("DB init failed");
        return;
    }

    netConf.init(&db, "netConf");

    bool usedDefault = false;
    auto res = netConf.getOr(&usedDefault);

    if (!res.ok()) {
        Serial.printf("Failed to read config: %s\n", res.message());
        return;
    }

    Serial.printf("Config source: %s\n", usedDefault ? "default" : "stored");
    serializeJsonPretty(Serial, res.data);
}

void loop() {}
