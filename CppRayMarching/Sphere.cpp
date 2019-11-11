#include "pch.h"
#include "RayMarching.h"

using namespace glm;

float distanceFunc(vec3 pos)
{
	const float sphereSize = 1.0;
	return length(pos) - sphereSize;
}

vec3 normalFunc(vec3 pos)
{
	float delta = 0.001;
	return normalize(vec3(
		distanceFunc(pos - vec3(delta, 0.0, 0.0)) - distanceFunc(pos),
		distanceFunc(pos - vec3(0.0, delta, 0.0)) - distanceFunc(pos),
		distanceFunc(pos - vec3(0.0, 0.0, delta)) - distanceFunc(pos)));
}



void Sphere(vec4& gl_FragColor, const vec4& gl_FragCoord, float time, const vec2 resolution)
{
	vec2 p = (gl_FragCoord.xy() * vec2(2.0, 2.0) - resolution) / min(resolution.x, resolution.y);

	vec3 cPos = vec3(0.0, 0.0, 2.0);
	vec3 cDir = vec3(0.0, 0.0, -1.0);
	vec3 cUp = vec3(0.0, 1.0, 0.0);
	vec3 cSide = cross(cDir, cUp);

	vec3 ray = normalize(cSide * p.x + cUp * p.y + cDir);

	float distance = 0.0;
	float rLen = 0.0;
	vec3  rPos = cPos;
	for (int i = 0; i < 16; i++)
	{
		distance = distanceFunc(rPos);
		if (abs(distance) < 0.001)
		{
			vec3 norm = normalFunc(rPos);
			vec3 light = normalize(vec3(sin(time), 0.0, -1.0 * cos(time)));
			float d = dot(norm, light);
			gl_FragColor = vec4(vec3(1.0 * d), 1.0);
			return;
		}

		rLen += distance;
		rPos = cPos + ray * rLen;
	}
	gl_FragColor = vec4(vec3(0.0), 1.0);
}