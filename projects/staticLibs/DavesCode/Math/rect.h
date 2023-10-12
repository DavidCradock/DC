#pragma once
#include "vector2f.h"

namespace DC
{
	// Represents a non-uniform cube aka a rectangle
	// Used to compute whether two rects intersect one another
	class Rect
	{
	public:
		// Default constructor, sets everything to zero
		Rect();

		// Construct the rectangle, setting to passed values
		Rect(int minX, int minY, int maxX, int maxY);

		// Sets this rect using given values
		void set(int minX, int minY, int maxX, int maxY);

		// Returns true if the passed Rect intersects with this one
		bool intersects(const Rect& other) const;

		// Resizes the rect so that it's dimensions are multiplied by the given int.
		// Example:
		// If min = (0,0) and max = (10,10) (total dims of (10,10)) and the given int is 2 then the new values would be
		// min = (-5,-5) and max = (15,15) (total dims of (20, 20))
		void resizeArea(int multiplier);

		// Given a position, returns whether it fits within this rect or not
		bool doesPositionFitWithin(int positionX, int positionY);

		// Given a position, returns whether it fits within this rect or not
		bool doesPositionFitWithin(const Vector2f& position);

		// Returns the centre position of the rect
		void getCentre(int& centreX, int& centreY);

		int minX;		// Left
		int minY;		// Bottom
		int maxX;		// Right
		int maxY;		// Top
	};
}
