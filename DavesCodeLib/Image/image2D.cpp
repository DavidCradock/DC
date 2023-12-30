#include "image2D.h"
#include "../Common/error.h"
#include "../Common/utilities.h"
#include "../Math/vector3f.h"
#include "../Math/mathUtilities.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../ThirdPartyLibs/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#include "../../ThirdPartyLibs/stb/stb_image_write.h"

namespace DC
{
	Image2D::Image2D()
	{
		data = 0;
		dataSize = 0;
		free();
	}

	Image2D::~Image2D()
	{
		free();
	}

	Image2D& Image2D::operator=(const Image2D& other)
	{
		// Guard against self assignment
		if (this == &other)
			return *this;

		other.copyTo(*this);
		return *this;
	}

	void Image2D::free(void)
	{
		if (data)
		{
			delete[] data;
			data = NULL;
			dataSize = 0;
		}
		width = height = numberOfChannels = 0;
	}

	void Image2D::createBlank(unsigned int widthPARAM, unsigned int heightPARAM, unsigned short numberOfChannelsPARAM)
	{
		free();
		ErrorIfTrue(widthPARAM < 1, L"Image2D::createBlank() failed as given width < 1.");
		ErrorIfTrue(heightPARAM < 1, L"Image2D::createBlank() failed as given height < 1.");
		ErrorIfTrue(numberOfChannelsPARAM < 3, L"Image2D::createBlank() failed as given number of channels < 1. (Only 3 or 4 is valid)");
		ErrorIfTrue(numberOfChannelsPARAM > 4, L"Image2D::createBlank() failed as given number of channels > 4. (Only 3 or 4 is valid)");

		width = widthPARAM;
		height = heightPARAM;
		numberOfChannels = numberOfChannelsPARAM;
		dataSize = width * height * numberOfChannels;
		data = new unsigned char[dataSize];
		ErrorIfTrue(!data, L"Image2D::createBlank() failed to allocate memory.");

		// Zero out the new memory all to zero
		for (unsigned int i = 0; i < dataSize; ++i)
			data[i] = 0;
	}

	bool Image2D::load(const String& filename, bool flipForOpenGL)
	{
		free();
		stbi_set_flip_vertically_on_load(flipForOpenGL);

		// Get number of channels in the image file
		int iDims[2];
		int iNumChannels = 3;
		loadInfo(filename, iDims[0], iDims[1], iNumChannels);
		stbi_uc* pixels = 0;
		if (4 == iNumChannels)
			pixels = stbi_load(filename.c_strn(), &width, &height, &numberOfChannels, STBI_rgb_alpha);
		else if (3 == iNumChannels)
			pixels = stbi_load(filename.c_strn(), &width, &height, &numberOfChannels, STBI_rgb);
		else if (1 == iNumChannels)
			pixels = stbi_load(filename.c_strn(), &width, &height, &numberOfChannels, 1);

		if (!pixels)
			return false;

		// If number of channels is 1, then we convert that 1 channel to 3 and duplicate the R to G and B
		if (1 == iNumChannels)
		{
			numberOfChannels = 3;
		}

		// Compute size and allocate
		dataSize = width * height * numberOfChannels;
		data = new unsigned char[dataSize];

		if (1 != iNumChannels)
			memcpy(data, pixels, static_cast<size_t>(dataSize));
		else // We need to copy the R to G and B
		{
			unsigned int iPixelIndex = 0;
			for (unsigned int i = 0; i < dataSize; i += 3)
			{
				data[i] = pixels[iPixelIndex];
				data[i+1] = pixels[iPixelIndex];
				data[i+2] = pixels[iPixelIndex];
				iPixelIndex++;
			}
		}
		stbi_image_free(pixels);
		return true;
	}

	bool Image2D::loadInfo(const String& filename, int& widthPARAM, int& heightPARAM, int& numberOfChannelsPARAM)
	{
		// To query the width, height and component count of an image without having to
		// decode the full file, you can use the stbi_info family of functions:
		//
		//   int ix,iy,n,ok;
		//   ok = stbi_info(filename, &ix, &iy, &n);
		//   // returns ok=1 and sets ix, iy, n if image is a supported format,
		//   // 0 otherwise.
		return (bool)stbi_info(filename.c_strn(), &widthPARAM, &heightPARAM, &numberOfChannelsPARAM);
	}

