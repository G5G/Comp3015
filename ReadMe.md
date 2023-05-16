# About this project
------------------

This project is a captivating example of graphics and pipelines created using OpenGL. It effectively showcases various shaders and graphic techniques that have been thoughtfully implemented within the project, demonstrating the capabilities of the system

# Controls
W - Move forward
A - Move Left
S - Move Backwards
D - Move Right
Left Shift(hold) - Move faster
Mouse - rotates to where the mouse moves.
 

# ExecutableVersion
----------------
To run this project as a standalone application, please use the following folder:

The ExecutableVersion folder contains the necessary files and resources for standalone execution.

- `Project_Template` (Do not rename)
	- `media` (contains all the textures and obj files)
	- `shader` (contains all the shader code)
	- `Project_Template.exe` (run this)

# Requirements
------------
The project has been tested on Windows 10 and Windows 11 operating systems. It specifically requires Visual Studio 2022 for proper execution. Additionally, ensure that the necessary dependencies are stored in the C:/ location. Lastly, make sure to have C++ installed on your system.

Visual Studio Version
---------------------

The folder contains:

- `Project_Template` (Do not rename)
	- `helper` (contains libraries)
	- `media` (contains all the textures and obj files)
	- `shader` (contains all the shader code)
	- `glad.c`
	- `main.cpp` (code for running the window)
	- `Project_Template.exe` (executable version of the project)
	- `Project_Template.slm` (Launch this for code inspection)
	- `Project_Template.vcxproj`
	- `Project_Template.vcxproj.filters`
	- `Project_Template.vcxproj.user`
	- `scenebasic_unifom.cpp` (main scene code done)
	- `scenebasic_uniform.h`
- `glad.c`

Notes: Runs on Windows 10 and Windows 11, used Visual Studio 2022.

# How it works
-------------

Essentially, the scenebasic_uniform.cpp portion of the code renders a scene and loads the object meshes, including three cabin houses, a campfire pit, and a skybox. Additionally, it sets up buffers for bloom functionality and loads texture files to be sent to the shader code. The basic_uniform.vert file calculates camera space values, such as normals and positions. Lastly, the basic_uniform.frag calculates light behavior through various calculations. The project currently incorporates two types of lighting: directional light and point light. After the lights are calculated, they are passed to the pass() functions, starting from pass1() to pass5(). During the first pass, the shaded colors are combined using the two different lighting techniques and are further enhanced by incorporating fog, which is calculated in the first pass. Subsequently, these values are passed to the remaining pass() functions, which determine how bloom is calculated and applied. 



### How Mouse and Keyboard Inputs were Captured
To capture mouse and keyboard inputs, the GLFW window was utilized. Since the GLFW window was already initialized in another file called scenerunner.h, the primary inputs were captured within scenerunner.h and then passed to scene.h. From there, the inputs were further forwarded to scenebasic_uniform.cpp, where the user's inputs were processed to enable scene manipulation. This processing creates the illusion of camera movement by moving the entire scene based on the user's inputs.

### How Fire/smoke works
As the fire and smoke particle system was a little bit more complicated it required to be placed in a seperate shader files called flat_frag.glsl and flat_vert.glsl which control the particle physics and rendering.

The transition from flame to smoke in the particle system is achieved by calculating the transparency of each particle based on its age. This process is handled in the flat_frag.glsl file, the fragment shader. As a particle ages, its transparency is increased, making it appear more like smoke over time. This creates a dynamic and realistic effect where particles appear as fire when they are first emitted and gradually transition to smoke as they age.

The fragment shader also handles other aspects of rendering, such as assigning colour to the particles and any additional effects that should be applied. It takes the outputs of the vertex shader, such as the particle position and transparency, and uses them to compute the final colour of each pixel that the particle covers on the screen.
Together, these two shaders work in concert to create a complex and visually striking fire and smoke particle system. The vertex shader manages the physical properties and behavior of the particles, while the fragment shader handles their appearance and the transition from fire to smoke.



## What grade I think that this coursework is worth
-----------------------------------------------
This project does manage to implement fire and smoke as well as having a nice looking scene and also having the ability to move around with mouse and keyboard is a pretty nice feature 


So overall, I think that my coursework is worth 54 marks.

YouTube video: [Link](https://youtu.be/dYi7a8LXG5c)
GitHub: [Link](https://github.com/G5G/Comp3015)
