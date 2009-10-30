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

#ifndef RECTPRISM_H
#define RECTPRISM_H

#include <gtl/geometry.hpp>
#include <gtl/vec3.hpp>
#include <gtl/ray.hpp>
#include <gtl/polyhedron.hpp>
#include <gtl/matrix3.hpp>
#include <vector>

namespace geo
{
    /*!
    \class RectPrism RectPrism.hpp geometry/RectPrism.hpp
    \brief Represents a rectangular prism as a collection of 8 3D vertices
    \ingroup base
    */

    template<typename Type>
	class RectPrism : public Polyhedron<Type>
    {
    public:
        //! The default constructor makes a cube.
        RectPrism()
        {
			geo::Vec3<Type> pts[8];

			if (m_vertices != NULL)
			{
				delete [] m_vertices;
			}
			m_vertices = NULL;
			m_num_vertices = 0;

			pts[0].setValue((Type)0.0, (Type)0.0, (Type)0.0);
			pts[1].setValue((Type)1.0, (Type)0.0, (Type)0.0);
			pts[2].setValue((Type)1.0, (Type)1.0, (Type)0.0);
			pts[3].setValue((Type)0.0, (Type)1.0, (Type)0.0);
			pts[4].setValue((Type)0.0, (Type)0.0, (Type)1.0);
			pts[5].setValue((Type)1.0, (Type)0.0, (Type)1.0);
			pts[6].setValue((Type)1.0, (Type)1.0, (Type)1.0);
			pts[7].setValue((Type)0.0, (Type)1.0, (Type)1.0);

			setVertices(pts);

			m_fill_init = 0;
			m_resolution = 1.0;
			m_num_fill_points = 1;
			
			m_i_dim = 1.0;
			m_j_dim = 1.0;
			m_k_dim = 1.0;

			m_ray_i = NULL;
			m_ray_j = NULL;
			m_ray_k = NULL;
        }

        //!	Constructs a polyhedron from the provided list of 3D points.
        RectPrism(Vec3<Type> *pts)
        {
			m_vertices = NULL;
			m_num_vertices = 0;

            if (setVertices(pts) < 0)
				RectPrism();	// if setVertices fails, construct default polyhedron...
        }

		int setVertices(Vec3<Type> *pts)
		{
			return Polyhedron::setVertices(pts, 8);
		}

		int getNumFillPoints()
		{
			if (m_fill_init == 0)
				prepareFill(1.0);

			return m_num_fill_points;
		}

		// this method must be called before calling any of the other fill methods
		void prepareFill(double resolution)
		{
			// 1. Start with arbitrary point (m_vertices[0])
			// 2. Find its 3 linearly independent link points (the 2 closest points are alway part of
			// the list of 3 linearly independent connected points, and the 3rd one is obtained from
			// the cross product of the 2 produced vectors)
			Vec3<Type> link_points[3];
			double min_dist[2];
			Vec3<Type> link_vectors[3];
			int min_index = 0;	// index of the vertex that is the first closest one to vertex 0

			for (int i = 0; i < 2; i++)
			{
				min_dist[i] = DBL_MAX;
				for (int j = 1; j < 8; j++)
				{
					if (j != min_index)
					{
						Vec3<Type> vec;

						vec.calcVector(m_vertices[0], m_vertices[j]);
						if (vec.length() < min_dist[i])
						{
							link_vectors[i] = vec;
							min_dist[i] = vec.length();
							min_index = j;
							link_points[i] = m_vertices[j];
						}
					}
				}
			}

			// preliminary link_vectors[2]... can point in the opposite direction.
			link_vectors[2] = link_vectors[0].cross(link_vectors[1]);

			// Establish 3 linearly independent rays from these points
			m_ray_i = new Ray<Type>(m_vertices[0], link_vectors[0]);
			m_ray_j = new Ray<Type>(m_vertices[0], link_vectors[1]);
			m_ray_k = new Ray<Type>(m_vertices[0], link_vectors[2]);

			// find link_points[2] (so that we can establish k_dim)
			Vec3<Type> closest_vertex;
			double min_ray_dist = DBL_MAX;

			for (int i = 1; i < 8; i++)
			{
				if (m_vertices[i] == link_points[0])
					continue;

				if (m_vertices[i] == link_points[1])
					continue;

				// NOTE: Ray::getDistance works in such a way that whether it doesn't matter if
				// the cross-product vector link_vectors[2] is currently pointing in the wrong direction
				double dist = m_ray_k->getDistance(m_vertices[i]);

				if (dist < min_ray_dist)
				{
					min_ray_dist = dist;
					closest_vertex = m_vertices[i];
				}
			}

			link_points[2] = closest_vertex;
			
			// now make sure the link_vectors[2] vector points in the right direction for our ray_k, because voxelize() will
			// depend on that direction being the right one...
			Vec3<Type> vec;
			vec.calcVector(m_vertices[0], link_points[2]);
			link_vectors[2] = vec;
			
			m_ray_k->setValue(m_ray_k->getOrigin(), link_vectors[2]);

			// Establish width, height and length, for index calculation
			m_i_dim = (link_points[0] - m_vertices[0]).length();
			m_j_dim = (link_points[1] - m_vertices[0]).length();
			m_k_dim = (link_points[2] - m_vertices[0]).length();

			m_resolution = resolution;
			m_fill_init = 1;

			voxelize();
		}

