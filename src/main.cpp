
#include <inttypes.h>
#include <cstring>

#include "esp_log.h"
#include "esp_psram.h"
#include "esp_heap_caps.h"

#include "coop_application.h"

namespace
{
    constexpr const char* TAG = "SmartChickenCoop";

    constexpr size_t PSRAM_TEST_SIZE = 1024 * 1024;

    void CheckMemory()
    {
        ESP_LOGI(TAG, "Internal free heap: %zu bytes",
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL));

        if (!esp_psram_is_initialized())
        {
            ESP_LOGE(TAG, "PSRAM initialization failed");
            return;
        }

        const size_t psramSize = esp_psram_get_size();

        ESP_LOGI(TAG, "PSRAM initialized");
        ESP_LOGI(TAG, "PSRAM detected: %zu bytes (%.2f MB)",
                 psramSize,
                 static_cast<double>(psramSize) / (1024.0 * 1024.0));

        ESP_LOGI(TAG, "PSRAM free heap: %zu bytes",
                 heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

        ESP_LOGI(TAG, "PSRAM largest free block: %zu bytes",
                 heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));

        void* memory = heap_caps_malloc(
            PSRAM_TEST_SIZE,
            MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
        );

        if (memory == nullptr)
        {
            ESP_LOGE(TAG, "Failed to allocate 1 MB in PSRAM");
            return;
        }

        std::memset(memory, 0xA5, PSRAM_TEST_SIZE);

        const auto* bytes = static_cast<const uint8_t*>(memory);

        const bool memoryValid =
            bytes[0] == 0xA5 &&
            bytes[PSRAM_TEST_SIZE / 2] == 0xA5 &&
            bytes[PSRAM_TEST_SIZE - 1] == 0xA5;

        heap_caps_free(memory);

        if (memoryValid)
        {
            ESP_LOGI(TAG, "PSRAM allocation and read/write test passed");
        }
        else
        {
            ESP_LOGE(TAG, "PSRAM read/write test failed");
        }
    }
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "SmartChickenCoop started");

    CheckMemory();

    CoopConfig config;

    // Временные настройки курятника.
    // Позже значения будут задаваться пользователем.
    config.totalBirds = 20;
    config.initialInsideBirds = 15;

    config.line = {
        {120.0f, 50.0f},
        {120.0f, 200.0f}
    };

    config.insideSide = Side::Positive;

    static CoopApplication application(config);

    if (!application.IsValid())
    {
        ESP_LOGE(TAG, "Invalid coop configuration");
        return;
    }

    const BirdCounter& counter = application.GetCounter();

    ESP_LOGI(TAG, "Application initialized");
    ESP_LOGI(TAG, "Total birds: %d", counter.GetTotal());
    ESP_LOGI(TAG, "Inside: %d", counter.GetInside());
    ESP_LOGI(TAG, "Outside: %d", counter.GetOutside());
}