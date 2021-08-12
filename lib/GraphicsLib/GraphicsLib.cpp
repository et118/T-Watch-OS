#include "GraphicsLib.h"

void Button::drawButton(TFT_eSPI* tft) {
	tft->fillRect(x1,y1,x2-x1,y2-y1,fillColor);
	tft->drawRect(x1,y1,x2-x1,y2-y1,outlineColor);
}

void Button::changeButtonColor(uint32_t fillColor, uint32_t outlineColor) {
	Button::fillColor = fillColor;
	Button::outlineColor = outlineColor;
}

bool Button::isButtonClicked(int x, int y) {
	if(x > x1 && x < x2 && y > y1 && y < y2) {
		return true;
	} else {
		return false;
	}
}