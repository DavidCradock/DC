#pragma once

#include "../DCMath/vector2f.h"
#include "../DCCommon/colour.h"
#include "../DCCommon/string.h"

namespace DC
{
	// A class for creating/loading/saving/modifying 2D images
	// Can read the following formats...
	// JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
	// PNG 1 / 2 / 4 / 8 / 16 - bit - per - channel
	// TGA(not sure what subset, if a subset)
	// BMP non - 1bpp, non - RLE
	// PSD(composited view only, no extra channels, 8 / 16 bit - per - channel)
	// GIF(*comp always reports as 4 - channel)
	// HDR(radiance rgbE format)
	// PIC(Softimage PIC)
	// PNM(PPM and PGM binary only)
	// Image pixels are stored in row first, then column. unsigned int iPixelIndex = iPixelPosX + (iPixelPosY * _miWidth);
	class Image
	{
	public:
		Image();
		~Image();

		// Sets this image to the one on the right
		Image& operator=(const Image& other);

		// Free image and memory
		void free(void);

		// Create a blank image.
		// If already created, the previous image is freed
		// Each channel contains black
		// Acceptable number of channels may be 3 or 4
		// Both dimensions must be at least 1
		// Memory consumption examples...
		//  Each pixel is either RGB or RGBA and each is a byte, so and RGBA image is 4 bytes per pixel.
		//  An image of dimensions 512x512x4 is 1,048,576 bytes or 1024 KBytes or 1 MByte
		//  An image of dimensions 1024x1024x4 is 4,194,304 bytes or 4096 KB or 4 MB
		//  An image of dimensions 4096x4096x4 is 64MB
		//  An image of dimensions 16384x16384x4 is 1024MB AKA 1GB
		// Calls ErrorIfTrue from DCCommon/error.h if invalid parameters are given.
		void createBlank(unsigned int Width, unsigned int Height, unsigned short NumberOfChannels);

		// Attempts to load the image data from a file stored on disk.
		// Depending upon the file name extension, determines the file type and loads it in.
		// If the image couldn't be loaded, false is returned, else true
		// The image is freed at the start of this method
		// Loads image from file using the stb_image library
		bool load(const String& Filename, bool FlipForOpenGL = false);

		// Attempts to read only the image width and height from the given filename, which is faster than loading the whole thing in.
		// Returns true if the image data was loaded, else false.
		bool loadInfo(const String& Filename, int& Width, int& Height, int& NumberOfChannels);

		// Save image as BMP file to disk.
		// Calls ErrorIfTrue from DCCommon/error.h if image contains no data or saving fails.
		void saveAsBMP(const String& Filename, bool FlipOnSave = false) const;

		// Save image to JPG file to disk
		// Calls ErrorIfTrue from DCCommon/error.h if image contains no data or saving fails.
		void saveAsJPG(const String& Filename, bool FlipOnSave = false, int Quality = 100) const;

		// Save image to PNG file to disk
		// Calls ErrorIfTrue from DCCommon/error.h if image contains no data or saving fails.
		void saveAsPNG(const String& Filename, bool FlipOnSave = false) const;

		// Save image to TGA file to disk
		// Calls ErrorIfTrue from DCCommon/error.h if image contains no data or saving fails.
		void saveAsTGA(const String& Filename, bool FlipOnSave = false) const;

		// Fills the image with the given colour values.
		// If the image only contains 3 colour channels, the alpha component is ignored. 
		// Calls ErrorIfTrue from DCCommon/error.h if image hasn't been created yet
		void fill(unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha = 255);

		// Return pointer to image data for manual modification. 
		// Please, BE CAREFULL if you're using this method to directly access the image data.
		// Returns The pointer to the image's data. 
		unsigned char* getData(void) const;

		// Get size of image data in bytes
		// Returns The size of the image data 
		unsigned int getDataSize(void) const;

		// Get width of image
		// Returns The width of the image. 
		unsigned int getWidth(void) const;

		// Get height of image
		// Returns The height of the image. 
		unsigned int getHeight(void) const;

		// Returns dimensions of the image
		Vector2f getDimensions(void) const;

		// Get number of channels
		// Returns The number of channels of the image. 
		unsigned int getNumChannels(void) const;

		// Returns whether this texture's width and height are to power of two.
		bool getDimsArePowerOfTwo(void) const;

		// Sets pixel at given coordinate to given values.
		// Due to additional overhead of this method due to function calling and bounds checking,
		// If you're wanting to modify many pixels, it's best to use the "unsafe" getData() method. 
		inline void setPixel(int PositionX, int PositionY, unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha);

