#pragma once
#include "image.h"
#include "../DCMath/vector2f.h"
#include <map>
#include <fstream>

namespace DC
{
	// Holds information about each packed image inside an atlas image
	class ImageAtlasDetails
	{
	public:
		struct STexCoords
		{
			Vector2f vTL;
			Vector2f vTR;
			Vector2f vBL;
			Vector2f vBR;
		};
		// Stuff read/written to file

		// The texture coordinates within the atlas image of the image
		STexCoords sTexCoords;

		// The atlas image number which the image is located in.
		// If setting the maximum size of the atlas images to a small size,
		// multiple atlas images may be created to accomodate all the images.
		unsigned int uiAtlasImage;

		// Whether the image was rotated clockwise to fit better or not.
		bool bRotated;

		// The dimensions of the image in pixels
		Vector2f vDims;

		// The filename which the image was created from.
		String strImageFilename;

		// Writes out this object's data to an already opened ofstream
		// If an error occurs, so does an exception
		void write(std::ofstream& file);

		// Reads in this object's data from an already opened ifsream
		// If an error occurs, so does an exception
		void read(std::ifstream& file);
	};

	// Image atlas class used to pack many smaller images into as few as possible larger atlas images.
	class ImageAtlasPacker
	{
	public:
		ImageAtlasPacker();
		~ImageAtlasPacker();

		// This creates texture atlas images.
		// We provide a vector of strings holding each individual image's filename.
		// Each individual image can have different dimensions.
		// We specify a maximum width and height that an individual atlas image may be.
		// We specify whether to allow or disallow rotation of images to allow better fitting of the images.
		// If rotation is enabled, the computed texture coordinates are rotated accordingly.
		// After calling this, we can call the various other methods to get at the information
		// If the vector of strings is empty, an exception occurs
		// If an individual image's dimensions, including spacing is greater than the max atlas dimensions, an exception occurs.
		// If all given images are the same dimensions, nothing is sorted.
		void createAtlasImages(
			const std::vector<String>& vstrImages,	// A vector of strings holding each individual image's filename which to add to the atlas image/s
			unsigned int uiMaxAtlasImageWidth,			// The maximum allowed width of the created atlas image/s
			unsigned int uiMaxAtlasImageHeight,			// The maximum allowed height of the created atlas image/s
			bool bAllowRotationOfImages,				// Whether to allow rotation of individual images to obtain greater packing efficiency or not.
			unsigned int uiImageSpacing = 1				// Amount of spacing between individual images.
		);

		// Resets everything, freeing computed data and atlas images.
		void reset(void);

		// Returns the number of generated atlas images holding the individual images
		// Call createAtlasImages() first.
		unsigned int getNumAtlases(void) const;

		// Returns a pointer to the atlas image at the specified index
		// Call createAtlasImages() first.
		// If an invalid index is given, or no atlases exist, an exception occurs
		Image* getAtlasImage(unsigned int uiIndex) const;

		// Returns a vector of CImageAtlasDetails containing each image's details
		// Call createAtlasImages() first.
		// If no image data exists, an exception occurs.
		std::vector<ImageAtlasDetails> getAllImageDetails(void) const;

		// Returns a pointer to _mvImageDetails containing each image's details
		// Call createAtlasImages() first.
		// If no image data exists, an exception occurs.
		// Usage:
		// std::vector<CImageAtlasDetails>* pviad = _mAtlases.getAllImageDetailsPointer();
		// unsigned int uiLastImageIndex = pviad->size() - 1;
		// unsigned int uiAtlasNumber = (*pviad)[uiLastImageIndex].uiAtlasImage;
		// or
		// unsigned int uiAtlasNumber = pviad->operator[](uiLastImageIndex).uiAtlasImage;
		// or
		// unsigned int uiAtlasNumber = pviad[0][uiLastImageIndex].uiAtlasImage;
		std::vector<ImageAtlasDetails>* getAllImageDetailsPointer(void);

		// Returns the total number of individual images added to all the atlases
		// Call createAtlasImages() first.
		unsigned int getNumIndividualImages(void) const;

		// Returns the named image's details
		// Call createAtlasImages() first.
		// If an invalid image name is given, an exception occurs
		ImageAtlasDetails getImageDetails(const String& strImageName) const;

		// Returns a pointer to the named image's details
		// Call createAtlasImages() first.
		// If an invalid image name is given, an exception occurs
		ImageAtlasDetails* getImageDetailsPointer(const String& strImageName) const;

		// Returns whether the named image exists or not
		bool getImageExists(const String& strImageName) const;
	private:
		std::vector<Image*> _mvAtlasImages;				// Holds each atlas image which contain all the added images.
		std::vector<ImageAtlasDetails> _mvImageDetails;	// Holds each image's details

		// This is so we can lookup an image by name using getImageDetails()
		mutable std::map<String, ImageAtlasDetails> _mmapImageDetails;
	};
}