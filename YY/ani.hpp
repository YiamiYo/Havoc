#ifndef YY_ANI_HPP
#define YY_ANI_HPP
#define YY_ANI_VER 0.10

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "tga.hpp"

typedef struct point {
	float x, y;
} point;

typedef struct frame {
	unsigned short int delay;
	unsigned char offsetX, offsetY;
} frame;

typedef struct ani {
	targa* tga;
	unsigned short int width, height;
	std::vector<frame> frames;
} ani;

class animations {
	private:
		bool ok;
		std::map<std::string, ani> anim;
		std::vector<targa*> tgas;
	public:
		animations(const char*);
		~animations(void);
		bool isOk(void);
		ani* operator[] (std::string);
};

class animated {
	private:
		ani *data, *prev_data;
		float *FSuv;
		point size;
		unsigned short int frame_margin;
		bool repeat, endlast, playing;
		bool prev_repeat, prev_endlast;
		bool flipx, flipy;
		unsigned char current;
		
		
		void calcTex(void);
	public:
		animated(ani*, float, float);
		~animated(void);
		void start(bool, bool);
		void stop(void);
		void stop(unsigned char);
		void refresh(unsigned int);
		void draw(void);
		void flip(bool, bool);
		void flipSet(bool, bool);
		void playAnimation(ani*, bool);
		void loadAnimation(ani*);
		point getSize(void);
};

animated::animated(ani* i_data, float i_width, float i_height) {
	if(!i_data) return;
	data = NULL;
	prev_data = NULL;
	
	flipx = false;
	flipy = false;
	FSuv = new (std::nothrow) float[4];
	
	loadAnimation(i_data);
	size.x = i_width;
	size.y = i_height;
	calcTex();
}

void animated::playAnimation(ani* i_data, bool i_repeat = false) {
	if(!i_data) return;
	prev_repeat = repeat;
	prev_data = data;
	prev_endlast = endlast;
	
	loadAnimation(i_data);
	start(i_repeat, false);
}

void animated::loadAnimation(ani* i_data) {
	if(!i_data) return;
	data = i_data;
	playing = false;
	frame_margin = 0;
	current = 0;
	calcTex();
}

void animated::calcTex(void) {
	if(flipx) {
		FSuv[2] = (float)data->frames[current].offsetX * (float)data->width / (float)data->tga->getWidth(); //MIN X
		FSuv[0] = (float)(data->frames[current].offsetX + 1) * (float)data->width / (float)data->tga->getWidth(); //MAX X
	} else {
		FSuv[0] = (float)data->frames[current].offsetX * (float)data->width / (float)data->tga->getWidth(); //MIN X
		FSuv[2] = (float)(data->frames[current].offsetX + 1) * (float)data->width / (float)data->tga->getWidth(); //MAX X
	}
	if(flipy) {
		FSuv[3] = -(float)data->frames[current].offsetY * (float)data->height / (float)data->tga->getHeight(); // MIN Y
		FSuv[1] = -(float)(data->frames[current].offsetY + 1) * (float)data->height / (float)data->tga->getHeight(); //MAX Y
	} else {
		FSuv[1] = -(float)data->frames[current].offsetY * (float)data->height / (float)data->tga->getHeight(); // MIN Y
		FSuv[3] = -(float)(data->frames[current].offsetY + 1) * (float)data->height / (float)data->tga->getHeight(); //MAX Y
	}
}

