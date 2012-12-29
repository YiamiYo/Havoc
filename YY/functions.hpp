#ifndef YY_FUNCTIONS_HPP
#define YY_FUNCTIONS_HPP

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <fstream>
#include <string>

extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource; //ARB too
extern PFNGLCOMPILESHADERPROC glCompileShader; //ARB too
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram; //ARB too
extern PFNGLGETPROGRAMIVPROC glGetProgramiv; //ARB too
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLGENBUFFERSPROC glGenBuffers; //ARB too
extern PFNGLBINDBUFFERPROC glBindBuffer; //ARB too
extern PFNGLBUFFERDATAPROC glBufferData; //ARB too
extern PFNGLBUFFERSUBDATAPROC glBufferSubData; //ARB too
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray; //ARB too
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer; //ARB too
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray; //ARB too
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation; //ARB too
extern PFNGLACTIVETEXTUREPROC glActiveTexture; //ARB too
extern PFNGLUNIFORM1IPROC glUniform1i; //ARB too
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv; //ARB too
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers; //ARB too
extern PFNGLDELETEPROGRAMPROC glDeleteProgram; //ARB too
//extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
//extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

int loadGLFunctions(void);
unsigned int LoadShaders(const char*,const char*);

/////////////////
///DEFINITIONS///
/////////////////


int loadGLFunctions(void) {
	if(!(glCreateShader = (PFNGLCREATESHADERPROC) SDL_GL_GetProcAddress("glCreateShader"))) return 10;
	if(!(glShaderSource = (PFNGLSHADERSOURCEPROC) SDL_GL_GetProcAddress("glShaderSource"))) return 20;
	if(!(glCompileShader = (PFNGLCOMPILESHADERPROC) SDL_GL_GetProcAddress("glCompileShader"))) return 30;
	if(!(glGetShaderiv = (PFNGLGETSHADERIVPROC) SDL_GL_GetProcAddress("glGetShaderiv"))) return 40;
	if(!(glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SDL_GL_GetProcAddress("glGetShaderInfoLog"))) return 50;
	if(!(glCreateProgram = (PFNGLCREATEPROGRAMPROC) SDL_GL_GetProcAddress("glCreateProgram"))) return 60;
	if(!(glAttachShader = (PFNGLATTACHSHADERPROC) SDL_GL_GetProcAddress("glAttachShader"))) return 70;
	if(!(glLinkProgram = (PFNGLLINKPROGRAMPROC) SDL_GL_GetProcAddress("glLinkProgram"))) return 80;
	if(!(glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SDL_GL_GetProcAddress("glGetProgramiv"))) return 90;
	if(!(glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SDL_GL_GetProcAddress("glGetProgramInfoLog"))) return 100;
	if(!(glDeleteShader = (PFNGLDELETESHADERPROC) SDL_GL_GetProcAddress("glDeleteShader"))) return 110;
	if(!(glGenBuffers = (PFNGLGENBUFFERSPROC) SDL_GL_GetProcAddress("glGenBuffers"))) return 120;
	if(!(glBindBuffer = (PFNGLBINDBUFFERPROC) SDL_GL_GetProcAddress("glBindBuffer"))) return 130;
	if(!(glBufferData = (PFNGLBUFFERDATAPROC) SDL_GL_GetProcAddress("glBufferData"))) return 140;
	if(!(glBufferSubData = (PFNGLBUFFERSUBDATAPROC) SDL_GL_GetProcAddress("glBufferSubData"))) return 141;
	if(!(glUseProgram = (PFNGLUSEPROGRAMPROC) SDL_GL_GetProcAddress("glUseProgram"))) return 150;
	if(!(glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glEnableVertexAttribArray"))) return 160;
	if(!(glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SDL_GL_GetProcAddress("glVertexAttribPointer"))) return 170;
	if(!(glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glDisableVertexAttribArray"))) return 180;
	if(!(glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SDL_GL_GetProcAddress("glGetUniformLocation"))) return 190;
	if(!(glActiveTexture = (PFNGLACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glActiveTexture"))) return 200;
	if(!(glUniform1i = (PFNGLUNIFORM1IPROC) SDL_GL_GetProcAddress("glUniform1i"))) return 210;
	if(!(glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4fv"))) return 211;
	if(!(glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteBuffers"))) return 220;
	if(!(glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SDL_GL_GetProcAddress("glDeleteProgram"))) return 230;
	return 0;
}

unsigned int LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
	}
	//printf("</--Vertex Shader--\\>%s\n<\\--Vertex Shader--/>\n\n", VertexShaderCode.c_str());

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    //printf("Code: \n %s \n", VertexSourcePointer);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char VertexShaderErrorMessage[InfoLogLength];
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
    printf("%s\n", VertexShaderErrorMessage);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char FragmentShaderErrorMessage[InfoLogLength];
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
    printf("%s\n", FragmentShaderErrorMessage);

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char ProgramErrorMessage[std::max(InfoLogLength, int(1))];
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
    printf("%s\n", ProgramErrorMessage);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

//////////////
///POINTERS///
//////////////


PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL; //ARB too
PFNGLCOMPILESHADERPROC glCompileShader = NULL; //ARB too
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL; //ARB too
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL; //ARB too
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL; //ARB too
PFNGLBINDBUFFERPROC glBindBuffer = NULL; //ARB too
PFNGLBUFFERDATAPROC glBufferData = NULL; //ARB too
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL; //ARB too
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL; //ARB too
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL; //ARB too
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL; //ARB too
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL; //ARB too
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL; //ARB too
PFNGLUNIFORM1IPROC glUniform1i = NULL; //ARB too
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL; //ARB too
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL; //ARB too
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL; //ARB too
//PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
//PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
#endif
