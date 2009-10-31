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

#ifndef CURVE_H
#define CURVE_H

#include <gtl/gtl.hpp>
#include <gtl/vec2.hpp>

namespace gtl
{
	/*!
	  \class Curve Curve.hpp gtl/Curve.hpp
	  \brief Represents a 2D curve as a collection of non-continuous 2D points
	  \ingroup base
	  */
	template<typename Type>
	class Curve
	{
	public:
		typedef enum
		{
			INTERPOL_LINEAR,
			INTERPOL_AKIMA,
			NUM_INTERPOL_TYPES
		} INTERPOL_TYPES;

		//! The default constructor makes a line segment with 2 points.
		Curve()
		{
			Vec2<Type> pts[2];

			m_points = NULL;
			m_num_points = 0;

			m_interpol_x = NULL;
			m_interpol_y = NULL;
			m_interpol_z = NULL;
			m_interpol_t = NULL;

			pts[0].x() = (Type)0.0;
			pts[0].y() = (Type)0.0;
			pts[1].x() = (Type)1.0;
			pts[1].y() = (Type)1.0;

			setPoints(pts, 2);
			init_linear(pts, 2);
		}

		//! Constructs a curve from the provided list of 2D points.
		Curve(const Vec2<Type> *pts, int num_points)
		{
			m_points = NULL;
			m_num_points = 0;

			m_interpol_x = NULL;
			m_interpol_y = NULL;
			m_interpol_z = NULL;
			m_interpol_t = NULL;

			if (setPoints(pts, num_points) < 0)
				Curve();	// if setVertices fails, construct default polyhedron...

			init_linear(pts, 2);
		}

		//! Performs an interpolation of the specified type (see INTERPOL_TYPES) on the current points for this curve,
		//! for the specified resolution.
		void set_interpol(INTERPOL_TYPES type)
		{
			switch(type)
			{
				case INTERPOL_AKIMA:
					init_akima(m_points, m_num_points);
					break;

				case INTERPOL_LINEAR:
					init_linear(m_points, m_num_points);
					break;
			}
		}

		//! Default destructor frees the internal list of 2D points.
		virtual ~Curve()
		{
			if (m_points != NULL)
				delete [] m_points;

			if (m_interpol_x != NULL)
				delete [] m_interpol_x;

			if (m_interpol_y != NULL)
				delete [] m_interpol_y;

			if (m_interpol_t != NULL)
				delete [] m_interpol_t;

			if (m_interpol_z != NULL)
				delete [] m_interpol_z;
		}

		//! Reset the initial points describing the curves (pre-interpolation)
		int setPoints(const Vec2<Type> *pts, int num_points)
		{
			if (m_points != NULL)
				delete [] m_points;

			m_points = new Vec2<Type> [num_points];
			m_num_points = num_points;

			for (int i = 0; i < m_num_points; i++)
			{
				m_points[i].setValue(pts[i].x(), pts[i].y());
			}

			set_interpol(m_current_interpol);	// re-interpolate

			return 0;
		}

		//! Returns the point along the curve at position x along the x axis. Returns 0 on success,
		//! -1 on error.
		int getPoint(Type x, Vec2<Type> &result) const
		{
			result.x() = x;

			switch(m_current_interpol)
			{
				case INTERPOL_LINEAR:
					result.y() = get_linear(x);
					return 0;

				case INTERPOL_AKIMA:
					result.y() = get_akima(x);
					return 0;
			}

			return -1;
		}

		//! Returns a reference to the start point of the curve.
		Vec2<Type> &start()
		{
			Vec2<Type> *start_pt = &m_points[0];

			for (int i = 1; i < m_num_points; i++)
			{
				if (m_points[i].x() < start_pt->x())
				{
					start_pt = &m_points[i];
				}
			}

			return *start_pt;
		}

		//! Returns a reference to the end point of the curve.
		Vec2<Type> &end()
		{
			Vec2<Type> *end_pt = &m_points[0];

			for (int i = 1; i < m_num_points; i++)
			{
				if (m_points[i].x() > end_pt->x())
				{
					end_pt = &m_points[i];
				}
			}

			return *end_pt;
		}

	protected:
		INTERPOL_TYPES  m_current_interpol;
		Vec2<Type> 	*m_points;
		int		m_num_points;

		double		*m_interpol_x;	// x values for the initial points
		double		*m_interpol_y;	// y values for the initial points
		double		*m_interpol_z;	// for akima interpolation only
		double		*m_interpol_t;	// for akima interpolation only

