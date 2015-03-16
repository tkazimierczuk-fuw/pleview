/** \file cell_2d.hh
 * \brief Header file for the voronoicell_2d class. */

#ifndef VOROPP_CELL_2D_HH
#define VOROPP_CELL_2D_HH

#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;

#include "common.hh"
#include "config.hh"

namespace voro {

/** \brief A class encapsulating all the routines for storing and calculating a
 * single Voronoi cell. */
class voronoicell_base_2d {
	public:
		/** This holds the current size of the ed and pts arrays. If
		 * more vertices are created than can fit in these arrays, then
		 * they are dynamically extended using the add_memory_vertices
		 * routine. */
		int current_vertices;
		/** This sets the size of the current delete stack. */
		int current_delete_size;
		/** The total nuber of vertices in the current cell. */
		int p;
		/** An array with size 2*current_vertices holding information
		 * about edge connections between vertices.*/
		int *ed;
		/** An array with size 2*current_vertices for holding
		 * the positions of the vertices. */
		double *pts;
		voronoicell_base_2d();
		~voronoicell_base_2d();
		void init_base(double xmin,double xmax,double ymin,double ymax);

		template<class vc_class>
		bool nplane(vc_class &vc,double x,double y,double rs,int p_id);
		template<class vc_class>
		bool nplane_cut(vc_class &vc,double x,double y,double rsq,int p_id,double u,int up);
		bool plane_intersects(double x,double y,double rs);
		inline bool plane_intersects_guess(double x,double y,double rs) {
			return plane_intersects(x,y,rs);
		}
		double max_radius_squared();
		double perimeter();
		double area();
		void vertices(vector<double> &v);
        void ordered_vertices(double x,double y,vector<double> &v);
		void vertices(double x,double y,vector<double> &v);
		void edge_lengths(vector<double> &vd);
		void normals(vector<double> &vd);	
		void centroid(double &cx,double &cy);
		virtual void neighbors(vector<int> &v) {v.clear();}
	protected:
		/** Computes the distance of a Voronoi cell vertex to a plane.
		 * \param[in] (x,y) the normal vector to the plane.
		 * \param[in] rsq the distance along this vector of the plane.
		 * \param[in] qp the index of the vertex to consider. */
		inline double pos(double x,double y,double rsq,int qp) {
			return x*pts[2*qp]+y*pts[2*qp+1]-rsq;
		}
	private:
		template<class vc_class>
		void add_memory_vertices(vc_class &vc);
		void add_memory_ds(int *&stackp);
		/** The delete stack, used to store the vertices that are
		 * deleted during the plane cutting procedure. */
		int *ds;
		/** A pointer to the end of the delete stack, used to detect
		 * when it is full. */
		int *stacke;
};

class voronoicell_2d : public voronoicell_base_2d {
	public:
		using voronoicell_base_2d::nplane;
		inline bool nplane(double x,double y,double rs,int p_id) {
			return nplane(*this,x,y,rs,0);
		}
		inline bool nplane(double x,double y,int p_id) {
			double rs=x*x+y*y;
			return nplane(*this,x,y,rs,0);
		}
		inline bool plane(double x,double y,double rs) {
			return nplane(*this,x,y,rs,0);
		}
		inline bool plane(double x,double y) {
			double rs=x*x+y*y;
			return nplane(*this,x,y,rs,0);
		}
		inline void init(double xmin,double xmax,double ymin,double ymax) {
			init_base(xmin,xmax,ymin,ymax);
		}
	private:
		inline void n_add_memory_vertices() {}
		inline void n_copy(int a,int b) {}
		inline void n_set(int a,int id) {}
		friend class voronoicell_base_2d;
};

class voronoicell_neighbor_2d : public voronoicell_base_2d {
	public:
		using voronoicell_base_2d::nplane;
		int *ne;
		voronoicell_neighbor_2d() : ne(new int[init_vertices]) {}
		~voronoicell_neighbor_2d() {delete [] ne;}
		inline bool nplane(double x,double y,double rs,int p_id) {
			return nplane(*this,x,y,rs,p_id);
		}
		inline bool nplane(double x,double y,int p_id) {
			double rs=x*x+y*y;
			return nplane(*this,x,y,rs,p_id);
		}
		inline bool plane(double x,double y,double rs) {
			return nplane(*this,x,y,rs,0);
		}
		inline bool plane(double x,double y) {
			double rs=x*x+y*y;
			return nplane(*this,x,y,rs,0);
		}
		void init(double xmin,double xmax,double ymin,double ymax);
		virtual void neighbors(vector<int> &v);
	private:
		inline void n_add_memory_vertices();
		inline void n_copy(int a,int b) {ne[a]=ne[b];}
		inline void n_set(int a,int id) {ne[a]=id;}
		friend class voronoicell_base_2d;
};

}
#endif
