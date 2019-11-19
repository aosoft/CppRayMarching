#include "pch.h"
#include "RayMarching.h"

using namespace glm;

//
//	Hologram Boxes
//	Created by kaneta in 2019-04-19
//	https://www.shadertoy.com/view/3tX3R4
//

#define PI 3.141592
#define TAU (PI*2.0)

float rand(vec2 n) {
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453f);
}

float noise(vec2 p) {
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u * u * (3.0f - 2.0f * u);

	float res = mix(
		mix(rand(ip), rand(ip + vec2(1.0, 0.0)), u.x),
		mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x), u.y);
	return res * res;
}

float fbm(vec2 p) {
	float r = 0.0;
	float amp = 1.0;
	float freq = 1.0;
	for (int i = 0; i < 3; i++) {
		r += amp * noise(freq * p);
		amp *= 0.5;
		freq *= 1.0 / 0.5;
	}
	return r;
}

inline mat2 rot(float th) { vec2 a = sin(vec2(1.5707963, 0) + th); return mat2(a.x, a.y, -a.y, a.x); }

float remap(float val, float im, float ix, float om, float ox)
{
	return clamp(om + (val - im) * (ox - om) / (ix - im), om, ox);
}

float cio(float t) {
	return t < 0.5f
		? 0.5f * (1.0f - sqrt(1.0f - 4.0f * t * t))
		: 0.5f * (sqrt((3.0f - 2.0f * t) * (2.0f * t - 1.0f)) + 1.0f);
}

float animHeight(vec2 p, float iTime)
{
	float s = 0., hs = 1.;
	float t = mod(iTime, 7.f);
	float tt = remap(t, 1., 2., 0., 1.);
	s = mix(0., .3, cio(tt));

	tt = remap(t, 3., 4., 0., 1.);
	p = p * rot(cio(tt) * 3.);

	tt = remap(t, 4., 5., 0., 1.);
	s = mix(s, 1.0f, cio(tt));

	tt = remap(t, 5., 6., 0., 1.);
	p = p * rot(-cio(tt) * 3.);

	tt = remap(t, 6.5, 7., 0., 1.);
	p = p * rot(cio(tt) * 1.);
	hs = mix(1., 0., cio(tt));

	float pls = (sin(t * TAU - PI * .5) * .5 + .5) * step(mod(t, 2.f), 1.f) * .2;
	return (fbm(p * s + t * .5f) + pls) * hs;
}

float sdBox(vec3 p, vec3 b)
{
	vec3 d = abs(p) - b;
	return length(max(d, 0.0f));
}

vec2 rep(vec2 p, vec2 c)
{
	return mod(p, c) - 0.5f * c;
}

float map(vec3 p, float iTime)
{
	float bd = length(p.xz()) - 5.0f;
	if (bd > 0.1) {
		return bd;
	}
	vec2 id = floor(p.xz() / 0.2f);
	float height = animHeight(id * 0.2f, iTime) * 0.5f;
	p.xz = rep(p.xz(), vec2(0.2f));
	p.y -= height;
	float box = sdBox(p, vec3(0.03, height, 0.03));
	return max(box, bd) * .5;
}

vec2 trace(vec3 p, vec3 ray, float mx, float iTime)
{
	float t = 0.0;
	vec3 pos;
	float dist;
	for (int i = 0; i < 128; i++) {
		pos = p + ray * t;
		dist = map(pos, iTime);
		if (dist < 0.002 || t > mx) {
			break;
		}
		t += dist;
	}
	return vec2(t, dist);
}

vec3 getColor(vec3 p, vec3 ray, float iTime)
{
	vec2 t = trace(p, ray, 100.0, iTime);
	vec3 pos = p + ray * t.x;
	if (t.x > 100.0) {
		return vec3(0.0);
	}
	return max(vec3(0.2, 0.5, 0.8) * 7.0f * pow(pos.y, 4.0f) * smoothstep(0.0f, -1.0f, length(pos.xz()) - 5.0f), vec3(0.0));
}

mat3 camera(vec3 ro, vec3 ta, float cr)
{
	vec3 cw = normalize(ta - ro);
	vec3 cp = vec3(sin(cr), cos(cr), 0.);
	vec3 cu = normalize(cross(cw, cp));
	vec3 cv = normalize(cross(cu, cw));
	return mat3(cu, cv, cw);
}

float luminance(vec3 col)
{
	return dot(vec3(0.3, 0.6, 0.1), col);
}

static vec3 acesFilm(const vec3 x) {
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}


void HologramBoxes(glm::vec4& fragColor, const glm::vec4& fragCoord, float iTime, const glm::vec2 iResolution)
{
	vec2 p = (fragCoord.xy * 2.0f - iResolution.xy) / min(iResolution.x, iResolution.y);

	float t = iTime * 0.1;
	vec3 ro = vec3(cos(t) * 10.0, 5.5, sin(t) * 10.0);
	vec3 ta = vec3(0.0, 1.0, 0.0);
	mat3 c = camera(ro, ta, 0.0);
	vec3 ray = c * normalize(vec3(p, 2.5));
	vec3 col = getColor(ro, ray, iTime);

	vec3 lp = vec3(0.0, 6.0, 0.0), rd = ray;

	float s = 7.5, vol = 0.0;
	for (int i = 0; i < 60; i++) {
		vec3 pos = ro + rd * s;

		vec3 v = -normalize(lp - pos);

		float tt = -(lp.y - 2.) / v.y;

		vec3 ppos = lp + v * tt;
		vol += pow(animHeight(ppos.xz(), iTime), 3.0) * 0.05 * smoothstep(0.0f, 1.5f, pos.y) * smoothstep(-1.0f, -4.0f, length(ppos.xz()) - 5.0f);

		s += 0.1;
	}

	col += 1.6f * vec3(0.3 * vol, 0.5 * vol, vol);

	col = acesFilm(col * 0.5f);
	col = pow(col, vec3(1.0 / 2.2));

	fragColor = vec4(col, 1.0);
}