		int getFillPoint(int index, Vec3<Type> &pt)
		{
			if (m_fill_init == 0)
				prepareFill(1.0);

			if (index < 0 || index >= m_num_fill_points)
			{
				pt.setValue(0.0, 0.0, 0.0);
				return -1;
			} else {
				pt = m_fill_points[index];
				return 0;
			}
		}

        //! Default destructor frees the internal list of 3D points.
        virtual ~RectPrism()
		{
		}

	private:
		int m_fill_init;
		double m_resolution;
		
		// 3 linearly independent rays meeting at an arbitrarily chosen corner
		Ray<Type> *m_ray_i;
		Ray<Type> *m_ray_j;
		Ray<Type> *m_ray_k;

		// Width, height and length
		double m_i_dim;
		double m_j_dim;
		double m_k_dim;

		std::vector< Vec3<Type> > m_fill_points;
		int m_num_fill_points;

		// generates the actual discrete fill points in m_fill_points
		void voxelize()
		{
			// traverse the linearly independent rays (local coordinate system) three-dimensionally
			for (double i = 0.0; round_to(i, m_resolution) < m_i_dim; i+=(m_resolution / 2.0))
			{
				Vec3<Type> ray_j_offset = m_ray_i->getValue(i);

				m_ray_j->setValue(ray_j_offset, m_ray_j->getDirection());

				for (double j = 0.0; round_to(j, m_resolution) < m_j_dim; j+=(m_resolution / 2.0))
				{
					Vec3<Type> ray_k_offset = m_ray_j->getValue(j);

					m_ray_k->setValue(ray_k_offset, m_ray_k->getDirection());

					for (double k = 0.0; round_to(k, m_resolution) < m_k_dim; k+=(m_resolution / 2.0))
					{
						Vec3<Type> temp;

						temp = m_ray_k->getValue(k);

						// discretize this voxel
						temp.x() = round_to(temp.x(), m_resolution);
						temp.y() = round_to(temp.y(), m_resolution);
						temp.z() = round_to(temp.z(), m_resolution);

						temp.x() /= m_resolution;
						temp.y() /= m_resolution;
						temp.z() /= m_resolution;

						// add it to our flat list
						m_fill_points.push_back(temp);
					}
				}
			}

			// finally, remove duplicate voxels from our flat list...
			remove_duplicates(m_fill_points);
			m_num_fill_points = (int)m_fill_points.size();
		}

		// this function rounds value to the closest modulus of specified resolution
		int round_to(Type value, Type resolution)
		{
			int low_boundary;
			int high_boundary;
			Type dist_to_low;
			Type dist_to_high;

			low_boundary = (int)((int)(value / resolution) * resolution);
			high_boundary = low_boundary + (int)((double)resolution + 0.5);

			dist_to_low = value - (Type)low_boundary;
			dist_to_high = (Type)high_boundary - value;

			if (dist_to_low < dist_to_high)
			{
				return low_boundary;
			} else {
				return high_boundary;
			}
		}

		template<typename elem_type>
		void remove_duplicates(std::vector<elem_type> &vec)
		{
			std::sort(vec.begin(), vec.end());
			vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
		}
	};

    typedef RectPrism<int>    RectPrismi;
    typedef RectPrism<float>  RectPrismf;
    typedef RectPrism<double> RectPrismd;
} // namespace geo

#endif
