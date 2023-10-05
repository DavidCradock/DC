#include "imageAtlas.h"
#include "../DCCommon/error.h"

namespace DC
{
	void ImageAtlasDetails::write(std::ofstream& file)
	{
		ErrorIfFalse(file.is_open(), L"ImageAtlasDetails::write() failed. The given ofstream is not open.");

		file.write((char*)&texCoords.BL, sizeof(Vector2f));
		file.write((char*)&texCoords.BR, sizeof(Vector2f));
		file.write((char*)&texCoords.TL, sizeof(Vector2f));
		file.write((char*)&texCoords.TR, sizeof(Vector2f));
		file.write((char*)&dimensions, sizeof(Vector2f));
		imageFilename.ofstreamWrite(file);
		file.write((char*)&atlasImageNumber, sizeof(unsigned int));
		file.write((char*)&rotated, sizeof(bool));

		ErrorIfFalse(file.good(), L"ImageAtlasDetails::write() failed. The ofstream is not good.");
	}

	void ImageAtlasDetails::read(std::ifstream& file)
	{
		ErrorIfFalse(file.is_open(), L"ImageAtlasDetails::read() failed. The given ifstream is not open.");

		file.read((char*)&texCoords.BL, sizeof(Vector2f));
		file.read((char*)&texCoords.BR, sizeof(Vector2f));
		file.read((char*)&texCoords.TL, sizeof(Vector2f));
		file.read((char*)&texCoords.TR, sizeof(Vector2f));
		file.read((char*)&dimensions, sizeof(Vector2f));
		imageFilename.ifstreamRead(file);
		file.read((char*)&atlasImageNumber, sizeof(unsigned int));
		file.read((char*)&rotated, sizeof(bool));

		ErrorIfFalse(file.good(), L"ImageAtlasDetails::read() failed. The ifstream is not good.");
	}

	ImageAtlasPacker::ImageAtlasPacker()
	{
	}

	ImageAtlasPacker::~ImageAtlasPacker()
	{
		reset();
	}