		void init_linear(const Vec2<Type> *pts, int num_points)
		{
		    if (pts == NULL)
		        return;
			
		    m_current_interpol = INTERPOL_LINEAR;

		    if (m_interpol_x != NULL)
		        delete [] m_interpol_x;

		    m_interpol_x = new double [num_points];
			
		    if (m_interpol_y != NULL)
		        delete [] m_interpol_y;

		    m_interpol_y = new double [num_points];

            	    for (int i = 0; i < num_points; ++i)
		    {
		        m_interpol_x[i] = pts[i].x();
		        m_interpol_y[i] = pts[i].y();

			std::cout << "m_interpol_x[" << i << "] = " << pts[i].x() << std::endl;
			std::cout << "m_interpol_y[" << i << "] = " << pts[i].y() << std::endl;
		    }
		}

		void init_akima(const Vec2<Type> *pts, int num_points)
		{
		    if (pts == NULL)
		        return;
		    
		    m_current_interpol = INTERPOL_AKIMA;

		    if (m_interpol_x != NULL)
		        delete [] m_interpol_x;

		    m_interpol_x = new double [num_points];
			
		    if (m_interpol_y != NULL)
		        delete [] m_interpol_y;

		    m_interpol_y = new double [num_points];

		    if (m_interpol_z != NULL)
		        delete [] m_interpol_z;

		    m_interpol_z = new double [num_points];
			
		    if (m_interpol_t != NULL)
		        delete [] m_interpol_t;

		    m_interpol_t = new double [num_points + 3];

		    for (int i = 0; i < num_points; ++i)
		    {
			    m_interpol_x[i] = pts[i].x();
			    m_interpol_y[i] = pts[i].y();
		    }

		    for (int i = 0; i < num_points - 1; ++i)
		    {
			    m_interpol_t[i + 2] = (pts[i + 1].y() - pts[i].y()) / (pts[i + 1].x() - pts[i].x());
		    }

		    m_interpol_t[1] = (2.0 * m_interpol_t[2]) - m_interpol_t[3];
		    m_interpol_t[0] = (2.0 * m_interpol_t[1]) - m_interpol_t[2];

		    m_interpol_t[num_points + 1] = (2.0 * m_interpol_t[num_points]) - m_interpol_t[num_points - 1];
		    m_interpol_t[num_points + 2] = (2.0 * m_interpol_t[num_points + 1]) - m_interpol_t[num_points];

		    for (int i = 0; i < num_points; ++i)
		    {
			    double left = fabs(m_interpol_t[i + 1] - m_interpol_t[i]);
			    double right = fabs(m_interpol_t[i + 3] - m_interpol_t[i + 2]);

			    // check for division by zero
			    if (left + right != 0.0)
			    {
				    m_interpol_z[i] = ((right * m_interpol_t[i + 1]) + (left * m_interpol_t[i + 2])) / (left + right);
			    } else {
				    // special case
				    m_interpol_z[i] = (m_interpol_t[i + 1] + m_interpol_t[i + 2]) / 2.0;
			    }
		    }
		}

		Type get_linear(Type x_pos) const
		{
			int i = 1;
			double a = 0.0;
			double y = 0.0;
			double x = (double)x_pos;

			while (x > m_interpol_x[i + 1])
				i++;

			a = (x - m_interpol_x[i - 1]) / (m_interpol_x[i] - m_interpol_x[i - 1]);

			y = (a * (m_interpol_y[i] - m_interpol_y[i - 1])) + m_interpol_y[i - 1];

			return (Type)y;
		}

		Type get_akima(Type x_pos) const
		{
			int i = 0;
			double dist_left = 0.0;
			double dist_right = 0.0;
			double y = 0.0;
			double x = (double)x_pos;

			while (x > m_interpol_x[i + 1])
				i++;

			dist_right = m_interpol_x[i + 1] - m_interpol_x[i];
			dist_left = x - m_interpol_x[i];

			y = m_interpol_y[i] + (m_interpol_z[i] * dist_left) + (3.0 * m_interpol_t[i + 2] - 2.0 * m_interpol_z[i] - m_interpol_z[i + 1])
				* SQR(dist_left) / dist_right + (m_interpol_z[i] + m_interpol_z[i + 1] - 2.0 * m_interpol_t[i + 2]) 
				* pow(dist_left, 3.0) / SQR(dist_right);

			return (Type)y;
		}
};

typedef Curve<int>    Curvei;
typedef Curve<float>  Curvef;
typedef Curve<double> Curved;
} // namespace gtl

#endif
