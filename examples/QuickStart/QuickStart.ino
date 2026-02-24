#include <ESPJsonDB.h>
#include <ESPStore.h>

ESPJsonDB db;
ESPStore store;

void setup() {
    Serial.begin(115200);

    if (!db.init("/db").ok()) {
        Serial.println("DB init failed");
        return;
    }

    store.init(&db, "netConf");

    JsonDocument cfg;
    cfg["ssid"] = "MyWiFi";
    cfg["password"] = "supersecret";
    cfg["hostname"] = "ESP_DEVICE";

    auto st = store.set(cfg.as<JsonVariantConst>());
    Serial.printf("Store set: %s\n", st.ok() ? "OK" : st.message);

    auto res = store.get();
    if (!res.ok()) {
        Serial.printf("Failed to load config: %s\n", res.message());
        return;
    }

    serializeJsonPretty(res.data, Serial);
    store.deinit();
}

void loop() {}
