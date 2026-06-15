#pragma once

namespace Rasterizer
{
	// TODO
	/// -----------------------------------------------------------------------
	/// \fn		FillTriangleNaive
	/// \brief	Rasterizes a CCW triangle defined by p1, p2, p3 using the naive 
	///			algorithm described in class. Each pixel in the triangle share
	///			the same color.
	/// \param	p1	First triangle vertex position.
	///	\param	p2	Second triangle vertex position.
	///	\param	p3	Third triangle vertex position.
	//	\param	c	Color to fill the triangle with.
	void FillTriangleNaive(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, const Color& c);

	/// -----------------------------------------------------------------------
	/// \fn		FillTriangleTopLeft
	/// \brief	Rasterizes a CCW triangle defined by p1, p2, p3, using the top
	///			left method described in class. Each pixel in the triangle share
	///			the same color. 
	/// \param	p1	First triangle vertex position.
	///	\param	p2	Second triangle vertex position.
	///	\param	p3	Third triangle vertex position.
	//	\param	c	Color to fill the triangle with.	
	void FillTriangleTopLeft(const AEVec2& v0, const AEVec2& v1, const AEVec2& v2, const Color& c);

	// PROVIDED
	/// -----------------------------------------------------------------------
	/// \enum	EDrawTriangleMethod
	/// \brief	Enumeration of all the internal algorithms used for rasterizing 
	///			with interpolation of color.
	enum EDrawTriangleMethod { eDT_BILINEAR, eDT_PLANE_NORMAL, eDT_BARYCENTRIC };

	// TODO
	/// -----------------------------------------------------------------------
	/// \fn		GetDrawTriangleMethod
	/// \brief	Getter for the current method used when DrawTriangle is called. 
	EDrawTriangleMethod GetDrawTriangleMethod();

	// TODO
	/// -----------------------------------------------------------------------
	/// \fn		SetDrawTriangleMethod
	/// \brief	Setter for the current method used when DrawTriangle is called. 
	void SetDrawTriangleMethod(EDrawTriangleMethod dtm);


	// TODO
	/// -----------------------------------------------------------------------
	/// \fn		DrawTriangle
	/// \brief	Rasterizes a CCW triangle defined by v0, v1, v2. It delegates
	///			the call to one of the methods implemented internally. See
	///			enum EDrawTriangleMethod above. 
	/// \param	v0	First triangle vertex (position/color).
	///	\param	v1	Second triangle vertex (position/color).
	///	\param	v2	Third triangle vertex (position/color).
	void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	/// ------------------------------------------------------------------------
	/// \fn		DrawTriangleBiLinear
	/// \brief	Rasterizes a CCW triangle defined by v0, v1, v2, using the top
	///			left method and the bi-linear interpolation method for color 
	/// \param	v0	First triangle vertex (position/color).
	///	\param	v1	Second triangle vertex (position/color).
	///	\param	v2	Third triangle vertex (position/color).

	void DrawTriangleBiLinear(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	void DrawTrianglePlaneNormal(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	void DrawTriangleBarycentric(const Vertex& v0, const Vertex& v1, const Vertex& v2);
}