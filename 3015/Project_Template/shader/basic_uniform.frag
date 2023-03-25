#version 460

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Vec;
layout(binding=3) uniform sampler2D Tex1;
layout(binding=4) uniform sampler2D Tex2;
layout(binding=5) uniform samplerCube sky;

layout(binding=0) uniform sampler2D HdrTex;
layout (binding=1) uniform sampler2D BlurTex1;
layout (binding=2) uniform sampler2D BlurTex2;



layout (location = 0) out vec4 FragColour;
//layout (location = 1) out vec3 HdrColor;

uniform float LumThresh;
uniform int Pass;
uniform float AveLum;


uniform mat3 rgb2xyz = mat3(
0.4124564, 0.2126729, 0.0193339,
0.3575761, 0.7151522, 0.1191920,
0.1804375, 0.0721750, 0.9503041 );
uniform mat3 xyz2rgb = mat3(
3.2404542, -0.9692660, 0.0556434,
-1.5371385, 1.8760108, -0.2040259,
-0.4985314, 0.0415560, 1.0572252 );

uniform float Exposure = 0.35;
uniform float White = 0.928;
//uniform bool DoToneMap = true;
uniform float PixOffset[10] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0);
uniform float Weight[10];
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
}Lights[3];

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

const vec3 lum = vec3(0.2126,0.7152,0.0722);

vec3 blinnPhong(vec3 n,vec4 pos, int i)
{
	vec4 HouseColour=texture(Tex1,TexCoord);
	vec4 GroundColour=texture(Tex2,TexCoord);

	vec3 texColour = mix(HouseColour.rgb,GroundColour.rgb,GroundColour.a);

	vec3 ambient=Lights[i].La*Material.Ka* texColour;
	//vec3 ambient=Lights[i].La*Material.Ka;
	vec3 s = normalize(Lights[i].Position.xyz - pos.xyz);

	
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

float luminance( vec3 color )
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}



/*void pass1()
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
}*/
vec4 pass1()
{
	vec3 n = normalize(Normal);
	vec3 color = vec3(0.0);
	vec3 shadeColour = vec3(0.0);
	float dist = abs(Position.z);
	float fogFactor = (Fog.MaxDist-dist) / (Fog.MaxDist - Fog.MinDist);
	fogFactor = clamp(fogFactor,0.0,1.0);
	for( int i = 0; i < 3; i++)
	{
		shadeColour = blinnPhong(n,Position,i);
		color += mix(Fog.Colour,shadeColour,fogFactor);
	}
	return vec4(color,1);
}
/*vec4 pass1()
{
	vec3 n = normalize(Normal);
	vec3 color = vec3(0.0);
	for( int i = 0; i < 3; i++)
		color += blinnPhong(n,Position, i);
	return vec4(color,1);
}*/

vec4 pass2() {
	vec4 val = texture(HdrTex, TexCoord);
	if(luminance(val.rgb) > LumThresh )
	{
		return val;
	}
	else
	{
		return vec4(0.0);
	}
}
vec4 pass3() {
	float dy = 1.0 / (textureSize(BlurTex1,0)).y;
	vec4 sum = texture(BlurTex1, TexCoord) * Weight[0];
	for( int i = 1; i < 10; i++ )
	{
		sum += texture( BlurTex1, TexCoord + vec2(0.0,PixOffset[i]) * dy ) * Weight[i];
		sum += texture( BlurTex1, TexCoord - vec2(0.0,PixOffset[i]) * dy ) * Weight[i];
	}
	return sum;
}
vec4 pass4() {
	float dx = 1.0 / (textureSize(BlurTex2,0)).x;
	vec4 sum = texture(BlurTex2, TexCoord) * Weight[0];
	for( int i = 1; i < 10; i++ )
	{
		sum += texture( BlurTex2, TexCoord + vec2(PixOffset[i],0.0) * dx ) * Weight[i];
		sum += texture( BlurTex2, TexCoord - vec2(PixOffset[i],0.0) * dx ) * Weight[i];
	}
	return sum;
}
vec4 pass5() {
	/////////////// Tone mapping ///////////////
	// Retrieve high-res color from texture
	vec4 color = texture( HdrTex, TexCoord );
	// Convert to XYZ
	vec3 xyzCol = rgb2xyz * vec3(color);
	// Convert to xyY
	float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
	vec3 xyYCol = vec3( xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);
	// Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
	float L = (Exposure * xyYCol.z) / AveLum;
	L = (L * ( 1 + L / (White * White) )) / ( 1 + L );
	// Using the new luminance, convert back to XYZ
	xyzCol.x = (L * xyYCol.x) / (xyYCol.y);
	xyzCol.y = L;
	xyzCol.z = (L * (1 - xyYCol.x - xyYCol.y))/xyYCol.y;
	// Convert back to RGB
	vec4 toneMapColor = vec4( xyz2rgb * xyzCol, 1.0);
	///////////// Combine with blurred texture /////////////
	// We want linear filtering on this texture access so that
	// we get additional blurring.
	vec4 blurTex = texture(BlurTex1, TexCoord);

	return toneMapColor + blurTex;
}
void main()
{
	if(Pass == 1)
		FragColour = pass1();
	else if(Pass == 2)
		FragColour = pass2();
	else if(Pass == 3)
		FragColour = pass3();
	else if(Pass == 4)
		FragColour = pass4();
	else if(Pass == 5)
		FragColour = pass5();
}
