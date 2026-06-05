#include <AEEngine.h>
#include "Rasterizer.h"


namespace Rasterizer
{
	/// -----------------------------------------------------------------------
	///	LINE ALGORITHM IMPLEMENTATIONS
	/// 
	/// 
	/// enum	EDrawLineMethod
	///	\brief	Specifies which method should be used when drawing a line. 
	EDrawLineMethod currentDrawLine = eDL_NAIVE;

	void DrawHorizontalLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//first point is on the left
		if (p1.x < p2.x) {
			//go right from that point drawing each pixel
			for (AEVec2 point = p1; point.x < p2.x; point.x++) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
		//second point is on the left
		else {
			//go right from that point drawing each pixel
			for (AEVec2 point = p2; point.x < p1.x; point.x++) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
	}

	void DrawVerticalLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//first point is lower
		if (p1.y < p2.y) {
			//go up from that point drawing each pixel
			for (AEVec2 point = p1; point.y < p2.y; point.y++) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
		//first point is lower
		else {
			//go up from that point drawing each pixel
			for (AEVec2 point = p2; point.y < p1.y; point.y++) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
	}

	void DrawDiagonalLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//going north-east
		if (p1.x < p2.x && p1.y < p2.y) {
			//go up from point one to point two drawing each pixel
			for (AEVec2 point = p1; point.y < p2.y; point.x++, point.y++) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
		//going south-east
		else if (p1.x < p2.x && p1.y > p2.y) {
			//go up from point one to point two drawing each pixel
			for (AEVec2 point = p1; point.y > p2.y; point.x++, point.y--) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
		//going north-west
		else if (p1.x > p2.x && p1.y < p2.y) {
			//go up from point one to point two drawing each pixel
			for (AEVec2 point = p1; point.y < p2.y; point.x--, point.y++) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
		//going south-west
		else {
			//go up from point one to point two drawing each pixel
			for (AEVec2 point = p1; point.y > p2.y; point.x--, point.y--) {
				FrameBuffer::SetPixel(Round(point.x), Round(point.y), c);
			}
		}
	}

	void DrawRect(const AEVec2& r, const AEVec2& size, const Color& c) {
		//get the for corners
		AEVec2 TopLeft = r + AEVec2(-size.x / 2.f, size.y / 2.f);
		AEVec2 TopRight = r + AEVec2(size.x / 2.f, -size.y / 2.f);
		AEVec2 BotLeft = r + AEVec2(-size.x / 2.f, -size.y / 2.f);
		AEVec2 BotRight = r + AEVec2(size.x / 2.f, size.y / 2.f);

		//draw horizontal lines on the top and bottom
		DrawHorizontalLine(TopLeft, TopRight, c);
		DrawHorizontalLine(BotLeft, BotRight, c);
		//draw vertical lines on the sides
		DrawVerticalLine(TopRight, BotRight, c);
		DrawVerticalLine(TopLeft, BotLeft, c);
	}

	void DrawLine(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//distances from point to point
		float deltaX = p2.x - p1.x;
		float deltaY = p2.y - p1.y;

		//the line is horizontal if it doesnt change on y
		if (Round(deltaY) == 0) {
			DrawHorizontalLine(p1, p2, c);
		}
		//the line is vertical if it doesnt change on x
		else if (Round(deltaX) == 0) {
			DrawVerticalLine(p1, p2, c);
		}
		//the line is diagonal if the slope is exactly one (or minus one)
		else if (abs(deltaY / deltaX) == 1) {
			DrawDiagonalLine(p1, p2, c);
		}
		//in any other case, use the specialized functions
		else {
			//check the drawing type we are using
			if (currentDrawLine == eDL_NAIVE)
				DrawLineNaive(p1, p2, c);
			if (currentDrawLine == eDL_DDA)
				DrawLineDDA(p1, p2, c);
			if (currentDrawLine == eDL_BRESENHAM)
				DrawLineBresenham(p1, p2, c);
		}
	}

	void DrawLineNaive(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//get the explicit line equation parameters
		float m = (p2.y - p1.y) / (p2.x - p1.x);
		float b = p1.y - (p1.x * m);
		//make an step
		float step = 1.f;

		//slope is < 1
		if (abs(m) < 1) {
			//check the direction of the line
			if ((p2.x - p1.x) < 0) {
				step = -1.f;
			}
			//set the begining and end of the line
			int sX = Round(p1.x);
			int eX = Round(p2.x) + step;
			//move from that point drawing each pixel
			for (int x = sX; x != eX; x += step) {
				//get the new y
				float y = (m * x) + b;
				FrameBuffer::SetPixel(x, Round(y), c);
			}
		}
		//slope is > 1
		else {
			//check the direction of the line
			if ((p2.y - p1.y) < 0) {
				step = -1.f;
			}
			//set the begining and end of the line
			int sY = Round(p1.y);
			int eY = Round(p2.y) + step;
			//move from that point drawing each pixel
			for (int y = sY; y != eY; y += step) {
				//get the new x
				float x = (y - b) / m;
				FrameBuffer::SetPixel(Round(x), y, c);
			}
		}
	}

	void DrawLineDDA(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//get the explicit line equation parameters
		float m = (p2.y - p1.y) / (p2.x - p1.x);
		float b = p1.y - (p1.x * m);

		//slope is < 1
		if (abs(m) < 1) {
			//get the rate of change
			float deltaX = 1;
			//check the direction of the line
			if (p2.x < p1.x) {
				deltaX = -1;
			}

			//get the begining and the end of the line
			int sX = Round(p1.x);
			int eX = Round(p2.x) + deltaX;
			//get the first y
			float y = p1.y;

			//move from that point drawing each pixel
			for (int x = sX; x != eX; x += deltaX) {
				//increase y
				y += m * deltaX;
				FrameBuffer::SetPixel(x, Round(y), c);
			}
		}
		//slope is > 1
		else {
			//get the rate of change
			float deltaY = 1;
			//check the direction of the line
			if (p2.y < p1.y) {
				deltaY = -1;
			}

			//get the begining and the end of the line
			int sY = Round(p1.y);
			int eY = Round(p2.y) + deltaY;
			//get the first x
			float x = p1.x;

			//move from that point drawing each pixel
			for (int y = sY; y != eY; y += deltaY) {
				//increase x
				x += (1 / m) * deltaY;
				FrameBuffer::SetPixel(Round(x), y, c);
			}
		}
	}

	void DrawLineBresenham(const AEVec2& p1, const AEVec2& p2, const Color& c) {
		//get the deltas for x and y
		int deltaX = p2.x - p1.x;
		int deltaY = p2.y - p1.y;
		//get the slope
		float m = (p2.y - p1.y) / (p2.x - p1.x);

		//get the decision parameter
		int dp = (2 * abs(deltaY)) - abs(deltaX);

		//get the steps depending on the direction
		int stepX = 1;
		if (deltaX < 0)
			stepX = -1;
		int stepY = 1;
		if (deltaY < 0)
			stepY = -1;

		//cases 1, 4, 5 and 8
		if (abs(m) < 1.f) {
			//set the begining and the end, as well as the first y
			int xs = Round(p1.x);
			int xe = Round(p2.x);

			int y = Round(p1.y);

			//loop around the x axis drawing each pixel of the line
			for (unsigned int i = xs; i != xe; i += stepX) {
				FrameBuffer::SetPixel(i, y, c);
				//move to the secondary direction depending on the case
				//NE(1), NW(4), SW(5), SE(8)
				if (dp > 0) {
					//move on the y axis
					y += stepY;
					//move the decision parameter
					dp += (2 * abs(deltaY)) - (2 * abs(deltaX));
				}
				//move to the main direction depending on the case
				//E(1,8), W(4,5)
				else
					//move the decision parameter
					dp += 2 * abs(deltaY);
			}
		}
		//cases 2, 3, 6 and 7
		else {
			//set the begining and the end, as well as the first x
			int ys = Round(p1.y);
			int ye = Round(p2.y);

			int x = Round(p1.x);

			//loop around the y axis drawing each pixel of the line
			for (unsigned int i = ys; i != ye; i += stepY) {
				FrameBuffer::SetPixel(x, i, c);
				//move to the secondary direction depending on the case
				//NE(2), NW(3), SW(6), SE(7)
				if (dp > 0) {
					//move on the x axis
					x += stepX;
					dp += (2 * abs(deltaX)) - (2 * abs(deltaY));
				}
				//move to the main direction depending on the case
				//N(2,3), S(6,7)
				else {
					dp += 2 * abs(deltaX);
				}
			}
		}
	}

	void DrawLine(const AEVec2& p1, const Color& c1, const AEVec2& p2, const Color& c2) 
	{
		//get the explicit line equation parameters
		float m = (p2.y - p1.y) / (p2.x - p1.x);
		float b = p1.y - (p1.x * m);
		//make an step
		float step = 1.f;

		//slope is < 1
		if (abs(m) < 1) {
			//get the color's step depending on the position of p1 and p2 on the x axis
			Color colorStep = (c1 - c2) / (p1.x - p2.x);
			if (p2.x < p1.x)
				colorStep = (c1 - c2) / (p2.x - p1.x);
			//c is the color that will be changed each iteration
			Color c = c1;
			//check the direction of the line
			if ((p2.x - p1.x) < 0) {
				step = -1.f;
			}
			//set the begining and end of the line
			int sX = Round(p1.x);
			int eX = Round(p2.x) + step;
			//move from that point drawing each pixel
			for (int x = sX; x != eX; x += step) {
				//get the new y
				float y = (m * x) + b;
				FrameBuffer::SetPixel(x, Round(y), c);
				//change the color
				c += colorStep;
			}
		}
		//slope is > 1
		else {
			//get the color's step depending on the position of p1 and p2 on the y axis
			Color colorStep = (c1 - c2) / (p1.y - p2.y);
			if (p2.y < p1.y)
				colorStep = (c1 - c2) / (p2.y - p1.y);
			Color c = c1;
			//check the direction of the line
			if ((p2.y - p1.y) < 0) {
				step = -1.f;
			}
			//set the begining and end of the line
			int sY = Round(p1.y);
			int eY = Round(p2.y) + step;
			//move from that point drawing each pixel
			for (int y = sY; y != eY; y += step) {
				//get the new x
				float x = (y - b) / m;
				FrameBuffer::SetPixel(Round(x), y, c);
				//change the color
				c += colorStep;
			}
		}
	}
	
	/// @TODO
	// ------------------------------------------------------------------------
	/// \fn	GetDrawLineMethod
	/// \brief	Return the current draw line method.
	EDrawLineMethod GetDrawLineMethod() {
		return currentDrawLine;
	}

	/// @TODO
	// ------------------------------------------------------------------------
	/// \fn	GetDrawLineMethod
	/// \brief	Set the current draw line method to that given as input.
	void SetDrawLineMethod(EDrawLineMethod lineMethod) {
		currentDrawLine = lineMethod;
	}
}