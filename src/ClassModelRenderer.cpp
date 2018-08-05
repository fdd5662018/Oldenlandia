#include "ClassModelRenderer.h"



ClassModelRenderer::ClassModelRenderer(Shader shader, Model *pModel)
	:Shader(shader.ID)
{
	mpModel = pModel;
}


ClassModelRenderer::~ClassModelRenderer()
{
}

void ClassModelRenderer::Draw()
{
	mpModel->Draw(Shader(this->ID));
}
