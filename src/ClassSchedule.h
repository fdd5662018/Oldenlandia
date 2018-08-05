#pragma once
#include "ClassObject.h"
#include "ClassToolObject.h"
#include "ClassPostProcessor.h"
#include "utility.h"
#include "collision/SDFbasedCollisionDetection.h"
#include "collision/modelWithSDFCollisionDetection.h"

class ClassSchedule
{
public:
	ClassSchedule();
	~ClassSchedule();
	// Initialize game state (load all shaders/textures/levels)
	void Init();
	// Loop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void DoCollisions();
	void Render();
private:
	std::vector<ClassObject *> mObjectPointers;
	std::vector<ClassToolObject *> mToolObjectPointers;
	PostProcessor *pPostProcessor;
	modelWithSDFCollisionDetection *m_p_modelSDF;
	//ClassToolChongxiqiaoObject *mpChongxiqiaoObject;
};

