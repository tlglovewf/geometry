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

#ifndef POINTSET2_H
#define POINTSET2_H

#include <gtl/gtl.hpp>
#include <gtl/vec2.hpp>

namespace gtl
{
	/*!
	  \class PointSet2 pointset2.hpp gtl/pointset2.hpp
	  \brief A generic set of independent 2D points.
	  \
	  \ Will implement various utility functions and transforms, such as rotation, scaling, etc.
	  \ All 2D shape classes should derive from this class.
	  \
	  \ingroup base
	  */
	template<typename Type>
	class PointSet2
	{
	public:
		//! The default constructor creates one 2D point at the origin.
		PointSet2()
		{
			Vec2<Type> origin((Type)0.0, (Type)0.0);

			setPoints(&origin, 1);
		}

		//! Saves the provided list of 2D points.
		PointSet2(const Vec2<Type> *pts, int num_points)
		{
			if (setPoints(pts, num_points) < 0)
				PointSet2();	// if setVertices fails, construct default polyhedron...
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

			return 0;
		}

		//! \brief Rotates the specified piont around the pivot by the specified angle in radians
		static void rotate(Vec2<Type> &point, Vec2<Type> &pivot, Type angle)
		{
			Type new_x, new_y;

			// first, substract the pivot offsets from the point to rotate (make it relative to origin)
			point.x() -= pivot.x();
			point.y() -= pivot.y();

			// second, rotate point around the origin
			new_x = (Type)((point.x() * cos((double)angle)) - (point.y() * sin((double)angle)));
			new_y = (Type)((point.x() * sin((double)angle)) + (point.y() * cos((double)angle)));

			// third, add the pivot offsets back to the point
			point.x() = new_x + pivot.x();
			point.y() = new_y + pivot.y();
		}

		//! \brief Rotates all points in the set around the pivot point by the specified angle in radians
		void rotate(Vec2<Type> &pivot, Type angle)
		{
			for (std::size_t i = 0; i < m_points.size(); i++)
			{
				rotate(m_points[i], pivot, angle);
			}
		}

		//! \brief Reset the initial points describing the curves (pre-interpolation)
		//!
		//! This one is for point vectors.
		int setPoints(std::vector< Vec2<Type> > &pts)
		{
			m_points = pts;
			return 0;
		}

		Vec2<Type> getMinX() const
		{
			const Vec2<Type> *start_pt = &m_points[0];

			for (std::size_t i = 1; i < m_points.size(); i++)
			{
				if (m_points[i].x() < start_pt->x())
				{
					start_pt = &m_points[i];
				}
			}

			return *start_pt;
		}

		Vec2<Type> getMaxX() const
		{
			const Vec2<Type> *end_pt = &m_points[0];

			for (std::size_t i = 1; i < m_points.size(); i++)
			{
				if (m_points[i].x() > end_pt->x())
				{
					end_pt = &m_points[i];
				}
			}

			return *end_pt;
		}

		//! \brief Returns a reference to the point vector.
		std::vector< Vec2<Type> > &getPointVector()
		{
			return m_points;
		}

		//! \brief Returns the number of points in the point vector.
		unsigned int getNumPoints() const
		{
			return m_points.size();
		}
	private:
		std::vector< Vec2<Type> > 	m_points;
	};

typedef PointSet2<int>    PointSet2i;
typedef PointSet2<float>  PointSet2f;
typedef PointSet2<double> PointSet2d;
} // namespace gtl

#endif
