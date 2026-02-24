#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <string>

#include <ESPJsonDB.h>

struct StoreResponse {
	DbStatus status{};
	JsonDocument data;
	const char *error = nullptr;

	bool ok() const { return status.ok(); }
	const char *message() const { return status.message; }

	void setStatus(const DbStatus &st) {
		status = st;
		error = status.ok() ? nullptr : status.message;
	}
};

class ESPStore {
  public:
	ESPStore() = default;
	~ESPStore() { deinit(); }

	DbStatus init(ESPJsonDB *db, const char *collection);
	DbStatus init(ESPJsonDB *db, const String &collection);
	DbStatus init(ESPJsonDB *db, const char *collection, const char *key);
	DbStatus init(ESPJsonDB *db, const String &collection, const String &key);
	void deinit();
	bool isInitialized() const { return _initialized; }

	DbStatus setDefault(JsonVariantConst value);
	StoreResponse get();
	StoreResponse getOr(bool *usedDefault = nullptr);
	StoreResponse getOr(JsonVariantConst fallback, bool *usedDefault = nullptr);

	DbStatus set(JsonVariantConst value);
	DbStatus clear();
	DbStatus syncNow();

	const std::string &collection() const { return _collection; }
	const std::string &key() const { return _key; }

  private:
	ESPJsonDB *_db = nullptr;
	std::string _collection;
	std::string _key;
	JsonDocument _defaultDoc;
	bool _hasDefault = false;
	bool _initialized = false;

	DbStatus ensureReady() const;
	DbStatus registerSchema();
};
