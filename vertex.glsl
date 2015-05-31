#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform vec3 eyePos;
uniform mat4 matrix;
out vec3 outray;

void main()
{
    // transform the vertex position
    vec4 pos = vec4(vertexPosition_modelspace,1.0);
    vec4 ray = pos*matrix;
    ray/=ray.w;
    ray -= vec4(eyePos,1.0);
    outray = normalize(vertexPosition_modelspace);//normalize(ray.xyz);
    gl_Position.xyz = vertexPosition_modelspace;//*vec3(400.0,300.0,1.0);
    gl_Position.w = 1.0;
}