	void ImageAtlasPacker::createAtlasImages(const std::vector<String>& images, unsigned int maxAtlasImageWidth, unsigned int maxAtlasImageHeight, bool allowRotationOfImages, unsigned int imageSpacing)
	{
		ErrorIfTrue(0 == images.size(), L"ImageAtlasPacker::createAtlasImages() failed. The passed vector<string> containing individual image filenames contains no data.");
		ErrorIfTrue(0 == maxAtlasImageWidth || 0 == maxAtlasImageHeight, L"ImageAtlasPacker::createAtlasImages() failed. uiMaxAtlasImageWidth or uiMaxAtlasImageHeight was set to zero.");

		// Reset, freeing atlas images if they exist
		reset();

		// Store each image filename and as we go, create and load each individual image into memory
		std::vector<String> vStrImageFilenames;
		std::vector<Image*> vImages;
		for (unsigned int ui = 0; ui < images.size(); ui++)
		{
			vStrImageFilenames.push_back(images[ui]);

			// Allocate memory for individual image
			Image* pNewImage = new Image;
			ErrorIfFalse(pNewImage, L"ImageAtlasPacker::createAtlasImages() failed. Unable to allocate memory for individual image.");
			vImages.push_back(pNewImage);

			// Load each individual image
			ErrorIfFalse(vImages[ui]->load(vStrImageFilenames[ui], false), L"ImageAtlasPacker::createAtlasImages() failed. Unable to load image from file: " + vStrImageFilenames[ui] + L".");

			// We check to see if the loaded image's dimensions are not the same as the first one and throw an exception
//			bool bNotSameDims = images[ui]->getWidth() != images[0]->getWidth() || images[ui]->getHeight() != images[0]->getHeight();
//			if (bNotSameDims)
//			{
//				std::wstring strTxt = "ImageAtlasPacker::createAtlasImages() failed. All images added to an atlas must have the same dimensions.";
//				strTxt += "The first image dims: " + std::to_string(images[0]->getWidth()) + "x" + std::to_string(images[0]->getHeight());
//				strTxt += " The current image dims: " + std::to_string(images[ui]->getWidth()) + "x" + std::to_string(images[ui]->getHeight());
//				strTxt += ".";
//				ThrowIfTrue(1, strTxt);
//			}
		}

		// Determine maximum width/height of the individual images and make sure, including image spacing, they can all fit within the specified max atlas image width/height
		unsigned int uiMaxImageWidth = 0;
		unsigned int uiMaxImageHeight = 0;
		unsigned int uiImageWidth;
		unsigned int uiImageHeight;
		unsigned int uiImageSpacingTimes2 = imageSpacing * 2;
		for (unsigned int ui = 0; ui < vImages.size(); ui++)
		{
			uiImageWidth = vImages[ui]->getWidth() + uiImageSpacingTimes2;
			uiImageHeight = vImages[ui]->getHeight() + uiImageSpacingTimes2;
			if (uiMaxImageWidth < uiImageWidth)
				uiMaxImageWidth = uiImageWidth;
			if (uiMaxImageHeight < uiImageHeight)
				uiMaxImageHeight = uiImageHeight;
		}
		if (uiMaxImageWidth > maxAtlasImageWidth)
		{
			String err;
			err += L"ImageAtlasPacker::createAtlasImages() failed. An individual image's width(";
			err.appendUnsignedInt(uiMaxImageWidth);
			err += L"), including spacing of ";
			err.appendUnsignedInt(imageSpacing);
			err += L" is greater than the specified max atlas image's width(";
			err.appendUnsignedInt(maxAtlasImageWidth);
			err += L".";
			ErrorIfTrue(1, err);
		}
		if (uiMaxImageHeight > maxAtlasImageHeight)
		{
			String err;
			err += L"ImageAtlasPacker::createAtlasImages() failed. An individual image's height(";
			err.appendUnsignedInt(uiMaxImageHeight);
			err += L"), including spacing of ";
			err.appendUnsignedInt(imageSpacing);
			err += L" is greater than the specified max atlas image's height(";
			err.appendUnsignedInt(maxAtlasImageHeight);
			err += L".";
			ErrorIfTrue(1, err);
		}

		// Create each image detail for each individual image, determining whether we should rotate the image or not
		// Also, rotate the images that should be
		for (unsigned int ui = 0; ui < vImages.size(); ui++)
		{
			ImageAtlasDetails imageDetail;
			imageDetail.rotated = false;
			imageDetail.imageFilename = vStrImageFilenames[ui];
			imageDetail.atlasImageNumber = 0;
			imageDetail.dimensions = vImages[ui]->getDimensions();
			if (allowRotationOfImages)
			{
				// Rotate image so that it's height is greatest
				if (imageDetail.dimensions.x > imageDetail.dimensions.y)
				{
					imageDetail.rotated = true;
					vImages[ui]->rotateClockwise();
					imageDetail.dimensions = vImages[ui]->getDimensions();
				}
			}
			imageDetails.push_back(imageDetail);
		}

		// Bubble sort temp vectors by their image's width
		// vectors which we have to sort...
		// std::vector<std::wstring> imageFilenames;			// Holds each individual image's filename 
		// std::vector<CImage*> images;						// Holds each individual image's image data
		// std::vector<CImageAtlasDetails> imageDetails;	// Holds each individual image's details
		String strFilenameTemp;
		Image* pImageTemp;
		ImageAtlasDetails imageAtlasDetailsTemp;
		for (int i = 0; i < (int)vImages.size(); ++i)
		{
			for (int j = 0; j < (int)vImages.size(); ++j)
			{
				// Swap by largest first
				if (imageDetails[j].dimensions.x < imageDetails[i].dimensions.x)
				{
					// Temporarily store image, filename and image details
					// Assign i to temp
					pImageTemp = vImages[i];
					strFilenameTemp = vStrImageFilenames[i];
					imageAtlasDetailsTemp = imageDetails[i];
					// Assign j to i
					vImages[i] = vImages[j];
					vStrImageFilenames[i] = vStrImageFilenames[j];
					imageDetails[i] = imageDetails[j];
					// Assign temp to j
					vImages[j] = pImageTemp;
					vStrImageFilenames[j] = strFilenameTemp;
					imageDetails[j] = imageAtlasDetailsTemp;
				}
			}
		}

		// Bubble sort temp vectors by their image's height
		for (int i = 0; i < (int)vImages.size(); ++i)
		{
			for (int j = 0; j < (int)vImages.size(); ++j)
			{
				// Swap by largest first
				if (imageDetails[j].dimensions.y < imageDetails[i].dimensions.y)
				{
					// Temporarily store image, filename and image details
					// Assign i to temp
					pImageTemp = vImages[i];
					strFilenameTemp = vStrImageFilenames[i];
					imageAtlasDetailsTemp = imageDetails[i];
					// Assign j to i
					vImages[i] = vImages[j];
					vStrImageFilenames[i] = vStrImageFilenames[j];
					imageDetails[i] = imageDetails[j];
					// Assign temp to j
					vImages[j] = pImageTemp;
					vStrImageFilenames[j] = strFilenameTemp;
					imageDetails[j] = imageAtlasDetailsTemp;
				}
			}
		}
		// Now, imageFilenames, images and imageDetails contain the filename and images sorted by greatest width, then height first

		// Now we have to following arrays filled...
		// std::vector<std::wstring> imageFilenames;			// Holds each individual image's filename 
		// std::vector<Image*> images;						// Holds each individual image's image data
		// std::vector<ImageAtlasDetails> imageDetails;		// Holds each individual image's details
		// imageDetails[X].texCoords is not computed yet, we'll temporarily use this to hold position of each individual image inside of texture atlas
		// and then convert to actual texture coordinates later further below.

		// We now need to compute each image's position within the large atlas image and once done, we'll have the dimensions of the atlas image/s and the number required.
		std::vector<Vector2f> vv2fAtlasDims;	// Will hold computed dims of each atlas image
		vv2fAtlasDims.push_back(Vector2f(0.0f, (float)vImages[0]->getHeight()));
		int iCurAtlasImage = 0;
		Vector2f v2fCurrentPositionInAtlas(0.0f, 0.0f);
		float fSpacing = (float)imageSpacing;
		float fSpacingTimesTwo = fSpacing * 2.0f;
		
		// We go from left to right for each row, adding images one at a time, then when we can no longer add any more
		// images along the X axis, we move down a row and add more images starting from the left again.
		for (unsigned int ui = 0; ui < vImages.size(); ui++)
		{
			// Compute position of current image in atlas
			Vector2f vImageDims = vImages[ui]->getDimensions();
			imageDetails[ui].texCoords.TL.x = fSpacing + v2fCurrentPositionInAtlas.x;
			imageDetails[ui].texCoords.TL.y = fSpacing + v2fCurrentPositionInAtlas.y;
			imageDetails[ui].texCoords.BR.x = fSpacingTimesTwo + v2fCurrentPositionInAtlas.x + vImageDims.x;
			imageDetails[ui].texCoords.BR.y = fSpacingTimesTwo + v2fCurrentPositionInAtlas.y + vImageDims.y;

			// If the image position fits in current atlas row
			if (imageDetails[ui].texCoords.BR.x <= (float)maxAtlasImageWidth)
			{
				// Image fits, fill in the rest of it's information
				imageDetails[ui].texCoords.TR.x = imageDetails[ui].texCoords.BR.x;
				imageDetails[ui].texCoords.TR.y = imageDetails[ui].texCoords.TL.y;
				imageDetails[ui].texCoords.BL.x = imageDetails[ui].texCoords.TL.x;
				imageDetails[ui].texCoords.BL.y = imageDetails[ui].texCoords.BR.y;

				// Update maximum atlas size
				if (vv2fAtlasDims[iCurAtlasImage].x < imageDetails[ui].texCoords.BR.x)
					vv2fAtlasDims[iCurAtlasImage].x = imageDetails[ui].texCoords.BR.x;
				if (vv2fAtlasDims[iCurAtlasImage].y < imageDetails[ui].texCoords.BR.y)
					vv2fAtlasDims[iCurAtlasImage].y = imageDetails[ui].texCoords.BR.y;

				// Move position in atlas right
				v2fCurrentPositionInAtlas.x += fSpacingTimesTwo + vImageDims.x;

				// Set atlas image index for current image
				imageDetails[ui].atlasImageNumber = iCurAtlasImage;
			}
			else // This image doesn't fit along current row, so start a new one
			{
				// Move position in atlas down by max image height in row
				v2fCurrentPositionInAtlas.x = 0.0f;
				v2fCurrentPositionInAtlas.y = vv2fAtlasDims[iCurAtlasImage].y - fSpacing;

				// Compute position of current image in atlas
				imageDetails[ui].texCoords.TL.x = fSpacing + v2fCurrentPositionInAtlas.x;
				imageDetails[ui].texCoords.TL.y = fSpacing + v2fCurrentPositionInAtlas.y;
				imageDetails[ui].texCoords.BR.x = fSpacingTimesTwo + v2fCurrentPositionInAtlas.x + vImageDims.x;
				imageDetails[ui].texCoords.BR.y = fSpacingTimesTwo + v2fCurrentPositionInAtlas.y + vImageDims.y;

				// If the image position fits in current atlas
				if (imageDetails[ui].texCoords.BR.y <= (float)maxAtlasImageHeight)
				{
					// Image fits, fill in the rest of it's information
					imageDetails[ui].texCoords.TR.x = imageDetails[ui].texCoords.BR.x;
					imageDetails[ui].texCoords.TR.y = imageDetails[ui].texCoords.TL.y;
					imageDetails[ui].texCoords.BL.x = imageDetails[ui].texCoords.TL.x;
					imageDetails[ui].texCoords.BL.y = imageDetails[ui].texCoords.BR.y;

					// Update maximum atlas size
					if (vv2fAtlasDims[iCurAtlasImage].x < imageDetails[ui].texCoords.BR.x)
						vv2fAtlasDims[iCurAtlasImage].x = imageDetails[ui].texCoords.BR.x;
					if (vv2fAtlasDims[iCurAtlasImage].y < imageDetails[ui].texCoords.BR.y)
						vv2fAtlasDims[iCurAtlasImage].y = imageDetails[ui].texCoords.BR.y;

					// Move position in atlas right
					v2fCurrentPositionInAtlas.x += fSpacingTimesTwo + vImageDims.x;

					// Set atlas image index for current image
					imageDetails[ui].atlasImageNumber = iCurAtlasImage;
				}
				else // This image doesn't fit in this atlas and maximum dims of the atlas have been reached
				{
					iCurAtlasImage++;
					vv2fAtlasDims.push_back(Vector2f(0.0f, (float)vImages[ui]->getHeight()));
					v2fCurrentPositionInAtlas.setZero();

					// Add image to new atlas image
					imageDetails[ui].texCoords.TL.x = fSpacing + v2fCurrentPositionInAtlas.x;
					imageDetails[ui].texCoords.TL.y = fSpacing + v2fCurrentPositionInAtlas.y;
					imageDetails[ui].texCoords.BR.x = fSpacingTimesTwo + v2fCurrentPositionInAtlas.x + vImageDims.x;
					imageDetails[ui].texCoords.BR.y = fSpacingTimesTwo + v2fCurrentPositionInAtlas.y + vImageDims.y;
					imageDetails[ui].texCoords.TR.x = imageDetails[ui].texCoords.BR.x;
					imageDetails[ui].texCoords.TR.y = imageDetails[ui].texCoords.TL.y;
					imageDetails[ui].texCoords.BL.x = imageDetails[ui].texCoords.TL.x;
					imageDetails[ui].texCoords.BL.y = imageDetails[ui].texCoords.BR.y;

					// Update maximum atlas size
					if (vv2fAtlasDims[iCurAtlasImage].x < imageDetails[ui].texCoords.BR.x)
						vv2fAtlasDims[iCurAtlasImage].x = imageDetails[ui].texCoords.BR.x;
					if (vv2fAtlasDims[iCurAtlasImage].y < imageDetails[ui].texCoords.BR.y)
						vv2fAtlasDims[iCurAtlasImage].y = imageDetails[ui].texCoords.BR.y;

					// Move position in atlas right
					v2fCurrentPositionInAtlas.x += fSpacingTimesTwo + vImageDims.x;

					// Set atlas image index for current image
					imageDetails[ui].atlasImageNumber = iCurAtlasImage;
				}
			}
		}

		// Now we get here...
		// iCurAtlasImage holds number of atlas images needed
		// vv2fAtlasDims[X] holds each atlas image's dimensions
		// imageDetails[X].sTexCoords holds position in atlas of each individual image
		
		// Create each atlas image
		for (unsigned int ui = 0; ui < (unsigned int)iCurAtlasImage + 1; ui++)
		{
			Image* pNewImage = new Image;
			ErrorIfFalse(pNewImage, L"ImageAtlasPacker::createAtlasImages() failed. Unable to allocate memory for atlas image.");
			pNewImage->createBlank((unsigned int)vv2fAtlasDims[ui].x, (unsigned int)vv2fAtlasDims[ui].y, 4);
			atlasImages.push_back(pNewImage);
		}

		// Now copy each image into atlas image
		for (unsigned int ui = 0; ui < vImages.size(); ui++)
		{
			vImages[ui]->copyRectTo(*atlasImages[imageDetails[ui].atlasImageNumber],
				0, 0,
				(int)vImages[ui]->getWidth(),
				(int)vImages[ui]->getHeight(),
				(int)imageDetails[ui].texCoords.TL.x,
				(int)imageDetails[ui].texCoords.TL.y);

			// Now compute texture coordinates
			Vector2f vAtlasDims = atlasImages[imageDetails[ui].atlasImageNumber]->getDimensions();
			// Prevent divide by zero
			ErrorIfTrue(vAtlasDims.x < 1.0f, L"ImageAtlasPacker::createAtlasImages() failed. Atlas width less than 1.");
			ErrorIfTrue(vAtlasDims.y < 1.0f, L"ImageAtlasPacker::createAtlasImages() failed. Atlas height less than 1.");
			Vector2f vAtlasDimsRecip(1.0f / vAtlasDims.x, 1.0f / vAtlasDims.y);

			// Compute correct image positions, not including the spacing
			// From above, top left position is correct for x and y.
			// All others include spacing and need to be recalculated.
			imageDetails[ui].texCoords.BR.x = imageDetails[ui].texCoords.TL.x + imageDetails[ui].dimensions.x;
			imageDetails[ui].texCoords.BR.y = imageDetails[ui].texCoords.TL.y + imageDetails[ui].dimensions.y;
			imageDetails[ui].texCoords.TR.x = imageDetails[ui].texCoords.TL.x + imageDetails[ui].dimensions.x;
			imageDetails[ui].texCoords.TR.y = imageDetails[ui].texCoords.TL.y;
			imageDetails[ui].texCoords.BL.x = imageDetails[ui].texCoords.TL.x;
			imageDetails[ui].texCoords.BL.y = imageDetails[ui].texCoords.TL.y + imageDetails[ui].dimensions.y;

			// Now all positions are correct

			// Now convert from pixel position in image to 0.0f - 1.0f
			imageDetails[ui].texCoords.BL.x *= vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.BL.y *= vAtlasDimsRecip.y;
			imageDetails[ui].texCoords.BR.x *= vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.BR.y *= vAtlasDimsRecip.y;
			imageDetails[ui].texCoords.TL.x *= vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.TL.y *= vAtlasDimsRecip.y;
			imageDetails[ui].texCoords.TR.x *= vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.TR.y *= vAtlasDimsRecip.y;

			// For nearest filtering this is all we have to do.
			// However, with linear filtering, we need to offset the texture coordinates
			// by half a texel so that everything looks great when using linear filtering.
			imageDetails[ui].texCoords.BL.x += vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.BL.y -= vAtlasDimsRecip.y;
			imageDetails[ui].texCoords.BR.x -= vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.BR.y -= vAtlasDimsRecip.y;
			imageDetails[ui].texCoords.TL.x += vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.TL.y += vAtlasDimsRecip.y;
			imageDetails[ui].texCoords.TR.x -= vAtlasDimsRecip.x;
			imageDetails[ui].texCoords.TR.y += vAtlasDimsRecip.y;
		}

		// Now we're done, we setup the hashmap for fast lookup of image details by individual image names
		for (unsigned int ui = 0; ui < imageDetails.size(); ui++)
		{
			mapImageDetails[imageDetails[ui].imageFilename] = imageDetails[ui];
		}
		
		// Free each individual loaded image
		for (unsigned int ui = 0; ui < vImages.size(); ui++)
		{
			delete vImages[ui];
		}
		vImages.clear();
	}