		// Gets pixel values at specified position
		// Due to additional overhead of this method due to function calling and bounds checking,
		// If you're wanting to modify many pixels, it's best to use the "unsafe" getData() method.
		inline void getPixel(int PositionX, int PositionY, unsigned char& Red, unsigned char& Green, unsigned char& Blue, unsigned char& Alpha) const;

		// Swap red and blue colour components around
		// If this image contains no data, an error occurs.
		void swapRedAndBlue(void);

		// Flip the image vertically
		// If this image contains no data, an error occurs.
		void flipVertically(void);

		// Inverts the colours of the image, AKA new colour = 255 - current colour
		// If this image contains no data, an error occurs.
		void invert(bool InvertColour = true, bool InvertAlpha = false);

		// Converts the image's RGB components to greyscale, simply finding mean average of RGB components
		// If this image contains no data, an error occurs.
		void greyscaleSimple(void);

		// Converts the image's RGB components to greyscale, taking into consideration the average human's eye sensitivity to the individual RGB components.
		// If default params are not used (They approximate the average human's eye sensitivity), they should be of unit length.
		// If this image contains no data, an error occurs.
		void greyscale(float RedSensitivity = 0.299f, float GreenSensitivity = 0.587f, float BlueSensitivity = 0.144f);

		// Adjusts brightness of colour components
		// Accepted range for iAmount can be between -255 to +255 which would make entire image totally black or white.
		// If this image contains no data, an error occurs.
		void adjustBrightness(int value);

		// Adjusts contrast of the colour components
		// Accepted range for iAmount is between -100 and +100
		// If this image contains no data, an error occurs.
		void adjustContrast(int value);

		// Copies this image into the one given
		// Silently fails if both this image and the one parsed are actually the same objects, or there is no image data to copy.
		// If this image contains no data, an error occurs.
		// The DestinationImage is totally replaced. 
		void copyTo(Image& DestinationImage) const;

		// Copies a rectangular region from this object, into the one given.
		// Automatic clipping is done so that if the source region doesn't fit into destination, it will be clipped.
		// The destination image can be this image itself IE img.copyRectTo(img)
		// DestinationImage The destination image to copy the rectangular region of this object into
		// If this image or the destination image contain no data, an error occurs.
		void copyRectTo(Image& DestinationImage, int SourcePositionX, int SourcePositionY, int SourceWidth, int SourceHeight, int DestinationPositionX, int DestinationPositionY) const;

		// Copies the contents of this image into the outputImage and gives the output image a border and sets the pixels around that
		// border to be the same as the ones in this image's edge pixels, thereby making the output image have dimensions which are +2 of this one.
		// This is typically used to simply calculating of stuff without having to take into consideration, edge cases.
		// If this image contains no data, an exception occurs.
		void copyToAddBorder(Image& outputImage) const;

		// Rotates the image 90 degrees clockwise
		// If this image contains no data, an error occurs.
		void rotateClockwise(void);

		// Edge detection.
		// Given a colour value which represents the "background colour" of the image, this detects where the pixels in the image are next to this
		// colour and sets the outputImage as white where the other coloured pixels meet this "background colour"
		// I wish I could place images here in the code, as it'd be much easier to show what this does with images.
		// Edges are detected by using the given colour value which should represent the colour of the image's background
		// If this image contains no data, or doesn't have at least 3 channels, an error occurs.
		void edgeDetect(Image& outputImage, unsigned char Red, unsigned char Green, unsigned char Blue);

		// Removes the alpha channel of the image, leaving the RGB components
		// If this image contains no data, or doesn't have 4 channels, an error occurs.
		void removeAlphaChannel(void);

		// Copies the alpha channel to each of the RGB components
		// If this image contains no data, or doesn't have 4 channels, an error occurs.
		void copyAlphaChannelToRGB(void);

		// Computes a normal map used for normal mapping from this image and stores the result in outputImage
		// This image should be a heightmap, where each pixel represents the height of a surface. White being max height, black being the lowest.
		// However, this image first creates a copy of itself in memory, then calls greyscaleSimple() on that to ensure proper computation of the normals.
		// Scale should be between 0.0f and 1.0f and affects how "intense" the normals are generated. This value is clamped internally. Lower values increase the effect
		// If this image contains no data, an error occurs.
		void normalmap(Image& outputImage, float Scale = 1.0f) const;

