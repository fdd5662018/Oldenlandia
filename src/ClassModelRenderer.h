#pragma once
#include "resource_manager.h"
class ClassModelRenderer :
	public Shader
{
public:
	ClassModelRenderer(Shader shader, Model *pmodel);
	~ClassModelRenderer();

	Model *mpModel;

	void Draw();
};

