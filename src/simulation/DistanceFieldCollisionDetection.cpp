#include "DistanceFieldCollisionDetection.h"
#undef max
#include <iostream>

DistanceFieldCollisionDetection::DistanceFieldCollisionDetection() :
	CollisionDetection()
{

}

DistanceFieldCollisionDetection::~DistanceFieldCollisionDetection()
{

}

bool DistanceFieldCollisionDetection::DistanceFieldCollisionObject::collisionTest(const Vector3r &x, const Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, const Real maxDist)
{
	return true;
}

void DistanceFieldCollisionDetection::DistanceFieldCollisionObject::approximateNormal(const Eigen::Vector3d &x, const Real tolerance, Vector3r &n)
{
}

void DistanceFieldCollisionDetection::collisionDetectionRBSolid(const ParticleData &pd, const uint offset, const uint numVert,
	DistanceFieldCollisionObject *col, RigidBody *rb2, DistanceFieldCollisionObject *co2,
	const Real restitutionCoeff, const Real frictionCoeff, std::vector<std::vector<ContactData>> &contacts_mt)
{
	const Vector3r &com2 = rb2->getPosition();

	// remove the rotation of the main axis transformation that is performed to get a diagonal inertia tensor since the distance function is evaluated in local coordinates
	// transformation world to local:
	// p_local = R_inertia^T (R_MAT R^T (p_world - x) - x_initial + x_MAT)
	// transformation local to world:
	// p_world = R R_MAT^T (R_inertia p_local + x_initial - x_MAT) + x
	const Matrix3r &R = rb2->getTransformationR();
	const Vector3r &v1 = rb2->getTransformationV1();
	const Vector3r &v2 = rb2->getTransformationV2();

	const PointCloudBSH &bvh = col->m_bvh;
	// predicate check if intersect
	std::function<bool(uint, uint)> predicate = [&](uint node_index, uint depth)
	{
		// hull is the bounding spheres
		const BoundingSphere &bs = bvh.hull(node_index);
		// center
		const Vector3r &sphere_x_w = bs.x();
		
		AlignedBox3r box3f;
		// m_p vector 3r
		box3f.extend(co2->m_aabb.m_p[0]);
		box3f.extend(co2->m_aabb.m_p[1]);
		const Real dist = box3f.exteriorDistance(sphere_x_w);

		// Test if center of bounding sphere intersects AABB
		if (dist < bs.r())
		{
			//Test if distance of center of bounding sphere to collision object is smaller than the radius.
			const Vector3r x = R * (sphere_x_w - com2) + v1;
			const double dist2 = co2->distance(x.template cast<double>(), m_tolerance);
			//the largest possible value for type
			if (dist2 == std::numeric_limits<double>::max())
				return true;
			if (dist2 < bs.r())
				return true;
		}
		return false;
	};

	std::function<void(uint, uint)> cb = [&](uint node_index, uint depth)
	{
		auto const &node = bvh.node(node_index);
		if (!node.is_leaf())
			return;

		for (auto i = node.begin; i < node.begin + node.n; ++i)
		{
			uint index = bvh.entity(i) + offset;
			const Vector3r &x_w = pd.getPosition(index);
			const Vector3r x = R * (x_w - com2) + v1;
			Vector3r cp, n;
			Real dist;
			if (co2->collisionTest(x, m_tolerance, cp, n, dist))
			{
				const Vector3r cp_w = R.transpose() * cp + v2;
				const Vector3r n_w = R.transpose() * n;
				
				std::cout << "collision detected in cb" << endl;
			}
		}
	};
//	bvh.traverse_depth_first(predicate, cb);

}

void DistanceFieldCollisionDetection::collisionDetection()
{

}

bool DistanceFieldCollisionDetection::isDistanceFieldCollisionObject(CollisionObject *) const
{
	return true;
}

/** Add collision cylinder
* @param bodyIndex index of corresponding body
* @param bodtyType type of corresponding body
* @param dim(radius height) of cylinder
*/
void DistanceFieldCollisionDetection::addCollisionCylinder(const uint bodyIndex, const uint bodyType, const Vector3r *vertices, const uint numVertices, const Vector2r &dim, const bool testMesh, const bool invertSDF)
{

}
