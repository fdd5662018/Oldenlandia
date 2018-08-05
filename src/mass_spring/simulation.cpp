#include "simulation.h"

Simulation::Simulation()
{
}

Simulation::~Simulation()
{
	clearConstraints();
}

void Simulation::Reset()
{
}

void Simulation::Update()
{
}

// main update sub-routines
// -------------------------------------------------------------------------
// cleanup all constraints
void Simulation::clearConstraints()
{
	for (uint i = 0; i < m_constraints.size(); i++)
	{
		delete m_constraints[i];
	}
	m_constraints.clear();

}

// initialize constraints
void Simulation::setupConstraints()
{
	clearConstraints();
	// generate stretch constraints, assign a stretch constraint for each edge.
	EigenVector3 p1, p2;
	for (std::vector<Edge>::iterator e = m_mesh->m_edge_list.begin(); e != m_mesh->m_edge_list.end(); ++e) {
		p1 = m_mesh->m_current_positions.block_vector(e->m_v1);
		p2 = m_mesh->m_current_positions.block_vector(e->m_v2);
		SpringConstraint *c = new SpringConstraint(&m_stiffness_stretch, e->m_v1, e->m_v2, (p1-p2).norm());
		m_constraints.push_back(c);
	}
}

// damp velocity at the end of each iteration.
// rigid body mode damping (objective damp m_mesh->m_current_velocities according to angular momentum)
void Simulation::dampVelocity()
{
	if (std::abs(m_damping_coefficient) < EPSILON)
		return;
	// post-processing damping.
	// --------------------------------------------------------------------------
	// average center pos and velocity.
	// --------------------------------
	EigenVector3 pos_mc(0.0, 0.0, 0.0), vel_mc(0.0, 0.0, 0.0);
	ScalarType denominator(0.0), mass(0.0);
	// vertexs size.
	uint size = m_mesh->m_vertices_number;
	for (uint i = 0; i < size; i++) {
		mass = m_mesh->m_mass_matrix.coeff(i*3, i*3);
		pos_mc += mass * m_mesh->m_current_positions.block_vector(i);
		vel_mc += mass * m_mesh->m_current_velocities.block_vector(i);
		denominator += mass;
	}
	assert(denominator != 0);
	// claculate the object center pos and velecity seeing the whole object as a particle.
	pos_mc /= denominator;
	vel_mc /= denominator;
	// calculate angular velocity.
	// --------------------------
	// L(t).
	EigenVector3 angular_momentum(0.0, 0.0, 0.0);
	// relative position r = p - x.
	EigenVector3 r(0.0, 0.0, 0.0);
	// r^*.
	EigenVector3 r_mat; r_mat.setZero();
	// I(t).
	EigenMatrix3 inertia; inertia.setZero();
	// compute angulaer momnetum L and inertia I anglular velecity w = I^-1*L.
	for (uint i = 0; i < size; i++)
	{
		// the three elements in 0 0, 1 1, 2 2 is the same for the mass of vertex 0.
		mass = m_mesh->m_mass_matrix.coeff(i * 3, i * 3);
		r = m_mesh->m_current_positions.block_vector(i) - pos_mc;
		angular_momentum += r.cross(mass * m_mesh->m_current_velocities.block_vector(i));
		//r_mat = {0.0,  r.z, -r.y,
		//        -r.z, 0.0,  r.x,
		//        r.y, -r.x, 0.0};
		r_mat.coeffRef(0, 1) = r[2];
		r_mat.coeffRef(0, 2) = -r[1];
		r_mat.coeffRef(1, 0) = -r[2];
		r_mat.coeffRef(1, 2) = r[0];
		r_mat.coeffRef(2, 0) = r[1];
		r_mat.coeffRef(2, 1) = -r[0];
		// I += r^* (r^*)^T m
		inertia += r_mat * r_mat.transpose() * mass;
	}
	// w = I^-1 L
	EigenVector3 angular_vel = inertia.inverse() * angular_momentum;
	// calulate the current velocity and damp it.
	// -----------------------------------------
	//v = v + damp*deltV, deltV = v_mc + w cross r - v_precious
	EigenVector3 delta_v(0.0, 0.0, 0.0);
	for (uint i = 0; i < size; ++i)
	{
		r = m_mesh->m_current_positions.block_vector(i) - pos_mc;
		delta_v = vel_mc + angular_vel.cross(r) - m_mesh->m_current_velocities.block_vector(i);
		m_mesh->m_current_velocities.block_vector(i) += m_damping_coefficient * delta_v;
	}
}

// calculate the inertia term: y = current_pos + current_vel*h.(explicit Euler prediction)
void Simulation::calculateInertiaY()
{
	m_inertia_y = m_mesh->m_current_positions + m_mesh->m_current_velocities * m_h;
}

// detect collision and return a vector of penetration(global).
VectorX Simulation::collisionDetection(const VectorX x)
{
	// Naive implementation of collision detection.
	VectorX penetration(m_mesh->m_system_dimension); 
	penetration.setZero();
	EigenVector3 normal;
	ScalarType dist;

	/* //Un DO
	for (uint i = 0; i < m_mesh->m_vertices_number; ++i) {
		EigenVector3 xi = x.block_vector(i);
		if (intersectionTest(xi, normal, dist)) {
			penetration.block_vector(i) += dist * normal;
		}
	}
	*/
	return penetration;
}

