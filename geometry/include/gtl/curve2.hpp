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
#include <gtl/pointset2.hpp>

namespace gtl
{
	/*!
	  \class Curve2 curve2.hpp gtl/curve2.hpp
	  \brief Represents a 2D curve as a collection of non-continuous 2D points.
	  \
	  \ The interpolation functionality only works if the defined curve has only 1 y value
	  \ per x value. Otherwise, there will be undefined behaviour for all methods using interpolated
	  \ points.
	  \
	  \ingroup base
	  */
	template<typename Type>
	class Curve2 : public PointSet2<Type>
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
					init_akima(PointSet2<Type>::getPointVector(), PointSet2<Type>::getNumPoints());
					break;

				case INTERPOL_LINEAR:
					init_linear(PointSet2<Type>::getPointVector(), PointSet2<Type>::getNumPoints());
					break;
			}
		}

		//! \brief Reset the initial points describing the curves (pre-interpolation)
		//!
		//! This one is for point arrays.
		int setPoints(const Vec2<Type> *pts, int num_points)
		{
			PointSet2<Type>::setPoints(pts, num_points);

			setInterpol(m_current_interpol);	// re-interpolate

			return 0;
		}

		//! \brief Reset the initial points describing the curves (pre-interpolation)
		//!
		//! This one is for point vectors.
		int setPoints(std::vector< Vec2<Type> > &pts)
		{
			PointSet2<Type>::setPoints(pts);

			setInterpol(m_current_interpol);	// re-interpolate

			return 0;
		}

		//! \brief Returns how many points points were used initially to declare the curve.
		int getNumInitPoints() const
		{
			return PointSet2<Type>::getNumPoints();
		}

		//! \brief Puts into "point" the initial curve declaration point indexed by "index".
		//!
		//! returns 0 if the point exists, -1 if it doesn't.
		int getInitPoint(int pt_index, Vec2<Type> &point)
		{
			if (pt_index >= (int)PointSet2<Type>::getNumPoints())
			{
				return -1;
			} else {
				point = PointSet2<Type>::getPointVector().at(pt_index);
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

		// ! \brief Establishes a chord along the curve starting a point "start_pt" of length "chord_len", and
		// ! returns the end point of this chord in "end_pt".
		// !
		// ! The start_pt is the first point, along the x-axis, of the chord. So the end_pt that will be found always
		// ! lies further along the x-axis than start_pt. If this end_pt goes past the end of the curve, an error condition
		// ! is returned.
		// !
		// ! returns 0 on success, -1 on error.
		int getChord(Vec2<Type> &start_pt, Type chord_len, Vec2<Type> &end_pt, Type precision) const
		{
			// check if the start_pt is on the curve.
			if (start_pt.x() < PointSet2<Type>::getMinX().x() || start_pt.x() > PointSet2<Type>::getMaxX().x())
			{
				return -1;
			}

			// check if the end_pt is on the curve.
			if ((start_pt - PointSet2<Type>::getMaxX()).length() < chord_len)
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

		//! \brief Goes through all the interpolated curve points by an x increment of "precision" until
		//! it finds the point with the minimum y value.
		void getMinY(Vec2<Type> &pt, Type precision) const
		{
			Vec2<Type> start_pt, end_pt;
			Vec2<Type> min_pt;

			start_pt = PointSet2<Type>::getMinX();
			end_pt = PointSet2<Type>::getMaxX();

			min_pt = start_pt;
			for (Type x = start_pt.x() + precision; x <= end_pt.x(); x+=precision)
			{
				Vec2<Type> tmp_pt;
				if (getPoint(x, tmp_pt) < 0)
					continue;

				if (tmp_pt.y() < min_pt.y())
					min_pt = tmp_pt;
			}

			pt = min_pt;
		}

		//! \brief Goes through all the interpolated curve points by an x increment of "precision" until
		//! it finds the point with the maximum y value.
		void getMaxY(Vec2<Type> &pt, Type precision) const
		{
			Vec2<Type> start_pt, end_pt;
			Vec2<Type> max_pt;

			start_pt = PointSet2<Type>::getMinX();
			end_pt = PointSet2<Type>::getMaxX();

			max_pt = start_pt;
			for (Type x = start_pt.x() + precision; x <= end_pt.x(); x+=precision)
			{
				Vec2<Type> tmp_pt;
				if (getPoint(x, tmp_pt) < 0)
					continue;

				if (tmp_pt.y() > max_pt.y())
					max_pt = tmp_pt;
			}

			pt = max_pt;
		}

		//! \brief Performs a rotation and re-interpolates from the new points
		void rotate(Vec2<Type> &pivot, Type angle)
		{
			PointSet2<Type>::rotate(pivot, angle);
			setInterpol(m_current_interpol);	// re-interpolate!	
		}

		//! \brief Finds all intersection points of the two curves, between x_start and x_end inclusively.
		void findIntersections(Curve2<Type> seg, Type precision_x, Type precision_y, Type x_start, Type x_end)
		{
			int intersect_pt_counter = 0;
			m_intersect_points.clear();	// reset the list

			// for all x values along the segment...
			for (Type x = x_start; x <= x_end; x+=precision_x)
			{
				Vec2<Type> curve_pt;
				Vec2<Type> seg_pt;

				if (getPoint(x, curve_pt) < 0)
					continue;

				if (seg.getPoint(x, seg_pt) < 0)
					continue;

				if (curve_pt.y() + precision_y >= seg_pt.y() &&
				    curve_pt.y() - precision_y <= seg_pt.y())
				{
					m_intersect_points.push_back(seg_pt);

					intersect_pt_counter++;
				}
			}
		}

		//! \brief Returns the number of intersection points found by findIntersections.
		unsigned int getNumIntersectPoints() const
		{
			return 	m_intersect_points.size();
		}

		//! \brief Returns a specific intersection point found by findIntersections.
		//!
		//! returns 0 on success, -1 on failure.
		int getIntersectPoint(int pt_index, Vec2<double> &pt) const
		{
			if (pt_index >= (int)m_intersect_points.size())
			{
				return -1;
			}

			pt = m_intersect_points[pt_index];
		}

	private:
		std::vector< Vec2<Type> > m_intersect_points;	// points found by findIntersections

		INTERPOL_TYPES  m_current_interpol;

		std::vector<double> m_interpol_x;	// x values for the initial points
		std::vector<double> m_interpol_y;	// y values for the initial points
		std::vector<double> m_interpol_z;	// for akima interpolation only
		std::vector<double> m_interpol_t;	// for akima interpolation only

		void init_linear(std::vector< Vec2<Type> > &pts, std::size_t num_points)
		{
		    if (num_points == 0)
		        return;

		    m_current_interpol = INTERPOL_LINEAR;

		    m_interpol_x.clear();
		    m_interpol_x.resize(num_points);

		    m_interpol_y.clear();
		    m_interpol_y.resize(num_points);

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

		    m_interpol_x.clear();
		    m_interpol_x.resize(num_points);
			
		    m_interpol_y.clear();
		    m_interpol_y.resize(num_points);
			    
		    m_interpol_z.clear();
		    m_interpol_z.resize(num_points);
			    
		    m_interpol_t.clear();
		    m_interpol_t.resize(num_points + 3);

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

			while (x > m_interpol_x[i])
				i++;

			if (i >= (int)PointSet2<Type>::getNumPoints())
			{
				// special case: we want the y of the last point
				y = PointSet2<Type>::getMaxX().y();

				return (Type)y;
			}

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
