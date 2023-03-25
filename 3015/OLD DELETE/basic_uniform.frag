#version 460

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Vec;
layout(binding=1) uniform sampler2D Tex1;
layout(binding=2) uniform sampler2D Tex2;
layout(binding=3) uniform samplerCube sky;
layout(binding=0) uniform sampler2D HdrTex;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec3 HdrColor;

uniform int Pass;
uniform float AveLum;



uniform mat3 rgb2xyz = mat3(
0.4124564, 0.2126729, 0.0193339,
0.3575761, 0.7151522, 0.1191920,
0.1804375, 0.0721750, 0.9503041);

uniform mat3 xyz2rgb = mat3(
3.240452, -0.969266, 0.0556434,
-1.5371385, 1.8760108, -0.2040259,
-0.4985314, 0.0415560, 1.0572252);

uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform bool DoToneMap = true;

uniform struct SpotlightInfo
{
	vec4 Position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 Direction;
	float Exponent;
	float Cutoff;
}Spot;

uniform struct lightinfo{
vec4 Position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
}Lights[2];

const int level = 5;
const float scaleFactor=1.0/level;


uniform struct MaterialInfo
{
	vec3 Ka;//ambient
	vec3 Kd;//diffuse
	vec3 Ks;//spec
	float Shininess;
}Material;

uniform struct FogInfo
{
    float MaxDist;
	float MinDist;
	vec3 Colour;
}Fog;

/*vec3 blinnphongSpot(vec3 n,vec4 pos)
{
	vec3 ambient=Spot.La*Material.Ka;
	vec3 diffuse=vec3(0);
	vec3 spec=vec3(0);
	
	
	vec3 s=normalize(vec3(Spot.Position-(pos*Spot.Position.w)));
	
	float cosAng=dot(-s,normalize(Spot.Direction));
	float angle=acos(cosAng);
	float spotScale=0.0f;
	if(angle<Spot.Cutoff)
	{
		spotScale=pow(cosAng,Spot.Exponent);
		float sDotN=max(dot(s,n),0.0);
		diffuse = Spot.Ld*Material.Kd*sDotN;
		spec = vec3(0.0);
		if(sDotN>0.0)
		{
			vec3 v=normalize(-pos.xyz);
			vec3 h=normalize(v+s);
			spec=Spot.Ls*Material.Ks*pow(max(dot(h,n),0.0),Material.Shininess);
		}
	}
	return ambient+diffuse+spec;
}*/
const vec3 lum = vec3(0.2126,0.7152,0.0722);

vec3 blinnPhong(vec3 n,vec4 pos, int i)
{
	vec4 HouseColour=texture(Tex1,TexCoord);
	vec4 GroundColour=texture(Tex2,TexCoord);

	vec3 texColour = mix(HouseColour.rgb,GroundColour.rgb,GroundColour.a);

	vec3 ambient=Lights[i].La*Material.Ka*texColour;
	vec3 s = normalize(Lights[i].Position.xyz - pos.w);

	
	float sDotN = max(dot(s,n),0.0);



	vec3 diffuse = Lights[i].Ld*Material.Kd*sDotN;

	vec3 spec = vec3(0.0);
	if(sDotN>0.0)
	{
		vec3 v = normalize(-pos.xyz);
		vec3 h = normalize(v+s);
		spec = Lights[i].Ls*Material.Ks*pow(max(dot(h,n),0.0),Material.Shininess);
	}
	return ambient+diffuse+spec;
}

float luminance(vec3 colour)
{
	return dot(lum,colour);
}

void pass1()
{
	vec3 n = normalize(Normal);

	HdrColor = vec3(0.0);
	vec3 shadeColour = vec3(0.0);
	float dist = abs(Position.z);
	float fogFactor = (Fog.MaxDist-dist) / (Fog.MaxDist - Fog.MinDist);
	fogFactor = clamp(fogFactor,0.0,1.0);

	for(int i = 0; i < 2; i++)
	{
		shadeColour = blinnPhong(n,Position,i);
		HdrColor += mix(Fog.Colour,shadeColour,fogFactor);
		

	}


}

void pass2()
{
	vec4 colour = texture( HdrTex,TexCoord );
	
	vec3 xyzCol = rgb2xyz * vec3(colour);
	float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
	vec3 xyYCol = vec3(xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y); 

	float L = (Exposure * xyYCol.z) / AveLum;
	L = (L * (1 + L / (White * White))) / (1+L);
	
	xyzCol.x = (L * xyYCol.x) / (xyYCol.y);

	xyzCol.y = L;

	xyzCol.z = (L * (1 - xyYCol.x - xyYCol.y)) / (xyYCol.y);

	if (DoToneMap)
	{
		FragColour = vec4(xyz2rgb * xyzCol, 1.0);
	}
	else
	{

		FragColour = colour;
	}

}


void main() {


	if(Pass == 1)
    {
		pass1();
	}
	else if(Pass == 2)
	{
		pass2();
	}

}
