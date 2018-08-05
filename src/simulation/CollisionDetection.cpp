#include"CollisionDetection.h"

CollisionDetection::CollisionDetection() :
	m_collisionObjects()
{
	m_collisionObjects.reserve(1000);
	m_contactCB = NULL;
	m_tolerance = static_cast<Real>(0.01);
}

CollisionDetection::~CollisionDetection()
{
	cleanup();
}

void CollisionDetection::cleanup()
{
	for (uint i = 0; i < m_collisionObjects.size(); i++)
		delete m_collisionObjects[i];
	m_collisionObjects.clear();
}

void CollisionDetection::addRigidBodyContact(const uint rbIndex1, const uint rbIndex2,
	const Vector3r &cp1, const Vector3r &cp2,
	const Vector3r &normal, const Real dist,
	const Real restitutionCoeff, const Real frictionCoeff)
{
	if (m_contactCB)
		m_contactCB(RigidBodyContactType, rbIndex1, rbIndex2, cp1, cp2, normal, dist, restitutionCoeff, frictionCoeff, m_contactCBUserData);
}

void CollisionDetection::addParticleRigidBodyContact(const uint particleIndex, const uint rbIndex,
	const Vector3r &cp1, const Vector3r &cp2,
	const Vector3r &normal, const Real dist,
	const Real restitutionCoeff, const Real frictionCoeff)
{
	if (m_contactCB)
		m_contactCB(ParticleRigidBodyContactType, particleIndex, rbIndex, cp1, cp2, normal, dist, restitutionCoeff, frictionCoeff, m_contactCBUserData);
}

void CollisionDetection::setContactCallback(CollisionDetection::ContactCallbackFunction val, void *userData)
{
	m_contactCB = val;
	m_contactCBUserData = userData;
}