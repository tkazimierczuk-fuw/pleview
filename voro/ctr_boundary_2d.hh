// Voro++, a cell-based Voronoi library
//
// Authors  : Chris H. Rycroft (LBL / UC Berkeley)
//            Cody Robert Dance (UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

/** \file ctr_boundary_2d.hh
 * \brief Header file for the container_boundary_2d and related classes. */

#ifndef VOROPP_CTR_BOUNDARY_2D_HH
#define VOROPP_CTR_BOUNDARY_2D_HH

#include <cstdlib>
#include <cmath>
#include <vector>
using namespace std;

#include "config.hh"
#include "common.hh"
#include "v_base_2d.hh"
#include "cell_2d.hh"
#include "c_loops_2d.hh"
#include "rad_option.hh"
#include "v_compute_2d.hh"

namespace voro {

/** \brief Class for representing a particle system in a three-dimensional
 * rectangular box.
 *
 * This class represents a system of particles in a three-dimensional
 * rectangular box. Any combination of non-periodic and periodic coordinates
 * can be used in the three coordinate directions. The class is not intended
 * for direct use, but instead forms the base of the container and
 * container_poly classes that add specialized routines for computing the
 * regular and radical Voronoi tessellations respectively. It contains routines
 * that are commonly between these two classes, such as those for drawing the
 * domain, and placing particles within the internal data structure.
 *
 * The class is derived from the wall_list class, which encapsulates routines
 * for associating walls with the container, and the voro_base class, which
 * encapsulates routines about the underlying computational grid. */
class container_boundary_2d : public voro_base_2d, public radius_mono {
	public:

		/** The minimum x coordinate of the container. */
		const double ax;
		/** The maximum x coordinate of the container. */
		const double bx;
		/** The minimum y coordinate of the container. */
		const double ay;
		/** The maximum y coordinate of the container. */
		const double by;
		/** A boolean value that determines if the x coordinate in
		 * periodic or not. */
		const bool xperiodic;
		/** A boolean value that determines if the y coordinate in
		 * periodic or not. */
		const bool yperiodic;
		/** This array holds the numerical IDs of each particle in each
		 * computational box. */
		int **id;
		/** A two dimensional array holding particle positions. For the
		 * derived container_poly class, this also holds particle
		 * radii. */
		double **p;
		/** This array holds the number of particles within each
		 * computational box of the container. */
		int *co;
		/** This array holds the maximum amount of particle memory for
		 * each computational box of the container. If the number of
		 * particles in a particular box ever approaches this limit,
		 * more is allocated using the add_particle_memory() function.
		 */
		int *mem;
		int **wid;
		int **nlab;
		int ***plab;
		int **bndpts;
		int boundary_track;
		int edbc;
		int edbm;
		int *edb;
		double *bnds;
	