		// Uses glReadPixels() to read the contents of the currently set backbuffer/framebuffer into this image.
		// This image must already have dimensions and it is those dimensions which are used during the glReadPixels call.
		// SourcePosTLCorner is the offset from the bottom left corner of the source to start reading pixels from.
		// If the position + dims go outside of the source, pixels are undefined
		// If the image doesn't have any dimensions, this silently fails.
		// Can read in either RGBA or RGB
//		void fillFromOpenGL(const Vector2f& SourcePosTLCorner = Vector2f());

		// Sets this image to the given width and height, sets it to have 4 colour channels and then draws a hue based colour wheel using the given brightness.
		void drawColourWheel(unsigned int WidthAndHeightOfImage, unsigned char Brightness = 255);

		// Not 100% image related, but given an X and Y coordinate over an imaginary drawn colour wheel, returns a CColour value which would
		// represent the colour of the colour wheel at those X and Y coordinates.
		// No checking of incorrect position values.
		Colour getColourWheelColour(unsigned int PositionX, unsigned int PositionY, unsigned int WidthAndHeightOfImage, unsigned char Brightness);

		// Creates this image with the specified values and draws a gradient between the two given colours.
		void drawGradient(unsigned int Width, unsigned int Height, unsigned int NumberOfChannels, const Colour& Colour0, const Colour& Colour1);

		// Creates this image with the specified values and draws a circle with the given colour values
		void drawCircle(unsigned int WidthAndHeightOfImage, const Colour& InnerColour, const Colour& OuterColour);
	private:
		unsigned char* data;
		unsigned int dataSize;
		int width;
		int height;
		int numberOfChannels;

		// Used by edgeDetect()
		inline bool _isPixelEdge(int PositionX, int PositionY, unsigned char Red, unsigned char Green, unsigned char Blue);
	};



	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// Definition
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////


	inline void Image::setPixel(int PositionX, int PositionY, unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha)
	{
		if (PositionX >= width)
			return;
		if (PositionY >= height)
			return;

		unsigned int iIndex = PositionX + (PositionY * width);
		iIndex *= numberOfChannels;
		switch (numberOfChannels)
		{
		case 1:
			data[iIndex] = Red;
			break;
		case 3:
			data[iIndex] = Red;
			data[iIndex + 1] = Green;
			data[iIndex + 2] = Blue;
			break;
		case 4:
			data[iIndex] = Red;
			data[iIndex + 1] = Green;
			data[iIndex + 2] = Blue;
			data[iIndex + 3] = Alpha;
			break;
		}
	}

	inline void Image::getPixel(int PositionX, int PositionY, unsigned char& Red, unsigned char& Green, unsigned char& Blue, unsigned char& Alpha) const
	{
		if (PositionX >= width)
			return;
		if (PositionY >= height)
			return;

		unsigned int iIndex = PositionX + (PositionY * width);
		iIndex *= numberOfChannels;
		switch (numberOfChannels)
		{
		case 1:
			Red = data[iIndex];
			break;
		case 3:
			Red = data[iIndex];
			Green = data[iIndex + 1];
			Blue = data[iIndex + 2];
			break;
		case 4:
			Red = data[iIndex];
			Green = data[iIndex + 1];
			Blue = data[iIndex + 2];
			Alpha = data[iIndex + 3];
			break;
		}
	}

	inline bool Image::_isPixelEdge(int PositionX, int PositionY, unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		// Don't check edge pixels of image
		if (PositionX == 0)
			return false;
		if (PositionY == 0)
			return false;
		if (PositionX >= int(width) - 1)
			return false;
		if (PositionY >= int(height) - 1)
			return false;

		// Get center pixel colour values
		unsigned char col[4];
		getPixel(PositionX, PositionY, col[0], col[1], col[2], col[3]);

		// If the center pixel's colour is same as mask, it's not an edge
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return false;

		// If we get here, the center pixel is not the same as the mask colour

		// Get bordering pixels of center pixel
		// If they are the same as the mask, then it's an edge

		// Left
		getPixel(PositionX - 1, PositionY, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;
		// Right
		getPixel(PositionX + 1, PositionY, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		// Top
		getPixel(PositionX, PositionY - 1, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		// Bottom
		getPixel(PositionX, PositionY + 1, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		// Top left
		getPixel(PositionX - 1, PositionY - 1, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		// Top right
		getPixel(PositionX + 1, PositionY - 1, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		// Bottom left
		getPixel(PositionX - 1, PositionY + 1, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		// Bottom right
		getPixel(PositionX + 1, PositionY + 1, col[0], col[1], col[2], col[3]);
		if (col[0] == Red)
			if (col[1] == Green)
				if (col[2] == Blue)
					return true;

		return false;	// Shouldn't get here, keep compiler happy
	}

}