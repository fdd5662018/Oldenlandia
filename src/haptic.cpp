#include"haptic.h"
#include"utility.h"

HDCallbackCode HDCALLBACK hapticMainCallback(void *pUserData);
HDCallbackCode HDCALLBACK hapticPositionCallback(void *pUserData);
HDCallbackCode HDCALLBACK hapticAngleCallback(void *pUserData);
HDCallbackCode HDCALLBACK hapticButtonCallback(void *pUserData);

Haptic::Haptic(std::string deviceName)
{
	HDErrorInfo error;
	hHD = hdInitDevice(deviceName.c_str());
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "\nPress any key to quit.\n");
		getch();
	}

	printf("haptic init: %s\n", deviceName.c_str());
}

void Haptic::Start() {
	HDErrorInfo error;
	hdMakeCurrentDevice(hHD);
	/* Schedule the haptic callback function for continuously monitoring the
	button state and rendering the anchored spring force. */
	gCallbackHandle = hdScheduleAsynchronous(hapticMainCallback, 0, HD_MAX_SCHEDULER_PRIORITY);

	hdEnable(HD_FORCE_OUTPUT);//all motor are turned on

							  /* Start the haptic rendering loop. */
	hdStartScheduler();
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to start scheduler");
		fprintf(stderr, "\nPress any key to quit.\n");
		getch();
	}
}
void Haptic::Stop() {
	/* Cleanup by stopping the haptics loop, unscheduling the asynchronous
	callback, disabling the device. */
	//hdMakeCurrentDevice(hHD);
	//hdStopScheduler();
	//hdUnschedule(gCallbackHandle);
}

Haptic::~Haptic()
{
	//hdDisableDevice(hHD);
}

int  Haptic::Update(glm::vec3 hapticPos, glm::vec3 &offsitePos, glm::vec3 &Angle, int &Button)
{
	hdMakeCurrentDevice(hHD);
	hduVector3Dd position;
	hduVector3Dd hAngle;
	hdScheduleSynchronous(hapticPositionCallback, position, HD_DEFAULT_SCHEDULER_PRIORITY);
	hdScheduleSynchronous(hapticAngleCallback, hAngle, HD_DEFAULT_SCHEDULER_PRIORITY);
	hdScheduleSynchronous(hapticButtonCallback, &Button, HD_DEFAULT_SCHEDULER_PRIORITY);

	//printf("haptic position(%.3f %.3f %.3f)\n", position[0], position[1], position[2]);
	//printf("haptic angle(%.3f %.3f %.3f)\n", angle[0], angle[1], angle[2]);
	glm::vec3 glmAngle(hAngle[0], hAngle[1], hAngle[2]);
	//Angle = glmAngle - Angle;
	Angle = glmAngle;
	//printf("haptic angle(%.3f %.3f %.3f)\n", Angle[0], Angle[1], Angle[2]);
	glm::vec3 offset(position[0], position[1], position[2]);
	//printf("haptic position(%f %f %f)\n", hapticPos.x, hapticPos.y, hapticPos.z);
	//printf("haptic offset position(%f %f %f)\n", offset[0], offset[1], offset[2]);
	//printf("haptic position(%f %f %f)\n", (hapticPos + offset)[0], (hapticPos + offset)[1], (hapticPos + offset)[2]);
	offset = env::gHapticSens * offset;

	offsitePos = (hapticPos + offset);
	return 0;
}

/******************************************************************************
* Main scheduler callback for update the position of haptic device.
*****************************************************************************/
HDCallbackCode HDCALLBACK hapticMainCallback(void *pUserData)
{
	//printf("(%f %f %f)\n" );
	return HD_CALLBACK_CONTINUE;
}

HDCallbackCode HDCALLBACK hapticPositionCallback(void *pUserData)
{
	HDdouble *pPosition = (HDdouble *)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdGetDoublev(HD_CURRENT_POSITION, pPosition);
	hdEndFrame(hdGetCurrentDevice());

	return HD_CALLBACK_DONE;
}

HDCallbackCode HDCALLBACK hapticAngleCallback(void *pUserData)
{
	HDdouble *pAngle = (HDdouble *)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, pAngle);
	hdEndFrame(hdGetCurrentDevice());

	return HD_CALLBACK_DONE;
}

HDCallbackCode HDCALLBACK hapticButtonCallback(void *pUserData)
{
	HDint *pButtonState = (HDint *)pUserData;

	hdBeginFrame(hdGetCurrentDevice());
	hdGetIntegerv(HD_CURRENT_BUTTONS, pButtonState);
	hdEndFrame(hdGetCurrentDevice());

	return HD_CALLBACK_DONE;
}