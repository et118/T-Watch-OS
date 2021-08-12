#pragma once
#include <config.h>
class Button {
	private:
		int x1;
		int y1;
		int x2;
		int y2;
		uint32_t fillColor;
		uint32_t outlineColor;

	public:
		Button(int x1, int y1, int x2, int y2, uint32_t fillColor, uint32_t outlineColor) {
			Button::x1 = x1;
			Button::x2 = x2;
			Button::y1 = y1;
			Button::y2 = y2;
			Button::fillColor = fillColor;
			Button::outlineColor = outlineColor;
		};

		void drawButton(TFT_eSPI* tft);

		void changeButtonColor(uint32_t fillColor, uint32_t outlineColor);

		bool isButtonClicked(int x, int y);

};