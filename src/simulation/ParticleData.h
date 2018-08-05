#ifndef __PARTICLEDATA_H__
#define __PARTICLEDATA_H__

#include "utility.h"
#include "CollisionDetection.h"

/** This class encapsulates the state of all vertices.
* All parameters are stored in individual arrays.
*/
class VertexData
{
private:
	std::vector<Vector3r> m_x;

public:
	FORCE_INLINE VertexData(void) :
		m_x()
	{
	}

	FORCE_INLINE ~VertexData(void)
	{
		m_x.clear();
	}

	FORCE_INLINE void addVertex(const Vector3r &vertex)
	{
		m_x.push_back(vertex);
	}

	FORCE_INLINE Vector3r &getPosition(const uint i)
	{
		return m_x[i];
	}

	FORCE_INLINE const Vector3r &getPosition(const uint i) const
	{
		return m_x[i];
	}

	FORCE_INLINE void setPosition(const uint i, const Vector3r &pos)
	{
		m_x[i] = pos;
	}

	/** Resize the array containing the particle data.
	*/
	FORCE_INLINE void resize(const uint newSize)
	{
		m_x.resize(newSize);
	}

	/** Reserve the array containing the particle data.
	*/
	FORCE_INLINE void reserve(const uint newSize)
	{
		m_x.reserve(newSize);
	}

	/** Release the array containing the particle data.
	*/
	FORCE_INLINE void release()
	{
		m_x.clear();
	}

	FORCE_INLINE uint size() const
	{
		return (uint)m_x.size();
	}

	FORCE_INLINE const std::vector<Vector3r> *getVertices()
	{
		return &m_x;
	}
};


/** This class encapsulates the state of all particles of a particle model.
* All parameters are stored in individual arrays.
*/
class ParticleData
{
private:
	// Mass
	// If the mass is zero, the particle is static.
	std::vector<Real> m_masses;
	std::vector<Real> m_invMasses;

	// Dynamic state
	std::vector<Vector3r> m_x0;
	std::vector<Vector3r> m_x;
	std::vector<Vector3r> m_v;
	std::vector<Vector3r> m_a;
	std::vector<Vector3r> m_oldX;
	std::vector<Vector3r> m_lastX;

public:
	FORCE_INLINE ParticleData(void) :
		m_masses(),
		m_invMasses(),
		m_x0(),
		m_x(),
		m_v(),
		m_a(),
		m_oldX(),
		m_lastX()
	{
	}

	FORCE_INLINE ~ParticleData(void)
	{
		m_masses.clear();
		m_invMasses.clear();
		m_x0.clear();
		m_x.clear();
		m_v.clear();
		m_a.clear();
		m_oldX.clear();
		m_lastX.clear();
	}

	FORCE_INLINE void addVertex(const Vector3r &vertex)
	{
		m_x0.push_back(vertex);
		m_x.push_back(vertex);
		m_oldX.push_back(vertex);
		m_lastX.push_back(vertex);
		m_masses.push_back(1.0);
		m_invMasses.push_back(1.0);
		m_v.push_back(Vector3r(0.0, 0.0, 0.0));
		m_a.push_back(Vector3r(0.0, 0.0, 0.0));
	}

	FORCE_INLINE Vector3r &getPosition(const uint i)
	{
		return m_x[i];
	}

	// allow other constant member function to call this function.Because constant function can't invoke non-constant function above.
	FORCE_INLINE const Vector3r &getPosition(const uint i) const
	{
		return m_x[i];
	}

	FORCE_INLINE void setPosition(const uint i, const Vector3r &pos)
	{
		m_x[i] = pos;
	}

	FORCE_INLINE Vector3r &getPosition0(const uint i)
	{
		return m_x0[i];
	}

	// allow other constant member function to call this function.Because constant function can't invoke non-constant function above.
	FORCE_INLINE const Vector3r &getPosition0(const uint i) const
	{
		return m_x0[i];
	}

	FORCE_INLINE void setPosition0(const uint i, const Vector3r &pos)
	{
		m_x0[i] = pos;
	}

	FORCE_INLINE Vector3r &getLastPosition(const uint i)
	{
		return m_lastX[i];
	}

	FORCE_INLINE const Vector3r &getLastPosition(const uint i) const
	{
		return m_lastX[i];
	}

	FORCE_INLINE void setLastPosition(const uint i, const Vector3r &pos)
	{
		m_lastX[i] = pos;
	}

	FORCE_INLINE Vector3r &getOldPosition(const uint i)
	{
		return m_oldX[i];
	}

	FORCE_INLINE const Vector3r &getOldPosition(const uint i) const
	{
		return m_oldX[i];
	}

	FORCE_INLINE void setOldPosition(const uint i, const Vector3r &pos)
	{
		m_oldX[i] = pos;
	}

	FORCE_INLINE Vector3r &getVelocity(const uint i)
	{
		return m_v[i];
	}

	FORCE_INLINE const Vector3r &getVelocity(const uint i) const
	{
		return m_v[i];
	}

	FORCE_INLINE void setVelocity(const uint i, const Vector3r &vel)
	{
		m_v[i] = vel;
	}

	FORCE_INLINE Vector3r &getAcceleration(const uint i)
	{
		return m_a[i];
	}

	FORCE_INLINE const Vector3r &getAcceleration(const uint i) const
	{
		return m_a[i];
	}

	FORCE_INLINE void setAcceleration(const uint i, const Vector3r &accel)
	{
		m_a[i] = accel;
	}

	FORCE_INLINE Real getMass(const uint i)
	{
		return m_masses[i];
	}

	FORCE_INLINE const Real getMass(const uint i) const
	{
		return m_masses[i];
	}

	FORCE_INLINE void setMass(const uint i, const Real mass)
	{
		m_masses[i] = mass;
		if (mass != 0.0)
			m_invMasses[i] = static_cast<Real>(1.0) / mass;
		else
			m_invMasses[i] = 0.0;
	}

	FORCE_INLINE const Real getInvMass(const uint i) const
	{
		return m_invMasses[i];
	}

	FORCE_INLINE const uint getNumberOfParticles() const
	{
		return (uint)m_x.size();
	}

	/** Resize the array containing the particle data.
	*/
	FORCE_INLINE void resize(const uint newSize)
	{
		m_masses.resize(newSize);
		m_invMasses.resize(newSize);
		m_x0.resize(newSize);
		m_x.resize(newSize);
		m_v.resize(newSize);
		m_a.resize(newSize);
		m_oldX.resize(newSize);
		m_lastX.resize(newSize);
	}

	/** Reserve the array containing the particle data.
	*/
	FORCE_INLINE void reserve(const uint newSize)
	{
		m_masses.reserve(newSize);
		m_invMasses.reserve(newSize);
		m_x0.reserve(newSize);
		m_x.reserve(newSize);
		m_v.reserve(newSize);
		m_a.reserve(newSize);
		m_oldX.reserve(newSize);
		m_lastX.reserve(newSize);
	}

	/** Release the array containing the particle data.
	*/
	FORCE_INLINE void release()
	{
		m_masses.clear();
		m_invMasses.clear();
		m_x0.clear();
		m_x.clear();
		m_v.clear();
		m_a.clear();
		m_oldX.clear();
		m_lastX.clear();
	}

	FORCE_INLINE uint size() const
	{
		return (uint)m_x.size();
	}
};

#endif