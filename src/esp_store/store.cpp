#include "store.h"

static const char *kErrNotReady = "store not initialized";
static const char *kErrBadCollection = "invalid collection";
static const char *kErrBadKey = "invalid key";
static const char *kErrValueMissing = "stored value missing";
static const char *kMsgDefaultUsed = "default value used";

DbStatus ESPStore::init(ESPJsonDB *db, const char *collection) {
	return init(db, collection, collection);
}

DbStatus ESPStore::init(ESPJsonDB *db, const String &collection) {
	return init(db, collection.c_str(), collection.c_str());
}

DbStatus ESPStore::init(ESPJsonDB *db, const char *collection, const char *key) {
	if (!db || !collection || !*collection) {
		return {DbStatusCode::InvalidArgument, kErrBadCollection};
	}
	if (!key || !*key) {
		return {DbStatusCode::InvalidArgument, kErrBadKey};
	}
	_db = db;
	_collection.assign(collection);
	_key.assign(key);
	_initialized = false;

	const DbStatus st = registerSchema();
	if (!st.ok()) {
		_db = nullptr;
		_collection.clear();
		_key.clear();
		return st;
	}

	_initialized = true;
	return st;
}

DbStatus ESPStore::init(ESPJsonDB *db, const String &collection, const String &key) {
	return init(db, collection.c_str(), key.c_str());
}

void ESPStore::deinit() {
	if (!_initialized && _db == nullptr && _collection.empty() && _key.empty() && !_hasDefault) {
		return;
	}

	_db = nullptr;
	_initialized = false;
	std::string().swap(_collection);
	std::string().swap(_key);
	_defaultDoc = JsonDocument();
	_hasDefault = false;
}

DbStatus ESPStore::ensureReady() const {
	if (!_initialized || !_db || _collection.empty() || _key.empty()) {
		return {DbStatusCode::InvalidArgument, kErrNotReady};
	}
	return {DbStatusCode::Ok, ""};
}

DbStatus ESPStore::registerSchema() {
	Schema s;
	s.fields = {
		{"key", FieldType::String, nullptr, true},
	};
	return _db->registerSchema(_collection, s);
}

DbStatus ESPStore::setDefault(JsonVariantConst value) {
	_defaultDoc.clear();
	_defaultDoc.set(value);
	_hasDefault = true;
	return {DbStatusCode::Ok, ""};
}

StoreResponse ESPStore::get() {
	StoreResponse res;
	auto ready = ensureReady();
	if (!ready.ok()) {
		res.setStatus(ready);
		return res;
	}

	JsonDocument filter;
	filter["key"] = _key.c_str();
	auto found = _db->findOne(_collection, filter);
	if (!found.status.ok()) {
		res.setStatus(found.status);
		return res;
	}

	JsonVariantConst value = found.value["value"];
	if (value.isNull()) {
		res.setStatus({DbStatusCode::NotFound, kErrValueMissing});
		return res;
	}

	res.data.set(value);
	res.setStatus({DbStatusCode::Ok, ""});
	return res;
}

StoreResponse ESPStore::getOr(bool *usedDefault) {
	if (!_hasDefault) {
		auto res = get();
		if (usedDefault) *usedDefault = false;
		return res;
	}
	return getOr(_defaultDoc.as<JsonVariantConst>(), usedDefault);
}

StoreResponse ESPStore::getOr(JsonVariantConst fallback, bool *usedDefault) {
	StoreResponse res = get();
	if (res.ok()) {
		if (usedDefault) *usedDefault = false;
		return res;
	}

	if (res.status.code == DbStatusCode::NotFound) {
		res.data.clear();
		res.data.set(fallback);
		res.setStatus({DbStatusCode::Ok, kMsgDefaultUsed});
		if (usedDefault) *usedDefault = true;
		return res;
	}

	if (usedDefault) *usedDefault = false;
	return res;
}

DbStatus ESPStore::set(JsonVariantConst value) {
	auto ready = ensureReady();
	if (!ready.ok()) return ready;

	JsonDocument filter;
	filter["key"] = _key.c_str();

	JsonDocument patch;
	patch["key"] = _key.c_str();
	patch["value"].set(value);

	return _db->updateOne(_collection, filter, patch, true);
}

DbStatus ESPStore::clear() {
	auto ready = ensureReady();
	if (!ready.ok()) return ready;
	auto removed = _db->removeMany(_collection, [this](const DocView &doc) {
		return doc["key"].as<std::string>() == _key;
	});
	return removed.status;
}

DbStatus ESPStore::syncNow() {
	auto ready = ensureReady();
	if (!ready.ok()) return ready;
	return _db->syncNow();
}
