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
    netConf.setDefault(defaults.as<JsonVariantConst>());

    if (!db.init("/db").ok()) {
        Serial.println("DB init failed");
        return;
    }

    netConf.init(&db, "netConf");

    bool usedDefault = false;
    auto res = netConf.getOr(&usedDefault);
    Serial.printf("Initial getOr: %s (%s)\n",
                  res.ok() ? "OK" : res.message(),
                  usedDefault ? "default" : "stored");

    JsonDocument runtime;
    runtime["ssid"] = "RuntimeWiFi";
    runtime["password"] = "secret";
    runtime["hostname"] = "ESP_RUNTIME";

    auto st = netConf.set(runtime.as<JsonVariantConst>());
    Serial.printf("Runtime set: %s\n", st.ok() ? "OK" : st.message);

    usedDefault = false;
    auto res2 = netConf.getOr(&usedDefault);
    Serial.printf("After set getOr: %s (%s)\n",
                  res2.ok() ? "OK" : res2.message(),
                  usedDefault ? "default" : "stored");

    serializeJsonPretty(res2.data, Serial);
}

void loop() {}
