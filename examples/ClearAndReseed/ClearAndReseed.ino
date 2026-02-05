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

    store.init(&db, "systemConf");

    JsonDocument cfg;
    cfg["mode"] = "normal";
    cfg["retries"] = 3;

    auto st = store.set(cfg.as<JsonVariantConst>());
    Serial.printf("Seed set: %s\n", st.ok() ? "OK" : st.message);

    st = store.syncNow();
    Serial.printf("Sync: %s\n", st.ok() ? "OK" : st.message);

    st = store.clear();
    Serial.printf("Clear: %s\n", st.ok() ? "OK" : st.message);

    JsonDocument fallback;
    fallback["mode"] = "safe";
    fallback["retries"] = 1;

    bool usedDefault = false;
    auto res = store.getOr(fallback.as<JsonVariantConst>(), &usedDefault);
    Serial.printf("After clear getOr: %s (%s)\n",
                  res.ok() ? "OK" : res.message(),
                  usedDefault ? "default" : "stored");

    serializeJsonPretty(res.data, Serial);
}

void loop() {}
