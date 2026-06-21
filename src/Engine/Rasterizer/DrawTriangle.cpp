#include <AEEngine.h>
#include <algorithm>
#include "Rasterizer.h"
namespace Rasterizer
{
	void triangleCases(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, AEVec2& top, AEVec2& mid, AEVec2& bot) {
		if (v0.y < v1.y) {
			//case 1
			if (v1.y < v2.y) {
				top = v2;
				mid = v1;
				bot = v0;
			}
			//case 3
			else if (v2.y > v0.y) {
				top = v1;
				mid = v2;
				bot = v0;
			}
			//case 2
			else {
				top = v1;
				mid = v0;
				bot = v2;
			}
		}
		else {
			//case 4
			if (v0.y < v2.y) {
				top = v2;
				mid = v0;
				bot = v1;
			}
			//case 5
			else if (v1.y > v2.y) {
				top = v0;
				mid = v1;
				bot = v2;
			}
			//case 6
			else {
				top = v0;
				mid = v2;
				bot = v1;
			}
		}
	}

	void FillTriangleNaive(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, const Color& c) {
		//triangle case identification
		AEVec2 top, mid, bot;
		//reorder the points
		triangleCases(v0, v1, v2, top, mid, bot);

		//check if the middle point is on the left using the dot product of the vectors
		float dotProd = (mid.x - top.x) * (bot.y - top.y) - (mid.y - top.y) * (bot.x - top.x);
		bool midIsLeft = 0;
		if (dotProd > 0)
			midIsLeft = 1;

		//get the inverted slopes
		float invSlopeTM = ((mid.x - top.x) / (mid.y - top.y));
		float invSlopeTB = ((bot.x - top.x) / (bot.y - top.y));
		float invSlopeMB = ((bot.x - mid.x) / (bot.y - mid.y));

		//set up the x to traverse each side of the triangle
		float xL = top.x;
		float xR = top.x;

		int y;
		//go from the top to the middle
		for (y = Round(top.y); y >= Round(mid.y); --y) {
			for (int x = Round(xL); x <= Round(xR); ++x) {
				FrameBuffer::SetPixel(x, y, c);
			}
			//update left and right depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeTM;
				xR -= invSlopeTB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeTM;
			}
		}

		//adjust xL or xR for any floating point error.
		if (midIsLeft)
			xL = mid.x;
		else
			xR = mid.x;