	unsigned int ImageAtlasPacker::getNumAtlases(void) const
	{
		return (unsigned int)atlasImages.size();
	}

	Image* ImageAtlasPacker::getAtlasImage(unsigned int index) const
	{
		ErrorIfTrue(0 == atlasImages.size(), L"ImageAtlasPacker::getAtlasImage(" + std::to_wstring(index) + L") failed. There are no atlas images.");
		ErrorIfTrue(index >= atlasImages.size(), L"ImageAtlasPacker::getAtlasImage(" + std::to_wstring(index) + L") failed. Invalid atlas index given.");
		return atlasImages[index];
	}

	std::vector<ImageAtlasDetails> ImageAtlasPacker::getAllImageDetails(void) const
	{
		ErrorIfFalse(imageDetails.size(), L"ImageAtlasPacker::getAllImageDetails() failed. No image data currently exists.");
		return imageDetails;
	}

	std::vector<ImageAtlasDetails>* ImageAtlasPacker::getAllImageDetailsPointer(void)
	{
		ErrorIfFalse(imageDetails.size(), L"ImageAtlasPacker::getAllImageDetailsPointer() failed. No image data currently exists.");
		return &imageDetails;
	}

	unsigned int ImageAtlasPacker::getNumIndividualImages(void) const
	{
		return unsigned int(imageDetails.size());
	}