void animated::draw(void) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, data->tga->getTex());
	glBegin(GL_TRIANGLES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glTexCoord2f(FSuv[0], FSuv[1]);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(FSuv[0], FSuv[3]);
	glVertex2f(0.0f, size.y);
	glTexCoord2f(FSuv[2], FSuv[3]);
	glVertex2f(size.x, size.y);
	glTexCoord2f(FSuv[0], FSuv[1]);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(FSuv[2], FSuv[1]);
	glVertex2f(size.x, 0.0f);
	glTexCoord2f(FSuv[2], FSuv[3]);
	glVertex2f(size.x, size.y);
	
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void animated::start(bool i_repeat = false, bool i_endlast = false) {
	repeat = i_repeat;
	endlast = i_endlast;
	playing = true;
	frame_margin = 0;
	current = 0;
}

void animated::stop(void) {
	playing = false;

	if(prev_data) {
		loadAnimation(prev_data);
		prev_data = NULL;
		start(prev_repeat, prev_endlast);
		//std::cout<<"restarting...\r\n";
	} else if(endlast) {
		current = data->frames.size() - 1;
	} else {
		current = 0;
	}
	calcTex();
}

void animated::stop(unsigned char nc) {
	playing = false;
	if(current == nc) return;
	if(nc >= data->frames.size()) current = 0;
	else current = nc;
	calcTex();
}

void animated::refresh(unsigned int t_diff) {
	if(!playing) return;
	unsigned short int margin_diff = data->frames[current].delay - frame_margin;
	while(margin_diff < t_diff) {
		frame_margin = 0;
		t_diff -= margin_diff;
		current++;
		if(current >= data->frames.size()) {
			if(repeat) {
				current = 0;
			} else {
				stop();
				break;
			}
		}
		margin_diff = data->frames[current].delay;
	}
	frame_margin += t_diff;
	
	calcTex();
}

void animated::flip(bool fx, bool fy) {
	flipx ^= fx;
	flipy ^= fy;
	if(fx) {
		float temp = FSuv[0];
		FSuv[0] = FSuv[2];
		FSuv[2] = temp;
	}
	if(fy) {
		float temp = FSuv[1];
		FSuv[1] = FSuv[3];
		FSuv[3] = temp;
	}
}

void animated::flipSet(bool fx, bool fy) {
	if(fx && !flipx || !fx && flipx) {
		float temp = FSuv[0];
		FSuv[0] = FSuv[2];
		FSuv[2] = temp;
		flipx = fx;
	}
	if(fy && !flipy || !fy && flipy) {
		float temp = FSuv[1];
		FSuv[1] = FSuv[3];
		FSuv[3] = temp;
		flipy = fy;
	}
}

animations::animations(const char* filename) {
	ok = false;
	char buffer[4];
	std::ifstream fin(filename, std::ifstream::in | std::ifstream::binary);
	if(fin.is_open()) {
		unsigned char TargaNum;
		if(!fin.read(buffer, 1).good()) return;
		TargaNum = (unsigned char)buffer[0];
		for(unsigned char i = 0; i < TargaNum; i++) {
			if(!fin.read(buffer, 2).good()) return;
			unsigned short int temp_size = (unsigned char)buffer[0] * 0x100 + (unsigned char)buffer[1];
			char temp_str[temp_size + 1];
			temp_str[temp_size] = 0;
			if(!fin.read(temp_str, temp_size).good()) return;
			targa* temp_tga = new (std::nothrow) targa(temp_str);
			tgas.push_back(temp_tga);
		}
		unsigned char AnimNum;
		if(!fin.read(buffer, 1).good()) return;
		AnimNum = (unsigned char)buffer[0];
		for(unsigned char i = 0; i < AnimNum; i++ ) {
			if(!fin.read(buffer, 1).good()) return;
			unsigned char temp_size = (unsigned char)buffer[0];
			char temp_str[temp_size + 1];
			temp_str[temp_size] = 0;
			if(!fin.read(temp_str, temp_size).good()) return;
			//std::cout<<temp_str<<"\r\n";
			ani temp_ani;
			if(!fin.read(buffer, 1).good()) return;
			temp_ani.tga = tgas[(unsigned char)buffer[0]];
			if(!fin.read(buffer, 2).good()) return;
			temp_ani.width = (unsigned char)buffer[0] * 0x100 + (unsigned char)buffer[1];
			if(!fin.read(buffer, 2).good()) return;
			temp_ani.height = (unsigned char)buffer[0] * 0x100 + (unsigned char)buffer[1];
			unsigned char frameNum;
			if(!fin.read(buffer, 1).good()) return;
			frameNum = (unsigned char)buffer[0];
			for(unsigned char f = 0; f < frameNum; f++) {
				if(!fin.read(buffer, 4).good()) return;
				frame temp_frame;
				temp_frame.delay = (unsigned char)buffer[0] * 0x100 + (unsigned char)buffer[1];
				temp_frame.offsetX = (unsigned char)buffer[2];
				temp_frame.offsetY = (unsigned char)buffer[3];
				temp_ani.frames.push_back(temp_frame);
			}
			anim[std::string(temp_str)] = temp_ani;
		}
		fin.close();
		ok = true;
	}
}

point animated::getSize(void) {
	return size;
}

animated::~animated(void) {
	delete[] FSuv;
}

animations::~animations(void) {
	while(tgas.size() > 0) {
		delete tgas[0];
		tgas.erase(tgas.begin());
	}
}

bool animations::isOk(void) {
	return ok;
}

ani* animations::operator[] (std::string name) {
	if(anim.find(name) == anim.end()) return NULL;
	return &(anim[name]);
}

#endif