/*
_______________________________________________________________________
__________________________ G E O M E T R Y ____________________________
|
|	This program is free software; you can redistribute it and/or modify
|   it under the terms of the GNU General Public License as published by
|   the Free Software Foundation; either version 2 of the License, or
|   (at your option) any later version.
|
|   This program is distributed in the hope that it will be useful,
|   but WITHOUT ANY WARRANTY; without even the implied warranty of
|   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|   GNU General Public License for more details.
|
|   You should have received a copy of the GNU General Public License
|   along with this program; if not, write to the Free Software
|   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|
________________ Copyright (C) 2009 Simon Ouellette. __________________
_______________________________________________________________________
*/

#ifndef LINE_H
#define LINE_H

#include <gtl/gtl.hpp>
#include <gtl/vec2.hpp>
#include <gtl/curve.hpp>

namespace gtl
{
	/*!
	  \class Line Line.hpp gtl/Line.hpp
	  \brief Represents a 2D line segment as a special case of a curve that is declared from 2 2D points.
	  \ingroup base
	  */
	template<typename Type>
	class Line : public Curve<Type>
	{
	public:
		//! The default constructor makes a line segment with 2 points.
		Line()
		{
			Vec2<Type> pts[2];

			pts[0].x() = (Type)0.0;
			pts[0].y() = (Type)0.0;
			pts[1].x() = (Type)1.0;
			pts[1].y() = (Type)1.0;

			m_initial_points[0] = pts[0];
			m_initial_points[1] = pts[1];

			Curve<Type>::setPoints(pts, 2);
		}

		//! Constructs a line segment from the 2 provided 2D points.
		Line(Vec2<Type> &start_pt, Vec2<Type> &end_pt)
		{
			if (setPoints(start_pt, end_pt) < 0)
				Line();	// if setPoints fails, construct default line...
		}

		//! Reset the two points from which the line segment is declared.
		int setPoints(Vec2<Type> &start_pt, Vec2<Type> &end_pt)
		{
			Vec2<Type> pts[2];

			pts[0] = start_pt;
			pts[1] = end_pt;

			if (Curve<Type>::setPoints(pts, 2))
			{
				m_initial_points[0] = pts[0];
				m_initial_points[1] = pts[1];

				return 0;
			} else {
				return -1;	
			}
		}

		//! Returns the distance from the specified point to this line
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

		//! Returns the equation of a line in the form y = mx + b, given two 2D points.
		static void getEquation(Vec2<Type> &pt1, Vec2<Type> &pt2, Type &m, Type &b)
		{
			m = (Type)((double)(pt1.y() - pt2.y()) / (double)(pt1.x() - pt2.x()));
			b = pt1.y() - (m * pt1.x());
		}

	protected:
		Vec2<Type>	m_initial_points[2];	// the 2 initial points from which this line was declared
};

typedef Line<int>    Linei;
typedef Line<float>  Linef;
typedef Line<double> Lined;
} // namespace gtl

#endif
