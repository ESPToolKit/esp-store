#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IPAddress.h>
#include <stdint.h>

#if defined(__has_include)
#if __has_include(<ESPDate.h>)
#define ESP_STORE_HAS_ESPDATE 1
#include <ESPDate.h>
#endif
#endif

class ESPStoreCodec {
  public:
	static bool encodeIpString(JsonVariant dst, const IPAddress &ip) {
		if (dst.isNull()) {
			dst.set(ip.toString());
			return true;
		}
		dst.set(ip.toString());
		return true;
	}

	static bool encodeIpArray(JsonVariant dst, const IPAddress &ip) {
		JsonArray arr = dst.to<JsonArray>();
		arr.clear();
		arr.add(ip[0]);
		arr.add(ip[1]);
		arr.add(ip[2]);
		arr.add(ip[3]);
		return true;
	}

	static bool decodeIpString(JsonVariantConst src, IPAddress &out) {
		if (src.isNull()) return false;
		const char *str = nullptr;
		if (src.is<const char *>()) {
			str = src.as<const char *>();
		} else if (src.is<String>()) {
			str = src.as<String>().c_str();
		} else {
			return false;
		}
		if (!str || !*str) return false;
		return out.fromString(str);
	}

	static bool decodeIpArray(JsonVariantConst src, IPAddress &out) {
		if (!src.is<JsonArrayConst>()) return false;
		JsonArrayConst arr = src.as<JsonArrayConst>();
		if (arr.size() != 4) return false;
		uint8_t octets[4];
		for (size_t i = 0; i < 4; ++i) {
			JsonVariantConst v = arr[i];
			if (!v.is<uint8_t>() && !v.is<int>()) return false;
			int val = v.as<int>();
			if (val < 0 || val > 255) return false;
			octets[i] = static_cast<uint8_t>(val);
		}
		out = IPAddress(octets[0], octets[1], octets[2], octets[3]);
		return true;
	}

	static bool decodeIp(JsonVariantConst src, IPAddress &out) {
		if (decodeIpString(src, out)) return true;
		return decodeIpArray(src, out);
	}

	static bool encodeEpochSeconds(JsonVariant dst, int64_t epochSeconds) {
		dst.set(epochSeconds);
		return true;
	}

	static bool decodeEpochSeconds(JsonVariantConst src, int64_t &out) {
		if (src.isNull()) return false;
		if (!(src.is<int64_t>() || src.is<long>() || src.is<int>())) return false;
		out = src.as<int64_t>();
		return true;
	}

#if defined(ESP_STORE_HAS_ESPDATE)
	static bool encodeDateTimeEpoch(JsonVariant dst, const DateTime &dt) {
		return encodeEpochSeconds(dst, dt.epochSeconds);
	}

	static bool decodeDateTimeEpoch(JsonVariantConst src, DateTime &out) {
		int64_t epoch = 0;
		if (!decodeEpochSeconds(src, epoch)) return false;
		out.epochSeconds = epoch;
		return true;
	}

	static bool encodeDateTimeIso(JsonVariant dst, const DateTime &dt, ESPDate &date) {
		char buf[32];
		if (!date.formatUtc(dt, ESPDateFormat::Iso8601, buf, sizeof(buf))) return false;
		dst.set(buf);
		return true;
	}

	static bool decodeDateTimeIso(JsonVariantConst src, DateTime &out, ESPDate &date) {
		if (src.isNull()) return false;
		const char *str = nullptr;
		if (src.is<const char *>()) {
			str = src.as<const char *>();
		} else if (src.is<String>()) {
			str = src.as<String>().c_str();
		} else {
			return false;
		}
		if (!str || !*str) return false;
		auto parsed = date.parseIso8601Utc(str);
		if (!parsed.ok) return false;
		out = parsed.value;
		return true;
	}

	static bool encodeLocalDateTime(JsonVariant dst, const LocalDateTime &value) {
		JsonObject obj = dst.to<JsonObject>();
		obj.clear();
		obj["epochSeconds"] = value.utc.epochSeconds;
		obj["offsetMinutes"] = value.offsetMinutes;
		return true;
	}

	static bool decodeLocalDateTime(JsonVariantConst src, LocalDateTime &out) {
		if (!src.is<JsonObjectConst>()) return false;
		JsonObjectConst obj = src.as<JsonObjectConst>();
		if (!obj.containsKey("epochSeconds") || !obj.containsKey("offsetMinutes")) return false;
		int64_t epoch = 0;
		if (!decodeEpochSeconds(obj["epochSeconds"], epoch)) return false;
		out.utc.epochSeconds = epoch;
		out.offsetMinutes = obj["offsetMinutes"].as<int>();
		out.ok = true;
		return true;
	}
#endif
};
