/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef HAPTIC_H
#define HAPTIC_H

#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>

#include <windows.h>
#include <conio.h>

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include<string>

class Haptic
{
public:
	// Constructor/Destructor
	Haptic(std::string deviceName);
	~Haptic();
	// hapticLoop
	int  Update(glm::vec3 hapticPos, glm::vec3 &offsitePos, glm::vec3 &glmAngle, int &Button);
	void Start();
	void Stop();
private:
	HDSchedulerHandle gCallbackHandle = 0;
	HHD hHD;
};

#endif