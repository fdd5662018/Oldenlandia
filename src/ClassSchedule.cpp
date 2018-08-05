#include "ClassSchedule.h"
#include "ClassEnvHypophysomaObject.h"
#include "ClassEnvNasalPassageObject.h"
#include "ClassEnvNoseFurObject.h"
#include "ClassEnvSphenoObject.h"
#include "ClassToolBiqianObject.h"
#include "ClassToolChongxiqiaoObject.h"
#include "ClassToolGuashaoObject.h"
#include "ClassToolJiandaoObject.h"
#include "ClassToolMozuanObject.h"
#include "ClassToolTanzhenObject.h"

namespace env {
	bool move2fixSwitch = false;
}

ClassSchedule::ClassSchedule()
{
	// build and compile shaders
	// -------------------------
	Shader pbrShader = ResourceManager::LoadShader("pbr", "shaders/IBLdiffuseAndSpecular/pbr.vs", "shaders/IBLdiffuseAndSpecular/pbr.fs");
	Shader sphenoShader = ResourceManager::LoadShader("spheno", "shaders/IBLdiffuseAndSpecular/spheno.vs", "shaders/IBLdiffuseAndSpecular/spheno.fs");
	Shader furShader = ResourceManager::LoadShader("fur", "shaders/IBLdiffuseAndSpecular/spheno.vs", "shaders/IBLdiffuseAndSpecular/spheno.fs");
	//Shader biqianShader = ResourceManager::LoadShader("biqian", "shaders/IBLdiffuseAndSpecular/biqian.vs", "shaders/IBLdiffuseAndSpecular/biqian.fs");
	Shader chongxiqiaoShader = ResourceManager::LoadShader("chongxiqiao", "shaders/tools/chongxiqiao.vs", "shaders/tools/chongxiqiao.fs");
	// tools
	Shader tanzhenShader = ResourceManager::LoadShader("tanzhen", "shaders/tools/chongxiqiao.vs", "shaders/tools/chongxiqiao.fs");
	Shader guashaoShader = ResourceManager::LoadShader("guashao", "shaders/tools/chongxiqiao.vs", "shaders/tools/chongxiqiao.fs");

	// load models
	// -----------------------------------------------------
	Model *pNasalPassageModel = ResourceManager::LoadpModel("nasalPassage", "resources/objects/env/biqiang/biqiang.obj");
	Model *pNoseFurModel = ResourceManager::LoadpModel("noseFur", "resources/objects/env/noise_fur/noise_fur.obj");
	Model *pSphenoModel = ResourceManager::LoadpModel("spheno", "resources/objects/env/diegu/diegu.obj");
	// Model *pJiandaoModel = ResourceManager::LoadpModel("jiandao", "resources/objects/tanzhen/tanzhen_texture/tanzhen.obj");
	Model *pChongxiqiaoModel = ResourceManager::LoadpModel("chongxiqiao", "resources/objects/tools/chongxiqiao/chongxiqiao.obj");
	// toolsModel
	Model *pTanzhenModel = ResourceManager::LoadpModel("tanzhen", "resources/objects/tools/tanzhen/tanzhen.obj");
	Model *pGuashaoModel = ResourceManager::LoadpModel("guashao", "resources/objects/tools/guashao/guashao.obj");
	// assemble model and shader for renderer
	// -----------------------------------------------------
	ClassModelRenderer *pMRnasalPassage = new ClassModelRenderer(pbrShader, pNasalPassageModel);
	ClassModelRenderer *pMRspheno = new ClassModelRenderer(sphenoShader, pSphenoModel);
	ClassModelRenderer *pMRfur = new ClassModelRenderer(furShader, pNoseFurModel);
	// ClassModelRenderer *pMRjiandao = new ClassModelRenderer(biqianShader, pJiandaoModel);
	ClassModelRenderer *pMRchongxiqiao = new ClassModelRenderer(chongxiqiaoShader, pChongxiqiaoModel);
	// tools
	ClassModelRenderer *pMRtanzhen = new ClassModelRenderer(tanzhenShader, pTanzhenModel);
	ClassModelRenderer *pMRguashao = new ClassModelRenderer(guashaoShader, pGuashaoModel);
	// Load renderer Objects
	// -----------------------------------------------------------
	ClassEnvNasalPassageObject *p_nPObjects = new ClassEnvNasalPassageObject(pMRnasalPassage);
	mObjectPointers.push_back(p_nPObjects);
	mObjectPointers.push_back(new ClassEnvSphenoObject(pMRspheno));
	mObjectPointers.push_back(new ClassEnvNoseFurObject(pMRfur));
	mObjectPointers.push_back(new ClassToolChongxiqiaoObject(pMRchongxiqiao));
	// tools
	// mpChongxiqiaoObject = new ClassToolChongxiqiaoObject(pMRchongxiqiao);
	ClassToolTanzhenObject *p_tanzhenObject = new ClassToolTanzhenObject(pMRtanzhen, HAPTIC_LEFT);
	mToolObjectPointers.push_back(p_tanzhenObject);
	mToolObjectPointers.push_back(new ClassToolGuashaoObject(pMRguashao, HAPTIC_RIGHT));
	// camera
	// ---------------------------------------------
	ResourceManager::GetCamera();
	// postProcessing
	// ------------------------------------------------
	Shader postProcessorShader = ResourceManager::LoadShader("postProcessor", "shaders/postProcessing/postProcessing.vs", "shaders/postProcessing/postProcessing.fs");
	pPostProcessor = new PostProcessor(postProcessorShader, env::SCR_WIDTH, env::SCR_HEIGHT);
	// collision test
	//Eigen::Vector3d x(0.0, 0.0, 0.0);
	//Eigen::Vector3d penetration(0.0, 0.0, 0.0);
	SDFbasedCollisionDetection *p_c_Sdf = new SDFbasedCollisionDetection();
	//cSdf.penetration(x, penetration);
	m_p_modelSDF = new modelWithSDFCollisionDetection(p_tanzhenObject, p_c_Sdf, p_nPObjects);
	//modelSDF.traverseAllverticesInModelAndProcessPenetration();
}

ClassSchedule::~ClassSchedule()
{
	for (int i = 0; i < mObjectPointers.size(); i++) {
		delete mObjectPointers[i];
	}
}

void ClassSchedule::Init()
{
	ResourceManager::LoadIBLMap();
	ResourceManager::BindIBLMap();
	ResourceManager::LoadHapticDevice(HAPTIC_BOTH);
}

void ClassSchedule::Update(GLfloat dt)
{
	for (int i = 0; i < mObjectPointers.size(); i++) {
		(mObjectPointers[i])->Update();
	}
	if (env::move2fixSwitch == true) {
		for (int i = 0; i < mToolObjectPointers.size(); i++) {
			(mToolObjectPointers[i])->Update();
		}
	}
	m_p_modelSDF->traverseAllverticesInModelAndProcessPenetration();
}

void ClassSchedule::ProcessInput(GLfloat dt)
{
}

void ClassSchedule::Render()
{
	//pPostProcessor->BeginRender();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < mObjectPointers.size(); i++) {
		(mObjectPointers[i])->Draw();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (env::move2fixSwitch == true) {
		for (int i = 0; i < mToolObjectPointers.size(); i++) {
			(mToolObjectPointers[i])->Draw();
		}
	}
	//pPostProcessor->EndRender();
	//pPostProcessor->Render();
}