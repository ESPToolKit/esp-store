# Host-side tests are not enabled because ESPStore depends on Arduino runtime
# and ESPJsonDB filesystem integrations that are not available on the default
# host toolchain. Component/PlatformIO tests live under test/test_esp_store
# and should be executed on-device.
message(STATUS "ESPStore: tests are disabled for the host build.")
