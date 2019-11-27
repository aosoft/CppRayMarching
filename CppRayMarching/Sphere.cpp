#include "pch.h"
#include "RayMarching.h"

using namespace glm;

static float distanceFunc(vec3 pos)
{
	const float sphereSize = 1.0;
	return length(pos) - sphereSize;
}

static vec3 normalFunc(vec3 pos)
{
	const float delta = 0.001;
	return normalize(vec3(
		distanceFunc(pos - vec3(delta, 0.0, 0.0)) - distanceFunc(pos),
		distanceFunc(pos - vec3(0.0, delta, 0.0)) - distanceFunc(pos),
		distanceFunc(pos - vec3(0.0, 0.0, delta)) - distanceFunc(pos)));
}

void Sphere(vec4& gl_FragColor, const vec4& gl_FragCoord, float time, const vec2 resolution)
{
	vec2 p = (gl_FragCoord.xy() * 2.0f - resolution) / min(resolution.x, resolution.y);

	vec3 ray = normalize(vec3(p, -1.0));
	vec3 rPos = vec3(0.0, 0.0, 2.0);
	for (int i = 0; i < 16; i++)
	{
		float distance = distanceFunc(rPos);
		if (abs(distance) < 0.001)
		{
			vec3 norm = normalFunc(rPos);
			vec3 light = normalize(vec3(sin(time), 0.0, -1.0 * cos(time)));
			float d = clamp(dot(norm, light), 0.05f, 1.0f);
			gl_FragColor = vec4(vec3(0.0, 0.5, 1.0) * d, 1.0);
			return;
		}
		rPos += ray * distance;
	}
	gl_FragColor = vec4(vec3(0.0), 1.0);
}