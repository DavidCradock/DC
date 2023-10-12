#include "colour.h"
#include "utilities.h"
#include <algorithm>

namespace DC
{
	Colour::Colour()
	{
		set(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Colour::Colour(float redPARAM, float greenPARAM, float bluePARAM, float alphaPARAM)
	{
		set(redPARAM, greenPARAM, bluePARAM, alphaPARAM);
	}

	void Colour::set(float redPARAM, float greenPARAM, float bluePARAM, float alphaPARAM)
	{
		clamp(redPARAM, 0.0f, 1.0f);
		clamp(greenPARAM, 0.0f, 1.0f);
		clamp(bluePARAM, 0.0f, 1.0f);
		clamp(alphaPARAM, 0.0f, 1.0f);
		red = redPARAM;
		green = greenPARAM;
		blue = bluePARAM;
		alpha = alphaPARAM;
	}

	void Colour::setFromHSB(float hueAmount, float saturation, float brightness)
	{
		clamp(hueAmount, 0.0f, 1.0f);
		clamp(saturation, 0.0f, 1.0f);
		clamp(brightness, 0.0f, 1.0f);

		// Compute colour
		hueAmount *= 360.0f;
		float fReciprical60 = 1.0f / 60.0f;
		if (hueAmount <= 60.0f)	// Inc green
		{
			red = 1.0f;
			green = hueAmount * fReciprical60;
			blue = 0.0f;
		}
		else if (hueAmount <= 120.0f)	// Dec red
		{
			red = 1.0f - ((hueAmount - 60.0f) * fReciprical60);
			green = 1.0f;
			blue = 0.0f;
		}
		else if (hueAmount <= 180.0f)	// Inc blue
		{
			red = 0.0f;
			green = 1.0f;
			blue = (hueAmount - 120.0f) * fReciprical60;
		}
		else if (hueAmount <= 240.0f)	// Dec green
		{
			red = 0.0f;
			green = 1.0f - ((hueAmount - 180.0f) * fReciprical60);
			blue = 1.0f;
		}
		else if (hueAmount <= 300.0f)	// Inc red
		{
			red = (hueAmount - 240.0f) * fReciprical60;
			green = 0.0f;
			blue = 1.0f;
		}
		else // dec blue
		{
			red = 1.0f;
			green = 0.0f;
			blue = 1.0f - ((hueAmount - 300.0f) * fReciprical60);
		}

		// Now RGB is set, apply saturation
		float saturationScaleR = 1.0f - red;
		float saturationScaleG = 1.0f - green;
		float saturationScaleB = 1.0f - blue;
		red += saturationScaleR * saturation;
		green += saturationScaleG * saturation;
		blue += saturationScaleB * saturation;
		clamp(red, 0.0f, 1.0f);
		clamp(green, 0.0f, 1.0f);
		clamp(blue, 0.0f, 1.0f);

		// Now saturation is added, apply brightness
		float brightnessInv = 1.0f - brightness;
		red -= brightnessInv;
		green -= brightnessInv;
		blue -= brightnessInv;
		clamp(red, 0.0f, 1.0f);
		clamp(green, 0.0f, 1.0f);
		clamp(blue, 0.0f, 1.0f);
	}

	void Colour::getHSB(float& hue, float& saturation, float& brightness) const
	{
		// Get maximum and minimum values of current RGB values
		float cmax = std::max(red, std::max(green, blue));
		float cmin = std::min(red, std::min(green, blue));
		float diff = cmax - cmin; // diff of cmax and cmin.
		hue = -1.0f, saturation = -1.0f;

		// If cmax and cmax are equal then hue = 0
		if (cmax == cmin)
			hue = 0;

		// If cmax equal r then compute hue
		else if (cmax == red)
			hue = float(fmod(60 * ((green - blue) / diff) + 360, 360));

		// If cmax equal g then compute hue
		else if (cmax == green)
			hue = float(fmod(60 * ((blue - red) / diff) + 120, 360));

		// If cmax equal b then compute hue
		else if (cmax == blue)
			hue = float(fmod(60 * ((red - green) / diff) + 240, 360));

		hue /= 360.0f;

		// If cmax equal zero
		if (cmax == 0)
			saturation = 0;
		else
			saturation = (diff / cmax);// *100;

		// Compute brightness
		brightness = cmax;// *100;
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