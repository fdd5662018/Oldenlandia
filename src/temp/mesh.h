#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include "texture.h"

// eigen
#include "Eigen/Core"
#include "Eigen/Dense"
#include "Eigen/Sparse"

using namespace std;

typedef double ScalarType;
// eigen vectors and matrices
typedef int IndexType;
typedef Eigen::Matrix<ScalarType, 3, 3, 0, 3, 3> EigenMatrix3;
typedef Eigen::Matrix<ScalarType, 3, 1, 0, 3, 1> EigenVector3;
typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, 1> VectorX;
typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef Eigen::SparseMatrix<ScalarType> SparseMatrix;
typedef Eigen::Triplet<ScalarType, IndexType> SparseMatrixTriplet;

// eigen vector accessor
#define block_vector(a) block<3,1>(3*(a), 0)

// eigen 2 glm, glm 2 eigen
glm::vec3 Eigen2GLM(const EigenVector3& eigen_vector);
EigenVector3 GLM2Eigen(const glm::vec3& glm_vector);

struct Vertex {
	// indice
	int index;
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Edge
{
	unsigned int m_v1, m_v2; // indices of endpoint vertices
	unsigned int m_tri1, m_tri2; // indices of adjacent faces
};

class Mesh {
public:
	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture2D> textures);
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture2D> textures;
    unsigned int VAO;

    // render the mesh
	void Draw(Shader shader);

private:
    /*  Render data  */
    unsigned int VBO, EBO;
	// mesh info
	unsigned int m_vertices_number; // m
	unsigned int m_system_dimension; // 3m
	// vertices positions/previous positions/mass
	VectorX m_current_positions; // 1x3m
	VectorX m_current_velocities; // 1x3m
	SparseMatrix m_mass_matrix; // 3mx3m
	SparseMatrix m_inv_mass_matrix; // 3mx3m
	SparseMatrix m_identity_matrix; // 3mx3m
	// for generating constraints.
	std::vector<Edge> m_edge_list;
	// for all
	ScalarType m_total_mass;

    /*  Functions    */
    // initializes all the buffer objects/arrays
	void setupMesh();
	void generateParticleList();
};
#endif