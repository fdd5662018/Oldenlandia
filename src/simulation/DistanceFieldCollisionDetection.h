#ifndef _SDFCOLLISIONDETECTION_H
#define _SDFCOLLISIONDETECTION_H

#include "CollisionDetection.h"
#include "ParticleData.h"
#include "RigidBody.h"
#include "BoundingSphere.h"
#include "BoundingSphereHierarchy.h"

/* Distance field collision detection */
class DistanceFieldCollisionDetection : public CollisionDetection
{
public:
	struct DistanceFieldCollisionObject : public CollisionObject
	{
		bool m_testMesh;
		Real m_invertSDF;
		PointCloudBSH m_bvh;

		DistanceFieldCollisionObject() { m_testMesh = true; m_invertSDF = 1.0; }
		virtual ~DistanceFieldCollisionObject() {}
		virtual bool collisionTest(const Vector3r &x, const Real tolerance, Vector3r &cp, Vector3r &n, Real &dist, const Real maxDist = 0.0);
		virtual void approximateNormal(const Eigen::Vector3d &x, const Real tolerance, Vector3r &n);

		virtual double distance(const Eigen::Vector3d &x, const Real tolerance) = 0;
	};

	struct ContactData
	{
		char m_type;
		uint m_index1;
		uint m_index2;
		Vector3r m_cp1;
		Vector3r m_cp2;
		Vector3r m_normal;
		Real m_dist;
		Real m_restitution;
		Real m_friction;
	};

protected:
	void collisionDetectionRBSolid(const ParticleData &pd, const uint offset, const uint numVert, 
		DistanceFieldCollisionObject *col, RigidBody *rb2, DistanceFieldCollisionObject *co2,
		const Real restitutionCoeff, const Real frictionCoeff, std::vector<std::vector<ContactData>> &contacts_mt);

public:
	DistanceFieldCollisionDetection();
	virtual ~DistanceFieldCollisionDetection();
	virtual void collisionDetection();
	virtual bool isDistanceFieldCollisionObject(CollisionObject *) const;

	/** Add collision cylinder
	* @param bodyIndex index of corresponding body
	* @param bodtyType type of corresponding body
	* @param dim(radius height) of cylinder
	*/
	void addCollisionCylinder(const uint bodyIndex, const uint bodyType, const Vector3r *vertices, const uint numVertices, const Vector2r &dim, const bool testMesh = true, const bool invertSDF = false);

	std::vector<ContactData> m_tempContacts;
};















#endif