		//go from the middle to the bottom
		for (; y >= Round(bot.y); --y) {
			for (int x = Round(xL); x <= Round(xR); ++x) {
				FrameBuffer::SetPixel(x, y, c);
			}
			//update left and right depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeMB;
				xR -= invSlopeTB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeMB;
			}
		}
	}

	void FillTriangleTopLeft(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, const Color& c)
	{
		//triangle case identification
		AEVec2 top, mid, bot;
		//reorder the points
		triangleCases(v0, v1, v2, top, mid, bot);

		//check if the middle point is on the left using the dot product of the vectors
		float dotProd = (mid.x - top.x) * (bot.y - top.y) - (mid.y - top.y) * (bot.x - top.x);
		bool midIsLeft = 0;
		if (dotProd > 0)
			midIsLeft = 1;

		//get the inverted slopes
		float invSlopeTM = ((mid.x - top.x) / (mid.y - top.y));
		float invSlopeTB = ((bot.x - top.x) / (bot.y - top.y));
		float invSlopeMB = ((bot.x - mid.x) / (bot.y - mid.y));

		//set up the x to traverse each side of the triangle
		float xL = top.x;
		float xR = top.x;

		int y;
		//go from the top to the middle, adjusting for top-left rule
		for (y = Ceiling(top.y); y >= Ceiling(mid.y) + 1; --y) {
			for (int x = Round(xL); x <= Round(xR) - 1; ++x) {
				FrameBuffer::SetPixel(x, y, c);
			}
			//update left and right depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeTM;
				xR -= invSlopeTB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeTM;
			}
		}

		//adjust xL or xR for any floating point error.
		if (midIsLeft)
			xL = mid.x;
		else
			xR = mid.x;

		//go from the middle to the bottom, adjusting for top-left rule
		for (; y >= Floor(bot.y); --y) {
			for (int x = Round(xL); x <= Round(xR) - 1; ++x) {
				FrameBuffer::SetPixel(x, y, c);
			}
			//update left and right depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeMB;
				xR -= invSlopeTB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeMB;
			}
		}
	}

	/// -----------------------------------------------------------------------
	///	DRAW TRIANGLE ALGORITHM IMPLEMENTATIONS
	/// 
	/// 
	/// enum	EDrawLineMethod
	///	\brief	Specifies which method should be used when drawing a line. 
	EDrawTriangleMethod currentDrawTriangle = eDT_BILINEAR;

	void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		if (currentDrawTriangle == eDT_BILINEAR)
			DrawTriangleBiLinear(v0, v1, v2);
		if (currentDrawTriangle == eDT_PLANE_NORMAL)
			DrawTrianglePlaneNormal(v0, v1, v2);
		if (currentDrawTriangle == eDT_BARYCENTRIC)
			DrawTriangleBarycentric(v0, v1, v2);
	}

	//another version of the triangle cases adapted for vertex
	void triangleCasesVertex(const Vertex& v0, const Vertex& v1, const Vertex& v2, Vertex& top, Vertex& mid, Vertex& bot) {
		if (v0.mPosition.y < v1.mPosition.y) {
			//case 1
			if (v1.mPosition.y < v2.mPosition.y) {
				top = v2;
				mid = v1;
				bot = v0;
			}
			//case 3
			else if (v2.mPosition.y > v0.mPosition.y) {
				top = v1;
				mid = v2;
				bot = v0;
			}
			//case 2
			else {
				top = v1;
				mid = v0;
				bot = v2;
			}
		}
		else {
			//case 4
			if (v0.mPosition.y < v2.mPosition.y) {
				top = v2;
				mid = v0;
				bot = v1;
			}
			//case 5
			else if (v1.mPosition.y > v2.mPosition.y) {
				top = v0;
				mid = v1;
				bot = v2;
			}
			//case 6
			else {
				top = v0;
				mid = v2;
				bot = v1;
			}
		}
	}

	void DrawTriangleBiLinear(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		//triangle case identification
		Vertex top, mid, bot;
		//reorder the points
		triangleCasesVertex(v0, v1, v2, top, mid, bot);

		//check if the middle point is on the left using the dot product of the vectors
		float dotProd = (mid.mPosition.x - top.mPosition.x) * (bot.mPosition.y - top.mPosition.y) - (mid.mPosition.y - top.mPosition.y) * (bot.mPosition.x - top.mPosition.x);
		bool midIsLeft = 0;
		if (dotProd > 0)
			midIsLeft = 1;

		//get the inverted slopes
		float invSlopeTM = 0;
		if (mid.mPosition.y != top.mPosition.y)
			invSlopeTM = ((mid.mPosition.x - top.mPosition.x) / (mid.mPosition.y - top.mPosition.y));
		float invSlopeTB = ((bot.mPosition.x - top.mPosition.x) / (bot.mPosition.y - top.mPosition.y));
		float invSlopeMB = ((bot.mPosition.x - mid.mPosition.x) / (bot.mPosition.y - mid.mPosition.y));

		//get the steps of the color
		Color stepTM = Color();
		if (mid.mPosition.y != top.mPosition.y)
			stepTM = (mid.mColor - top.mColor) / (mid.mPosition.y - top.mPosition.y);
		stepTM.a = 0;
		Color stepTB = (bot.mColor - top.mColor) / (bot.mPosition.y - top.mPosition.y);
		stepTB.a = 0;
		Color stepMB = (bot.mColor - mid.mColor) / (bot.mPosition.y - mid.mPosition.y);
		stepMB.a = 0;

		//set up the x and color to traverse each side of the triangle
		float xL = top.mPosition.x;
		float xR = top.mPosition.x;
		Color cL = top.mColor;
		Color cR = top.mColor;

		//if the top of the triangle is flat, change right or left to fix it
		if (mid.mPosition.y == top.mPosition.y) {
			if (mid.mPosition.x < top.mPosition.x)
				cL = mid.mColor;
			else
				cR = mid.mColor;
		}

		int y;
		//go from the top to the middle
		for (y = Round(top.mPosition.y); y > Round(mid.mPosition.y); --y) {
			//initialize the color to be used
			Color c = cL;
			//get the step of the color for the line currently being drawn
			Color stepIn = (cL - cR) / (xL - xR);
			//draw a line using linear interpolation from left to right
			for (int x = Round(xL); x < Round(xR); ++x) {
				FrameBuffer::SetPixel(x, y, c);
				c += stepIn;
			}
			//update left and right of x and color depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeTM;
				xR -= invSlopeTB;
				cL -= stepTM;
				cR -= stepTB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeTM;
				cL -= stepTB;
				cR -= stepTM;
			}
		}

		//adjust xL or xR for any floating point error
		if (midIsLeft){
			xL = mid.mPosition.x;
			cL = mid.mColor;
		}
		else {
			xR = mid.mPosition.x;
			cR = mid.mColor;
		}

		//go from the middle to the bottom
		for (; y >= Round(bot.mPosition.y); --y) {
			//initialize the color to be used
			Color c = cL;
			//get the step of the color for the line currently being drawn
			Color stepIn = (cL - cR) / (xL - xR);
			//draw a line using linear interpolation from left to right
			for (int x = Round(xL); x < Round(xR); ++x) {
				FrameBuffer::SetPixel(x, y, c);
				c += stepIn;
			}
			//update left and right of x and color depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeMB;
				xR -= invSlopeTB;
				cL -= stepMB;
				cR -= stepTB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeMB;
				cL -= stepTB;
				cR -= stepMB;
			}
		}
	}

	void DrawTrianglePlaneNormal(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		//triangle case identification
		Vertex top, mid, bot;
		//reorder the points
		triangleCasesVertex(v0, v1, v2, top, mid, bot);

		//check if the middle point is on the left using the dot product of the vectors
		float dotProd = (mid.mPosition.x - top.mPosition.x) * (bot.mPosition.y - top.mPosition.y) - (mid.mPosition.y - top.mPosition.y) * (bot.mPosition.x - top.mPosition.x);
		bool midIsLeft = 0;
		if (dotProd > 0)
			midIsLeft = 1;

		//get the inverted slopes
		float invSlopeTM = 0;
		if (mid.mPosition.y != top.mPosition.y)
			invSlopeTM = ((mid.mPosition.x - top.mPosition.x) / (mid.mPosition.y - top.mPosition.y));
		float invSlopeTB = ((bot.mPosition.x - top.mPosition.x) / (bot.mPosition.y - top.mPosition.y));
		float invSlopeMB = ((bot.mPosition.x - mid.mPosition.x) / (bot.mPosition.y - mid.mPosition.y));

		//set up the x and color to traverse each side of the triangle
		float xL = top.mPosition.x;
		float xR = top.mPosition.x;

		//get the normal on red
		AEVec3 R01 = AEVec3(mid.mPosition.x - top.mPosition.x, mid.mPosition.y - top.mPosition.y, mid.mColor.r - top.mColor.r);
		AEVec3 R02 = AEVec3(bot.mPosition.x - top.mPosition.x, bot.mPosition.y - top.mPosition.y, bot.mColor.r - top.mColor.r);
		AEVec3 normalR = (R01.Cross(R02)).Normalize();

		//get the normal on green
		AEVec3 G01 = AEVec3(mid.mPosition.x - top.mPosition.x, mid.mPosition.y - top.mPosition.y, mid.mColor.g - top.mColor.g);
		AEVec3 G02 = AEVec3(bot.mPosition.x - top.mPosition.x, bot.mPosition.y - top.mPosition.y, bot.mColor.g - top.mColor.g);
		AEVec3 normalG = (G01.Cross(G02)).Normalize();

		//get the normal on blue
		AEVec3 B01 = AEVec3(mid.mPosition.x - top.mPosition.x, mid.mPosition.y - top.mPosition.y, mid.mColor.b - top.mColor.b);
		AEVec3 B02 = AEVec3(bot.mPosition.x - top.mPosition.x, bot.mPosition.y - top.mPosition.y, bot.mColor.b - top.mColor.b);
		AEVec3 normalB = (B01.Cross(B02)).Normalize();

		//get the normal on alpha
		AEVec3 A01 = AEVec3(mid.mPosition.x - top.mPosition.x, mid.mPosition.y - top.mPosition.y, mid.mColor.a - top.mColor.a);
		AEVec3 A02 = AEVec3(bot.mPosition.x - top.mPosition.x, bot.mPosition.y - top.mPosition.y, bot.mColor.a - top.mColor.a);
		AEVec3 normalA = (A01.Cross(A02)).Normalize();

		//get the steps of the colors
		Color cStepX = Color(-normalR.x / normalR.z, -normalG.x / normalG.z, -normalB.x / normalB.z, -normalA.x / normalA.z);
		Color cStepY = Color(-normalR.y / normalR.z, -normalG.y / normalG.z, -normalB.y / normalB.z, -normalA.y / normalA.z);

		int y;
		Color c, cL;
		cL = top.mColor;
		//go from the top to the middle
		for (y = Round(top.mPosition.y); y > Round(mid.mPosition.y); --y) {
			c = cL;
			//draw a line using linear interpolation from left to right
			for (int x = Round(xL); x < Round(xR); ++x) {
				FrameBuffer::SetPixel(x, y, c);
				//"move" the color
				c += cStepX;
			}
			//update left and right of x and color left depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeTM;
				xR -= invSlopeTB;
				cL -= cStepY + cStepX * invSlopeTM;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeTM;
				cL -= cStepY + cStepX * invSlopeTB;
			}
		}

		//adjust xL or xR for any floating point error
		if (midIsLeft) {
			xL = mid.mPosition.x;
		}
		else {
			xR = mid.mPosition.x;
		}

		//go from the middle to the bottom
		for (; y >= Round(bot.mPosition.y); --y) {
			c = cL;
			//draw a line using linear interpolation from left to right
			for (int x = Round(xL); x < Round(xR); ++x) {
				FrameBuffer::SetPixel(x, y, c);
				//"move" the color
				c += cStepX;
			}
			//update left and right of x and color left depending on which side the middle point is
			if (midIsLeft) {
				xL -= invSlopeMB;
				xR -= invSlopeTB;
				cL -= cStepY + cStepX * invSlopeMB;
			}
			else {
				xL -= invSlopeTB;
				xR -= invSlopeMB;
				cL -= cStepY + cStepX * invSlopeTB;
			}
		}
	}

	void DrawTriangleBarycentric(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
		//copy the vertexes to non constant vertexes
		Vertex V0 = v0, V1 = v1, V2 = v2;

		//Calculate the bounding box yMin, yMax, xMin, xMax
		float xMin = min(V0.mPosition.x, min(V1.mPosition.x, V2.mPosition.x));
		float xMax = max(V0.mPosition.x, max(V1.mPosition.x, V2.mPosition.x));

		float yMin = min(V0.mPosition.y, min(V1.mPosition.y, V2.mPosition.y));
		float yMax = max(V0.mPosition.y, max(V1.mPosition.y, V2.mPosition.y));

		//get the vectors from v0 to v1 and v0 to v2
		Vertex v0v1 = { V1.mPosition - V0.mPosition , V1.mColor - V0.mColor };
		Vertex v0v2 = { V2.mPosition - V0.mPosition , V2.mColor - V0.mColor };

		//get n
		float n = (V1.mPosition.x - V0.mPosition.x) * (V2.mPosition.y - V0.mPosition.y) - (V1.mPosition.y - V0.mPosition.y) * (V2.mPosition.x - V0.mPosition.x);
		//Ensure counter clockwise order
		if (n < 0.f) {
			n = -n;
			std::swap(V1, V2);
		}

		//Calculate Lambda increments using partial derivatives
		float Lambda_1_XInc = (V2.mPosition.y - V0.mPosition.y) / n;
		float Lambda_1_YInc = (V0.mPosition.x - V2.mPosition.x) / n;

		float Lambda_2_XInc = (V0.mPosition.y - V1.mPosition.y) / n;
		float Lambda_2_YInc = (V1.mPosition.x - V0.mPosition.x) / n;

		//barycentrics coordinates at first pixel

		AEVec2 p((float)Round(xMin), (float)Round(yMin));

		//calculate the initial value of lambda1 and lambda2 for first pixel and row
		float Lambda1Row = ((V2.mPosition.y - V0.mPosition.y) * p.x + (V0.mPosition.x - V2.mPosition.x) * p.y + (V2.mPosition.x * V0.mPosition.y - V2.mPosition.y * V0.mPosition.x)) / n;
		float Lambda2Row = ((V0.mPosition.y - V1.mPosition.y) * p.x + (V1.mPosition.x - V0.mPosition.x) * p.y + (V0.mPosition.x * V1.mPosition.y - V0.mPosition.y * V1.mPosition.x)) / n;

		//traverse the bounding square on the y axis
		for (int y = Round(yMin); y <= Round(yMax); ++y) {
			//get the 1 and 2 lambdas to be moved
			float Lambda_1 = Lambda1Row, Lambda_2 = Lambda2Row;
			//traverse the bounding square on the x axis
			for (int x = Round(xMin); x <= Round(xMax); ++x) {
				//calculate 0 lambda
				float Lambda_0 = 1 - Lambda_1 - Lambda_2;
				//check the lambdas are inside the triangle
				if (Lambda_0 >= 0 && Lambda_1 >= 0 && Lambda_2 >= 0) {
					//Interpolate the color using the Lambdas 
					Color c = V0.mColor * Lambda_0 + V1.mColor * Lambda_1 + V2.mColor * Lambda_2;
					//draw the pixel
					FrameBuffer::SetPixel(x, y, c);
				}
				//increase the lambdas
				Lambda_1 += Lambda_1_XInc, Lambda_2 += Lambda_2_XInc;
			}
			//increase the lambda rows
			Lambda1Row += Lambda_1_YInc, Lambda2Row += Lambda_2_YInc;
		}
	}

	EDrawTriangleMethod GetDrawTriangleMethod() {
		return currentDrawTriangle;
	}

	void SetDrawTriangleMethod(EDrawTriangleMethod triangleMethod) {
		currentDrawTriangle = triangleMethod;
	}
}