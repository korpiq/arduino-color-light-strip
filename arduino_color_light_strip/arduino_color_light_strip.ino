#include <FastLED.h>

#define COLOR_LEDS_COUNT 240
#define COLOR_LEDS_PIN 12
#define DELAY 10
#define STRIPE_WIDTH 16

CRGBArray<COLOR_LEDS_COUNT> leds;

DEFINE_GRADIENT_PALETTE(dim_map_gp) {
	0, 16, 7, 1,
	32, 32, 16, 2,
	64, 128, 64, 8,
	96, 255, 160, 40,
	120, 255, 160, 64,
	136, 255, 160, 64,
	160, 255, 160, 40,
	192, 128, 64, 8,
	224, 32, 16, 2,
	255, 16, 7, 1
};

CRGBPalette256 color_palette = dim_map_gp;

void setup()
{
	FastLED.addLeds<NEOPIXEL, COLOR_LEDS_PIN>(leds, COLOR_LEDS_COUNT);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
}

int stripe_start = -STRIPE_WIDTH;

void loop()
{
	uint32_t milliseconds = millis();
	light_strip(milliseconds);

	uint32_t delay_ms = DELAY - (millis() - milliseconds);
	delay(delay_ms < DELAY ? delay_ms : DELAY);
}

void light_strip(uint32_t milliseconds) {
	for (int led_at = 0; led_at < COLOR_LEDS_COUNT; ++led_at) {
		leds[led_at] = color_for_led(led_at, milliseconds);
	}

	FastLED.show();
}

CRGB color_for_led(uint16_t led_index, uint32_t milliseconds) {
	uint32_t frame_number = milliseconds / 10; // 100 fps
	uint32_t led_index_uint32 = led_index;
	uint32_t led_frame_number = frame_number + led_index_uint32; // loop over whole color palette
	uint8_t color_index = has_led_base_color(led_index) ? 0 : led_frame_number;

	return led_flash(led_index, milliseconds, ColorFromPalette(color_palette, color_index));
}

bool has_led_base_color(uint16_t led_index) {
	return led_index & 7;
}

uint16_t led_flash_get_next_index() {
	return (random() % COLOR_LEDS_COUNT);
}

uint32_t led_flash_last_time = 0, led_flash_duration = 1000, led_flash_interval = 1000;
uint16_t led_flash_current_index = -1, led_flash_next_index = led_flash_get_next_index();
CRGB led_flash_color = CRGB(255,96,32); // ColorFromPalette(color_palette, 128);
CRGB led_flash_current_color = CRGB();

CRGB led_flash(uint16_t led_index, uint32_t milliseconds, CRGB led_default_color) {

	if (led_index == led_flash_current_index) {
		if (milliseconds < led_flash_last_time + led_flash_duration) {
			uint32_t gone_milliseconds = milliseconds - led_flash_last_time;
			uint32_t gone_fraction = 256UL * gone_milliseconds / led_flash_duration;
			uint8_t gone_fract8 = (uint8_t) gone_fraction;

			led_flash_current_color.red = lerp8by8(led_flash_color.red, led_default_color.red, gone_fract8);
			led_flash_current_color.green = lerp8by8(led_flash_color.green, led_default_color.green, gone_fract8);
			led_flash_current_color.blue = lerp8by8(led_flash_color.blue, led_default_color.blue, gone_fract8);

			return led_flash_current_color;
		}
		led_flash_current_index = -1;
	}
	
	if (led_index == led_flash_next_index) {
		if (milliseconds > led_flash_last_time + led_flash_interval) {
			led_flash_last_time = milliseconds;
			led_flash_current_index = led_index;
			led_flash_next_index = led_flash_get_next_index();

			return led_flash_color;
		}
	} 

	return led_default_color;
}