	void Image2D::saveAsBMP(const String& filename, bool flipOnSave) const
	{
		ErrorIfTrue(!data, L"Image2D::saveAsBMP() failed. Image2D not yet created.");
		stbi_flip_vertically_on_write(flipOnSave); // flag is non-zero to flip data vertically
		String err;
		err += L"Image2D::saveAsBMP(\"";
		err += filename;
		err += L"\") failed.Image2D failed to be written.";
		ErrorIfTrue(!stbi_write_bmp(filename.c_strn(), width, height, numberOfChannels, data), err);
	}

	void Image2D::saveAsJPG(const String& filename, bool flipOnSave, int quality) const
	{
		ErrorIfTrue(!data, L"Image2D::saveAsJPG() failed. Image2D not yet created.");
		stbi_flip_vertically_on_write(flipOnSave); // flag is non-zero to flip data vertically
		String err;
		err += L"Image2D::saveAsJPG(\"";
		err += filename;
		err += L"\") failed.Image2D failed to be written.";
		ErrorIfTrue(!stbi_write_jpg(filename.c_strn(), width, height, numberOfChannels, data, quality), err);
	}

	void Image2D::saveAsPNG(const String& filename, bool flipOnSave) const
	{
		ErrorIfTrue(!data, L"Image2D::saveAsPNG() failed. Image2D not yet created.");
		stbi_flip_vertically_on_write(flipOnSave); // flag is non-zero to flip data vertically
		String err;
		err += L"Image2D::saveAsPNG(\"";
		err += filename;
		err += L"\") failed. Image2D failed to be written.";
		ErrorIfTrue(!stbi_write_png(filename.c_strn(), width, height, numberOfChannels, data, width * numberOfChannels), err);
	}

	void Image2D::saveAsTGA(const String& filename, bool flipOnSave) const
	{
		ErrorIfTrue(!data, L"Image2D::saveAsTGA() failed. Image2D not yet created.");
		stbi_flip_vertically_on_write(flipOnSave); // flag is non-zero to flip data vertically
		String err;
		err += L"Image2D::saveAsTGA(\"";
		err += filename;
		err += L"\") failed.Image2D failed to be written.";
		ErrorIfTrue(!stbi_write_tga(filename.c_strn(), width, height, numberOfChannels, data), err);
	}

