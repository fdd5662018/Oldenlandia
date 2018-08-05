#include "ClassToolChongxiqiaoObject.h"
#include "utility.h"


ClassToolChongxiqiaoObject::ClassToolChongxiqiaoObject(ClassModelRenderer *fModelRenderer, env::DEV_HAPTIC_TYPE Type) :
	ClassToolObject(fModelRenderer, Type)
{
}


ClassToolChongxiqiaoObject::~ClassToolChongxiqiaoObject()
{
}

void ClassToolChongxiqiaoObject::Update()
{
	// father class
	ClassToolObject::Update();
	// button state variances
	static bool fixView = false;
	static bool buttonPushing = false;
	// process button(falling edge)
	if (mButton & env::HAPTIC_BUTTON_1 == true) {
		buttonPushing = true;
	}
	else if (buttonPushing == true) {
		buttonPushing = false;
		fixView = !fixView;
		env::move2fixSwitch = !env::move2fixSwitch;
	}
	// cout << "Button: " << Button << endl;
	// camera
	if (fixView == false) {
		ResourceManager::GetCamera()->ProcessHapticMovement(mModelMatrix);
	}
}

void ClassToolChongxiqiaoObject::Draw()
{

}
