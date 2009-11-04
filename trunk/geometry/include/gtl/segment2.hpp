/*
_______________________________________________________________________
__________________________ G E O M E T R Y ____________________________
|
| THIS FILE IS PART OF THE GEOMETRY TEMPLATE LIBRARY.
| USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     
| GOVERNED BY A BSD-STYLE SOURCE LICENSE.
| PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       
_______________________________________________________________________
_______________________________________________________________________
*/

#ifndef SEGMENT2_H
#define SEGMENT2_H

#include <gtl/gtl.hpp>
#include <gtl/vec2.hpp>
#include <gtl/curve2.hpp>

namespace gtl
{
	/*!
	  \class Segment2 segment2.hpp gtl/segment2.hpp
	  \brief Represents a 2D line segment as a special case of a curve that is declared from 2 2D points.
	  \ingroup base
	  */
	template<typename Type>
	class Segment2 : public Curve2<Type>
	{
	public:
		//! \brief The default constructor makes a line segment with 2 points.
		Segment2()
		{
			Vec2<Type> pts[2];

			pts[0].x() = (Type)0.0;
			pts[0].y() = (Type)0.0;
			pts[1].x() = (Type)1.0;
			pts[1].y() = (Type)1.0;

			m_initial_points[0] = pts[0];
			m_initial_points[1] = pts[1];

			Curve2<Type>::setPoints(pts, 2);
		}

		//! \brief Constructs a line segment from the 2 provided 2D points.
		Segment2(Vec2<Type> &start_pt, Vec2<Type> &end_pt)
		{
			if (setPoints(start_pt, end_pt) < 0)
				Segment2();	// if setPoints fails, construct default line...
		}

		//! \brief Reset the two points from which the line segment is declared.
		int setPoints(Vec2<Type> &start_pt, Vec2<Type> &end_pt)
		{
			Vec2<Type> pts[2];

			pts[0] = start_pt;
			pts[1] = end_pt;

			if (Curve2<Type>::setPoints(pts, 2) == 0)
			{
				m_initial_points[0] = pts[0];
				m_initial_points[1] = pts[1];

				return 0;
			} else {
				return -1;	
			}
		}

		//! \brief Returns the distance from the specified point to the line extended from this line segment.
		Type getDistance(Vec2<Type> point)
		{
			Type y, m, x, b;	// variables of the line equation
			Type distance;

			y = m_initial_points[0].y();
			x = m_initial_points[0].x();
			getEquation(m_initial_points[0], m_initial_points[1], m, b);

			distance = (Type)(fabs( ((double)m * (double)point.x()) - point.y() + b ) / (double) sqrt( SQR(m) + 1.0 ));

			return distance;
		}

		//! \brief Returns the equation of a line in the form y = mx + b, given two 2D points.
		static void getEquation(Vec2<Type> &pt1, Vec2<Type> &pt2, Type &m, Type &b)
		{
			m = (Type)((double)(pt1.y() - pt2.y()) / (double)(pt1.x() - pt2.x()));
			b = pt1.y() - (m * pt1.x());
		}

		//! \brief Returns the length of the line segment.
		Type length()
		{
			return (m_initial_points[0] - m_initial_points[1]).length();
		}

	protected:
		Vec2<Type>	m_initial_points[2];	// the 2 initial points from which this line was declared
	};

	typedef Segment2<int>    Segment2i;
	typedef Segment2<float>  Segment2f;
	typedef Segment2<double> Segment2d;
} // namespace gtl

#endif
