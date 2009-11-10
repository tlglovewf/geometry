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
	  \
	  \ Because this class derives from curve2, the same limitation applies for the interpolation functionality:
	  \ if your line segment is a perfectly vertical line, you will have undefined behaviour where interpolated points
	  \ are used.
	  \
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
				return 0;
			} else {
				return -1;	
			}
		}

		//! \brief Reset the segment from a starting point, a direction vector and a length.
		void setValue(Vec2<Type> &start_pt, Vec2<Type> dir_vec, Type length)
		{
			Vec2<Type> pts[2];
			
			dir_vec.normalize();

			dir_vec *= length;

			pts[0] = start_pt;
			pts[1] = start_pt + dir_vec;

			Curve2<Type>::setPoints(pts, 2);
		}

		//! \brief Returns the absolute distance between 2 points
		static Type getPtDistance(Vec2<Type> pt1, Vec2<Type> pt2)
		{
			double distance;

			distance = sqrt(SQR(pt1.y() - pt2.y()) + SQR(pt1.x() - pt2.x()));

			return (Type)distance;
		}

		//! \brief Returns the distance from the specified point to the line extended from this line segment.
		Type getDistance(Vec2<Type> point)
		{
			double distance;
			Vec2<Type> intersect;
			double u;
			Vec2<double> pt1, pt2;
			Curve2<Type>::getInitPoint(0, pt1);
			Curve2<Type>::getInitPoint(1, pt2);

			u = ((point.x() - pt1.x()) * (pt2.x() - pt1.x())
				+
			    (point.y() - pt1.y()) * (pt2.y() - pt1.y()))
			        /
			    (SQR(getPtDistance(pt1, pt2)));

			double x = pt1.x() + (u * (pt2.x() - pt1.x()));
			double y = pt1.y() + (u * (pt2.y() - pt1.y()));

			intersect.setValue(x, y);

			distance = getPtDistance(intersect, point);

			return (Type)distance;
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
			Vec2<double> pt1, pt2;
			Curve2<Type>::getInitPoint(0, pt1);
			Curve2<Type>::getInitPoint(1, pt2);

			return (pt2 - pt1).length();
		}
	};

	typedef Segment2<int>    Segment2i;
	typedef Segment2<float>  Segment2f;
	typedef Segment2<double> Segment2d;
} // namespace gtl

#endif
