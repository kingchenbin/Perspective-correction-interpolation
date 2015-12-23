#version 420

//vertex attribs
in vec4 model_postion;
in vec2 tex_coord_in;

//uniform variables
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

//vs output
out vec4 eye_coord;
out vec4 clip_coord;
out vec2 tex_coord;

void main(void)
{
    tex_coord = tex_coord_in;
    eye_coord = modelViewMatrix * model_postion;
    clip_coord = projectionMatrix * eye_coord;
    gl_Position = clip_coord;
}
