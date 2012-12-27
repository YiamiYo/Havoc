//#include <iostream>
#include <sstream>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
//#include "YY/functions.hpp"
#include "classes.hpp"
#include "YY/tga.hpp"
#include "YY/ani.hpp"
#include "YY/ttf.hpp"

#define YY_SPF 16

int main(int argc, char **argv) {
	unsigned int vflags = SDL_OPENGL | SDL_DOUBLEBUF; // | SDL_RESIZABLE
	unsigned int scr_width = 1024, scr_height = 1024;

	if(SDL_Init(SDL_INIT_EVERYTHING)) return 1;
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if(!SDL_SetVideoMode(scr_width, scr_height, 0, vflags)) return 2;
	SDL_WM_SetCaption("HAVOC", NULL);

	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	//glClearDepth(1.0f);
	glViewport(0, 0, scr_width, scr_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, (float)scr_width, (float)scr_height, 0.0f, 1.0f, -1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);

	//ttf alianna("AltamonteNF.ttf", 64);
	animations all_animations("anim.ani");
	if(!all_animations.isOk()) return 7;
	
	
	unsigned int t_start = SDL_GetTicks(), t_diff = 0, t_span = 0;
	unsigned int t_prev = t_start, t_now = t_start, t_before = t_start;
	srand(t_start);
	game havoc(&all_animations, scr_width, scr_height);
	SDL_Event event;
	bool cont = true;
	std::stringstream ss;
	unsigned int score = 0;
	while(cont) {
		while(SDL_PollEvent(&event) && cont) cont = havoc.handle(&event);
		havoc.refresh(t_diff);
		havoc.draw();
		
		t_prev = t_now;
		t_now = SDL_GetTicks();
		t_diff = t_now - t_prev;
		t_span = t_now - t_before;
		if(t_span < YY_SPF) SDL_Delay(YY_SPF - t_span);
		t_before = SDL_GetTicks();
	}
	SDL_Quit();
	return 0;
}