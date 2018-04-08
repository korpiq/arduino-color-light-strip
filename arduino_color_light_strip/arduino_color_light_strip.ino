#include <FastLED.h>

#define COLOR_LEDS_COUNT 240
#define COLOR_LEDS_PIN 12
#define DELAY 50
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
	if (++stripe_start > COLOR_LEDS_COUNT)
		stripe_start = -STRIPE_WIDTH;

	light_stripe(stripe_start);
	delay(DELAY);
}

void light_stripe(int start_index) {
	paint_strip(ColorFromPalette(color_palette, 0));

	for (int offset = 0; offset < STRIPE_WIDTH; ++offset) {
		int led_at = start_index + offset;

		if (led_at > 0 && led_at < COLOR_LEDS_COUNT) {
			uint8_t color = (256 / STRIPE_WIDTH) * offset;
			leds[led_at] = ColorFromPalette(color_palette, color);
		}
	}

	FastLED.show();
}

void paint_strip(CRGB color) {
	leds(0, COLOR_LEDS_COUNT - 1) = color;
}
