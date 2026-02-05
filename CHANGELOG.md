# Changelog

All notable changes to this project are documented in this file.

The format follows Keep a Changelog and the project adheres to Semantic Versioning.

## [Unreleased]
- Added `setDefault()` and the no-argument `getOr()` overload for stored defaults.
- Added `ESPStoreCodec` helpers for IPAddress and ESPDate DateTime (epoch + ISO-8601).
- Added examples for codec usage, runtime overrides, and clearing/reseeding stores.
- Added LocalDateTime codec example using `{ epochSeconds, offsetMinutes }`.
- Added `ESPStoreCodec::decodeLocalDateTime` helper.
- Added `ESPStoreCodec::encodeLocalDateTime` helper.

## [1.0.0] - 2026-02-05
### Added
- Initial release of ESPStore, a single-document key/value store built on ESPJsonDB.
- `get`, `set`, and `getOr` helpers for configuration-style storage.
- Examples for quick start usage and default-value retrieval.

[Unreleased]: https://github.com/ESPToolKit/esp-store/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/ESPToolKit/esp-store/releases/tag/v1.0.0
