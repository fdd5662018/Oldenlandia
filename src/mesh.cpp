#include "mesh.h"

// constructor
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture2D> textures)
{
	//cout << "indicesSize: " << indices.size() << endl;
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	setupMesh();
	glBindVertexArray(0);
}

// render the mesh
void Mesh::Draw(Shader shader)
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		// active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		// retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream

		// now set the sampler to the correct texture unit
		//cout << (name + number) << endl;
		glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);
	}
	//cout << endl;
	// draw mesh
	glBindVertexArray(VAO);
	setupMesh();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
}

// initialize every particle pos / vel / mass / color.
void Mesh::generateParticleList()
{
	m_vertices_number = vertices.size();
	m_system_dimension = m_vertices_number * 3;

	ScalarType unit_mass = m_total_mass / m_vertices_number;

	m_current_positions.resize(m_system_dimension);
	m_current_velocities.resize(m_system_dimension);
	m_mass_matrix.resize(m_system_dimension, m_system_dimension);
	m_inv_mass_matrix.resize(m_system_dimension, m_system_dimension);
	m_identity_matrix.resize(m_system_dimension, m_system_dimension);

	// Assign initial position to all the vertices.
	m_current_positions.setZero();
	for (int i = 0; i < m_vertices_number; i++) {
		m_current_positions.block_vector(i) = EigenVector3(vertices[i].Position[0], vertices[i].Position[1], vertices[i].Position[2]);
	}

	// Assign initial velocity to zero
	m_current_velocities.setZero();

	// Assign mass matrix and an equally sized identity matrix
	std::vector<SparseMatrixTriplet> i_triplets;
	std::vector<SparseMatrixTriplet> m_triplets;
	std::vector<SparseMatrixTriplet> m_inv_triplets;
	i_triplets.clear();
	m_triplets.clear();
	ScalarType inv_unit_mass = 1.0 / unit_mass;
	for (unsigned int index = 0; index < m_system_dimension; index++)
	{
		i_triplets.push_back(SparseMatrixTriplet(index, index, 1));
		m_triplets.push_back(SparseMatrixTriplet(index, index, unit_mass));
		m_inv_triplets.push_back(SparseMatrixTriplet(index, index, inv_unit_mass));
	}
	m_mass_matrix.setFromTriplets(m_triplets.begin(), m_triplets.end());
	m_inv_mass_matrix.setFromTriplets(m_inv_triplets.begin(), m_inv_triplets.end());
	m_identity_matrix.setFromTriplets(i_triplets.begin(), i_triplets.end());
}

// generate edge list from the geometry representation.
void Mesh::generateEdgeList()
{
	// generate all the edges from the vertices and triangle list.
	// courtesy of Eric Lengyel, "Building an Edge List for an Arbitrary Mesh". Terathon Software 3D Graphics Library.
	// http://www.terathon.com/code/edges.html
	unsigned int vert_num = m_vertices_number;
	unsigned int tri_num = indices.size() / 3;

	unsigned int *first_edge = new unsigned int[vert_num + 3 * tri_num];
	unsigned int *next_edge = first_edge + vert_num;

	for (unsigned int i = 0; i < vert_num; ++i)
		first_edge[i] = 0xFFFFFFFF;
	// First pass over all triangles. Finds out all the edges satisfying the condition that
	// the first vertex index is less than the second vertex index when the direction from 
	// the first to the second represents a counterclockwise winding around the triangle to
	// which the edge belongs. For each edge found, the edge index is stored in a linked 
	// list of edges belonging to the lower-numbered vertex index i. This allows us to 
	// quickly find an edge in the second pass whose higher-numbered vertex is i.

	unsigned int edge_count = 0;
	const unsigned int* triangle = &indices[0];
	unsigned int i1, i2;
	for (unsigned int t = 0; t < tri_num; ++t)
	{
		i1 = triangle[2];
		for (unsigned int n = 0; n < 3; ++n)
		{
			i2 = triangle[n];
			if (i1 < i2)
			{
				Edge new_edge;
				new_edge.m_v1 = i1;
				new_edge.m_v2 = i2;
				new_edge.m_tri1 = t;
				new_edge.m_tri2 = t;
				m_edge_list.push_back(new_edge);

				unsigned int edge_idx = first_edge[i1];
				if (edge_idx == 0xFFFFFFFF)
				{
					first_edge[i1] = edge_count;
				}
				else
				{
					while (true)
					{
						unsigned int idx = next_edge[edge_idx];
						if (idx == 0xFFFFFFFF)
						{
							next_edge[edge_idx] = edge_count;
							break;
						}
						edge_idx = idx;
					}
				}

				next_edge[edge_count] = 0xFFFFFFFF;
				edge_count++;
			}
			i1 = i2;
		}
		triangle += 3;
	}

	// Second pass over all triangles. Finds out all the edges satisfying the condition that
	// the first vertex index is greater than the second vertex index when the direction from 
	// the first to the second represents a counterclockwise winding around the triangle to
	// which the edge belongs. For each of these edges, the same edge should have already been
	// found in the first pass for a different triangle. So we search the list of edges for the
	// higher-numbered index for the matching edge and fill in the second triangle index. The 
	// maximum number of the comparisons in this search for any vertex is the number of edges
	// having that vertex as an endpoint.
	triangle = &indices[0];
	for (unsigned int t = 0; t < tri_num; ++t)
	{
		i1 = triangle[2];
		for (unsigned int n = 0; n < 3; ++n)
		{
			i2 = triangle[n];
			if (i1 > i2)
			{
				bool is_new_edge = true;
				for (unsigned int edge_idx = first_edge[i2]; edge_idx != 0xFFFFFFFF; edge_idx = next_edge[edge_idx])
				{
					Edge *edge = &m_edge_list[edge_idx];
					if ((edge->m_v2 == i1) && (edge->m_tri1 == edge->m_tri2))
					{
						edge->m_tri2 = t;
						is_new_edge = false;
						break;
					}
				}
				// for case where a edge belongs to only one triangle. i.e. mesh is not watertight.
				if (is_new_edge)
				{
					Edge new_edge;
					new_edge.m_v1 = i1;
					new_edge.m_v2 = i2;
					new_edge.m_tri1 = t;
					new_edge.m_tri2 = t;
					m_edge_list.push_back(new_edge);

					unsigned int edge_idx = first_edge[i1];
					if (edge_idx == 0xFFFFFFFF)
					{
						first_edge[i1] = edge_count;
					}
					else
					{
						while (true)
						{
							unsigned int idx = next_edge[edge_idx];
							if (idx == 0xFFFFFFFF)
							{
								next_edge[edge_idx] = edge_count;
								break;
							}
							edge_idx = idx;
						}
					}

					next_edge[edge_count] = 0xFFFFFFFF;
					edge_count++;
				}
			}
			i1 = i2;
		}
		triangle += 3;
	}

	delete[] first_edge;
	//printf("Edge number: %u.\n", m_edge_list.size());
}