#include "BoundingSphereHierarchy.h"
#include "MiniBall.h"

#include <iostream>
#include <unordered_set>
#include <set>

using namespace Eigen;
using pool_set = std::set<uint>;

struct PCBSHCoordAccessor
{
	PCBSHCoordAccessor(const Vector3r *vertices_,
		std::vector<uint> const *lst_)
		: vertices(vertices_), lst(lst_)
	{
	}

	using Pit = uint;
	using Cit = Real const *;
	inline Cit operator() (Pit it) const {
		return vertices[(*lst)[it]].data();
	}
	const Vector3r *vertices;
	std::vector<uint> const *lst;
};

PointCloudBSH::PointCloudBSH()
	: super(0, 10)
{
}

Vector3r const &PointCloudBSH::entity_position(uint i) const
{
	return m_vertices[i];
}

void PointCloudBSH::compute_hull(uint b, uint n, BoundingSphere &hull) const
{
	// 3 is dimensional b and b + n is start and end index m_vertices is the whole vertices set and m_lst is the vertexs set in this minibal.
	auto mb = Miniball::Miniball<PCBSHCoordAccessor>(3, b, b + n, { m_vertices, &m_lst });

	hull.x() = Map<Vector3r const>(mb.center());
	hull.r() = std::sqrt(mb.squared_radius());
}

void PointCloudBSH::compute_hull_approx(uint b, uint n, BoundingSphere &hull) const
{
	// compute center
	Vector3r x;
	x.setZero();
	for (uint i = b; i < b + n; i++)
		x += m_vertices[m_lst[i]];
	x /= (Real)n;

	Real radius2 = 0.0;
	for (uint i = b; i < b + n; i++) {
		radius2 = max(radius2, (x - m_vertices[m_lst[i]]).squaredNorm());
	}

	hull.x() = x;
	hull.r() = sqrt(radius2);
}

void PointCloudBSH::init(const Vector3r *vertices, const uint numVertices)
{
	// the list of vertices in the sphere.
	m_lst.resize(numVertices);
	// the whole vertices.
	m_vertices = vertices;
	// the num of vertices in the sphere.
	m_numVertices = numVertices;
}

