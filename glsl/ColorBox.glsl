precision mediump float;
uniform float time;
uniform vec2 resolution;

mat3 rotationMatrixY(float angle){
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
          c, 0.0,   s,
        0.0, 1.0, 0.0,
         -s, 0.0,   c);
}

mat3 rotationMatrixZ(float angle){
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
          c,  -s, 0.0,
          s,   c, 0.0,
        0.0, 0.0, 1.0);
}

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float distanceFunc(vec3 pos)
{
    pos = mod(pos, 4.0) - 2.0;
    return length(max(abs(pos) - vec3(1.0, 1.0, 1.0), 0.0));
}

vec3 normalFunc(vec3 pos)
{
    const float delta = 0.001;
    return normalize(vec3(
        distanceFunc(pos - vec3(delta, 0.0, 0.0)) - distanceFunc(pos),
        distanceFunc(pos - vec3(0.0, delta, 0.0)) - distanceFunc(pos),
        distanceFunc(pos - vec3(0.0, 0.0, delta)) - distanceFunc(pos)));
}

void main(void)
{
    vec2 p = (gl_FragCoord.xy * 2.0 - resolution) / min(resolution.x, resolution.y);

    vec3 cPos = vec3(0.0,  0.0,  2.0 - time);
    vec3 cDir = vec3(0.0,  0.0, -1.0);
    vec3 cUp  = vec3(0.0,  1.0,  0.0);
    vec3 cSide = cross(cDir, cUp);

    vec3 ray = normalize(vec3(p, -1.0));
    vec3 rPos = cPos;
    mat3 mat = rotationMatrixY(time * 0.01) * rotationMatrixZ(time * 0.1);
    for (int i = 0; i < 128; i++)
    {
        vec3 rPos2 = mat * rPos;
        float distance = distanceFunc(rPos2);
        if (abs(distance) < 0.001)
        {
            vec3 norm = normalFunc(rPos2);
            vec3 light = normalize(mat * vec3(0.5, 0.0, -1.0));
            float d = clamp(dot(norm, light), 0.2, 1.0);
            gl_FragColor = vec4(hsv2rgb(vec3(ceil(rPos2.z / 4.0) * 0.1, 1.0, 1.0)) * d, 1.0);
            return;
        }
        rPos += ray * distance;
    }
    gl_FragColor = vec4(vec3(0.0), 1.0);
}