#pragma once

namespace DC
{
	// Used to represent a colour using floats to hold the individual colour values for red, green, blue and alpha.
	class Colour
	{
	public:
		// Constructor, sets to default values (white with full alpha)
		Colour();

		// Constructor, sets to passed values
		Colour(float red, float green, float blue, float alpha);

		// Sets the colour to the given values.
		// Clamps the values in the range of 0.0f and 1.0f
		void set(float red, float green, float blue, float alpha);

		// Sets this colour to RGB using the given the HSB (Hue, Saturation and Brightness) values.
		// hueAmount value should be between 0 and 1 (Although it is clamped)
		// saturation value should be between 0 and 1 (Although it is clamped)
		// brightness value should be between 0 and 1 (Although it is clamped)
		void setFromHSB(float hueAmount, float saturation = 1.0f, float brightness = 1.0f);

		// From this colour in RGBA format, computes and sets the passed variables to hold the HSB (Hue, saturation and brightness values)
		void getHSB(float& hue, float& saturation, float& brightness) const;

		// Interpolates between this colour and the one given by the given float and returns the new colour.
		// If the float is 0.0f, the returned colour will be this colour.
		// If the float is 0.5f, the returned colour will be half way bewteen this colour and the one given.
		// If the float is 1.0f, the returned colour will be the one given.
		Colour interpolate(const Colour other, float value) const;

		bool operator == (const Colour& col)
		{
			return (red == col.red && green == col.green && blue == col.blue && alpha == col.alpha);
		}

		// Overload the * operator to multiply two colours together
		Colour operator*(const Colour& other) const
		{
			return Colour(red * other.red, green * other.green, blue * other.blue, alpha * other.alpha);
		}
		float red;		// Red colour between 0.0f and 1.0f
		float green;	// Green colour between 0.0f and 1.0f
		float blue;		// Blue colour between 0.0f and 1.0f
		float alpha;	// Alpha colour between 0.0f and 1.0f
	};
}