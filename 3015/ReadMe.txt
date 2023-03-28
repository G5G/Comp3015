ExecutableVersion--------------------------------------
The ExecutableVersion folder contains the following:
-Project_Template (Do not rename)
	-media (contains all the textures and obj files)
	-shader (contains all the shader code)
	-Project_Template.exe (run this)

Notes: I have tested this on my Desktop(Windows 10) with cuda drivers and visual studio 2022 installed
This has also been tested on a laptop (Windows 10) without cuda drivers or visual studio, it even ran without being extracted out of the rar file
--------------------------------------------------------

Visual studio Version-----------------------------------
Folder contains:

-Project_Template (Do not rename)
	-helper (contains libraries)
	-media (contains all the textures and obj files)
	-shader (contains all the shader code)
	-glad.c 
	-main.cpp (code for running the window)
	-Project_Template.exe (executable version of the project)
	-Project_Template.slm	(Launch this for code inspection)
	-Project_Template.vcxproj	
	-Project_Template.vcxproj.filters
	-Project_Template.vcxproj.user
	-scenebasic_unifom.cpp (the juicy part of the code :D  sets up all of the scene and talks to the shader to render a nice looking scene )
	-scenebasic_uniform.h 
-glad.c

Notes: I have used windows 10, visual studio 2022 nothing too fancy
---------------------------------------------------------

How it works:

Essentially the scenebasic_uniform.cpp part of the code renders a scene and loads in the object meshes which are: three cabin houses, a campfire pit and a skybox.
additionally it also sets up buffers for bloom to work and it loads in the texture files so it can be sent off to the the shader part of the code.
The basic_uniform.vert file gets the camera space values which are the normals and positions which it calculates. 
Lastly the basic_uniform.frag calculates how light should behave by applying some calculations, there is currently two different light types, in the project,
the directional light and the point light. Once these lights get calculates they get passed to the pass() functions starting from pass1() all the way to pass5()
and during the first pass, the shade colours are combined results of the two different light techniques and they get added with the fog(which is calculated in the 1st pass)
and later on passed on to the rest of the pass() functions which they define how the bloom gets calculated and applied.


  
What grade I think that this coursework is worth:

by checking the Rubrics, my project does have a moderately complex scene, it has two seperate custom objects that weren't provided which is the house and the 
campfire. It also possesed the plane and the skybox,
it has two basic lighting techniques the directional light and the point light as well as having a post processing bloom effect.
and it has two seperate textures, one being the wood texture and the other being the vines, unfortunatly the skybox texture would not work
as whenever it was added it would draw the skybox texture all over the house and the plane.

So overall I think that my coursework is worth to be worth 60 marks.

 



Youtube video: https://youtu.be/ovLimjDYTHc
github:https://github.com/G5G/Comp3015   -Gave access to view to this email: ji-jian.chin@plymouth.ac.uk
