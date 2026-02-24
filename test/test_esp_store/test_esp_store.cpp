#include <Arduino.h>
#include <ESPJsonDB.h>
#include <ESPStore.h>
#include <unity.h>

static constexpr const char *kTestDbPath = "/db_store_contract";

static void test_deinit_is_safe_before_init() {
    ESPStore store;
    TEST_ASSERT_FALSE(store.isInitialized());

    store.deinit();
    store.deinit();

    TEST_ASSERT_FALSE(store.isInitialized());

    auto res = store.get();
    TEST_ASSERT_FALSE(res.ok());
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(DbStatusCode::InvalidArgument),
                            static_cast<uint8_t>(res.status.code));
}

static void test_deinit_is_idempotent_after_init() {
    ESPJsonDB db;
    TEST_ASSERT_TRUE(db.init(kTestDbPath).ok());

    ESPStore store;
    TEST_ASSERT_TRUE(store.init(&db, "store_teardown_contract", "idempotent_key").ok());
    TEST_ASSERT_TRUE(store.isInitialized());

    store.deinit();
    TEST_ASSERT_FALSE(store.isInitialized());

    store.deinit();
    TEST_ASSERT_FALSE(store.isInitialized());

    auto res = store.get();
    TEST_ASSERT_FALSE(res.ok());
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(DbStatusCode::InvalidArgument),
                            static_cast<uint8_t>(res.status.code));

    db.deinit();
}

static void test_init_deinit_init_lifecycle() {
    ESPJsonDB db;
    TEST_ASSERT_TRUE(db.init(kTestDbPath).ok());

    ESPStore store;

    JsonDocument defaults;
    defaults["mode"] = "safe";
    TEST_ASSERT_TRUE(store.setDefault(defaults.as<JsonVariantConst>()).ok());

    TEST_ASSERT_TRUE(store.init(&db, "store_teardown_contract", "first_key").ok());
    TEST_ASSERT_TRUE(store.clear().ok());

    bool usedDefault = false;
    auto first = store.getOr(&usedDefault);
    TEST_ASSERT_TRUE(first.ok());
    TEST_ASSERT_TRUE(usedDefault);

    store.deinit();
    TEST_ASSERT_FALSE(store.isInitialized());

    TEST_ASSERT_TRUE(store.init(&db, "store_teardown_contract", "second_key").ok());
    TEST_ASSERT_TRUE(store.isInitialized());
    TEST_ASSERT_TRUE(store.clear().ok());

    usedDefault = false;
    auto withoutDefault = store.getOr(&usedDefault);
    TEST_ASSERT_FALSE(withoutDefault.ok());
    TEST_ASSERT_FALSE(usedDefault);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(DbStatusCode::NotFound),
                            static_cast<uint8_t>(withoutDefault.status.code));

    JsonDocument runtime;
    runtime["retries"] = 3;
    TEST_ASSERT_TRUE(store.set(runtime.as<JsonVariantConst>()).ok());

    auto loaded = store.get();
    TEST_ASSERT_TRUE(loaded.ok());
    TEST_ASSERT_EQUAL_INT(3, loaded.data["retries"] | 0);

    store.deinit();
    db.deinit();
}

void setUp() {
}

void tearDown() {
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_deinit_is_safe_before_init);
    RUN_TEST(test_deinit_is_idempotent_after_init);
    RUN_TEST(test_init_deinit_init_lifecycle);
    UNITY_END();
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
