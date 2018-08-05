#pragma once

/*Can't include the header that recursively include the utility.h*/
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

#include "texture.h"
#include "model.h"
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// holds all OpenGL type declarations
#include <glad/glad.h>

#define HAPTIC_LEFT 0x00
#define HAPTIC_RIGHT 0x01
#define HAPTIC_BOTH 0x02

namespace  env {
	const GLuint SCR_WIDTH = 800;
	const GLuint SCR_HEIGHT = 600;
	// haptic
	const int HAPTIC_BUTTON_1 = 1;
	const int HAPTIC_BUTTON_2 = 2;
	// settings
	const unsigned char irradianceMap = 3;
	const unsigned char prefilterMap = 4;
	const unsigned char brdfLUTTexture = 5;
	const glm::vec3 gCameraPos(1.77628f, 35.8592f, 12.6633f);
	const glm::vec3 gToolInsertPos(1.042f, 35.118f, 20.329f);
	const glm::vec3 gHapticSens(0.13f, 0.13f, 0.13f);
	typedef unsigned char DEV_HAPTIC_TYPE;
	// switch
	extern bool move2fixSwitch;
}
typedef float Real;
using Vector2r = Eigen::Matrix<Real, 2, 1>;
using Vector3r = Eigen::Matrix<Real, 3, 1>;
using Vector4r = Eigen::Matrix<Real, 4, 1>;
using Vector5r = Eigen::Matrix<Real, 5, 1>;
using Vector6r = Eigen::Matrix<Real, 6, 1>;
using Matrix2r = Eigen::Matrix<Real, 2, 2>;
using Matrix3r = Eigen::Matrix<Real, 3, 3>;
using Matrix4f = Eigen::Matrix<float, 4, 4>;
using Matrix4r = Eigen::Matrix<Real, 4, 4>;
using AlignedBox2r = Eigen::AlignedBox<Real, 2>;
using AlignedBox3r = Eigen::AlignedBox<Real, 3>;
using AngleAxisr = Eigen::AngleAxis<Real>;
using Quaternionr = Eigen::Quaternion<Real>;