// integration method
// ------------------
// one frame/render update
void Simulation::integrateOptimizationMethod()
{
	// take a initial guess
	VectorX pos_next = m_inertia_y;
	// loop until converge or exceeds maximum iterations.
	bool converge = false;
	for (uint iteration_num = 0; !converge && iteration_num < m_iterations_per_frame; ++iteration_num)
	{
		converge = integrateLocalGlobalOneIteration(pos_next);
	}
	// update q_{n+1}
	updatePosAndVel(pos_next);
}

// all those "OneIteration" functions will be called in a loop
// x is initially passed as the initial guess of the next postion (i.e. inertia term): x = y = current_pos + current_vel*h
// x will be changed during these subroutines in EVERY iteration
// the final value of x will be the next_pos that we used to update all vertices.
bool Simulation::integrateLocalGlobalOneIteration(VectorX &x)
{
	// only do once all the loop.
	prefactorize();
	// local step fix x.
	VectorX d;
	evaluateDVector(x, d);
	// global step fix d
	// ------------------
	// b = My + h^2(J*d+f)---x independent(Ax = b)
	VectorX b = m_mesh->m_mass_matrix * m_inertia_y + m_h * m_h * (m_J_matrix * d + m_external_force);
	// return the solution x of Ax = b using the current decomposition of A.
	x = m_prefactored_LLT_solver.solve(b);
	// no converse condition.
	return false;
}

// for local global method
// ---------------------------------------------------------------
// d-vector will be evaluate every iteration, fix x and evaluate d.(the direction of spring)
void Simulation::evaluateDVector(const VectorX &x, VectorX &d)
{
	// 3s
	d.resize(m_constraints.size() * 3);
	d.setZero();

	for (uint index = 0; index < m_constraints.size(); ++index)
	{
		m_constraints[index]->EvaluateDVector(index, x, d);
	}
}

// J matrix is only dependent on connectivity and stiffness, so it is evaluated only once.
void Simulation::evaluateJMatrix(SparseMatrix &J)
{
	// 3m * 3s
	J.resize(m_mesh->m_system_dimension, m_constraints.size() * 3);
	std::vector<SparseMatrixTriplet> J_triplets;
	J_triplets.clear();

	for (uint index = 0; index < m_constraints.size(); ++index)
	{
		m_constraints[index]->EvaluateJMatrix(index, J_triplets);
	}
	J.setFromTriplets(J_triplets.begin(), J_triplets.end());
}

// matrics and prefactorizations
void Simulation::setWeightedLaplacianMatrix()
{
	// 3m*3m
	m_weighted_laplacian.resize(m_mesh->m_system_dimension, m_mesh->m_system_dimension);
	// A small structure to hold a non zero as a triplet(i, j, value).
	std::vector<SparseMatrixTriplet> l_triplets;
	l_triplets.clear();
	// traverse all constraints
	for (std::vector<Constraint *>::iterator it = m_constraints.begin(); it != m_constraints.end(); ++it) {
		(*it)->EvaluateWeightedLaplacian(l_triplets);
	}
	// the ith rows and jth columes' value is the value of triplet. 
	m_weighted_laplacian.setFromTriplets(l_triplets.begin(), l_triplets.end());
}

void Simulation::setJMatrix()
{
	evaluateJMatrix(m_J_matrix);
}

void Simulation::prefactorize()
{
	if (m_prefactorization_flag == false)
	{
		SparseMatrix A;
		ScalarType h2 = m_h * m_h;
		// J L and A can be prefactored and A = M + h^2L
		setWeightedLaplacianMatrix();
		setJMatrix();
		A = m_mesh->m_mass_matrix + h2 * m_weighted_laplacian;
		// A direct sparse LLT Cholesky factorizations
		factorizeDirectSolverLLT(A, m_prefactored_LLT_solver);
		m_prefactorization_flag = true;
	}
}

// uyility functions
// -----------------
// current_vel = (next_pos - current_pos)/h; current_pos = next_pos;(Implicit Euler)
void Simulation::updatePosAndVel(const VectorX &new_pos)
{
	m_mesh->m_current_velocities = (new_pos - m_mesh->m_current_positions)/m_h;
	m_mesh->m_current_positions = new_pos;
}

// factorize matrix A using LL^T decomposition
void Simulation::factorizeDirectSolverLLT(const SparseMatrix &A, Eigen::SimplicialLLT<SparseMatrix, Eigen::Upper> &lltSolver, char *warning_msg)
{
	SparseMatrix A_prime = A;
	lltSolver.analyzePattern(A_prime);
	lltSolver.factorize(A_prime);
	ScalarType Regularization = 0.00001;
	bool success = true;
	while (lltSolver.info() != Eigen::Success)
	{
		Regularization *= 10;
		A_prime = A_prime + Regularization * m_mesh->m_identity_matrix;
		lltSolver.factorize(A_prime);
		success = false;
	}
	if (!success)
		std::cout << "Warning: " << warning_msg << "adding " << Regularization << "identities.(llt solver)" << std::endl;
}