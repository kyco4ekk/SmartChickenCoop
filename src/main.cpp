
#include "esp_log.h"

#include "coop_application.h"

namespace
{
    constexpr const char* TAG = "SmartChickenCoop";
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "SmartChickenCoop started");

    CoopConfig config;

    // Временные настройки курятника.
    config.totalBirds = 20;
    config.initialInsideBirds = 15;

    config.line = {
        {120.0f, 50.0f},
        {120.0f, 200.0f}
    };

    config.insideSide = Side::Positive;

    // Статический объект сохраняет время жизни
    // после завершения app_main.
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
