#version 420

//ps input
in vec4 eye_coord;
in vec4 clip_coord;
in vec2 tex_coord;

//uniform variables
uniform sampler2D tex;

//ps output
out vec4 color;

void main(void) 
{
    color = texture(tex, tex_coord);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}
