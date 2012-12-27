#ifndef YY_TGA_HPP
#define YY_TGA_HPP
#define YY_TGA_VER 0.10

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <SDL/SDL_opengl.h>

#define TGA_HEADER_SIZE 0x12
#define TGA_IMAGE_TYPE_CM 0x01
#define TGA_IMAGE_TYPE_TC 0x02
#define TGA_IMAGE_TYPE_BW 0x03
#define TGA_IMAGE_COMP_BIT 0x08
#define TGA_ALPHA_CHANNEL_DEPTH 0x0F
#define TGA_REVERSE_DIRECTION 0x20

class targa {
	private:
		bool ok;
		char tgah[TGA_HEADER_SIZE];

		/* HEADER CALCULATED FROM TGAH */
		unsigned char ID_Length, ColorMap_Type, Image_Type;

		bool hasColorMap;
		unsigned short int ColorMap_Offset, ColorMap_Length;
		unsigned char ColorMap_BitsPP, ColorMap_BytesPP;

		bool hasImageData;
		unsigned short int Origin_X, Origin_Y, Image_Width, Image_Height;
		unsigned char Image_BitsPP, Image_BytesPP, Image_Desc;

		/* DATA */
		char *Image_ID;
		unsigned char *ColorMap_Data, *Image_Data;

		/* RAW Image */
		unsigned char *Image_Raw;
		
		/* Texture ID */
		unsigned int tex;

		//FUNCTIONS
		void calcTGAHeader(void);
		void makeRaw(void);
		void unmap(void);
	public:
		targa(const char*);
		~targa(void);
		void freeTGA(void);
		void freeRAW(void);
		targa* glTex2D(void);
		void flipVert(void);
		bool isType(unsigned char);
		unsigned char* getRaw(void);
		unsigned int getWidth(void);
		unsigned int getHeight(void);
		unsigned int getBytesPP(void);
		unsigned int getBitsPP(void);
		unsigned char getType(void);
		unsigned int getTex(void);
};


/////////////////
///DEFINITIONS///
/////////////////

//PRIVATE:

void targa::calcTGAHeader(void) {
	ID_Length = (unsigned char)tgah[0];
	ColorMap_Type = (unsigned char)tgah[1];
	Image_Type = (unsigned char)tgah[2];
	if(ColorMap_Type == 0x01) {
		hasColorMap = true;
		ColorMap_Offset = (unsigned char)tgah[3] + (unsigned char)tgah[4] * 0x100;
		ColorMap_Length = (unsigned char)tgah[5] + (unsigned char)tgah[6] * 0x100;
		ColorMap_BitsPP = (unsigned char)tgah[7];
		ColorMap_BytesPP = (ColorMap_BitsPP + 1) / 8;
	} else hasColorMap = false;
	if(Image_Type & 0x07) {
		hasImageData = true;
		Origin_Y = (unsigned char)tgah[10] + (unsigned char)tgah[11] * 0x100;
		Origin_X = (unsigned char)tgah[8] + (unsigned char)tgah[9] * 0x100;
		Image_Width = (unsigned char)tgah[12] + (unsigned char)tgah[13] * 0x100;
		Image_Height = (unsigned char)tgah[14] + (unsigned char)tgah[15] * 0x100;
		Image_BitsPP = (unsigned char)tgah[16];
		Image_BytesPP = (Image_BitsPP + 1) / 8;
		Image_Desc = (unsigned char)tgah[17];
	} else hasImageData = false;
}

void targa::makeRaw(void) {
	Image_Raw = new (std::nothrow) unsigned char[Image_Width * Image_Height * getBytesPP()];

	if(isType(TGA_IMAGE_TYPE_CM)) unmap();
	else memcpy(Image_Raw, Image_Data, Image_Width * Image_Height * Image_BytesPP);

	if(Image_Desc & TGA_REVERSE_DIRECTION) flipVert();
}

void targa::unmap(void) {
	for(unsigned short int i = 0; i < Image_Width; i++) {
		for(unsigned short int r = 0; r < Image_Height; r++) {
			unsigned short int index = 0;
			if(Image_BytesPP == 1) {
				index = (unsigned char)(*(Image_Data + i * Image_BytesPP + r * Image_Width * Image_BytesPP));
			} else if(Image_BytesPP == 2) {
				index = (unsigned char)(*(Image_Data + i * Image_BytesPP + r * Image_Width * Image_BytesPP)) + (unsigned char)(*(Image_Data + i * Image_BytesPP + r * Image_Width * Image_BytesPP + 1)) * 0x100;
			} //else error
			memcpy(Image_Raw + i * ColorMap_BytesPP + r * Image_Width * ColorMap_BytesPP, ColorMap_Data + index * ColorMap_BytesPP, ColorMap_BytesPP);
		}
	}
}

