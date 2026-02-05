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

    store.init(&db, "codecDemo");

    JsonDocument doc;

    IPAddress ip(192, 168, 1, 42);
    ESPStoreCodec::encodeIpString(doc["ipString"], ip);
    ESPStoreCodec::encodeIpArray(doc["ipArray"], ip);

    DateTime dt{};
    dt.epochSeconds = 1730000000; // fixed sample epoch
    ESPStoreCodec::encodeDateTimeEpoch(doc["timeEpoch"], dt);
    ESPStoreCodec::encodeDateTimeIso(doc["timeIso"], dt, date);

    LocalDateTime local = date.nowLocal();
    ESPStoreCodec::encodeLocalDateTime(doc["localTime"], local);

    auto st = store.set(doc.as<JsonVariantConst>());
    Serial.printf("Store set: %s\n", st.ok() ? "OK" : st.message);

    auto res = store.get();
    if (!res.ok()) {
        Serial.printf("Store get failed: %s\n", res.message());
        return;
    }

    IPAddress ipStr;
    IPAddress ipArr;
    ESPStoreCodec::decodeIpString(res.data["ipString"], ipStr);
    ESPStoreCodec::decodeIpArray(res.data["ipArray"], ipArr);

    DateTime dtEpoch{};
    DateTime dtIso{};
    ESPStoreCodec::decodeDateTimeEpoch(res.data["timeEpoch"], dtEpoch);
    ESPStoreCodec::decodeDateTimeIso(res.data["timeIso"], dtIso, date);

    LocalDateTime localDecoded{};
    bool localOk = ESPStoreCodec::decodeLocalDateTime(res.data["localTime"], localDecoded);

    Serial.printf("IP string: %s\n", ipStr.toString().c_str());
    Serial.printf("IP array: %s\n", ipArr.toString().c_str());
    Serial.printf("Epoch: %lld\n", static_cast<long long>(dtEpoch.epochSeconds));
    Serial.printf("ISO epoch: %lld\n", static_cast<long long>(dtIso.epochSeconds));
    if (localOk) {
        Serial.printf("Local epoch: %lld offset: %d\n",
                      static_cast<long long>(localDecoded.utc.epochSeconds),
                      localDecoded.offsetMinutes);
    }
}

void loop() {}
