#ifndef __SGP_PRIMITIVETYPE_HEADER__
#define __SGP_PRIMITIVETYPE_HEADER__

	//! Enumeration for all primitive types there are.
	enum SGP_PRIMITIVE_TYPE
	{
		//! All vertices are non-connected points.
		SGPPT_POINTS = 0,

		//! All vertices form a single connected line.
		SGPPT_LINE_STRIP,

		//! Just as LINE_STRIP, but the last and the first vertex is also connected.
		SGPPT_LINE_LOOP,

		//! Every two vertices are connected creating n/2 lines.
		SGPPT_LINES,

		//! After the first two vertices each vertex defines a new triangle.
		//! Always the two last and the new one form a new triangle.
		SGPPT_TRIANGLE_STRIP,

		//! After the first two vertices each vertex defines a new triangle.
		//! All around the common first vertex.
		SGPPT_TRIANGLE_FAN,

		//! Explicitly set all vertices for each triangle.
		SGPPT_TRIANGLES,

#if !defined(BUILD_OGLES2)
		//! The single vertices are expanded to quad billboards on the GPU.
		SGPPT_POINT_SPRITES,
#endif
	};

#endif		// __SGP_PRIMITIVETYPE_HEADER__