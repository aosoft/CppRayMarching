#include "pch.h"
#include "RayMarching.h"

using namespace glm;

//
//	Kaleidoscope Tunnel
//	Created by kaneta in 2019-02-10
//	https://www.shadertoy.com/view/WsSGWG
//

#define PI 3.141592654

inline mat2 rot(float x)
{
	return mat2(cos(x), sin(x), -sin(x), cos(x));
}

vec2 foldRotate(vec2& p, float s) {
	float a = PI / s - atan(p.x, p.y);
	float n = PI * 2. / s;
	a = floor(a / n) * n;
	p = p * rot(a);
	return p;
}

float sdRect(vec2 p, vec2 b)
{
	vec2 d = abs(p) - b;
	return min(max(d.x, d.y), 0.0f) + length(max(d, 0.0f));
}

// TheGrid by dila
// https://www.shadertoy.com/view/llcXWr
float tex(vec2 p, float z)
{
	p = foldRotate(p, 8.0);
	vec2 q = (fract(p / 10.0f) - 0.5f) * 10.0f;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 2; j++) {
			q = abs(q) - .25f;
			q = q* rot(PI * .25);
		}
		q = abs(q) - vec2(1.0, 1.5);
		q = q * rot(PI * .25 * z);
		q = foldRotate(q, 3.0);
	}
	float d = sdRect(q, vec2(1., 1.));
	float f = 1.0 / (1.0 + abs(d));
	return smoothstep(.9f, 1.f, f);
}

// The Drive Home by BigWings
// https://www.shadertoy.com/view/MdfBRX
float Bokeh(vec2 p, vec2 sp, float size, float mi, float blur)
{
	float d = length(p - sp);
	float c = smoothstep(size, size * (1.f - blur), d);
	c *= mix(mi, 1.f, smoothstep(size * .8f, size, d));
	return c;
}

vec2 hash(vec2 p) {
	p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
	return fract(sin(p) * 43758.5453f) * 2.0f - 1.0f;
}

float dirt(vec2 uv, float n)
{
	vec2 p = fract(uv * n);
	vec2 st = (floor(uv * n) + 0.5f) / n;
	vec2 rnd = hash(st);
	return Bokeh(p, vec2(0.5, 0.5) + vec2(0.2) * rnd, 0.05, abs(rnd.y * 0.4) + 0.3, 0.25 + rnd.x * rnd.y * 0.2);
}

float sm(float start, float end, float t, float smo)
{
	return smoothstep(start, start + smo, t) - smoothstep(end - smo, end, t);
}

void KaleidoscopeTunnel(glm::vec4& fragColor, const glm::vec4& fragCoord, float iTime, const glm::vec2 iResolution)
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	uv = uv * 2.0f - 1.0f;
	uv.x *= iResolution.x / iResolution.y;
	uv *= 2.0;

	vec3 col = vec3(0.0);
#define N 6
#define NN float(N)
#define INTERVAL 3.0f
#define INTENSITY vec3((NN * INTERVAL - t) / (NN * INTERVAL))


	float time = iTime;
	for (int i = 0; i < N; i++) {
		float t;
		float ii = float(N - i);
		t = ii * INTERVAL - mod(time - INTERVAL * 0.75f, INTERVAL);
		col = mix(col, INTENSITY, dirt(mod(uv * max(0.0f, t) * 0.1f + vec2(.2f, -.2f) * time, 1.2f), 3.5f));

		t = ii * INTERVAL - mod(time + INTERVAL * 0.5f, INTERVAL);
		col = mix(col, INTENSITY * vec3(0.7, 0.8, 1.0) * 1.3f, tex(uv * max(0.0f, t), 4.45f));

		t = ii * INTERVAL - mod(time - INTERVAL * 0.25f, INTERVAL);
		col = mix(col, INTENSITY * vec3(1.), dirt(mod(uv * max(0.0f, t) * 0.1f + vec2(-.2, -.2) * time, 1.2f), 3.5f));

		t = ii * INTERVAL - mod(time, INTERVAL);
		float r = length(uv * 2.0f * max(0.0f, t));
		float rr = sm(-24.0, -0.0, (r - mod(time * 30.0, 90.0)), 10.0);
		col = mix(col, mix(INTENSITY * vec3(1.), INTENSITY * vec3(0.7, 0.5, 1.0) * 3.0f, rr), tex(uv * 2.0f * max(0.0f, t), 0.27 + (2.0 * rr)));

	}

	fragColor = vec4(col, 1.0);
}
