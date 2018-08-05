/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture2D::Texture2D()
	:mWidth(0), mHeight(0), mInternal_Format(GL_RGB), mImage_Format(GL_RGB)
{
	glGenTextures(1, &(this->ID));
	mWrap_S = GL_REPEAT;
	mWrap_T = GL_REPEAT; 
	mFilter_Min = GL_LINEAR; 
	mFilter_Max = GL_LINEAR;
}

Texture2D::Texture2D(const GLchar *file, bool gammaCorrection, GLuint fWrap_S, GLuint fWrap_T, GLuint fFilter_Min, GLuint fFilter_Max)
	: mWrap_S(fWrap_S), mWrap_T(fWrap_T), mFilter_Min(fFilter_Min), mFilter_Max(fFilter_Max), \
	mWidth(0), mHeight(0), mInternal_Format(GL_RGB), mImage_Format(GL_RGB)
{
	// Create Texture object
	unsigned char *data = stbi_load(file, &mWidth, &mHeight, &mNrComponents, 0);
	if (data)
	{
		if (mNrComponents == 1)
		{
			mInternal_Format = mImage_Format = GL_RED;
		}
		else if (mNrComponents == 3)
		{
			mInternal_Format = gammaCorrection ? GL_SRGB : GL_RGB;
			mImage_Format = GL_RGB;
		}
		else if (mNrComponents == 4)
		{
			mInternal_Format = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			mImage_Format = GL_RGBA;
		}
		// Now generate texture
		glGenTextures(1, &(this->ID));
		// Create Texture
		Generate(data, mWidth, mHeight);
		// And finally free image data
		//SOIL_free_image_data(image);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << file << std::endl;
		stbi_image_free(data);
	}
}

void Texture2D::Generate(unsigned char* data, GLuint width, GLuint height)
{
	mWidth = width;
	mHeight = height;
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternal_Format, mWidth, mHeight, 0, mImage_Format, GL_UNSIGNED_BYTE, data);
	if (mFilter_Min == GL_LINEAR_MIPMAP_LINEAR)
		glGenerateMipmap(GL_TEXTURE_2D);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}