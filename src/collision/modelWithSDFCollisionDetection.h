#pragma once

#include "model.h"
#include "SDFbasedCollisionDetection.h"
#include "ClassObject.h"

class modelWithSDFCollisionDetection
{
public:
	modelWithSDFCollisionDetection(ClassObject *f_p_SDFObject, SDFbasedCollisionDetection *f_p_CSdf, ClassObject *f_p_envObject);
	~modelWithSDFCollisionDetection();

	SDFbasedCollisionDetection *m_p_CSdf;
	ClassObject *m_p_SDFObject;
	ClassObject *m_p_envObject;

	void traverseAllverticesInModelAndProcessPenetration();


};