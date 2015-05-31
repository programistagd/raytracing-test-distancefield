#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

void main()
{
    gl_Position.xyz = vertexPosition_modelspace;//*vec3(400.0,300.0,1.0);
    gl_Position.w = 1.0;
}
