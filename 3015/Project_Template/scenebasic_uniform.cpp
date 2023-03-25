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
	angle = glm::pi<float>() / 2.0f;
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
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts,
	GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);
	// Set up the vertex array object
	glGenVertexArrays(1, &fsQuad);
	glBindVertexArray(fsQuad);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0); // Vertex position
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2); // Texture coordinates
	glBindVertexArray(0);
	//vec3 intense = vec3(0.000000000001f);
	prog.setUniform("Lights[0].Ld", vec3(0.6f));
	prog.setUniform("Lights[1].Ld", vec3(0.6f));
	prog.setUniform("Lights[0].Ls", vec3(0.6f));
	prog.setUniform("Lights[1].Ls", vec3(0.6f));
	//intense = vec3(0.03f);
	prog.setUniform("Lights[0].La", vec3(0.2f));
	prog.setUniform("Lights[1].La", vec3(0.2f));

	prog.setUniform("Fog.MaxDist", 2000.0f);
	prog.setUniform("Fog.MinDist", 20.0f);
	prog.setUniform("Fog.Colour", vec3(0.1f, 0.2f, 0.3f));

	prog.setUniform("LumThresh", 1.7f);
	float weights[10], sum, sigma2 = 25.0f;
	weights[0] = gauss(0, sigma2);
	sum = weights[0];
	for (int i = 1; i < 10; i++) {
		weights[i] = gauss(float(i), sigma2);
		sum += 2 * weights[i];
	}
	// Normalize the weights and set the uniform
	for (int i = 0; i < 10; i++) {
		std::stringstream uniName;
		uniName << "Weight[" << i << "]";
		float val = weights[i] / sum;
		prog.setUniform(uniName.str().c_str(), val);
	}
	// Set up two sampler objects for linear and nearest filtering
	GLuint samplers[2];
	glGenSamplers(2, samplers);
	linearSampler = samplers[0];
	nearestSampler = samplers[1];
	GLfloat border[] = { 0.0f,0.0f,0.0f,0.0f };
	// Set up the nearest sampler
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameterfv(nearestSampler, GL_TEXTURE_BORDER_COLOR, border);
	// Set up the linear sampler
	glSamplerParameteri(linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameterfv(linearSampler, GL_TEXTURE_BORDER_COLOR, border);
	// We want nearest sampling except for the last pass.
	glBindSampler(0, nearestSampler);
	glBindSampler(1, nearestSampler);
	glBindSampler(2, nearestSampler);




	/*GLuint trex11 = Texture::loadTexture("media/base_color.png");
	GLuint trex22 = Texture::loadTexture("media/vine.png");
	GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tex2);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);*/
	//view = glm::lookAt(vec3(40.0f, 10.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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
	tPrev = t;
	angle += 0.25f * deltaT;
	if (angle > glm::two_pi<float>()) {
		angle -= glm::two_pi<float>();
	}
	vec4 lightPos = vec4(5.0f * vec3(cosf(angle) * 7.5f, 1.5f, sinf(angle) * 7.5f), 1.0f);
	prog.setUniform("Lights[0].Position", vec4(lightPos));
	lightPos.x = +4.10f;
	prog.setUniform("Lights[1].Position", vec4(lightPos));
	lightPos.x = -4.10f;
	prog.setUniform("Lights[2].Position", vec4(lightPos));

}

