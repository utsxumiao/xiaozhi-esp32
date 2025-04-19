#include "iot/thing.h"
#include "board.h"
#include "led/circular_strip.h"

#include <esp_log.h>

#define TAG "LedStrip"

namespace iot {

class LedStrip : public Thing {
private:
    gpio_num_t gpio_num_ = GPIO_NUM_17;
    const uint8_t LED_COUNT = 16;
    CircularStrip* led_strip_ = nullptr;
    bool power_ = false;
    int brightness_ = 50;
    std::string color_ = "white";

    void InitializeLedStrip() {
        led_strip_ = new CircularStrip(gpio_num_, LED_COUNT);
        led_strip_->SetBrightness(DEFAULT_BRIGHTNESS, LOW_BRIGHTNESS);
        
        // Turn off initially
        StripColor black = {0, 0, 0};
        led_strip_->SetAllColor(black);
    }

    void UpdateLedStrip() {
        if (!led_strip_) return;
        
        if (!power_) {
            // Turn off all LEDs
            StripColor black = {0, 0, 0};
            led_strip_->SetAllColor(black);
            return;
        }
        
        // Calculate brightness level (0-255)
        uint8_t level = (brightness_ * 255) / 100;
        
        // Set color based on the current color name
        StripColor color = {0, 0, 0};
        if (color_ == "white") {
            color = {level, level, level};
        } else if (color_ == "red") {
            color = {level, 0, 0};
        } else if (color_ == "green") {
            color = {0, level, 0};
        } else if (color_ == "blue") {
            color = {0, 0, level};
        } else if (color_ == "yellow") {
            color = {level, level, 0};
        } else if (color_ == "purple") {
            color = {level, 0, level};
        } else if (color_ == "cyan") {
            color = {0, level, level};
        }
        
        led_strip_->SetAllColor(color);
    }

public:
    LedStrip() : Thing("LedStrip", "可控制的LED灯带"), power_(false), brightness_(50), color_("white") {
        InitializeLedStrip();

        // 定义设备的属性
        properties_.AddBooleanProperty("power", "灯带是否打开", [this]() -> bool {
            return power_;
        });
        
        properties_.AddNumberProperty("brightness", "灯带亮度 (0-100)", [this]() -> int {
            return brightness_;
        });
        
        properties_.AddStringProperty("color", "灯带颜色", [this]() -> std::string {
            return color_;
        });

        // 定义设备可以被远程执行的指令
        methods_.AddMethod("TurnOn", "打开灯带", ParameterList(), [this](const ParameterList& parameters) {
            power_ = true;
            UpdateLedStrip();
        });

        methods_.AddMethod("TurnOff", "关闭灯带", ParameterList(), [this](const ParameterList& parameters) {
            power_ = false;
            UpdateLedStrip();
        });
        
        // 定义设备可以被远程执行的指令
        methods_.AddMethod("SetBrightness", "设置灯带亮度", ParameterList({
            Parameter("level", "亮度级别 (0-100)", kValueTypeNumber, true)
        }), [this](const ParameterList& parameters) {
                uint8_t level = static_cast<uint8_t>(parameters["level"].number());
                if (level > 100) level = 100;
                brightness_ = level;
                UpdateLedStrip();
            }
        );
        
        methods_.AddMethod("SetColor", "设置灯带颜色", ParameterList({
            Parameter("color", "颜色名称 (white, red, green, blue, yellow, purple, cyan)", kValueTypeString, true)
        }), [this](const ParameterList& parameters) {
                std::string color = parameters["color"].string();
                color_ = color;
                UpdateLedStrip();
            }
        );

        methods_.AddMethod("StartBlink", "开始频闪效果", ParameterList(), [this](const ParameterList& parameters) {
            if (!led_strip_) return;
            power_ = true;

            uint8_t level = (brightness_ * 255) / 100;
            StripColor color = {level, level, level};

            if (color_ == "red") {
                color = {level, 0, 0};
            } else if (color_ == "green") {
                color = {0, level, 0};
            } else if (color_ == "blue") {
                color = {0, 0, level};
            } else if (color_ == "yellow") {
                color = {level, level, 0};
            } else if (color_ == "purple") {
                color = {level, 0, level};
            } else if (color_ == "cyan") {
                color = {0, level, level};
            }

            led_strip_->Blink(color, 1000);
        });
        
        methods_.AddMethod("StartBreathing", "开始呼吸效果", ParameterList(), [this](const ParameterList& parameters) {
            if (!led_strip_) return;
            power_ = true;
            
            StripColor low = {0, 0, 0};
            uint8_t level = (brightness_ * 255) / 100;
            StripColor high = {level, level, level};
            
            if (color_ == "red") {
                high = {level, 0, 0};
            } else if (color_ == "green") {
                high = {0, level, 0};
            } else if (color_ == "blue") {
                high = {0, 0, level};
            } else if (color_ == "yellow") {
                high = {level, level, 0};
            } else if (color_ == "purple") {
                high = {level, 0, level};
            } else if (color_ == "cyan") {
                high = {0, level, level};
            }
            
            led_strip_->Breathe(low, high, 50);
        });
        
        methods_.AddMethod("StartScrolling", "开始滚动效果", ParameterList(), [this](const ParameterList& parameters) {
            if (!led_strip_) return;
            power_ = true;
            
            StripColor low = {0, 0, 0};
            uint8_t level = (brightness_ * 255) / 100;
            StripColor high = {level, level, level};
            
            if (color_ == "red") {
                high = {level, 0, 0};
            } else if (color_ == "green") {
                high = {0, level, 0};
            } else if (color_ == "blue") {
                high = {0, 0, level};
            } else if (color_ == "yellow") {
                high = {level, level, 0};
            } else if (color_ == "purple") {
                high = {level, 0, level};
            } else if (color_ == "cyan") {
                high = {0, level, level};
            }
            
            led_strip_->Scroll(low, high, 3, 100);
        });
    }
};

} // namespace iot

DECLARE_THING(LedStrip);