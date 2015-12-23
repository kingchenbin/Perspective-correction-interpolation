#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>

#pragma comment(lib,"glew32s.lib")

//Shader类型
typedef enum {
    EVertexShader,
    EFragmentShader,
} EShaderType;

//窗口句柄
static GLint window;

GLuint prog = 0;                        //程序对象
GLuint progNoPersectiveCorrect  = 0;    //程序对象

//InterpolateMode
//1:no qualifier(perspective)
//2:noPerspective
//3:simulate perspective
//4:simulate noPerspective
GLuint InterpolateMode = 1;

static float vertexData[] =
{
    -50.0, -50.0, - 50.0, 1.0, 0.0, 0.0,
     50.0, -50.0, - 50.0, 1.0, 1.0, 0.0,
     50.0,  50.0, -150.0, 1.0, 1.0, 1.0,

     50.0,  50.0, -150.0, 1.0, 1.0, 1.0,
    -50.0,  50.0, -150.0, 1.0, 0.0, 1.0,
    -50.0, -50.0, - 50.0, 1.0, 0.0, 0.0
};

static float modelViewMat[] =
{
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
};

//void Frustum( -100.0, 100.0, -100.0, 100.0, 50.0, 150.0);
static float projectionMat[] =
{
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, -2.0, -150.0,
    0.0, 0.0, -1.0, 0.0
};

static float vertexWindowCoords[] =
{
    200.5, 200.5,
    600.5, 200.5,
    466.5, 466.5
};

static float vertexClipCoords[] =
{
    -25.0, -25.0, -50.0,  50.0,
     25.0, -25.0, -50.0,  50.0,
     25.0,  25.0, 150.0, 150.0
};

static float vertexTexCoords[] =
{
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0
};

static float vertexWindowCoords1[] =
{
    466.5, 466.5,
    333.5, 466.5,
    200.5, 200.5
};

static float vertexClipCoords1[] =
{
     25.0,  25.0, 150.0, 150.0,
    -25.0,  25.0, 150.0, 150.0,
     25.0,  25.0, -50.0,  50.0
};

static float vertexTexCoords1[] =
{
    1.0, 1.0,
    0.0, 1.0,
    0.0, 0.0
};

//用来检查OpenGL版本，需要GLSL 2.0支持
void getGlVersion( int *major, int *minor )
{
    const char* verstr = (const char*)glGetString( GL_VERSION );
    if( (verstr == NULL) || (sscanf( verstr, "%d.%d", major, minor ) != 2) )
    {
        *major = *minor = 0;
        fprintf( stderr, "Invalid GL_VERSION format!!!\n" );
    }
}

static void display(void)
{
    GLuint loc;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up attrib values
    loc = glGetAttribLocation(progNoPersectiveCorrect, "model_postion");
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 24, (GLvoid *)0);
    glEnableVertexAttribArray(loc);
    loc = glGetAttribLocation(progNoPersectiveCorrect, "tex_coord_in");
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 24, (GLvoid *)16);
    glEnableVertexAttribArray(loc);

    // Set up initial uniform values
    glUniformMatrix4fv(glGetUniformLocation(progNoPersectiveCorrect, "modelViewMatrix"), 1, GL_TRUE, modelViewMat);
    glUniformMatrix4fv(glGetUniformLocation(progNoPersectiveCorrect, "projectionMatrix"), 1, GL_TRUE, projectionMat);
    glUniform1i(glGetUniformLocation(progNoPersectiveCorrect, "tex"), 0);
    glUniform2fv(glGetUniformLocation(progNoPersectiveCorrect, "vertexWindowCoords"), 3, vertexWindowCoords);
    glUniform4fv(glGetUniformLocation(progNoPersectiveCorrect, "vertexClipCoords"), 3, vertexClipCoords);
    glUniform2fv(glGetUniformLocation(progNoPersectiveCorrect, "vertexTexCoords"), 3, vertexTexCoords);
    glUniform1i(glGetUniformLocation(progNoPersectiveCorrect, "interpolateMode"), (int)InterpolateMode);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Set up attrib values
    loc = glGetAttribLocation(progNoPersectiveCorrect, "model_postion");
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 24, (GLvoid *)72);
    glEnableVertexAttribArray(loc);
    loc = glGetAttribLocation(progNoPersectiveCorrect, "tex_coord_in");
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 24, (GLvoid *)88);
    glEnableVertexAttribArray(loc);

    // Set up initial uniform values
    glUniform2fv(glGetUniformLocation(progNoPersectiveCorrect, "vertexWindowCoords"), 3, vertexWindowCoords1);
    glUniform4fv(glGetUniformLocation(progNoPersectiveCorrect, "vertexClipCoords"), 3, vertexClipCoords1);
    glUniform2fv(glGetUniformLocation(progNoPersectiveCorrect, "vertexTexCoords"), 3, vertexTexCoords1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glFlush();
    glutSwapBuffers();
}

