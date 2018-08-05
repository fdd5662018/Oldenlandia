#ifndef _COLLISIONDETECTION_H
#define _COLLISIONDETECTION_H

#include "mesh.h"

#define FORCE_INLINE __forceinline

typedef double Real;
using Vector2r = Eigen::Matrix<Real, 2, 1>;
using Vector3r = Eigen::Matrix<Real, 3, 1>;
using Vector4r = Eigen::Matrix<Real, 4, 1>;
using Vector5r = Eigen::Matrix<Real, 5, 1>;
using Vector6r = Eigen::Matrix<Real, 6, 1>;
using Matrix2r = Eigen::Matrix<Real, 2, 2>;
using Matrix3r = Eigen::Matrix<Real, 3, 3>;
using Matrix4r = Eigen::Matrix<Real, 4, 4>;

using Quaternionr = Eigen::Quaternion<Real>;

class AABB
{
public:
	Vector3r m_p[2];

	AABB &operator = (const AABB &aabb)
	{
		m_p[0] = aabb.m_p[0];
		m_p[1] = aabb.m_p[1];
		return *this;
	}

	static bool pointInAABB(const AABB &a, const Vector3r &p)
	{
		//lower bound
		if ((p[0] < a.m_p[0][0]) || (p[1] < a.m_p[0][1]) || (p[2] < a.m_p[0][2]))
			return false;
		if ((p[0] > a.m_p[1][0]) || (p[1] > a.m_p[1][1]) || (p[2] > a.m_p[1][2]))
			return false;
		return true;
	}

	static void getEdge(const AABB &a, char i, Vector3r &p1, Vector3r &p2)
	{
		char c1, c2;
		// get endpoint index
		getEdgeIndex(i, c1, c2);
		// get endpoint position
		cornerPoint(a, c1, p1);
		cornerPoint(a, c2, p2);
	}

	// get edge's endpoints index
	static void getEdgeIndex(char i, char &p1, char &p2)
	{
		//                           0    1    2    3    4    5    6    7    8    9    10   11
		static char index[12 * 2] = {0,1, 0,2, 1,3, 2,3, 0,4, 1,5, 2,6, 3,7, 4,5, 4,6, 5,7, 6,7};
		p1 = index[2 * i + 0];
		p2 = index[2 * i + 1];
	}

	// get endpoint position
	static void cornerPoint(const AABB &a, char i, Vector3r &p)
	{
		switch (i) {
		case 0:
			p = Vector3r(a.m_p[0][0], a.m_p[0][1], a.m_p[0][2]);
			break;
		case 1:
			p = Vector3r(a.m_p[1][0], a.m_p[0][1], a.m_p[0][2]);
			break;
		case 2:
			p = Vector3r(a.m_p[0][0], a.m_p[1][1], a.m_p[0][2]);
			break;
		case 3:
			p = Vector3r(a.m_p[1][0], a.m_p[1][1], a.m_p[0][2]);
			break;
		case 4:
			p = Vector3r(a.m_p[0][0], a.m_p[0][1], a.m_p[1][2]);
			break;
		case 5:
			p = Vector3r(a.m_p[1][0], a.m_p[0][1], a.m_p[1][2]);
			break;
		case 6:
			p = Vector3r(a.m_p[0][0], a.m_p[1][1], a.m_p[1][2]);
			break;
		case 7:
			p = Vector3r(a.m_p[1][0], a.m_p[1][1], a.m_p[1][2]);
			break;
		}
	}

	static FORCE_INLINE bool intersection(const AABB& a1, const AABB& a2)
	{
		// get the three dimension max and min and compare.
		for (char i = 0; i < 3; i++) {
			const Real min0 = a1.m_p[0][i];
			const Real max0 = a1.m_p[1][i];
			
			const Real min1 = a2.m_p[0][i];
			const Real max1 = a2.m_p[1][i];

			if ((min0 < min1) || (max0 > max1))
				return false;
		}
		return true;
	}
};

class CollisionDetection
{
public:
	static const uint RigidBodyContactType = 0;
	static const uint ParticleContactType = 1;
	static const uint ParticleRigidBodyContactType = 2;

	typedef void(*ContactCallbackFunction)(const uint contactType, const uint bodyIndex1, const uint bodyIndex2,
		const Vector3r &cp1, const Vector3r &cp2,
		const Vector3r &normal, const Real dist,
		const Real restitutionCoeff, const Real frictionCoeff, void *userData);

	struct CollisionObject
	{
		static const uint RigidBodyCollisionObjectType = 0;
		static const uint TrangleModeCollisonObjectType = 1;

		AABB m_aabb;
		uint m_bodyIndex;
		uint m_bodyType;

		virtual ~CollisionObject() {}
		virtual int &getTypeId() const = 0;
	};

protected:
	Real m_tolerance;
	ContactCallbackFunction m_contactCB;
	void *m_contactCBUserData;
	std::vector<CollisionObject *> m_collisionObjects;

	void updateAABB(const Vector3r &p, AABB &aabb);

public:
	CollisionDetection();
	virtual ~CollisionDetection();

	void cleanup();

	Real getTolerance() const { return m_tolerance; }
	void setTolerance(Real val) { m_tolerance = val; }

	void addRigidBodyContact(const uint rbIndex1, const uint rbIndex2,
		const Vector3r &cp1, const Vector3r &cp2,
		const Vector3r &normal, const Real dist,
		const Real restitutionCoeff, const Real frictionCoeff);

	void addParticleRigidBodyContact(const uint particleIndex, const uint rbIndex,
		const Vector3r &cp1, const Vector3r &cp2,
		const Vector3r &normal, const Real dist,
		const Real restitutionCoeff, const Real frictionCoeff);

	std::vector<CollisionObject *> &getCollisionObjects() { return m_collisionObjects; }

	// pure virtual function.
	virtual void collisionDetection() = 0;

	void setContactCallback(CollisionDetection::ContactCallbackFunction val, void *userData);
};

#endif