#version 420

//ps input
in vec4 eye_coord;
in vec4 clip_coord;
in vec2 tex_coord;
noperspective in vec2 tex_coord_NoCorrection;
flat in vec2 tex_coord_Flat;

//uniform variables
uniform sampler2D tex;
uniform vec2 vertexWindowCoords[3];
uniform vec4 vertexClipCoords[3];
uniform vec2 vertexTexCoords[3];
uniform int interpolateMode;

//ps output
out vec4 color;

float calcLength(vec2 start, vec2 end)
{
    return sqrt(pow(start.x-end.x, 2) + pow(start.y-end.y,2));
}

float calcSquare(vec2 A, vec2 B, vec2 C)
{
    float lenA, lenB, lenC, p;
    lenA = calcLength(B, C);
    lenB = calcLength(A, C);
    lenC = calcLength(A, B);
    p = (lenA+lenB+lenC)/2.0;
    return sqrt(p*(p-lenA)*(p-lenB)*(p-lenC));
}

void main(void) 
{
    if (interpolateMode == 2)
    {
        color = texture(tex, tex_coord_NoCorrection);
    }
    else if (interpolateMode == 3 || interpolateMode == 4)
    {
        float a,b,c;
        float squareA,squareB,squareC,square;

        square = calcSquare(vertexWindowCoords[0], vertexWindowCoords[1], vertexWindowCoords[2]);
        squareA = calcSquare(gl_FragCoord.xy, vertexWindowCoords[1], vertexWindowCoords[2]);
        squareB = calcSquare(gl_FragCoord.xy, vertexWindowCoords[0], vertexWindowCoords[2]);
        squareC = calcSquare(gl_FragCoord.xy, vertexWindowCoords[0], vertexWindowCoords[1]);

        a = squareA/square;
        b = squareB/square;
        c = squareC/square;
        
        if (interpolateMode == 3)
        {
            vec2 texCoordCorrection;
            texCoordCorrection = a * vertexTexCoords[0] / vertexClipCoords[0].w + b * vertexTexCoords[1] / vertexClipCoords[1].w + c * vertexTexCoords[2] / vertexClipCoords[2].w;
            texCoordCorrection = texCoordCorrection / (a / vertexClipCoords[0].w + b / vertexClipCoords[1].w + c / vertexClipCoords[2].w);
            color = texture(tex, texCoordCorrection);
        }
        else
        {
            vec2 texCoordNoCorrection;
            texCoordNoCorrection = a * vertexTexCoords[0] + b * vertexTexCoords[1] + c * vertexTexCoords[2];
            color = texture(tex, tex_coord_NoCorrection);
        }
    }
    else
    {
        color = texture(tex, tex_coord);
    }
}