void SceneBasic_Uniform::render()
{
	view = glm::lookAt(vec3(40.0f * cos(angle),7.5f, 7.5f*sin(angle)), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	//view = glm::lookAt(vec3(40.0f , 7.5f, 7.5f) , vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	//vec4 lightPos = vec4(300.0f, 30.0f, 10.0f, 1.0f);
	//vec4 lightPos = vec4(0.0f, 14.0f, 0.0f, 1.0f);

	pass1();
	computeLogAveLuminance();
	pass2();
	pass3();
	pass4();
	pass5();
	
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
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//view = glm::lookAt(vec3(2.0f, 0.0f, 14.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 100.0f);
	drawScene();

}

void SceneBasic_Uniform::pass2()
{
	prog.setUniform("Pass", 2);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFbo);
	// We're writing to tex1 this time
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,tex1, 0);
	glViewport(0, 0, bloomBufWidth, bloomBufHeight);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	model = mat4(1.0f);
	view = mat4(1.0f);
	projection = mat4(1.0f);
	setMatrices();
	// Render the full-screen quad
	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void SceneBasic_Uniform::pass3()
{
	prog.setUniform("Pass", 3);
	// We're writing to tex2 this time
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		tex2, 0);
	// Render the full-screen quad
	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void SceneBasic_Uniform::pass4()
{
	prog.setUniform("Pass", 4);
	// We're writing to tex1 this time
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,tex1, 0);
	// Render the full-screen quad
	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void SceneBasic_Uniform::pass5()
{
	prog.setUniform("Pass", 5);
	// Bind to the default framebuffer, this time we're going to
	// actually draw to the screen!
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	// In this pass, we're reading from tex1 (unit 1) and we want
	// linear sampling to get an extra blur
	glBindSampler(1, linearSampler);
	// Render the full-screen quad
	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// Revert to nearest sampling
	glBindSampler(1, nearestSampler);
}

void SceneBasic_Uniform::computeLogAveLuminance()
{
	int size = width * height;
	std::vector<GLfloat> texData(size * 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());
	float sum = 0.0f;
	for (int i = 0; i < size; i++) {
		float lum = glm::dot(vec3(texData[i * 3 + 0], texData[i * 3 + 1],
			texData[i * 3 + 2]),
			vec3(0.2126f, 0.7152f, 0.0722f));
		sum += logf(lum + 0.00001f);
	}
	prog.setUniform("AveLum", expf(sum / size));
}

void SceneBasic_Uniform::setupFBO()
{
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &hdrFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);
	// Create the texture object
	glGenTextures(1, &hdrTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		hdrTex, 0);
	// Create the depth buffer
	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, depthBuf);
	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	// Create an FBO for the bright-pass filter and blur
	glGenFramebuffers(1, &blurFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFbo);
	// Create two texture objects to ping-pong for the bright-pass filter
	// and the two-pass blur
	bloomBufWidth = width / 8;
	bloomBufHeight = height / 8;
	glGenTextures(1, &tex1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, bloomBufWidth, bloomBufHeight);
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &tex2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, bloomBufWidth, bloomBufHeight);
	// Bind tex1 to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		tex1, 0);
	glDrawBuffers(1, drawBuffers);
	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
float SceneBasic_Uniform::gauss(float x, float sigma2)
{
	double coeff = 1.0 / glm::two_pi<double>() * sigma2;
	double expon = -(x * x) / (2.0 * sigma2);
	return (float)(coeff * exp(expon));
}

void SceneBasic_Uniform::drawScene()
{
	
	//vec4 lightPos = vec4(0.0f, 4.0f, 2.5f, 1.0f);
	//lightPos.x = -7.0f;
	//prog.setUniform("Lights[0].Position", view * lightPos);
	//lightPos.x = 0.0f;
	//prog.setUniform("Lights[1].Position", view * lightPos);
	//lightPos.x = 7.0f;
	//prog.setUniform("Lights[2].Position", view * lightPos);


	prog.setUniform("Lights[0].Ld", vec3(0.2f));
	prog.setUniform("Lights[1].Ld", vec3(0.2f));
	prog.setUniform("Lights[2].Ld", vec3(0.2f));
	prog.setUniform("Lights[0].Ls", vec3(0.2f));
	prog.setUniform("Lights[1].Ls", vec3(0.2f));
	prog.setUniform("Lights[2].Ls", vec3(0.2f));

	prog.setUniform("Lights[0].La", vec3(0.2f));
	prog.setUniform("Lights[1].La", vec3(0.2f));
	prog.setUniform("Lights[2].La", vec3(0.2f));



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
	prog.setUniform("Material.Shininess", 100.0f);
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