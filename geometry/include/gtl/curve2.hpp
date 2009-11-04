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

#ifndef CURVE2_H
#define CURVE2_H

#include <gtl/gtl.hpp>
#include <gtl/vec2.hpp>

namespace gtl
{
	/*!
	  \class Curve2 curve2.hpp gtl/curve2.hpp
	  \brief Represents a 2D curve as a collection of non-continuous 2D points
	  \ingroup base
	  */
	template<typename Type>
	class Curve2
	{
	public:
		typedef enum
		{
			INTERPOL_LINEAR,
			INTERPOL_AKIMA,
			NUM_INTERPOL_TYPES
		} INTERPOL_TYPES;

		//! The default constructor makes a line segment with 2 points.
		Curve2()
		{
			Vec2<Type> pts[2];

			m_interpol_x = NULL;
			m_interpol_y = NULL;
			m_interpol_z = NULL;
			m_interpol_t = NULL;

			pts[0].x() = (Type)0.0;
			pts[0].y() = (Type)0.0;
			pts[1].x() = (Type)1.0;
			pts[1].y() = (Type)1.0;

			setPoints(pts, 2);
			setInterpol(INTERPOL_LINEAR);	// default is linear
		}

		//! Constructs a curve from the provided list of 2D points.
		Curve2(const Vec2<Type> *pts, int num_points)
		{
			m_interpol_x = NULL;
			m_interpol_y = NULL;
			m_interpol_z = NULL;
			m_interpol_t = NULL;

			if (setPoints(pts, num_points) < 0)
				Curve2();	// if setVertices fails, construct default polyhedron...

			setInterpol(INTERPOL_LINEAR);	// default is linear
		}

		//! Performs an interpolation of the specified type (see INTERPOL_TYPES) on the current points for this curve,
		//! for the specified resolution.
		void setInterpol(INTERPOL_TYPES type)
		{
			switch(type)
			{
				case INTERPOL_AKIMA:
					init_akima(m_points, m_points.size());
					break;

				case INTERPOL_LINEAR:
					init_linear(m_points, m_points.size());
					break;
			}
		}

		//! Default destructor frees the internal list of 2D points.
		virtual ~Curve2()
		{
			if (m_interpol_x != NULL)
				delete [] m_interpol_x;

			if (m_interpol_y != NULL)
				delete [] m_interpol_y;

			if (m_interpol_t != NULL)
				delete [] m_interpol_t;

			if (m_interpol_z != NULL)
				delete [] m_interpol_z;
		}

		//! \brief Reset the initial points describing the curves (pre-interpolation)
		//!
		//! This one is for point arrays.
		int setPoints(const Vec2<Type> *pts, int num_points)
		{
			m_points.resize(num_points);

			for (int i = 0; i < num_points; i++)
			{
				m_points[i].setValue(pts[i].x(), pts[i].y());
			}

			setInterpol(m_current_interpol);	// re-interpolate

			return 0;
		}

		//! \brief Reset the initial points describing the curves (pre-interpolation)
		//!
		//! This one is for point vectors.
		int setPoints(std::vector< Vec2<Type> > &pts, int num_points)
		{
			m_points = pts;
			setInterpol(m_current_interpol);	// re-interpolate

			return 0;
		}

		//! \brief Returns how many points points were used initially to declare the curve.
		int getNumInitPoints() const
		{
			return (int)m_points.size();
		}

		//! \brief Puts into "point" the initial curve declaration point indexed by "index".
		//!
		//! returns 0 if the point exists, -1 if it doesn't.
		int getInitPoint(int pt_index, Vec2<Type> &point) const
		{
			if (pt_index >= (int)m_points.size())
			{
				return -1;
			} else {
				point = m_points[pt_index];
				return 0;
			}
		}

		//! \brief Returns the interpolated point along the curve at position x along the x axis.
		//! 
		//! returns 0 on success, -1 on error.
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

			for (std::size_t i = 1; i < m_points.size(); i++)
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

			for (std::size_t i = 1; i < m_points.size(); i++)
			{
				if (m_points[i].x() > end_pt->x())
				{
					end_pt = &m_points[i];
				}
			}

			return *end_pt;
		}

		// ! \brief Establishes a chord along the curve starting a point "start_pt" of length "chord_len", and
		// ! returns the end point of this chord in "end_pt".
		// !
		// ! The start_pt is the first point, along the x-axis, of the chord. So the end_pt that will be found always
		// ! lies further along the x-axis than start_pt. If this end_pt goes past the end of the curve, an error condition
		// ! is returned.
		// !
		// ! returns 0 on success, -1 on error.
		int getChord(Vec2<Type> &start_pt, Type chord_len, Vec2<Type> &end_pt, Type precision)
		{
			// check if the start_pt is on the curve.
			if (start_pt.x() < start().x() || start_pt.x() > end().x())
			{
				return -1;
			}

			// check if the end_pt is on the curve.
			if ((start_pt - end()).length() < chord_len)
			{
				return -1;
			}

			// all seems good, now figure out the end_pt
			Vec2<double> pt;
			if (getPoint(start_pt.x() + chord_len, pt))
				return -1;

			while ((start_pt - pt).length() > chord_len && pt.x() > start_pt.x())
			{
				if (getPoint(pt.x() - precision, pt))
					return -1;
			}

			pt.x() += precision;

			end_pt = pt;

			return 0;
		}
	private:
		std::vector< Vec2<Type> > 	m_points;

		INTERPOL_TYPES  m_current_interpol;

		double		*m_interpol_x;	// x values for the initial points
		double		*m_interpol_y;	// y values for the initial points
		double		*m_interpol_z;	// for akima interpolation only
		double		*m_interpol_t;	// for akima interpolation only

		void init_linear(std::vector< Vec2<Type> > &pts, std::size_t num_points)
		{
		    if (num_points == 0)
		        return;

		    m_current_interpol = INTERPOL_LINEAR;

		    if (m_interpol_x != NULL)
		        delete [] m_interpol_x;

		    m_interpol_x = new double [num_points];
			
		    if (m_interpol_y != NULL)
		        delete [] m_interpol_y;

		    m_interpol_y = new double [num_points];

            	    for (std::size_t i = 0; i < num_points; ++i)
		    {
		        m_interpol_x[i] = pts[i].x();
		        m_interpol_y[i] = pts[i].y();
		    }
		}

		void init_akima(std::vector< Vec2<Type> > &pts, std::size_t num_points)
		{
		    if (num_points == 0)
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

		    for (std::size_t i = 0; i < num_points; ++i)
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

typedef Curve2<int>    Curve2i;
typedef Curve2<float>  Curve2f;
typedef Curve2<double> Curve2d;
} // namespace gtl

#endif