static void reshape(int wid, int ht)
{
    return;
}


//Shader related functions
static int shaderSize(char *fileName, EShaderType shaderType)
{
    //返回顶点着色器或者片段着色器的大小
    char name[100];
    strcpy(name, fileName);

    switch (shaderType)
    {
    case EVertexShader:
        strcat(name, ".vert");
        break;
    case EFragmentShader:
        strcat(name, ".frag");
        break;
    default:
        printf("ERROR: unknown shader file type\n");
        exit(1);
        break;
    }

    int count = -1;
    // Open the file, seek to the end to find its length
    int fd = _open(name, _O_RDONLY);
    if (fd != -1)
    {
        count = _lseek(fd, 0, SEEK_END) + 1;
        _close(fd);
    }
    return count;
}


static int readShader(char *fileName, EShaderType shaderType, char *shaderText, int size)
{
    //
    // Reads a shader from the supplied file and returns the shader in the
    // arrays passed in. Returns 1 if successful, 0 if an error occurred.
    // The parameter size is an upper limit of the amount of bytes to read.
    // It is ok for it to be too big.
    //
    FILE *fh;
    char name[100];
    int count;

    strcpy(name, fileName);

    switch (shaderType) 
    {
    case EVertexShader:
        strcat(name, ".vert");
        break;
    case EFragmentShader:
        strcat(name, ".frag");
        break;
    default:
        printf("ERROR: unknown shader file type\n");
        exit(1);
        break;
    }

    // Open the file
    fh = fopen(name, "r");
    if (!fh)
        return -1;

    // Get the shader from a file.
    fseek(fh, 0, SEEK_SET);
    count = (int) fread(shaderText, 1, size, fh);
    shaderText[count] = '\0';

    if (ferror(fh))
        count = 0;

    fclose(fh);
    return count;
}


/*public*/
int readShaderSource(char *fileName, GLchar **vertexShader, GLchar **fragmentShader)
{
    int vSize, fSize;

    //
    // Allocate memory to hold the source of our shaders.
    //
    vSize = shaderSize(fileName, EVertexShader);
    fSize = shaderSize(fileName, EFragmentShader);

    if ((vSize == -1) || (fSize == -1))
    {
        printf("Cannot determine size of the shader %s\n", fileName);
        return 0;
    }

    *vertexShader = (GLchar *) malloc(vSize);
    *fragmentShader = (GLchar *) malloc(fSize);

    //
    // Read the source code
    //
    if (!readShader(fileName, EVertexShader, *vertexShader, vSize))
    {
        printf("Cannot read the file %s.vert\n", fileName);
        return 0;
    }

    if (!readShader(fileName, EFragmentShader, *fragmentShader, fSize))
    {
        printf("Cannot read the file %s.frag\n", fileName);
        return 0;
    }

    return 1;
}


/*public*/
int installShaders(const GLchar *Vertex,
                   const GLchar *Fragment,
                   GLuint *Program)
{
    GLuint VertexShaderObject   = 0;    //顶点着色器对象
    GLuint FragmentShaderObject = 0;    //片段着色器对象
    GLint vertCompiled, fragCompiled;   //status values
    GLint linked;

    // Create a vertex shader object and a fragment shader object

    VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // Load source code strings into shaders

    glShaderSource(VertexShaderObject, 1, &Vertex, NULL);
    glShaderSource(FragmentShaderObject, 1, &Fragment, NULL);

    // Compile the brick vertex shader, and print out
    // the compiler log file.

    glCompileShader(VertexShaderObject);
    glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &vertCompiled);

    // Compile the brick vertex shader, and print out
    // the compiler log file.

    glCompileShader(FragmentShaderObject);
    glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &fragCompiled);

    char fragLog[1024];
    glGetShaderInfoLog(FragmentShaderObject, 1024, NULL, fragLog);

    if (!vertCompiled || !fragCompiled)
        return 0;

    // Create a program object and attach the two compiled shaders

    *Program = glCreateProgram();
    glAttachShader(*Program, VertexShaderObject);
    glAttachShader(*Program, FragmentShaderObject);

    // Link the program object and print out the info log

    glLinkProgram(*Program);
    glGetProgramiv(*Program, GL_LINK_STATUS, &linked);

    if (!linked)
        return 0;

    return 1;
}