	void Image2D::fill(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	{
		ErrorIfTrue(!data, L"Image2D::fill() failed. Image2D not yet created.");

		unsigned int i = 0;

		// 3 Colour channels
		if (3 == numberOfChannels)
		{
			while (i < dataSize)
			{
				data[i] = red;
				data[i+1] = green;
				data[i+2] = blue;
				i += numberOfChannels;
			}
			return;
		}

		// 4 colour channels
		if (4 == numberOfChannels)
		{
			while (i < dataSize)
			{
				data[i] = red;
				data[i+1] = green;
				data[i+2] = blue;
				data[i+3] = alpha;
				i += numberOfChannels;
			}
		}
	}

	unsigned char* Image2D::getData(void) const
	{
		return data;
	}

	unsigned int Image2D::getDataSize(void) const
	{
		return dataSize;
	}

	unsigned int Image2D::getWidth(void) const
	{
		return width;
	}

	unsigned int Image2D::getHeight(void) const
	{
		return height;
	}

	Vector2f Image2D::getDimensions(void) const
	{
		return Vector2f((float)width, (float)height);
	}

	unsigned int Image2D::getNumChannels(void) const
	{
		return numberOfChannels;
	}

	bool Image2D::getDimsArePowerOfTwo(void) const
	{
		int iX = 1;
		int iY = 1;
		while (iX < width)
			iX *= 2;
		while (iY < height)
			iY *= 2;
		if (iX != width || iY != height)
			return false;
		return true;
	}

	void Image2D::swapRedAndBlue(void)
	{
		ErrorIfTrue(!data, L"Image2D::swapRedAndBlue() failed. Image2D not yet created.");

		unsigned int i = 0;
		int i2;
		unsigned char chTemp;
		while (i < dataSize)
		{
			i2 = i+2;
			chTemp = data[i];
			data[i] = data[i2];
			data[i2] = chTemp;
			i += numberOfChannels;
		}
	}

	void Image2D::flipVertically(void)
	{
		ErrorIfTrue(!data, L"Image2D::flipVertically() failed. Image2D not yet created.");

		// Size of a row
		unsigned int iRowSize = width * numberOfChannels;

		// Allocate new flipped image
		unsigned char *pNewImageStartAddress = new unsigned char[dataSize];
		unsigned char *pNewImage = pNewImageStartAddress;
		ErrorIfTrue(0==pNewImage, L"Image2D::flipVertically() failed to allocate memory.");

		// Get pointer to current image
		unsigned char *pOldImage = data;
		// Increment old image pointer to point to last row
		pOldImage += iRowSize * (height - 1);

		// Copy each row into new image
		unsigned int iRowSizeBytes = iRowSize * sizeof(unsigned char);
		for (int iRow=0; iRow<height; ++iRow)
		{
			memcpy(pNewImage, pOldImage, iRowSizeBytes);
			// Adjust pointers
			pNewImage += iRowSizeBytes;
			pOldImage -= iRowSizeBytes;
		}
		// Now pNewImage contains flipped image data
		delete[] data;	// Delete old image data
		data = pNewImageStartAddress;	// Make image data point to the new data
	}

	void Image2D::invert(bool invertColour, bool invertAlpha)
	{
		ErrorIfTrue(!data, L"Image2D::invert() failed. Image2D not yet created.");

		unsigned int i = 0;
		int iIndex;
		if (invertColour)
		{
			while (i < dataSize)
			{
				iIndex = i;
				data[iIndex] = 255 - data[iIndex]; ++iIndex;
				data[iIndex] = 255 - data[iIndex]; ++iIndex;
				data[iIndex] = 255 - data[iIndex];
				i += numberOfChannels;
			}
		}

		if (numberOfChannels == 4 && invertAlpha)
		{
			i = 3;
			while (i < dataSize)
			{
				data[i] = 255 - data[i];
				i += numberOfChannels;
			}
		}
	}

	void Image2D::greyscaleSimple(void)
	{
		ErrorIfTrue(!data, L"Image2D::greyscaleSimple() failed. Image2D not yet created.");

		unsigned int i = 0;
		float f1Over3 = 1.0f / 3.0f;
		float fTmp;
		unsigned char cTmp;
		while (i < dataSize)
		{
			fTmp = float(data[i]);
			fTmp += float(data[i+1]);
			fTmp += float(data[i+2]);
			fTmp *= f1Over3;
			cTmp = (unsigned char)fTmp;
			data[i] = cTmp;
			data[i+1] = cTmp;
			data[i+2] = cTmp;
			i += numberOfChannels;
		}
	}


	void Image2D::greyscale(float redSensitivity, float greenSensitivity, float blueSensitivity)
	{
		ErrorIfTrue(!data, L"Image2D::greyscale() failed. Image2D not yet created.");

		Vector3f vCol(redSensitivity, greenSensitivity, blueSensitivity);

		unsigned int i = 0;
		float fTmp;
		unsigned char cTmp;
		while (i < dataSize)
		{
			fTmp = 0.0f;
			fTmp = float(data[i]) * vCol.x;
			fTmp += float(data[i+1]) * vCol.y;
			fTmp += float(data[i+2]) * vCol.z;
			cTmp = (unsigned char)fTmp;
			data[i] = cTmp;
			data[i+1] = cTmp;
			data[i+2] = cTmp;
			i += numberOfChannels;
		}
	}

	void Image2D::adjustBrightness(int value)
	{
		ErrorIfTrue(!data, L"Image2D::adjustBrightness() failed. Image2D not yet created.");

		unsigned int i = 0;
		int iCol;
		while (i < dataSize)
		{
			iCol = (int)data[i] + value;
			clamp(iCol, 0, 255);
			data[i] = unsigned char(iCol);

			iCol = (int)data[i+1] + value;
			clamp(iCol, 0, 255);
			data[i+1] = unsigned char(iCol);

			iCol = (int)data[i+2] + value;
			clamp(iCol, 0, 255);
			data[i+2] = unsigned char(iCol);
			i += numberOfChannels;
		}
	}

	void Image2D::adjustContrast(int value)
	{
		ErrorIfTrue(!data, L"Image2D::adjustContrast() failed. Image2D not yet created.");

		clamp(value, -100, 100);
		double dPixel;
		double d1Over255 = 1.0 / 255.0;
		double dContrast = (100.0 + double(value)) * 0.01; // 0 and 2
		dContrast *= dContrast;	// 0 and 4
		unsigned int i = 0;
		int iIndex;
		while (i < dataSize)
		{
			iIndex = i;
			dPixel = double(data[iIndex]) * d1Over255;
			dPixel -= 0.5;
			dPixel *= dContrast;
			dPixel += 0.5;
			dPixel *= 255;
			clamp(dPixel, 0.0, 255.0);
			data[iIndex] = unsigned char(dPixel);
			++iIndex;

			dPixel = double(data[iIndex]) * d1Over255;
			dPixel -= 0.5;
			dPixel *= dContrast;
			dPixel += 0.5;
			dPixel *= 255;
			clamp(dPixel, 0.0, 255.0);
			data[iIndex] = unsigned char(dPixel);
			++iIndex;

			dPixel = double(data[iIndex]) * d1Over255;
			dPixel -= 0.5;
			dPixel *= dContrast;
			dPixel += 0.5;
			dPixel *= 255;
			clamp(dPixel, 0.0, 255.0);
			data[iIndex] = unsigned char(dPixel);

			i += numberOfChannels;
		}
	}

	void Image2D::copyTo(Image2D& destinationImage) const
	{
		ErrorIfTrue(!data, L"Image2D::copyTo() failed. Source image not yet created.");

		// If destination image is the same as this one, do nothing
		if (destinationImage.data == this->data)
			return;

		destinationImage.free();
		destinationImage.createBlank(width, height, numberOfChannels);
		memcpy(destinationImage.data, data, sizeof(unsigned char) * dataSize);
	}

	void Image2D::copyRectTo(Image2D& destinationImage, int sourcePositionX, int sourcePositionY, int sourceWidth, int sourceHeight, int destinationPositionX, int destinationPositionY) const
	{
		// Check that both images have data
		ErrorIfTrue(!data, L"Image2D::copyRectTo() failed. Source image not yet created.");
		ErrorIfTrue(!destinationImage.data, L"Image2D::copyRectTo() failed. Destination image not yet created.");

		// Compute source rect
		int iSrcLeft = sourcePositionX;
		int iSrcBot = sourcePositionY;
		int iSrcRight = iSrcLeft + sourceWidth;
		int iSrcTop = iSrcBot + sourceHeight;
		// Compute destination rect
		int iDstLeft = destinationPositionX;
		int iDstBot = destinationPositionY;
		int iDstRight = iDstLeft + sourceWidth;
		int iDstTop = iDstBot + sourceHeight;

		// The above may be invalid due to different sizes, invalid positions, dims etc.
		// Invalid starting positions
		if (iSrcLeft >= width)
			return;
		if (iSrcBot >= height)
			return;
		if (iDstLeft >= destinationImage.width)
			return;
		if (iDstBot >= destinationImage.height)
			return;
		// Clamp right and top to edges of their respective images
		clamp(iSrcRight, iSrcLeft, width);
		clamp(iSrcTop, iSrcBot, height);
		clamp(iDstRight, iDstLeft, destinationImage.width);
		clamp(iDstTop, iDstBot, destinationImage.height);
		// Compute rect dims for both images
		unsigned int iSrcRectWidth = iSrcRight - iSrcLeft;
		unsigned int iSrcRectHeight = iSrcTop - iSrcBot;
		unsigned int iDstRectWidth = iDstRight - iDstLeft;
		unsigned int iDstRectHeight = iDstTop - iDstBot;
		// Compute smallest rect
		unsigned int iMinWidth = iSrcRectWidth;
		if (iMinWidth > iDstRectWidth)
			iMinWidth = iDstRectWidth;
		unsigned int iMinHeight = iSrcRectHeight;
		if (iMinHeight > iDstRectHeight)
			iMinHeight = iDstRectHeight;
		// If minimum = zero, then do nothing
		if (iMinWidth == 0)
			return;
		if (iMinHeight == 0)
			return;

		unsigned char colTmp[4];
		unsigned int isx, isy;
		unsigned int idx, idy;
		for (unsigned int ix = 0; ix < iMinWidth; ++ix)
		{
			for (unsigned int iy = 0; iy < iMinHeight; ++iy)
			{
				isx = iSrcLeft + ix;
				isy = iSrcBot + iy;
				idx = iDstLeft + ix;
				idy = iDstBot + iy;
				getPixel(isx, isy, colTmp[0], colTmp[1], colTmp[2], colTmp[3]);
				destinationImage.setPixel(idx, idy, colTmp[0], colTmp[1], colTmp[2], colTmp[3]);
			}
		}
	}

	void Image2D::copyToAddBorder(Image2D& outputImage) const
	{
		ErrorIfTrue(!data, L"Image2D::copyToAddBorder() failed. Image2D data doesn't exist.");

		// Compute new larger dimensions and create the larger image
		int newWidth = width + 2;
		int newHeight = height + 2;
		outputImage.createBlank(newWidth, newHeight, numberOfChannels);

		// Copy this image to the centre of the larger image
		copyRectTo(outputImage, 0, 0, width, height, 1, 1);

		// Now copy the edges of this image to the destination image
		unsigned char r, g, b, a;
		int heightOfOutputImageMinusOne = newHeight - 1;
		// Top and bottom edges
		for (int iX = 0; iX < width; iX++)
		{
			// Top pixel row
			getPixel(iX, 0, r, g, b, a);
			outputImage.setPixel(iX + 1, 0, r, g, b, a);

			// Bottom pixel row
			getPixel(iX, height - 1, r, g, b, a);
			outputImage.setPixel(iX + 1, heightOfOutputImageMinusOne, r, g, b, a);
		}
		int widthOfOutputImageMinusOne = newWidth - 1;
		// Left and right edges
		for (int iY = 0; iY < height; iY++)
		{
			// Left pixel column
			getPixel(0, iY, r, g, b, a);
			outputImage.setPixel(0, iY + 1, r, g, b, a);

			// Right pixel column
			getPixel(width - 1, iY, r, g, b, a);
			outputImage.setPixel(widthOfOutputImageMinusOne, iY + 1, r, g, b, a);
		}
	}

	void Image2D::rotateClockwise(void)
	{
		Image2D oldImage;
		copyTo(oldImage);

		unsigned char col[4];
		int idstX;
		int idstY;

		// Non squared?
		if (width != height)
		{
			createBlank(height, width, numberOfChannels);
		}

		for (int isrcX=0; isrcX < oldImage.width; ++isrcX)
		{
			idstY = height - isrcX - 1;
			for (int isrcY=0; isrcY < oldImage.height; ++isrcY)
			{
				idstX = isrcY;
				oldImage.getPixel(isrcX, isrcY, col[0], col[1], col[2], col[3]);
				setPixel(idstX, idstY, col[0], col[1], col[2], col[3]);
			}
		}
	}

	void Image2D::edgeDetect(Image2D& outputImage, unsigned char red, unsigned char green, unsigned char blue)
	{
		ErrorIfTrue(!data, L"Image2D::edgeDetect() failed. Image2D data doesn't exist.");
		ErrorIfTrue(numberOfChannels < 3, L"Image2D::edgeDetect() failed. Some image data exists, but doesn't have enough colour channels.");

		outputImage.createBlank(width, height, 4);
		int iX = 0;
		int iY = 0;
		while (iX < (int)width)
		{
			while (iY < (int)height)
			{
				if (_isPixelEdge(iX, iY, red, green, blue))
					outputImage.setPixel(iX, iY, 255, 255, 255, 255);
				else
					outputImage.setPixel(iX, iY, 0, 0, 0, 0);
				++iY;
			}
			++iX;
			iY = 0;
		}
	}

	void Image2D::removeAlphaChannel(void)
	{
		ErrorIfTrue(!data, L"Image2D::removeAlphaChannel() failed. Image2D data doesn't exist.");
		ErrorIfTrue(numberOfChannels != 4, L"Image2D::removeAlphaChannel() failed. Some image data exists, but the alpha data doesn't exist (Image2D doesn't hold 4 channels)");

		// Copy this image to a new tmp image
		Image2D old;
		copyTo(old);

		// Recreate this one, but with 3 channels
		createBlank(old.getWidth(), old.getHeight(), 3);

		// Copy RGB from old to this...
		unsigned int iIndex = 0;
		int iIndexOld = 0;
		while (iIndex < dataSize)
		{
			data[iIndex] = old.data[iIndexOld];		// Red
			data[iIndex+1] = old.data[iIndexOld+1];	// Green
			data[iIndex+2] = old.data[iIndexOld+2];	// Blue
			iIndex+=3;
			iIndexOld+=4;
		}
	}

	void Image2D::copyAlphaChannelToRGB(void)
	{
		ErrorIfTrue(!data, L"Image2D::copyAlphaChannelToRGB() failed. Image2D data doesn't exist.");
		ErrorIfTrue(numberOfChannels != 4, L"Image2D::copyAlphaChannelToRGB() failed. Some image data exists, but the alpha data doesn't exist (Image2D doesn't hold 4 channels)");

		unsigned int iIndex = 0;
		while (iIndex < dataSize)
		{
			data[iIndex] = data[iIndex+3];	// Red
			data[iIndex+1] = data[iIndex+3];	// Green
			data[iIndex+2] = data[iIndex+3];	// Blue
			iIndex+=4;
		}
	}

	void Image2D::normalmap(Image2D& outputImage, float scale) const
	{
		ErrorIfTrue(!data, L"Image2D::normalmap() failed. Image2D data doesn't exist.");

		clamp(scale, 0.0f, 1.0f);

		// Copy this image into a new one so this is left unaffected.
		// This uses the copyToAddBorder() method which adds a border and copies the edge pixels to the new pixels in the border.
		// This makes it so we don't have to mess around with edge cases.
		Image2D imageGreyscale;
		copyToAddBorder(imageGreyscale);

		// Greyscale the image
		imageGreyscale.greyscaleSimple();

		// Create output image with the same size as this one
		outputImage.createBlank(width, height, 3);

		// Now loop through greyscale image, computing each normal and storing in the output image.
		unsigned char r[3], g[3], b[3], a;
		float fX, fY, fZ;
		float fLength;
		for (int y = 0; y < height; y++)
		{
			for (int ix = 0; ix < width; ix++)
			{
				// we add +1 to imageGreyscale pixel positions as it has a border

				// Get height values of centre and surrounding pixels
				imageGreyscale.getPixel(ix + 1, y + 1, r[0], g[0], b[0], a);	// Current pixel
				imageGreyscale.getPixel(ix, y + 1, r[1], g[1], b[1], a);		// Left pixel
				imageGreyscale.getPixel(ix + 1, y + 2, r[2], g[2], b[2], a);	// Above pixel
				
				fX = float(r[1] - r[0]) / 255.0f;	// Convert to -1.0f to 1.0f
				fY = float(r[2] - r[0]) / 255.0f;	// ....
				fZ = scale;

				// Compute length of vector and normalize
				fLength = sqrt((fX * fX) + (fY * fY) + (fZ * fZ));
				if (areFloatsEqual(fLength, 0.0f))	// If length is nearly zero, just set as up vector
				{
					fX = 0.0f;
					fY = 0.0f;
					fZ = scale;
				}
				else
				{
					fX = fX / fLength;
					fY = fY / fLength;
					fZ = fZ / fLength;
				}

				// Convert from -1, +1 to 0, 255
				fX += 1.0f;	fX *= 127.0f;
				fY += 1.0f;	fY *= 127.0f;
				fZ += 1.0f;	fZ *= 127.0f;
				r[0] = unsigned char(fX);
				g[0] = unsigned char(fY);
				b[0] = unsigned char(fZ);
				outputImage.setPixel(ix, y, r[0], g[0], b[0], a);
			}
		}
	}

	/*
	void Image2D::fillFromOpenGL(const Vector2f& sourcePosTLCorner)
	{
		if (!data)
			return;
		if (4 == _miNumChannels)
			glReadPixels((int)sourcePosTLCorner.x, (int)sourcePosTLCorner.y, _miWidth, _miHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else if (3 == _miNumChannels)
			glReadPixels((int)sourcePosTLCorner.x, (int)sourcePosTLCorner.y, _miWidth, _miHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
		flipVertically();
	}
	*/

	void Image2D::drawColourWheel(unsigned int widthAndHeightOfImage, unsigned char brightness)
	{
		ErrorIfTrue(widthAndHeightOfImage < 1, L"Image2D::drawColourWheel() failed. Parsed widthAndHeightOfImage must be at least 1");
		createBlank(widthAndHeightOfImage, widthAndHeightOfImage, 4);

		float fBrightness = float(brightness) / 255.0f;
		Vector2f vCentrePixelPosition;
		vCentrePixelPosition.x = float(widthAndHeightOfImage) * 0.5f;
		vCentrePixelPosition.y = vCentrePixelPosition.x;

		Vector2f vCurrentPixelPosition;
		Vector2f vCurrentPixelOffsetFromCentre;
		Colour colour;
		float fCircleRadius = float(widthAndHeightOfImage) * 0.5f;
		float fDistanceFromCentre;
		float fSaturation;	// 0.0f = white, 1.0f = full colour
		float fAngleDegrees;
		float fOneOver360 = 1.0f / 360.0f;
		unsigned int iPixelIndex = 0;
		for (unsigned int iPosX = 0; iPosX < (unsigned int)width; iPosX++)
		{
			vCurrentPixelPosition.x = (float)iPosX;
			for (unsigned int iPosY = 0; iPosY < (unsigned int)height; iPosY++)
			{
				vCurrentPixelPosition.y = (float)iPosY;
				vCurrentPixelOffsetFromCentre = vCurrentPixelPosition - vCentrePixelPosition;
				fDistanceFromCentre = vCurrentPixelOffsetFromCentre.getMagnitude();
				fSaturation = fCircleRadius - fDistanceFromCentre;
				fSaturation /= fCircleRadius;	// 0 at edge of circle, 1 at centre. Can be < 0 which is outside circle
				fAngleDegrees = vCurrentPixelOffsetFromCentre.getAngleDegrees360();
				fAngleDegrees *= fOneOver360;	// 0 when pixel is north, 0.25 when east etc.
				if (fSaturation < 0.0f)
					colour.set(0.0f, 0.0f, 0.0f, 0.0f);
				else
				{
					colour.setFromHSB(fAngleDegrees, fSaturation, fBrightness);
					colour.alpha = 1.0f;
				}
				data[iPixelIndex] = unsigned char(colour.red * 255);
				data[iPixelIndex+1] = unsigned char(colour.green * 255);
				data[iPixelIndex+2] = unsigned char(colour.blue * 255);
				data[iPixelIndex + 3] = unsigned char(colour.alpha * 255);
				iPixelIndex += 4;
			}
		}
	}

	Colour Image2D::getColourWheelColour(unsigned int positionX, unsigned int positionY, unsigned int widthAndHeightOfImage, unsigned char brightness)
	{
		ErrorIfTrue(widthAndHeightOfImage < 1, L"Image2D::getColourWheelColour() failed. Parsed widthAndHeightOfImage must be at least 1");

		Colour colour;
		Vector2f vCurrentPixelPosition((float)positionX, float(positionY));
		Vector2f vCentrePixelPosition;
		vCentrePixelPosition.x = float(widthAndHeightOfImage) * 0.5f;
		vCentrePixelPosition.y = vCentrePixelPosition.x;
		Vector2f vCurrentPixelOffsetFromCentre = vCurrentPixelPosition - vCentrePixelPosition;
		float fDistanceFromCentre = vCurrentPixelOffsetFromCentre.getMagnitude();
		float fCircleRadius = float(widthAndHeightOfImage) * 0.5f;
		float fSaturation = fCircleRadius - fDistanceFromCentre;
		fSaturation /= fCircleRadius;	// 0 at edge of circle, 1 at centre. Can be < 0 which is outside circle
		float fAngleDegrees = vCurrentPixelOffsetFromCentre.getAngleDegrees360();
		fAngleDegrees /= 360.0f;	// 0 when pixel is north, 0.25 when east etc.
		if (fSaturation < 0.0f)
			colour.set(0.0f, 0.0f, 0.0f, 0.0f);
		else
		{
			colour.setFromHSB(fAngleDegrees, fSaturation, float(brightness) / 255.0f);
			colour.alpha = 1.0f;
		}
		return colour;
	}

	void Image2D::drawGradient(unsigned int widthPARAM, unsigned int heightPARAM, unsigned int numberOfChannelsPARAM, const Colour& colour0, const Colour& colour1)
	{
		ErrorIfTrue(widthPARAM < 1 || heightPARAM < 1, L"Image2D::drawGradient() failed. Invalid dimensions given.");
		ErrorIfTrue(numberOfChannelsPARAM < 3 || numberOfChannelsPARAM > 4, L"Image2D::drawGradient() failed. Number of channels must be either 3 or 4.");
		createBlank(widthPARAM, heightPARAM, numberOfChannelsPARAM);
		bool bHorizontal = true;
		if (heightPARAM > widthPARAM)
			bHorizontal = false;

		Colour colour;
		unsigned int iPixelIndex = 0;
		if (bHorizontal)
		{
			for (unsigned int iPosX = 0; iPosX < widthPARAM; iPosX++)
			{
				colour = colour0.interpolate(colour1, float(iPosX) / float(widthPARAM));
				for (unsigned int iPosY = 0; iPosY < heightPARAM; iPosY++)
				{
					iPixelIndex = iPosX + (iPosY * width);
					iPixelIndex *= numberOfChannelsPARAM;
					data[iPixelIndex] = unsigned char(colour.red * 255);
					data[iPixelIndex + 1] = unsigned char(colour.green * 255);
					data[iPixelIndex + 2] = unsigned char(colour.blue * 255);
					if (numberOfChannelsPARAM == 4)
						data[iPixelIndex + 3] = unsigned char(colour.alpha * 255);
				}
			}
		}
		else
		{
			for (unsigned int iPosY = 0; iPosY < heightPARAM; iPosY++)
			{
				colour = colour0.interpolate(colour1, float(iPosY) / float(heightPARAM));
				for (unsigned int iPosX = 0; iPosX < widthPARAM; iPosX++)
				{
					iPixelIndex = iPosX + (iPosY * width);
					iPixelIndex *= numberOfChannelsPARAM;
					data[iPixelIndex] = unsigned char(colour.red * 255);
					data[iPixelIndex + 1] = unsigned char(colour.green * 255);
					data[iPixelIndex + 2] = unsigned char(colour.blue * 255);
					if (numberOfChannelsPARAM == 4)
						data[iPixelIndex + 3] = unsigned char(colour.alpha * 255);
				}
			}
		}
	}

	void Image2D::drawCircle(unsigned int widthAndHeightOfImage, const Colour& innerColour, const Colour& outerColour)
	{
		ErrorIfTrue(widthAndHeightOfImage < 1, L"Image2D::drawCircle() failed. Parsed WidthAndHeightOfImage must be at least 1");
		createBlank(widthAndHeightOfImage, widthAndHeightOfImage, 4);

		Vector2f vCentrePixelPosition;
		vCentrePixelPosition.x = float(widthAndHeightOfImage) * 0.5f;
		vCentrePixelPosition.y = vCentrePixelPosition.x;

		Vector2f vCurrentPixelPosition;
		Vector2f vCurrentPixelOffsetFromCentre;
		Colour colour;
		float fCircleRadius = float(widthAndHeightOfImage) * 0.5f;
		float fDistanceFromCentre;
		float fOneOver360 = 1.0f / 360.0f;
		unsigned int iPixelIndex = 0;
		for (unsigned int iPosX = 0; iPosX < (unsigned int)width; iPosX++)
		{
			vCurrentPixelPosition.x = (float)iPosX;
			for (unsigned int iPosY = 0; iPosY < (unsigned int)height; iPosY++)
			{
				vCurrentPixelPosition.y = (float)iPosY;
				vCurrentPixelOffsetFromCentre = vCurrentPixelPosition - vCentrePixelPosition;
				fDistanceFromCentre = fCircleRadius - vCurrentPixelOffsetFromCentre.getMagnitude();
				fDistanceFromCentre /= fCircleRadius;	// 0 at edge of circle, 1 at centre. Can be < 0 which is outside circle
				if (fDistanceFromCentre < 0.0f)
					colour.set(0.0f, 0.0f, 0.0f, 0.0f);
				else
					colour = outerColour.interpolate(innerColour, fDistanceFromCentre);
				data[iPixelIndex] = unsigned char(colour.red * 255);
				data[iPixelIndex + 1] = unsigned char(colour.green * 255);
				data[iPixelIndex + 2] = unsigned char(colour.blue * 255);
				data[iPixelIndex + 3] = unsigned char(colour.alpha * 255);
				iPixelIndex += 4;
			}
		}
	}
}