		/** The amount of memory in the array structure for each
		 * particle. This is set to 2 when the basic class is
		 * initialized, so that the array holds (x,y) positions. If the
		 * 2D container class is initialized as part of the derived class
		 * container_poly_2d, then this is set to 3, to also hold the
		 * particle radii. */
		const int ps;
		container_boundary_2d(double ax_,double bx_,double ay_,double by_,
			     int nx_,int ny_,bool xperiodic_,bool yperiodic_,int init_mem);
		~container_boundary_2d();
		/** Initializes the Voronoi cell prior to a compute_cell
		 * operation for a specific particle being carried out by a
		 * voro_compute class. The cell is initialized to fill the
		 * entire container. For non-periodic coordinates, this is set
		 * by the position of the walls. For periodic coordinates, the
		 * space is equally divided in either direction from the
		 * particle's initial position. Plane cuts made by any walls
		 * that have been added are then applied to the cell.
		 * \param[in,out] c a reference to a voronoicell_nonconvex_2d object.
		 * \param[in] ij the block that the particle is within.
		 * \param[in] q the index of the particle within its block.
		 * \param[in] (ci,cj) the coordinates of the block in the
		 * 		      container coordinate system.
		 * \param[out] (i,j) the coordinates of the test block relative
		 *		     to the voro_compute coordinate system.
		 * \param[out] (x,y) the position of the particle.
		 * \param[out] disp a block displacement used internally by the
		 *		    compute_cell routine.
		 * \return False if the plane cuts applied by walls completely
		 * removed the cell, true otherwise. */
		template<class v_cell_2d>
		inline bool initialize_voronoicell(v_cell_2d &c,int ij,int q,int ci,int cj,
				int &i,int &j,double &x,double &y,int &disp) {
			double x1,x2,y1,y2,*pp=p[ij]+ps*q;
			x=*(pp++);y=*(pp++);
			if(xperiodic) {x1=-(x2=0.5*(bx-ax));i=nx;} else {x1=ax-x;x2=bx-x;i=ci;}
			if(yperiodic) {y1=-(y2=0.5*(by-ay));j=ny;} else {y1=ay-y;y2=by-y;j=cj;}
			if(bndpts[ij][q]==-1) c.init(x1,x2,y1,y2);
			else {
				int &bid=bndpts[ij][q];
				double cx=bnds[2*bid],cy=bnds[2*bid+1];
				int nwid=edb[2*bid],lwid=edb[2*bid+1];
				double lx=bnds[2*lwid],ly=bnds[2*lwid+1];
				double nx=bnds[2*nwid],ny=bnds[2*nwid+1];
				c.init_nonconvex(x1,x2,y1,y2,nx-cx,ny-cy,lx-cx,ly-cy);
			}
			disp=ij-i-nx*j;
			return true;
		}
		bool point_inside(double x,double y);
		template<class v_cell_2d>
		bool boundary_cuts(v_cell_2d &c,int ij,double x,double y);
		/** Initializes parameters for a find_voronoi_cell call within
		 * the voro_compute template.
		 * \param[in] (ci,cj) the coordinates of the test block in
		 * 		      the container coordinate system.
		 * \param[in] ij the index of the test block
		 * \param[out] (i,j) the coordinates of the test block relative
		 * 		     to the voro_compute coordinate system.
		 * \param[out] disp a block displacement used internally by the
		 *		    find_voronoi_cell routine. */
		inline void initialize_search(int ci,int cj,int ij,int &i,int &j,int &disp) {
			i=xperiodic?nx:ci;
			j=yperiodic?ny:cj;
			disp=ij-i-nx*j;
		}
		/** Returns the position of a particle currently being computed
		 * relative to the computational block that it is within. It is
		 * used to select the optimal worklist entry to use.
		 * \param[in] (x,y) the position of the particle.
		 * \param[in] (ci,cj) the block that the particle is within.
		 * \param[out] (fx,fy) the position relative to the block.
		 */
		inline void frac_pos(double x,double y,double ci,double cj,
				double &fx,double &fy) {
			fx=x-ax-boxx*ci;
			fy=y-ay-boxy*cj;
		}
		/** Calculates the index of block in the container structure
		 * corresponding to given coordinates.
		 * \param[in] (ci,cj) the coordinates of the original block in
		 *		      the current computation, relative to the
		 *		      container coordinate system.
		 * \param[in] (ei,ej) the displacement of the current block
		 *		      from the original block.
		 * \param[in,out] (qx,qy) the periodic displacement that must
		 *			  be added to the particles within the
		 *			  computed block.
		 * \param[in] disp a block displacement used internally by the
		 * 		   find_voronoi_cell and compute_cell routines.
		 * \return The block index. */
		inline int region_index(int ci,int cj,int ei,int ej,double &qx,double &qy,int &disp) {
			if(xperiodic) {if(ci+ei<nx) {ei+=nx;qx=-(bx-ax);} else if(ci+ei>=(nx<<1)) {ei-=nx;qx=bx-ax;} else qx=0;}
			if(yperiodic) {if(cj+ej<ny) {ej+=ny;qy=-(by-ay);} else if(cj+ej>=(ny<<1)) {ej-=ny;qy=by-ay;} else qy=0;}
			return disp+ei+nx*ej;
		}
		/** Sums up the total number of stored particles.
		 * \return The number of particles. */
		inline int total_particles() {
			int tp=*co;
			for(int *cop=co+1;cop<co+nxy;cop++) tp+=*cop;
			return tp;
		}
		inline void start_boundary() {boundary_track=edbc;}
		void end_boundary();
		void register_boundary(double x,double y);
		void clear();
		void put(int n,double x,double y);
		void put(particle_order &vo,int n,double x,double y);


		void compute_all_cells();
		double sum_cell_areas();

		//bool find_voronoi_cell(double x,double y,double &rx,double &ry,int &pid);
		/** Computes the Voronoi cell for a particle currently being
		 * referenced by a loop class.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] vl the loop class to use.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell_2d,class c_loop_2d>
		inline bool compute_cell(v_cell_2d &c,c_loop_2d &vl) {
			return vc.compute_cell(c,vl.ij,vl.q,vl.i,vl.j);
		}
		/** Computes the Voronoi cell for given particle.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] ij the block that the particle is within.
		 * \param[in] q the index of the particle within the block.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell_2d>
		inline bool compute_cell(v_cell_2d &c,int ij,int q) {
			int j=ij/nx,i=ij-j*nx;
			return vc.compute_cell(c,ij,q,i,j);
		}
		void setup();
		bool skip(int ij,int l,double x,double y);
	private:
		int *soi;
		int *tmp;
		int *tmpp;
		int *tmpe;
		void create_label_table();
		void add_particle_memory(int i);
		inline bool put_locate_block(int &ij,double &x,double &y);
		inline bool put_remap(int &ij,double &x,double &y);
		inline bool remap(int &ai,int &aj,int &ci,int &cj,double &x,double &y,int &ij);
		void add_temporary_label_memory();
		void add_boundary_memory();
		void tag_line(int &ij,int ije,int wid);
		inline void tag(int ij,int wid);
		void tag_walls(double x1,double y1,double x2,double y2,int wid);
		inline bool cross_product(double x1,double y1,double x2,double y2) {
			return x1*y2>x2*y1;
		}
		void semi_circle_labeling(double x1,double y1,double x2,double y2,int bid);
		voro_compute_2d<container_boundary_2d> vc;
		friend class voro_compute_2d<container_boundary_2d>;
};

}

#endif
