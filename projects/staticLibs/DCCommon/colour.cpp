#include "colour.h"
#include "utilities.h"
#include <algorithm>

namespace DC
{
	Colour::Colour()
	{
		set(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Colour::Colour(float Red, float Green, float Blue, float Alpha)
	{
		set(Red, Green, Blue, Alpha);
	}

	void Colour::set(float Red, float Green, float Blue, float Alpha)
	{
		clamp(Red, 0.0f, 1.0f);
		clamp(Green, 0.0f, 1.0f);
		clamp(Blue, 0.0f, 1.0f);
		clamp(Alpha, 0.0f, 1.0f);
		red = Red;
		green = Green;
		blue = Blue;
		alpha = Alpha;
	}

	void Colour::setFromHSB(float HueAmount, float Saturation, float Brightness)
	{
		clamp(HueAmount, 0.0f, 1.0f);
		clamp(Saturation, 0.0f, 1.0f);
		clamp(Brightness, 0.0f, 1.0f);

		// Compute colour
		HueAmount *= 360.0f;
		float fReciprical60 = 1.0f / 60.0f;
		if (HueAmount <= 60.0f)	// Inc green
		{
			red = 1.0f;
			green = HueAmount * fReciprical60;
			blue = 0.0f;
		}
		else if (HueAmount <= 120.0f)	// Dec red
		{
			red = 1.0f - ((HueAmount - 60.0f) * fReciprical60);
			green = 1.0f;
			blue = 0.0f;
		}
		else if (HueAmount <= 180.0f)	// Inc blue
		{
			red = 0.0f;
			green = 1.0f;
			blue = (HueAmount - 120.0f) * fReciprical60;
		}
		else if (HueAmount <= 240.0f)	// Dec green
		{
			red = 0.0f;
			green = 1.0f - ((HueAmount - 180.0f) * fReciprical60);
			blue = 1.0f;
		}
		else if (HueAmount <= 300.0f)	// Inc red
		{
			red = (HueAmount - 240.0f) * fReciprical60;
			green = 0.0f;
			blue = 1.0f;
		}
		else // dec blue
		{
			red = 1.0f;
			green = 0.0f;
			blue = 1.0f - ((HueAmount - 300.0f) * fReciprical60);
		}

		// Now RGB is set, apply saturation
		float saturationScaleR = 1.0f - red;
		float saturationScaleG = 1.0f - green;
		float saturationScaleB = 1.0f - blue;
		red += saturationScaleR * Saturation;
		green += saturationScaleG * Saturation;
		blue += saturationScaleB * Saturation;
		clamp(red, 0.0f, 1.0f);
		clamp(green, 0.0f, 1.0f);
		clamp(blue, 0.0f, 1.0f);

		// Now saturation is added, apply brightness
		float brightnessInv = 1.0f - Brightness;
		red -= brightnessInv;
		green -= brightnessInv;
		blue -= brightnessInv;
		clamp(red, 0.0f, 1.0f);
		clamp(green, 0.0f, 1.0f);
		clamp(blue, 0.0f, 1.0f);
	}

	void Colour::getHSB(float& Hue, float& Saturation, float& Brightness) const
	{
		// Get maximum and minimum values of current RGB values
		float cmax = std::max(red, std::max(green, blue));
		float cmin = std::min(red, std::min(green, blue));
		float diff = cmax - cmin; // diff of cmax and cmin.
		Hue = -1.0f, Saturation = -1.0f;

		// If cmax and cmax are equal then fHue = 0
		if (cmax == cmin)
			Hue = 0;

		// If cmax equal r then compute fHue
		else if (cmax == red)
			Hue = float(fmod(60 * ((green - blue) / diff) + 360, 360));

		// If cmax equal g then compute fHue
		else if (cmax == green)
			Hue = float(fmod(60 * ((blue - red) / diff) + 120, 360));

		// If cmax equal b then compute fHue
		else if (cmax == blue)
			Hue = float(fmod(60 * ((red - green) / diff) + 240, 360));

		Hue /= 360.0f;

		// If cmax equal zero
		if (cmax == 0)
			Saturation = 0;
		else
			Saturation = (diff / cmax);// *100;

		// Compute fBrightness
		Brightness = cmax;// *100;
	}

	Colour Colour::interpolate(const Colour other, float value) const
	{
		clamp(value, 0.0f, 1.0f);

		Colour colour;
		float fDiff = red - other.red;		// -1.0f (This is 0.0f and other is 1.0f) to 1.0f (This is 1.0f and other is 0.0f)
		fDiff *= -1.0f;						// 1.0f (This is 0.0f and other is 1.0f) to -1.0f (This is 1.0f and other is 0.0f)
		colour.red = red + (fDiff * value);

		fDiff = green - other.green;
		fDiff *= -1.0f;
		colour.green = green + (fDiff * value);

		fDiff = blue - other.blue;
		fDiff *= -1.0f;
		colour.blue = blue + (fDiff * value);

		fDiff = alpha - other.alpha;
		fDiff *= -1.0f;
		colour.alpha = alpha + (fDiff * value);
		return colour;
	}
}