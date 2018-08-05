#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <vector>
#include <iostream>
// mesh contains eigen and some tydef of eigen.
#include "mesh.h" 
// the src placed folder will be added into the include place but constraint is not in the root src folder.
#include "constraints/constraint.h"

#define EPSILON 1e-6

class Simulation
{
public:
	Simulation();
	virtual ~Simulation();

	void Reset();
	void Update();

	// inline functions
	inline void SetReprefactorFlag() { m_prefactorization_flag = false; }
	inline void SetMesh(Mesh *mesh) { m_mesh = mesh; }

private:
	// variables
	// ---------------------------------------------------------------------------
	// key simulation components
	Mesh *m_mesh;
	std::vector<Constraint *> m_constraints;
	// simulation constants
	// --------------------
	// time step
	ScalarType m_h;
	ScalarType m_stiffness_stretch;
	ScalarType m_damping_coefficient;
	// external force (gravity, wind, etc...)
	VectorX m_external_force;
	// inertia term
	VectorX m_inertia_y;
	// for optimization method, number of iterations.
	uint m_iterations_per_frame;
	// for prefactorization
	// --------------------
	bool m_prefactorization_flag;
	SparseMatrix m_weighted_laplacian;
	SparseMatrix m_J_matrix;
	// A direct sparse LL^T cholesky factorizations
	Eigen::SimplicialLLT<SparseMatrix, Eigen::Upper> m_prefactored_LLT_solver;
	// main update sub-routines
	// -------------------------------------------------------------------------
	// cleanup all constraints
	void clearConstraints();
	// initialize constraints
	void setupConstraints();
	// damp velocity at the end of each iteration.
	void dampVelocity();
	// calculate the inertia term: y = current_pos + current_vel*h.
	void calculateInertiaY();
	// detect collision and return a vector of penetration(global).
	VectorX collisionDetection(const VectorX x);
	// integration method
	// ------------------
	void integrateOptimizationMethod();
	// all those "OneIteration" functions will be called in a loop
	// x is initially passed as the initial guess of the next postion (i.e. inertia term): x = y = current_pos + current_vel*h
	// x will be changed during these subroutines in EVERY iteration
	// the final value of x will be the next_pos that we used to update all vertices.
	bool integrateLocalGlobalOneIteration(VectorX &x);
	// for local global method
	// d-vector will be evaluate every iteration, fix x and evaluate d.
	void evaluateDVector(const VectorX &x, VectorX &d);
	// J matrix is only dependent on connectivity and stiffness, so it is evaluated only once.
	void evaluateJMatrix(SparseMatrix &J);
	// matrics and prefactorizations
	void setWeightedLaplacianMatrix();
	void setJMatrix();
	void prefactorize();
	// uyility functions
	// -----------------
	// current_vel = (next_pos - current_pos)/h; current_pos = next_pos;(Implicit Euler)
	void updatePosAndVel(const VectorX &new_pos);
	// factorize matrix A using LL^T decomposition
	void factorizeDirectSolverLLT(const SparseMatrix &A, Eigen::SimplicialLLT<SparseMatrix, Eigen::Upper> &lltSolver, char *warning_msg = (char *)"");
};


#endif