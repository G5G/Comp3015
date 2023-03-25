#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include <glad/glad.h>
#include "helper/scene.h"
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include <glm/glm.hpp>
#include "helper/objmesh.h"
#include "helper/skybox.h"
class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    GLuint hdrFBO;
    GLuint quad;
    GLuint hdrTex, avgTex;

    Teapot teapot;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> fire;
    SkyBox sky;
	float angle;
    float tmp;
	float tPrev;
    void compile();
    void setupFBO();
    void pass1();
    void pass2();
    void computeLogAveLuminance();
    void drawScene();


public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
	void setMatrices();
};

#endif // SCENEBASIC_UNIFORM_H
