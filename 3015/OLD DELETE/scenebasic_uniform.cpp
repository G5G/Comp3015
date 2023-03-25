#include "scenebasic_uniform.h"
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "helper/texture.h"

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;
#include <glm/gtc/matrix_transform.hpp>
#include "helper/glutils.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0.0f), plane(300.0f,300.0f,1,1),teapot(80,glm::mat4(1.0f)),sky(120.0f)
{
	mesh = ObjMesh::load("../Project_Template/media/forest_cabin_LOD0.obj", true);
	fire = ObjMesh::load("../Project_Template/media/fire.obj", true);
}

void SceneBasic_Uniform::initScene()
{

	compile();
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	projection = mat4(1.0f);
	angle = glm::pi<float>() / 4.0f;
	tmp = 1.1f;
	setupFBO();
	GLfloat verts[] =
	{
		-1.0f,-1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
	};
	GLfloat tc[] =
	{
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};
	unsigned int handle[2];
	glGenBuffers(2, handle);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);
	glGenVertexArrays(1, &quad);
	glBindVertexArray(quad);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	//vec3 intense = vec3(0.000000000001f);
	prog.setUniform("Lights[0].Ld", vec3(0.000000001f));
	prog.setUniform("Lights[1].Ld", vec3(0.000000001f));
	prog.setUniform("Lights[0].Ls", vec3(0.03f));
	prog.setUniform("Lights[1].Ls", vec3(0.03f));
	//intense = vec3(0.03f);
	prog.setUniform("Lights[0].La", vec3(0.001f));
	prog.setUniform("Lights[1].La", vec3(0.03f));

	prog.setUniform("Fog.MaxDist", 2000.0f);
	prog.setUniform("Fog.MinDist", 20.0f);
	prog.setUniform("Fog.Colour", vec3(0.1f, 0.2f, 0.3f));

	GLuint tex1 = Texture::loadTexture("media/base_color.png");
	GLuint tex2 = Texture::loadTexture("media/vine.png");
	GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	view = glm::lookAt(vec3(40.0f, 10.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}


void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t)
{
	//view = glm::lookAt(vec3(40.0f, 10.0f, 7.5f*angle), vec3(0.0f, 0.75f, 0.0f+angle), vec3(0.0f, 1.0f + angle, 0.0f));
	//prog.setUniform("Lights[0].Ld", vec3(angle/10));
	//prog.setUniform("Lights[0].Ls", vec3(t));
	//prog.setUniform("Lights[0].La", vec3(t));

	//prog.setUniform("Lights[1].Ld", vec3(t / 10));
	//prog.setUniform("Lights[1].Ls", vec3(t));
	//prog.setUniform("Lights[1].La", vec3(t));
	

	float deltaT = t - tPrev;
	if (tPrev == 0.0f) {
		deltaT = 0.0f;
	}
	if (tmp < 0.0f) {
		tmp = 0.0f;
	}
	else if (tmp > 0.0001f) {
		tmp = tmp - 0.01f;
	}
	tPrev = t;
	angle += 0.25f * deltaT;
	if (angle > glm::two_pi<float>()) {
		angle -= glm::two_pi<float>();
	}

}

void SceneBasic_Uniform::render()
{
	view = glm::lookAt(vec3(40.0f * cos(angle),7.5f, 7.5f*sin(angle)), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	pass1();
	computeLogAveLuminance();
	pass2();
	
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
    
    
}
void SceneBasic_Uniform::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix",glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
	prog.setUniform("ProjectionMatrix", projection);
    
}


void SceneBasic_Uniform::pass1()
{
	
	prog.setUniform("Pass", 1);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);
	drawScene();

}

void SceneBasic_Uniform::pass2()
{

	prog.setUniform("Pass", 2);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	view = mat4(1.0);
	model = mat4(1.0);
	projection = mat4(1.0);
	setMatrices();

	glBindVertexArray(quad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::computeLogAveLuminance()
{
	int size = width * height;
	std::vector<GLfloat> texData(size * 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());
	float sum = 0.0f;
	for (int i = 0; i < size; i++) 
	{
		float lum = glm::dot(vec3(texData[i * 3 + 0], texData[i * 3 + 1], texData[i * 3 + 2]), vec3(0.2126f, 0.7152, 0.0722f));
		sum += logf(lum + 0.01f);
		
	}
	prog.setUniform("AveLum", expf(sum / size));
}

void SceneBasic_Uniform::setupFBO()
{
	GLuint depthBuf;
	// Create and bind the FBO
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// The depth buffer
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// The HDR color buffer
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &hdrTex);
	glBindTexture(GL_TEXTURE_2D, hdrTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Attach the images to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuf);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		hdrTex, 0);
	GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(2, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SceneBasic_Uniform::drawScene()
{
	//vec4 lightPos = vec4(300.0f, 30.0f, 10.0f, 1.0f);
	vec4 lightPos = vec4(0.0f, 14.0f, 0.0f, 1.0f);
	prog.setUniform("Lights[0].Position", vec4(lightPos));
	lightPos.x =+ 17.0f;
	prog.setUniform("Lights[1].Position", vec4(lightPos));

	prog.setUniform("Material.Kd", 0.204f, 0.128f, 0.89f);
	prog.setUniform("Material.Ks", 0.5f, 0.5f, 0.5f);
	prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
	prog.setUniform("Material.Shininess", 100.0f);
	model = mat4(1.0f);
	setMatrices();
	plane.render();

	
	prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
	prog.setUniform("Material.Shininess", 1000.0f);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 14.0f, 0.0f));
	setMatrices();
	mesh->render();

	
	prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
	prog.setUniform("Material.Shininess", 100.0f);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 1.0f, 20.0f));
	setMatrices();
	fire->render();

	sky.render();
	

}