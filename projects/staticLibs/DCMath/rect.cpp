#include "rect.h"

namespace DC
{
	Rect::Rect()
	{
	}

	Rect::Rect(int minXPARAM, int minYPARAM, int maxXPARAM, int maxYPARAM)
	{
		minX = minXPARAM;
		minY = minYPARAM;
		maxX = maxXPARAM;
		maxY = maxYPARAM;
	}

	void Rect::set(int minXPARAM, int minYPARAM, int maxXPARAM, int maxYPARAM)
	{
		minX = minXPARAM;
		minY = minYPARAM;
		maxX = maxXPARAM;
		maxY = maxYPARAM;
	}

	bool Rect::intersects(const Rect& other) const
	{
		if (maxX < other.minX)
			return false;
		if (minX > other.maxX)
			return false;
		if (maxY < other.minY)
			return false;
		if (minY > other.maxY)
			return false;

		return true;
	}

	void Rect::resizeArea(int multiplier)
	{
		// Get current width and height of the rect
		int iCurrentWidth = maxX - minX;
		int iCurrentHeight = maxY - minY;

		// Compute new dimensions
		int iNewWidth = iCurrentWidth * multiplier;
		int iNewHeight = iCurrentHeight * multiplier;

		// Quarter the new dims, to get the amount to offset min and max by
		int iOffsetX = iNewWidth / 4;
		int iOffsetY = iNewHeight / 4;

		// Offset mvMin and mvMax
		minX -= iOffsetX;
		maxX += iOffsetX;
		minY -= iOffsetY;
		maxY += iOffsetY;
	}

	bool Rect::doesPositionFitWithin(int positionX, int positionY)
	{
		if (positionX < minX)
			return false;
		if (positionX > maxX)
			return false;
		if (positionY < minY)
			return false;
		if (positionY > maxY)
			return false;
		return true;
	}

	bool Rect::doesPositionFitWithin(const Vector2f& position)
	{
		return doesPositionFitWithin(int(position.x), int(position.y));
	}

	void Rect::getCentre(int& centreX, int& centreY)
	{
		// Compute dims
		centreX = maxX - minX;
		centreY = maxY - minY;

		// Compute centre
		centreX /= 2;
		centreY /= 2;
	}
}