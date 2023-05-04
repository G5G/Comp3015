#version 460


uniform int Pass;
const float PI = 3.14159265359;
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexAge;

out float Age;
out float Transp;
out vec3 Velocity;
out vec3 Position;
out vec2 TexCoord;

uniform mat4 MV;
uniform mat4 Proj;
uniform sampler1D RandomTex;

uniform float Time;
uniform float DeltaT;
uniform vec3 Accel;
uniform float ParticleLifetime;
uniform vec3 Emitter = vec3(0);
uniform mat3 EmitterBasis;
uniform float ParticleSize;
uniform float MinParticleSize=1.1;
uniform float MaxParticleSize = 2.5;


const vec3 offsets[] = vec3[](
vec3(-0.5,-0.5,0),vec3(0.5,-0.5,0),vec3(0.5,0.5,0),
vec3(-0.5,-0.5,0),vec3(0.5,0.5,0),vec3(-0.5,0.5,0));

const vec2 texCoords[] = vec2[](
vec2(0,0),vec2(1,0),vec2(1,1),
vec2(0,0),vec2(1,1),vec2(0,1));

vec3 randomInitialVelocity()
{
    float velocity = mix(0.1,0.5,texelFetch(RandomTex,2*gl_VertexID,0).r);
    return EmitterBasis * vec3(0,velocity,0);

}
vec3 randomInitialPosition(){
    float offset = mix(-2.0,2.0,texelFetch(RandomTex,2*gl_VertexID + 1,0).r);
    return Emitter + vec3(offset,0,0);
}

void update()
{
    Age = VertexAge+DeltaT;

    if (VertexAge < 0 || VertexAge > ParticleLifetime)
    {
        Position = randomInitialPosition();
        Velocity = randomInitialVelocity();

        if (VertexAge > ParticleLifetime) 
        {
            Age = (VertexAge - ParticleLifetime) + DeltaT;
        }

    }
    else
    {
        Position = VertexPosition + VertexVelocity * DeltaT;
        Velocity = VertexVelocity + Accel * DeltaT;
    }



}

void render()
{
    Transp = 0.0;
    vec3 posCam = vec3(0.0);
    if (VertexAge > 0.0)
    {
        float agePct = VertexAge/ParticleLifetime;
        Transp = clamp(1.0-agePct,0,1);

        posCam = (MV * vec4(VertexPosition, 1)).xyz + (offsets[gl_VertexID] * mix(MinParticleSize,MaxParticleSize,agePct));
    }
    TexCoord = texCoords[gl_VertexID];
    gl_Position = Proj * vec4(posCam, 1);
}

void main(){
	if(Pass == 1)
    {
	    update();
    }
	else
    {
		render();

    }
	
}