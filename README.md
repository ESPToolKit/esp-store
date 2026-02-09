# ESPStore

ESPStore is a tiny key/value store built on top of ESPJsonDB. It is designed for **simple configuration blobs** (network settings, device settings, calibration data) where you just want `get()` / `set()` without managing document IDs directly.

Each `ESPStore` instance maps to one ESPJsonDB collection and one store `key`. By default the key is the collection name, but you can pass a custom key to keep multiple stores in the same collection without overwriting each other.

## CI / Release / License
[![CI](https://github.com/ESPToolKit/esp-store/actions/workflows/ci.yml/badge.svg)](https://github.com/ESPToolKit/esp-store/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/ESPToolKit/esp-store?sort=semver)](https://github.com/ESPToolKit/esp-store/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.md)

## Features
- Single-document store per key: simple `get()` and `set()` APIs.
- Built on ESPJsonDB, so autosync and filesystem management are already handled.
- `getOr(...)` helper to return safe defaults when the stored value is missing.
- `setDefault(...)` lets you define defaults before or after `init()`.
- ArduinoJson-based data model so you can store objects, arrays, or primitives.
- Works with both Arduino and ESP-IDF builds (C++17).

## Quick Start

```cpp
#include <ESPJsonDB.h>
#include <ESPStore.h>

ESPJsonDB db;
ESPStore netConf;

void setup() {
    Serial.begin(115200);

    if (!db.init("/db").ok()) {
        Serial.println("DB init failed");
        return;
    }

    netConf.init(&db, "netConf", "wifi");

    JsonDocument cfg;
    cfg["ssid"] = "MyWiFi";
    cfg["password"] = "supersecret";
    cfg["hostname"] = "ESP_DEVICE";

    auto st = netConf.set(cfg.as<JsonVariantConst>());
    Serial.printf("Store set: %s\n", st.ok() ? "OK" : st.message);

    auto res = netConf.get();
    if (!res.ok()) {
        Serial.printf("Failed to load config: %s\n", res.message());
        return;
    }

    serializeJsonPretty(Serial, res.data);
}
```

## Multiple Stores In One Collection

```cpp
ESPStore testStore;
ESPStore exampleStore;

testStore.init(&db, "settings", "test");
exampleStore.init(&db, "settings", "example");
```

Both stores use the same `settings` collection, but each instance reads/writes only its own key.

## Defaults with getOr

```cpp
JsonDocument defaults;
defaults["ssid"] = "";
defaults["password"] = "";
defaults["hostname"] = "ESP_DEVICE";

netConf.setDefault(defaults.as<JsonVariantConst>());

auto res = netConf.getOr();
if (res.ok()) {
    serializeJsonPretty(Serial, res.data);
}
```

## API Summary
- `DbStatus init(ESPJsonDB* db, const char* collection)`
- `DbStatus init(ESPJsonDB* db, const String& collection)`
- `DbStatus init(ESPJsonDB* db, const char* collection, const char* key)`
- `DbStatus init(ESPJsonDB* db, const String& collection, const String& key)`
- `StoreResponse get()`
- `DbStatus setDefault(JsonVariantConst value)`
- `StoreResponse getOr(bool* usedDefault = nullptr)`
- `StoreResponse getOr(JsonVariantConst fallback, bool* usedDefault = nullptr)`
- `DbStatus set(JsonVariantConst value)`
- `DbStatus clear()` (removes only this store key from its collection)
- `DbStatus syncNow()`

## ESPStoreCodec (IP + Date helpers)

`ESPStoreCodec` provides small helpers to encode/decode common embedded types.

```cpp
#include <ESPStoreCodec.h>

JsonDocument doc;
IPAddress ip(192, 168, 1, 42);

ESPStoreCodec::encodeIpString(doc["ip"], ip); // "192.168.1.42"
// or
ESPStoreCodec::encodeIpArray(doc["ipArr"], ip); // [192,168,1,42]

IPAddress decoded;
ESPStoreCodec::decodeIp(doc["ip"], decoded);
ESPStoreCodec::decodeIp(doc["ipArr"], decoded);
```

ESPDate integration is optional. If `ESPDate.h` is available, you can store DateTime in epoch or ISO-8601:

```cpp
ESPDate date;
DateTime now = date.nowUtc();

ESPStoreCodec::encodeDateTimeEpoch(doc["lastSyncEpoch"], now);
ESPStoreCodec::encodeDateTimeIso(doc["lastSyncIso"], now, date);

DateTime parsed{};
ESPStoreCodec::decodeDateTimeEpoch(doc["lastSyncEpoch"], parsed);
ESPStoreCodec::decodeDateTimeIso(doc["lastSyncIso"], parsed, date);
```

LocalDateTime decode helper:

```cpp
LocalDateTime local{};
ESPStoreCodec::encodeLocalDateTime(doc["localTime"], local);
if (ESPStoreCodec::decodeLocalDateTime(doc["localTime"], local)) {
    Serial.printf("Epoch: %lld offset: %d\n",
                  static_cast<long long>(local.utc.epochSeconds),
                  local.offsetMinutes);
}
```

## Examples
- `examples/QuickStart` – basic get/set usage.
- `examples/Defaults` – use `getOr` to return default configuration.
- `examples/CodecAll` – IPAddress string/array plus DateTime epoch/ISO helpers.
- `examples/RuntimeOverrides` – override defaults with runtime values.
- `examples/ClearAndReseed` – clear the store and seed with defaults.
- `examples/LocalDateTime` – store LocalDateTime as `{ epochSeconds, offsetMinutes }`.

## License
MIT — see [LICENSE.md](LICENSE.md).

## ESPToolKit
- Check out other libraries: <https://github.com/orgs/ESPToolKit/repositories>
- Hang out on Discord: <https://discord.gg/WG8sSqAy>
- Support the project: <https://ko-fi.com/esptoolkit>
- Visit the website: <https://www.esptoolkit.hu/>
