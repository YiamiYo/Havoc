//#include <iostream>
#include <windows.h>
#include <sstream>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
//#include "YY/functions.hpp"
#include "classes.hpp"
#include "YY/tga.hpp"
#include "YY/ani.hpp"
#include "YY/ttf.hpp"

#define YY_SPF 16

#ifdef WINDOWS__
//WNDPROC oldProc;
HICON icon;
HWND hwnd;
/*
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_SETCURSOR)
	{
		if (LOWORD(lParam) == HTCLIENT)
		{
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return TRUE;
		}
	}

	return ::CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
}*/

void init_win32()
{
	HINSTANCE handle = ::GetModuleHandle(NULL);
	icon = ::LoadIcon(handle, "DEFAULT_ICON");

	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version)
	if (SDL_GetWMInfo(&wminfo) != 1)
	{
		// error: wrong SDL version
	}

	hwnd = wminfo.window;

	::SetClassLong(hwnd, GCL_HICON, (LONG) icon);

	oldProc = (WNDPROC) ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG) WndProc);
}

void done_win32()
{
	::DestroyIcon(icon);
}

#endif

int main(int argc, char **argv) {
	unsigned int vflags = SDL_OPENGL | SDL_DOUBLEBUF; // | SDL_RESIZABLE
	unsigned int scr_width = 1200, scr_height = 800;

	if(SDL_Init(SDL_INIT_EVERYTHING)) return 1;
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	/*Uint32 colorKey;
	SDL_Surface *image;
	image = SDL_LoadBMP("icon32.bmp");
	colorKey = SDL_MapRGB(image->format, 255, 0, 0);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorKey);*/
	//SDL_WM_SetIcon(DEFAULT_ICO, NULL);
	SDL_WM_SetCaption("HAVOC", "HAVOC");
	#ifdef WINDOWS__
	init_win32();
	#endif
	
	if(!SDL_SetVideoMode(scr_width, scr_height, 0, vflags)) return 2;
	

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
	#ifdef WINDOWS__
	done_win32();
	#endif
	return 0;
}