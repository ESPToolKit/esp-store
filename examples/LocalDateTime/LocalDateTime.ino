#include <ESPJsonDB.h>
#include <ESPStore.h>
#include <ESPStoreCodec.h>
#include <ESPDate.h>

ESPJsonDB db;
ESPStore store;
ESPDate date;

void setup() {
    Serial.begin(115200);

    if (!db.init("/db").ok()) {
        Serial.println("DB init failed");
        return;
    }

    store.init(&db, "localTimeConf");

    JsonDocument doc;
    LocalDateTime nowLocal = date.nowLocal();
    ESPStoreCodec::encodeLocalDateTime(doc["localTime"], nowLocal);

    auto st = store.set(doc.as<JsonVariantConst>());
    Serial.printf("Store set: %s\n", st.ok() ? "OK" : st.message);

    auto res = store.get();
    if (!res.ok()) {
        Serial.printf("Store get failed: %s\n", res.message());
        return;
    }

    LocalDateTime loaded{};
    if (!ESPStoreCodec::decodeLocalDateTime(res.data["localTime"], loaded)) {
        Serial.println("Failed to decode LocalDateTime");
        return;
    }

    Serial.printf("Loaded epoch: %lld\n", static_cast<long long>(loaded.utc.epochSeconds));
    Serial.printf("Loaded offset minutes: %d\n", loaded.offsetMinutes);
    store.deinit();
}

void loop() {}