//PUBLIC:
targa::targa(const char* filename) {
	//std::cout<<"Tga loading...\n"; fflush(stdout);
	tex = 0;
	Image_ID = NULL;
	ColorMap_Data = NULL;
	Image_Data = NULL;
	Image_Raw = NULL;
	ok = false;
	std::ifstream fin(filename, std::ifstream::in | std::ifstream::binary);
	if(fin.is_open()) {
		if(fin.read(tgah, TGA_HEADER_SIZE).good()) {
			calcTGAHeader();
			Image_ID = new (std::nothrow) char[ID_Length];
			fin.read(Image_ID, ID_Length);
			if(hasColorMap) {
				int colormap_size = ColorMap_Length * ColorMap_BytesPP;
				ColorMap_Data = new (std::nothrow) unsigned char[colormap_size];
				//if(Image_Type == 0x01 || Image_Type == 0x09) {
				fin.read((char*)ColorMap_Data, colormap_size);
				//} else fin.seekg(temp_size, std::ios::cur);
			}
			if(hasImageData) {
				int image_size = Image_Width * Image_Height * Image_BytesPP;
				Image_Data = new (std::nothrow) unsigned char[image_size];
				if(!(Image_Type & 0x08)) {
					fin.read((char*)Image_Data, image_size);
				} else {
					unsigned char ch, *offset = Image_Data;
					while(offset - Image_Data < image_size) {
						ch = (unsigned char)fin.get();
						if(ch & 0x80) {
							unsigned char cd[Image_BytesPP];
							fin.read((char*)cd, Image_BytesPP);
							for(unsigned char i = 0; i < ch - 127; i++, offset += Image_BytesPP) memcpy(offset, cd, Image_BytesPP);
						} else {
							//for(unsigned char i = 0; i <= ch & 0x7F; i++) fin.read(offset += Image_BytesPP, Image_BytesPP);
							fin.read((char*)offset, Image_BytesPP * (ch + 1));
							offset += Image_BytesPP * (ch + 1);
						}
					}
				}
			}
			makeRaw();
			freeTGA();
			glTex2D();
			freeRAW();
		}
		fin.close();
	}
}

targa::~targa(void) {
	//std::cout<<"Freeing resources...\n"; fflush(stdout);
	freeTGA();
	freeRAW();
	if(tex != 0) {
		glDeleteTextures(1, &tex);
		tex = 0;
	}
}

void targa::freeTGA(void) {
	if(Image_ID) {
		delete[] Image_ID;
		Image_ID = NULL;
	}
	if(ColorMap_Data) {
		delete[] ColorMap_Data;
		ColorMap_Data = NULL;
	}
	if(Image_Data) {
		delete[] Image_Data;
		Image_Data = NULL;
	}
}

void targa::freeRAW(void) {
	if(Image_Raw) {
		delete[] Image_Raw;
		Image_Raw = NULL;
	}
}

targa* targa::glTex2D(void) {
	//std::cout<<"Generating texture...\n"; fflush(stdout);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	unsigned int TexFormat = 0, TexType = 0, TexChannels = 0;
	if(isType(TGA_IMAGE_TYPE_BW)) {
		if(getBitsPP() == 8) {
			TexFormat = GL_LUMINANCE;
			TexChannels = 1;
			TexType = GL_UNSIGNED_BYTE;
		} //else error
	} else if(isType(TGA_IMAGE_TYPE_CM) || isType(TGA_IMAGE_TYPE_TC)) {
		/*if(getBitsPP() == 8 || getBitsPP() == 15 || getBitsPP() == 24) {
			TexFormat = GL_BGR;
			TexChannels = 3;
		} else if(getBitsPP() == 16 || getBitsPP() == 32) {
			TexFormat = GL_BGRA;
			TexChannels = 4;
		}*/
		if(Image_Desc & TGA_ALPHA_CHANNEL_DEPTH) {
			TexFormat = GL_BGRA;
			TexChannels = 4;
		} else {
			TexFormat = GL_BGR;
			TexChannels = 3;
		}
		if(getBytesPP() == 1 || getBytesPP() == 3 || getBytesPP() == 4) TexType = GL_UNSIGNED_BYTE;
		else if(getBytesPP() == 2) TexType = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		//else error
	} //else error
	if(Image_Raw && TexFormat && TexType && TexChannels) glTexImage2D(GL_TEXTURE_2D, 0, TexChannels, Image_Width, Image_Height, 0, TexFormat, TexType, (void*)Image_Raw);
	//else error
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return this;
}

void targa::flipVert(void) {
	for(unsigned short int i = 0; i < Image_Width; i++) {
		for(unsigned short int r = 0; r < Image_Height / 2; r++) {
			unsigned char temp[getBytesPP()];
			memcpy(temp, Image_Raw + i * getBytesPP() + (Image_Height - 1 - r) * Image_Width * getBytesPP(), getBytesPP());
			memcpy(Image_Raw + i * getBytesPP() + (Image_Height - 1 - r) * Image_Width * getBytesPP(), Image_Raw + i * getBytesPP() + r * Image_Width * getBytesPP(), getBytesPP());
			memcpy(Image_Raw + i * getBytesPP() + r * Image_Width * getBytesPP(), temp, getBytesPP());
			//memcpy(Image_Raw + i * Image_BytesPP + r * Image_Width * Image_BytesPP, Image_Data + i * Image_BytesPP + (Image_Height - 1 - r) * Image_Width * Image_BytesPP, Image_BytesPP);
		}
	}
}

bool targa::isType(unsigned char t) {
	return ((Image_Type | TGA_IMAGE_COMP_BIT) == (t | TGA_IMAGE_COMP_BIT));
}

unsigned char* targa::getRaw(void) {
	return Image_Raw;
}

unsigned int targa::getWidth(void) {
	return Image_Width;
}

unsigned int targa::getHeight(void) {
	return Image_Height;
}

unsigned int targa::getBytesPP(void) {
	if(isType(TGA_IMAGE_TYPE_CM)) return ColorMap_BytesPP;
	return Image_BytesPP;
}

unsigned int targa::getBitsPP(void) {
	if(isType(TGA_IMAGE_TYPE_CM)) return ColorMap_BitsPP;
	return Image_BitsPP;
}

unsigned char targa::getType(void) {
	return Image_Type;
}

unsigned int targa::getTex(void) {
	return tex;
}

#endif