	ImageAtlasDetails ImageAtlasPacker::getImageDetails(const String& imageName) const
	{
		std::map<String, ImageAtlasDetails>::iterator it = mapImageDetails.find(imageName);
		ErrorIfTrue(it == mapImageDetails.end(), L"ImageAtlasPacker::getImageDetails(" + imageName + L") failed. Named image doesn't exist.");
		return it->second;
	}

	ImageAtlasDetails* ImageAtlasPacker::getImageDetailsPointer(const String& imageName) const
	{
		std::map<String, ImageAtlasDetails>::iterator it = mapImageDetails.find(imageName);
		ErrorIfTrue(it == mapImageDetails.end(), L"ImageAtlasPacker::getImageDetailsPointer(" + imageName + L") failed. Named image doesn't exist.");
		return &it->second;
	}

	void ImageAtlasPacker::reset(void)
	{
		for (unsigned int ui = 0; ui < atlasImages.size(); ui++)
		{
			delete atlasImages[ui];
		}
		imageDetails.clear();
		mapImageDetails.clear();
	}

	bool ImageAtlasPacker::getImageExists(const String& imageName) const
	{
		std::map<String, ImageAtlasDetails>::iterator it = mapImageDetails.find(imageName);
		return (it != mapImageDetails.end());
	}
}