#ifndef __BOUNDINGSPHEREHIERARCHY_H__
#define __BOUNDINGSPHEREHIERARCHY_H__

#include "CollisionDetection.h"
#include "BoundingSphere.h"
#include "kdTree.h"

class PointCloudBSH : public KDTree<BoundingSphere>
{
public:

	using super = KDTree<BoundingSphere>;
	PointCloudBSH();

	void init(const Vector3r *vertices, const uint numVertices);
	Vector3r const &entity_position(uint i) const final;
	void compute_hull(uint b, uint n, BoundingSphere &hull) const final;
	void compute_hull_approx(uint b, uint n, BoundingSphere &hull) const final;

private:
	const Vector3r *m_vertices;
	uint m_numVertices;
};


#endif