/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "ClassPostProcessor.h"
#include "resource_manager.h"
#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height) 
    : PostProcessingShader(shader), Texture(), Width(width), Height(height)
{
    // Initialize renderbuffer/framebuffer object
    glGenFramebuffers(1, &this->MSFBO);
    glGenFramebuffers(1, &this->FBO);
    glGenRenderbuffers(1, &this->RBO);
	// rboDepthStencil
	unsigned int rboDepthStencil;
	glGenRenderbuffers(1, &rboDepthStencil);
    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height); // Allocate storage for render buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // Attach MS render buffer object to framebuffer
	// initialize depth and stencil renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
	// use a single renderbuffer object for both a depth AND stencil buffer.
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// now actually attach it
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);
	// complete test
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
    // Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    this->Texture.Generate(NULL, width, height);
	// this framebuffer is rendered on 2D so we don't need depth test.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0); // Attach texture to framebuffer as its color attachment
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Initialize render data and uniforms.
    this->initRenderData();
	// If the use is necessary?
	this->PostProcessingShader.use();
	this->PostProcessingShader.setInt("scene", 0);
	// cut texture
	ResourceManager::LoadTexture("texCut", "resources/textures/texCut.jpg");
	this->PostProcessingShader.setInt("texCut", 1);
}

void PostProcessor::BeginRender()
{
	// if this->MSFBO was not initialized then printf and cout will be vanished , so surprising but it is the fact.
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
    // Now resolve multisampled color-buffer into intermediate FBO to store to texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
    glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::Render(GLfloat time)
{
    // Set uniforms/options
    this->PostProcessingShader.use();
    // Render textured quad
    glActiveTexture(GL_TEXTURE0);
    this->Texture.Bind();
	glActiveTexture(GL_TEXTURE1);
	ResourceManager::GetTexture("texCut").Bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos        // Tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}