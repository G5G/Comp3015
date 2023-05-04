#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include <glad/glad.h>
#include "helper/scene.h"
#include "helper/glslprogram.h"

#include "helper/teapot.h"
#include "helper/plane.h"
#include <glm/glm.hpp>
#include "helper/objmesh.h"
#include "helper/skybox.h"

#include <GLFW/glfw3.h>

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog,flatProg;
    //Random rand;

    GLuint posBuf[2], velBuf[2], age[2];

    GLuint particleArray[2];

    GLuint feedback[2];

    glm::vec3 position = glm::vec3(0, 0, 5);
    float hor = 3.14f;
    float ver = 0.0f;
    float Initialfov = 45.0f;
    float speed = 3.0f;
    float mousespeed = 0.005f;

    
    int nParticles;
    float particleLifetime;
    float angle;
    float time, deltaT;
    GLuint drawBuf; 
    glm::vec3 emitterPos,emitterDir;

    GLuint fsQuad;
    GLuint hdrFbo, blurFbo;
    GLuint hdrTex, tex1, tex2;
    GLuint linearSampler, nearestSampler;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> fire;
    SkyBox sky;
    int bloomBufWidth, bloomBufHeight;
	float tPrev;
    void compile();
    void setupFBO();
    void pass1();
    void pass2();
    void pass3();
    void pass4();
    void pass5();
    float gauss(float, float);
    void computeLogAveLuminance();
    void drawScene();
    void setMatrices(GLSLProgram&);
    void initBuffers();
    
public:

    SceneBasic_Uniform();
    void initScene();
    void userinput(double,double,bool,bool,bool,bool,bool);
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
