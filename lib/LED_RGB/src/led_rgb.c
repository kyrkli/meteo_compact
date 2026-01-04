#include "led_rgb.h"
#include "IO_Macros.h"

humidity_level get_humidity_level(double hum){
	if(hum <= 20) return HUM_VERY_DRY;
	else if(hum <= 40) return HUM_DRY;
	else if(hum <= 60) return HUM_COMFORT;
	else if(hum <= 75) return HUM_HUMID;
	else return HUM_VERY_HUMID;
}

void rgb_hum_level(humidity_level hum_level){
	switch (hum_level)
	{
		case HUM_VERY_DRY: 	// red
			DigitalWrite(LED_RED_PIN, High);
			DigitalWrite(LED_GREEN_PIN, Low);
			DigitalWrite(LED_BLUE_PIN, Low);
			break;
		case HUM_DRY: 		// yellow
			DigitalWrite(LED_RED_PIN, High);
			DigitalWrite(LED_GREEN_PIN, High);
			DigitalWrite(LED_BLUE_PIN, Low);
			break;
		case HUM_COMFORT: 	// green
			DigitalWrite(LED_RED_PIN, Low);
			DigitalWrite(LED_GREEN_PIN, High);
			DigitalWrite(LED_BLUE_PIN, Low);
			break;
		case HUM_HUMID:		// cyan
			DigitalWrite(LED_RED_PIN, Low);
			DigitalWrite(LED_GREEN_PIN, High);
			DigitalWrite(LED_BLUE_PIN, High);
			break;
		case HUM_VERY_HUMID: // blue
			DigitalWrite(LED_RED_PIN, Low);
			DigitalWrite(LED_GREEN_PIN, Low);
			DigitalWrite(LED_BLUE_PIN, High);
			break;
		default: 			// purple (error state)
			DigitalWrite(LED_RED_PIN, High);
			DigitalWrite(LED_GREEN_PIN, Low);
			DigitalWrite(LED_BLUE_PIN, High);
			break;
	}
}