bool init()
{
    GLchar *VertexShaderSource, *FragmentShaderSource;
    bool success = true;
    GLuint vb = 0, tex = 0;
    GLint loc = 0;

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, 144, vertexData, GL_STATIC_DRAW);

    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    int texSize = 1024;
    int blockSize = 64;
    int rowPitch = texSize * 4;
    unsigned char* texData = new unsigned char[texSize*texSize*4];
    for (int i = 0; i < texSize/blockSize; ++i)
    {
        for (int j = 0; j < texSize/blockSize; ++j)
        {
            int startX = i*blockSize;
            int startY = j*blockSize;
            bool white = (bool)(i%2 ^ j%2);

            for (int m = startX; m < startX + blockSize; ++m)
            {
                for (int n = startY; n < startY + blockSize; ++n)
                {
                    texData[n * rowPitch + m * 4 + 0] = white ? 255 : 0;
                    texData[n * rowPitch + m * 4 + 1] = white ? 255 : 0;
                    texData[n * rowPitch + m * 4 + 2] = white ? 255 : 0;
                    texData[n * rowPitch + m * 4 + 3] = 255;
                }
            }
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    /*
    readShaderSource("perspective", &VertexShaderSource, &FragmentShaderSource);
    success = (bool)installShaders(VertexShaderSource, FragmentShaderSource, &prog);
    if (!success) goto Exit;

    // Install program object as part of current state
    glUseProgram(prog);

    // Set up attrib values
    loc = glGetAttribLocation(prog, "model_postion");
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 24, (GLvoid *)0);
    glEnableVertexAttribArray(loc);
    loc = glGetAttribLocation(prog, "tex_coord_in");
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 24, (GLvoid *)16);
    glEnableVertexAttribArray(loc);

    // Set up initial uniform values
    glUniformMatrix4fv(glGetUniformLocation(prog, "modelViewMatrix"), 1, GL_TRUE, modelViewMat);
    glUniformMatrix4fv(glGetUniformLocation(prog, "projectionMatrix"), 1, GL_TRUE, projectionMat);
    glUniform1i(glGetUniformLocation(prog, "tex"), 0);
    */

    readShaderSource("noPersectiveCorrect", &VertexShaderSource, &FragmentShaderSource);
    success = (bool)installShaders(VertexShaderSource, FragmentShaderSource, &progNoPersectiveCorrect);
    if (!success) goto Exit;

    // Install program object as part of current state
    glUseProgram(progNoPersectiveCorrect);

    glViewport(0,0,800,800);
    glClearColor(0.2,0.2,0.2,1.0);
    glClearDepth(0.0);
    glDisable(GL_DEPTH_TEST);

Exit:
    return success;
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
      case '1':
          InterpolateMode = 1;
          glUniform1i(glGetUniformLocation(progNoPersectiveCorrect, "interpolateMode"), (int)InterpolateMode);
          glutPostRedisplay();
          break;
      case '2':
          InterpolateMode = 2;
          glUniform1i(glGetUniformLocation(progNoPersectiveCorrect, "interpolateMode"), (int)InterpolateMode);
          glutPostRedisplay();
          break;
      case '3':
          InterpolateMode = 3;
          glUniform1i(glGetUniformLocation(progNoPersectiveCorrect, "interpolateMode"), (int)InterpolateMode);
          glutPostRedisplay();
          break;
      case '4':
          InterpolateMode = 4;
          glUniform1i(glGetUniformLocation(progNoPersectiveCorrect, "interpolateMode"), (int)InterpolateMode);
          glutPostRedisplay();
          break;
      case 'q':
          exit(0);
          break;
      default:
          break;
    }
}

/******************************************************************************/
/*
/* Main
/*
/******************************************************************************/
int main( int argc, char **argv )
{
    int success = 0;
    int gl_major, gl_minor;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    window = glutCreateWindow( "Perspective Correction");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Initialize the "OpenGL Extension Wrangler" library
    glewInit();

    // Make sure that OpenGL 2.0 is supported by the driver
    getGlVersion(&gl_major, &gl_minor);
    printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);

    if (gl_major < 2)
    {
        printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);
        printf("Support for OpenGL 2.0 is required for this demo...exiting\n");
        exit(1);
    }

    success = init();

    if (success)
        glutMainLoop();

    return 0;
}