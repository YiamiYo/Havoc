#ifndef YY_TTF_HPP
#define YY_TTF_HPP
#define YY_TTF_VER 0.10

#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>

unsigned int npo2( unsigned int num ) {
	unsigned int ret = 1;
	while( ret < num ) ret *= 2;
	return ret;
}

class ttf {
	private:
		unsigned int chars[128];
		float char_w[128], char_h[128], char_wf[128], char_hf[128], char_maxh;
		bool ok;
	public:
		ttf(const char*, int);
		~ttf(void);
		void draw(std::string);
		void drawCenter(std::string);
};

void ttf::draw(std::string str) {
	float curx = 0.0f, cury = 0.0f;
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for(unsigned int i = 0; i < str.size(); i++) {
		if(str[i] == '\n') {
			cury += char_maxh;
			curx = 0;
		} else {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, chars[str[i]]);
			glBegin(GL_TRIANGLES);
			
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(curx, cury);
			glTexCoord2f(0.0f, char_hf[str[i]]);
			glVertex2f(curx, cury + char_h[str[i]]);
			glTexCoord2f(char_wf[str[i]], char_hf[str[i]]);
			glVertex2f(curx + char_w[str[i]], cury + char_h[str[i]]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(curx, cury);
			glTexCoord2f(char_wf[str[i]], 0.0f);
			glVertex2f(curx + char_w[str[i]], cury);
			glTexCoord2f(char_wf[str[i]], char_hf[str[i]]);
			glVertex2f(curx + char_w[str[i]], cury + char_h[str[i]]);
			
			glEnd();
			glDisable(GL_TEXTURE_2D);
			
			curx += char_w[str[i]];
		}
	}
}

void ttf::drawCenter(std::string str) {
	float total_width = 0.0f, total_height = char_maxh, curw = 0.0f;
	for(unsigned int i = 0; i < str.size(); i++) {
		if(str[i] == '\n') {
			total_height += char_maxh;
			curw = 0.0f;
		} else {
			curw += char_w[str[i]];
		}
		if(curw > total_width) total_width = curw;
	}
	glTranslatef(-total_width / 2.0f, -total_height / 2.0f, 0.0f);
	//std::cout<<str.size()<<": "<<total_width<<", "<<total_height<<"\r\n";
	draw(str);
}

ttf::ttf(const char *fontpath, int size) {
	ok = false;
	TTF_Font *font;
	if(TTF_Init() != 0) return;
	if(!(font = TTF_OpenFont(fontpath, size))) return;
	glGenTextures( 128, chars );
	char fonttempbuff[2] = { 0, 0 };
	SDL_Color color;
	color.r = 0xFF;
	color.g = 0xFF;
	color.b = 0xFF;
	//char_maxw = 0;
	char_maxh = 0;
	for(int c = 1; c < 128; c++) {
		fonttempbuff[0] = c;
		SDL_Surface *init = TTF_RenderText_Solid(font, fonttempbuff, color);
		unsigned int new_width = npo2( init->w );
		unsigned int new_height = npo2( init->h );
		char_wf[c] = (float)init->w / (float)new_width;
		char_hf[c] = (float)init->h / (float)new_height;
		char_w[c] = (float)init->w;
		char_h[c] = (float)init->h;
		SDL_Surface *ready = SDL_CreateRGBSurface(0, new_width, new_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		SDL_BlitSurface(init, 0, ready, 0);
		glBindTexture(GL_TEXTURE_2D, chars[(int)c]);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, new_width, new_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ready->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//if((unsigned int)init->w > char_maxw) char_maxw = (unsigned int)init->w;
		if((float)init->h > char_maxh) char_maxh = (float)init->h;
	}
	TTF_CloseFont(font);
	TTF_Quit();
	ok = true;
}

ttf::~ttf(void) {
	if(ok) glDeleteTextures(128, chars);
}

#endif