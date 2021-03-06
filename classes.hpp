#include <list>
#include <cmath>
#include <sstream>
#include "YY/ani.hpp"
#include "YY/ttf.hpp"

#define GAME_STATE_MENU 1
#define GAME_STATE_START 2
#define GAME_STATE_PLAYING 3
#define GAME_STATE_GAMEOVER 4
#define GAME_STATE_SCORE 5
#define GAME_STATE_ABOUT 6
#define GAME_STATE_MUSIC 7
#define GAME_STATE_PAUSE 10

//GAME_STATE_MENU
#define MENU_BUTTON_WIDTH_PERC (325.0f / 1024.0f)
#define MENU_BUTTON_HEIGHT_PERC (115.0f / 1024.0f)
#define MENU_POSITION_X_PERC (571.0f / 1024.0f)
#define MENU_POSITION_Y_PERC (341.0f / 1024.0f)
#define MENU_MARGIN_Y_PERC (171.0f / 1024.0f)

//GAME_STATE_START
#define INIT_COUNTDOWN 3000

//GAME_STATE_START / GAME_STATE_PLAYING
#define INIT_SCROLL_SPEED 100.0f
#define INIT_PLATFORM_SIZE 5

#define PLAYER_WIDTH 128.0f
#define PLAYER_HEIGHT 128.0f
#define PLAYER_SPEED 500.0f
#define PLAYER_JUMP 950.0f;
#define PLAYER_GRAVITY 2000.0f
#define PLAYER_ATTACK_TIME 250
#define PLAYER_INVULNERABLE_TIME 2000

#define POINTS_FOR_LEVEL 300
#define LEVEL_INCREASE_SPEED 0.25f
#define POINTS_PER_PLATFORM 15
#define POINTS_PER_MONSTER 30
#define POINTS_PER_SHIELD 50
#define POINTS_PER_CANDY 10

#define TERRAIN_SPACE_Y 200.0f
#define MOB_SPAWN_CHANCE 0.35f
#define TERRAIN_TILE_WIDTH 31.25f //62.5f
#define TERRAIN_EDGE_WIDTH 47.75f //95.5f
#define TERRAIN_HEIGHT 66.75f //133.5f

#define BUFF_SHIELD_CHANCE 0.1f
#define BUFF_CANDY_CHANCE 0.2f
#define BUFF_SIZE 64
#define BUFF_TYPE_SHIELD 1
#define BUFF_TYPE_CANDY 2
#define BUFF_CANDY_TIME 5000
#define BUFF_CANDY_EFFECT 0.25f
#define BUFF_CANDY_GOOD_CHANCE 0.35f

#define MOB_SPEED 100.0f
#define MOB_WIDTH 128.0f
#define MOB_HEIGHT 128.0f

std::string itos(int num) {
	std::stringstream ss;
	ss<<num;
	return ss.str();
}

typedef struct color {
	float r, g, b;
} color;

typedef struct player {
	animated *sprite;
	point pos, vel;
	bool fall, shield;
	float fall_min, fall_max;
	unsigned int score, level;
	unsigned int atk_time, inv_time, candy_slow_time, candy_fast_time;
} player;

typedef struct monster {
	animated *sprite;
	float posx, velx;
	monster(ani* s_ani, float i_posx = 0.0f, float i_velx = MOB_SPEED) {
		sprite = new (std::nothrow) animated(s_ani, MOB_WIDTH, MOB_HEIGHT);
		posx = i_posx;
		velx = i_velx;
	}
} monster;

typedef struct terrain {
	point pos;
	float width;
	unsigned char size;
	monster *mob;
} terrain;

typedef struct buff {
	point pos;
	unsigned char type;
} buff;

typedef struct button {
	animated *sprite;
	point pos;
} button;

class game {
	private:
		animations *anim;
		unsigned char state;
		unsigned int width, height;
		ttf *cd_font, *score_font, *txt_font;
		
		//GAME_STATE_MENU
		animated *menu_bg;
		button menu_play, menu_music, menu_score, menu_about;
		char menu_sel;
		
		//GAME_STATE_START
		int countdown;
		
		//GAME_STATE_START/GAME_STATE_PLAYING
		color c_top, c_bot, cd_top, cd_bot;
		player main_char;
		std::list<terrain> ters;
		std::list<buff> buffs;
		float scroll_pos, scroll_speed;
		unsigned char platform_size;
		animated *platform_edge, *platform_tile;
		animated *shield, *candy;
		
		void setDir(void);
		void setState(unsigned char);
		void unsetState(void);
	public:
		game(animations*, unsigned int, unsigned int);
		~game(void);
		bool handle(SDL_Event*);
		void refresh(unsigned int);
		void draw(void);
};

void game::unsetState(void) {
	//DESTROY PERVIOUS STATE
	if(state == GAME_STATE_MENU) {
		if(menu_bg) {
			delete menu_bg;
			menu_bg = NULL;
		}
		if(menu_play.sprite) {
			delete menu_play.sprite;
			menu_play.sprite = NULL;
		}
		if(menu_music.sprite) {
			delete menu_music.sprite;
			menu_music.sprite = NULL;
		}
		if(menu_score.sprite) {
			delete menu_score.sprite;
			menu_score.sprite = NULL;
		}
		if(menu_about.sprite) {
			delete menu_about.sprite;
			menu_about.sprite = NULL;
		}
	} else if(state == GAME_STATE_GAMEOVER) {
		if(main_char.sprite) {
			delete main_char.sprite;
			main_char.sprite = NULL;
		}
		if(platform_edge) {
			delete platform_edge;
			platform_edge = NULL;
		}
		if(platform_tile) {
			delete platform_tile;
			platform_tile = NULL;
		}
		if(shield) {
			delete shield;
			shield = NULL;
		}
		if(candy) {
			delete candy;
			candy = NULL;
		}
	}
	state = 0;
}

void game::setState(unsigned char ns) {
	unsetState();
	//CREATE NEW STATE
	state = ns;
	if(state == GAME_STATE_MENU) {
		float button_width = MENU_BUTTON_WIDTH_PERC * width, button_height = MENU_BUTTON_HEIGHT_PERC * height, button_margin_y = MENU_MARGIN_Y_PERC * height;
		point menu_pos;
		menu_pos.x = MENU_POSITION_X_PERC * width;
		menu_pos.y = MENU_POSITION_Y_PERC * height;
		menu_bg = new (std::nothrow) animated((*anim)["menu_background"], width, height);
		menu_play.pos = menu_pos;
		menu_play.sprite = new (std::nothrow) animated((*anim)["menu_button_play"], button_width, button_height);
		menu_music.pos.x = menu_play.pos.x;
		menu_music.pos.y = menu_play.pos.y + button_margin_y;
		menu_music.sprite = new (std::nothrow) animated((*anim)["menu_button_music"], button_width, button_height);
		menu_score.pos.x = menu_music.pos.x;
		menu_score.pos.y = menu_music.pos.y + button_margin_y;
		menu_score.sprite = new (std::nothrow) animated((*anim)["menu_button_score"], button_width, button_height);
		menu_about.pos.x = menu_score.pos.x;
		menu_about.pos.y = menu_score.pos.y + button_margin_y;
		menu_about.sprite = new (std::nothrow) animated((*anim)["menu_button_about"], button_width, button_height);
		menu_sel = 1;
		menu_play.sprite->stop(1);
	} else if(state == GAME_STATE_START) {
		c_top.r = 0.4f;
		c_top.g = 0.95f;
		c_top.b = 0.94f;
		c_bot.r = 0.06f;
		c_bot.g = 0.05f;
		c_bot.b = 0.4f;
		cd_top.r = 0.0f;
		cd_top.g = 0.0f;
		cd_top.b = 0.0f;
		cd_bot.r = 0.0f;
		cd_bot.g = 0.0f;
		cd_bot.b = 0.0f;
	
		while(ters.begin() != ters.end()) {
			if(ters.front().mob) delete ters.front().mob;
			ters.pop_front();
		}
		while(buffs.begin() != buffs.end()) {
			buffs.pop_front();
		}
		countdown = INIT_COUNTDOWN;
		scroll_pos = 0.0f;
		scroll_speed = INIT_SCROLL_SPEED;
		platform_size = INIT_PLATFORM_SIZE;
		unsigned int tps = ((unsigned int)(height / TERRAIN_SPACE_Y) + 1);
		for(unsigned int i = 1; i <= tps; i++) {
			terrain temp_t;
			temp_t.size = platform_size;
			temp_t.width = TERRAIN_EDGE_WIDTH * 2.0f + platform_size * TERRAIN_TILE_WIDTH;
			temp_t.pos.y = scroll_pos + i * TERRAIN_SPACE_Y;
			temp_t.pos.x = temp_t.width / 2.0f + ((float)rand() / (float)RAND_MAX) * ((float)width - temp_t.width);
			if((float)rand() / (float)RAND_MAX <= MOB_SPAWN_CHANCE) {
				temp_t.mob = new (std::nothrow) monster((*anim)["pumpkin_main"], 0.0f, MOB_SPEED);
				temp_t.mob->sprite->start(true, false);
			}
			else temp_t.mob = NULL;
			ters.push_back(temp_t);
		}
		main_char.sprite = new (std::nothrow) animated((*anim)["char_main"], PLAYER_WIDTH, PLAYER_HEIGHT);
		main_char.score = 0;
		main_char.level = 0;
		main_char.pos = ters.begin()->pos;
		main_char.fall = false;
		main_char.fall_min = ters.begin()->pos.x - ters.begin()->width / 2.0f;
		main_char.fall_max = ters.begin()->pos.x + ters.begin()->width / 2.0f;
		main_char.vel.x = 0.0f;
		main_char.vel.y = 0.0f;
		main_char.atk_time = 0;
		main_char.inv_time = 0;
		main_char.candy_fast_time = 0;
		main_char.candy_slow_time = 0;
		main_char.shield = true;
		
		platform_edge = new (std::nothrow) animated((*anim)["platform_edge"], TERRAIN_EDGE_WIDTH, TERRAIN_HEIGHT);
		platform_tile = new (std::nothrow) animated((*anim)["platform_tile"], TERRAIN_TILE_WIDTH, TERRAIN_HEIGHT);
		shield = new (std::nothrow) animated((*anim)["shield"], BUFF_SIZE, BUFF_SIZE);
		candy = new (std::nothrow) animated((*anim)["candy"], BUFF_SIZE, BUFF_SIZE);
	} else if(state == GAME_STATE_GAMEOVER) {
	
	}
}

game::game(animations* i_anim, unsigned int scr_width, unsigned int scr_height) {
	anim = i_anim;
	width = scr_width;
	height = scr_height;
	state = 0;
	
	//cd_font = new (std::nothrow) ttf("Alien Bold.ttf", 256);
	cd_font = new (std::nothrow) ttf("AltamonteNF.ttf", 256);
	score_font = new (std::nothrow) ttf("AltamonteNF.ttf", 64);
	txt_font = new (std::nothrow) ttf("Alan Den.ttf", 32);
	
	setState(GAME_STATE_MENU);
}

game::~game(void) {
	unsetState();
	if(cd_font) {
		delete cd_font;
		cd_font = NULL;
	}
	if(score_font) {
		delete score_font;
		score_font = NULL;
	}
	if(txt_font) {
		delete txt_font;
		txt_font = NULL;
	}
}

void game::setDir(void) {
	if(main_char.vel.x != 0.0f) {
		main_char.sprite->flipSet(main_char.vel.x < 0.0f, false);
		main_char.sprite->start(true, false);
	}
	else main_char.sprite->stop();
}

bool game::handle(SDL_Event *event) {
	bool cont = true;
	if(event->type == SDL_QUIT) cont = false;
	if(state == GAME_STATE_MENU) {
		if(event->type == SDL_MOUSEMOTION) {
			if(event->motion.x > menu_play.pos.x && event->motion.x < menu_play.pos.x + menu_play.sprite->getSize().x && event->motion.y > menu_play.pos.y && event->motion.y < menu_play.pos.y + menu_play.sprite->getSize().y) {
				menu_play.sprite->stop(1);
				menu_sel = 1;
			} else {
				menu_play.sprite->stop(0);
				if(event->motion.x > menu_music.pos.x && event->motion.x < menu_music.pos.x + menu_music.sprite->getSize().x && event->motion.y > menu_music.pos.y && event->motion.y < menu_music.pos.y + menu_music.sprite->getSize().y) {
				menu_music.sprite->stop(1);
				menu_sel = 2;
				} else {
					menu_music.sprite->stop(0);
					if(event->motion.x > menu_score.pos.x && event->motion.x < menu_score.pos.x + menu_score.sprite->getSize().x && event->motion.y > menu_score.pos.y && event->motion.y < menu_score.pos.y + menu_score.sprite->getSize().y) {
						menu_score.sprite->stop(1);
						menu_sel = 3;
					} else {
						menu_score.sprite->stop(0);
						if(event->motion.x > menu_about.pos.x && event->motion.x < menu_about.pos.x + menu_about.sprite->getSize().x && event->motion.y > menu_about.pos.y && event->motion.y < menu_about.pos.y + menu_about.sprite->getSize().y) {
							menu_about.sprite->stop(1);
							menu_sel = 4;
						} else {
							menu_about.sprite->stop(0);
							menu_sel = 0;
						}
					}
				}
			}
		} else if(event->type == SDL_MOUSEBUTTONDOWN) {
			if(event->button.x > menu_play.pos.x && event->button.x < menu_play.pos.x + menu_play.sprite->getSize().x && event->button.y > menu_play.pos.y && event->button.y < menu_play.pos.y + menu_play.sprite->getSize().y) setState(GAME_STATE_START);
			else if(event->button.x > menu_music.pos.x && event->button.x < menu_music.pos.x + menu_music.sprite->getSize().x && event->button.y > menu_music.pos.y && event->button.y < menu_music.pos.y + menu_music.sprite->getSize().y) setState(GAME_STATE_MUSIC);
			else if(event->button.x > menu_score.pos.x && event->button.x < menu_score.pos.x + menu_score.sprite->getSize().x && event->button.y > menu_score.pos.y && event->button.y < menu_score.pos.y + menu_score.sprite->getSize().y) setState(GAME_STATE_SCORE);
			else if(event->button.x > menu_about.pos.x && event->button.x < menu_about.pos.x + menu_about.sprite->getSize().x && event->button.y > menu_about.pos.y && event->button.y < menu_about.pos.y + menu_about.sprite->getSize().y) setState(GAME_STATE_ABOUT);
		} else if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == SDLK_RETURN) {
				if(menu_sel == 1) setState(GAME_STATE_START);
				else if(menu_sel == 2) setState(GAME_STATE_MUSIC);
				else if(menu_sel == 3) setState(GAME_STATE_SCORE);
				else if(menu_sel == 4) setState(GAME_STATE_ABOUT);
			} else if(event->key.keysym.sym == SDLK_DOWN) {
				if(menu_sel == 1) menu_play.sprite->stop(0);
				else if(menu_sel == 2) menu_music.sprite->stop(0);
				else if(menu_sel == 3) menu_score.sprite->stop(0);
				else if(menu_sel == 4) menu_about.sprite->stop(0);
				menu_sel++;
				if(menu_sel > 4) menu_sel = 1;
				if(menu_sel == 1) menu_play.sprite->stop(1);
				else if(menu_sel == 2) menu_music.sprite->stop(1);
				else if(menu_sel == 3) menu_score.sprite->stop(1);
				else if(menu_sel == 4) menu_about.sprite->stop(1);
			} else if(event->key.keysym.sym == SDLK_UP) {
				if(menu_sel == 1) menu_play.sprite->stop(0);
				else if(menu_sel == 2) menu_music.sprite->stop(0);
				else if(menu_sel == 3) menu_score.sprite->stop(0);
				else if(menu_sel == 4) menu_about.sprite->stop(0);
				menu_sel--;
				if(menu_sel < 1) menu_sel = 4;
				if(menu_sel == 1) menu_play.sprite->stop(1);
				else if(menu_sel == 2) menu_music.sprite->stop(1);
				else if(menu_sel == 3) menu_score.sprite->stop(1);
				else if(menu_sel == 4) menu_about.sprite->stop(1);
			}
		}
	} else if(state == GAME_STATE_PLAYING) {
		if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == SDLK_DOWN) {
				main_char.fall = true;
			} else if(event->key.keysym.sym == SDLK_UP && ! main_char.fall) {
				main_char.vel.y = -PLAYER_JUMP;
				main_char.fall = true;
			} else if(event->key.keysym.sym == SDLK_RIGHT) {
				main_char.vel.x += PLAYER_SPEED;
				setDir();
			} else if(event->key.keysym.sym == SDLK_LEFT) {
				main_char.vel.x -= PLAYER_SPEED;
				setDir();
			} else if(event->key.keysym.sym == SDLK_SPACE && main_char.atk_time == 0) {
				main_char.sprite->playAnimation((*anim)["main_char_hit"], false);
				main_char.atk_time = PLAYER_ATTACK_TIME;
			} else if(event->key.keysym.sym == SDLK_ESCAPE) {
				setState(GAME_STATE_PAUSE);
			}
		} else if(event->type == SDL_KEYUP) {
			if(event->key.keysym.sym == SDLK_RIGHT && main_char.vel.x > 0.0f) {
				main_char.vel.x -= PLAYER_SPEED;
				setDir();
			} else if(event->key.keysym.sym == SDLK_LEFT && main_char.vel.x < 0.0f) {
				main_char.vel.x += PLAYER_SPEED;
				setDir();
			}
		}
	} else if(state == GAME_STATE_GAMEOVER) {
		if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == SDLK_RETURN) setState(GAME_STATE_MENU);
		}
	} else if(state == GAME_STATE_PAUSE) {
		if(event->type == SDL_KEYDOWN) {
			if(event->key.keysym.sym == SDLK_RETURN) {
				setState(GAME_STATE_GAMEOVER);
				setState(GAME_STATE_MENU);
			}
			else if(event->key.keysym.sym == SDLK_ESCAPE) setState(GAME_STATE_PLAYING);
		}
	}
	return cont;
}

void game::refresh(unsigned int t_diff) {
	float fsec = (float)t_diff / 1000.0f;
	
	if(state == GAME_STATE_MENU) {
	
	} else if(state == GAME_STATE_START) {
		countdown -= t_diff;
		if(countdown <= 0) setState(GAME_STATE_PLAYING);
	} else if(state == GAME_STATE_PLAYING || state == GAME_STATE_GAMEOVER) {
		c_top.r += cd_top.r * fsec;
		c_top.g += cd_top.g * fsec;
		c_top.b += cd_top.b * fsec;
		c_bot.r += cd_bot.r * fsec;
		c_bot.g += cd_bot.g * fsec;
		c_bot.b += cd_bot.b * fsec;
		
		scroll_pos += scroll_speed * fsec;
		bool coll = false;
		point npos;
		
		npos.x = main_char.pos.x + (1.0f + BUFF_CANDY_EFFECT * (float)(main_char.candy_fast_time != 0) - BUFF_CANDY_EFFECT * (float)(main_char.candy_slow_time != 0)) * main_char.vel.x * fsec;
		if(npos.x < main_char.fall_min || npos.x > main_char.fall_max) main_char.fall = true;
		npos.y = main_char.pos.y;
		if(main_char.fall) npos.y = main_char.pos.y + main_char.vel.y * fsec + 0.5f * PLAYER_GRAVITY * fsec * fsec;
		if(main_char.inv_time > t_diff) main_char.inv_time -= t_diff;
		else main_char.inv_time = 0;
		if(main_char.candy_slow_time > t_diff) main_char.candy_slow_time -= t_diff;
		else main_char.candy_slow_time = 0;
		if(main_char.candy_fast_time > t_diff) main_char.candy_fast_time -= t_diff;
		else main_char.candy_fast_time = 0;
		
		//TERRAIN
		while(buffs.begin() != buffs.end() && buffs.front().pos.y + BUFF_SIZE < scroll_pos) buffs.pop_front();
		std::list<buff>::iterator bit, rit;
		for(bit = buffs.begin(); bit != buffs.end(); bit++) {
			float dx = bit->pos.x - main_char.pos.x;
			float dy = bit->pos.y - main_char.pos.y;
			if(dx < 0.0f) dx = -dx;
			if(dy < 0.0f) dy = -dy;
			if((dx < main_char.sprite->getSize().x / 2.0f) && (dy < main_char.sprite->getSize().y / 2.0f)) {
				if(bit->type == BUFF_TYPE_SHIELD) {
					if(main_char.shield) {
						main_char.score += POINTS_PER_SHIELD;
						main_char.level = main_char.score / POINTS_FOR_LEVEL;
						platform_size = INIT_PLATFORM_SIZE - (main_char.level / 2);
						scroll_speed = INIT_SCROLL_SPEED + INIT_SCROLL_SPEED * ((main_char.level + 1) / 2) * LEVEL_INCREASE_SPEED;
					} else main_char.shield = true;
				} else if(bit->type == BUFF_TYPE_CANDY) {
					main_char.score += POINTS_PER_CANDY;
					main_char.level = main_char.score / POINTS_FOR_LEVEL;
					platform_size = INIT_PLATFORM_SIZE - (main_char.level / 2);
					scroll_speed = INIT_SCROLL_SPEED + INIT_SCROLL_SPEED * ((main_char.level + 1) / 2) * LEVEL_INCREASE_SPEED;
					if((float)rand() / (float)RAND_MAX > BUFF_CANDY_GOOD_CHANCE) {
						main_char.candy_slow_time = BUFF_CANDY_TIME;
					} else {
						main_char.candy_fast_time = BUFF_CANDY_TIME;
					}
				}
				rit = bit;
				buffs.erase(rit);
				bit--;
			}
		}
		
		if(ters.front().pos.y + platform_edge->getSize().y < scroll_pos) {
			terrain temp_t;
			temp_t.size = platform_size;
			temp_t.width = TERRAIN_EDGE_WIDTH * 2.0f + temp_t.size * TERRAIN_TILE_WIDTH;
			temp_t.pos.y = ters.back().pos.y + TERRAIN_SPACE_Y;
			temp_t.pos.x = temp_t.width / 2.0f + ((float)rand() / (float)RAND_MAX) * ((float)width - temp_t.width);
			if((float)rand() / (float)RAND_MAX <= MOB_SPAWN_CHANCE) {
				temp_t.mob = new (std::nothrow) monster((*anim)["pumpkin_main"], 0.0f, MOB_SPEED);
				temp_t.mob->sprite->start(true, false);
			}
			else temp_t.mob = NULL;
			ters.push_back(temp_t);
			if(ters.front().mob) {
				delete ters.front().mob;
				ters.front().mob = NULL;
			}
			if(state == GAME_STATE_PLAYING) {
				main_char.score += POINTS_PER_PLATFORM;
				main_char.level = main_char.score / POINTS_FOR_LEVEL;
				platform_size = INIT_PLATFORM_SIZE - (main_char.level / 2);
				scroll_speed = INIT_SCROLL_SPEED + INIT_SCROLL_SPEED * ((main_char.level + 1) / 2) * LEVEL_INCREASE_SPEED;
			}
			ters.pop_front();
			float tr = (float)rand() / (float)RAND_MAX;
			if(tr <= BUFF_SHIELD_CHANCE + BUFF_CANDY_CHANCE) {
				buff bt;
				bt.pos.y = temp_t.pos.y + (float)rand() / (float)RAND_MAX * ((float)TERRAIN_SPACE_Y - (float)TERRAIN_HEIGHT) + (float)TERRAIN_HEIGHT;
				bt.pos.x = (float)rand() / (float)RAND_MAX * ((float)width - 2.0f * (float)BUFF_SIZE) + (float)BUFF_SIZE;
				if(tr <= BUFF_SHIELD_CHANCE) bt.type = BUFF_TYPE_SHIELD;
				else bt.type = BUFF_TYPE_CANDY;
				buffs.push_back(bt);
			}
		}
		
		
		//TERRAIN - MONSTER
		std::list<terrain>::iterator it;
		for(it = ters.begin(); it != ters.end(); it++) {
			if(state == GAME_STATE_PLAYING) {
				if(main_char.fall && !coll && npos.y > it->pos.y && main_char.pos.y < it->pos.y) {
					float d = main_char.vel.y * main_char.vel.y + 2 * PLAYER_GRAVITY * (main_char.pos.y - it->pos.y);
					if(d >= 0.0f) {
						float sd = sqrt(d), nt = fsec;
						float nt1 = (-main_char.vel.y + sd) / -PLAYER_GRAVITY;
						float nt2 = (-main_char.vel.y - sd) / -PLAYER_GRAVITY;
						if(nt1 >= nt2) {
							if(nt1 >= 0.0f && nt1 <= fsec) nt = nt1;
							else if(nt2 >= 0.0f && nt2 <= fsec) nt = nt2;
						} else {
							if(nt2 >= 0.0f && nt2 <= fsec) nt = nt2;
							else if(nt1 >= 0.0f && nt1 <= fsec) nt = nt1;
						}
						float ntposx = main_char.pos.x + (1.0f + BUFF_CANDY_EFFECT * (float)(main_char.candy_fast_time != 0) - BUFF_CANDY_EFFECT * (float)(main_char.candy_slow_time != 0)) * main_char.vel.x * nt;
						if(ntposx >= it->pos.x - it->width / 2.0f && ntposx <= it->pos.x + it->width / 2.0f) {
							npos.y = main_char.pos.y + main_char.vel.y * nt + 0.5f * PLAYER_GRAVITY * nt * nt;
							coll = true;
						}
						if(npos.x >= it->pos.x - it->width / 2.0f && npos.x <= it->pos.x + it->width / 2.0f) {
							main_char.fall = false;
							main_char.fall_min = it->pos.x - it->width / 2.0f;
							main_char.fall_max = it->pos.x + it->width / 2.0f;
						}
					}
				}
			}
			if(it->mob) {
				if(state == GAME_STATE_PLAYING) {
					float dx = it->pos.x - it->width / 2.0f + it->mob->posx - main_char.pos.x;
					float dy = it->pos.y - main_char.pos.y;
					if(dx < 0.0f) dx = -dx;
					if(dy < 0.0f) dy = -dy;
					if((dx < main_char.sprite->getSize().x / 4.0f + it->mob->sprite->getSize().x / 4.0f) && (dy < it->mob->sprite->getSize().y / 2.0f)) {
						if(main_char.atk_time > 0) {
							main_char.score += POINTS_PER_MONSTER;
							main_char.level = main_char.score / POINTS_FOR_LEVEL;
							platform_size = INIT_PLATFORM_SIZE - (main_char.level / 2);
							scroll_speed = INIT_SCROLL_SPEED + INIT_SCROLL_SPEED * ((main_char.level + 1) / 2) * LEVEL_INCREASE_SPEED;
							delete it->mob;
							it->mob = NULL;
							//main_char.atk_time = 0;
							break;
						} else {
							if(main_char.shield) {
								main_char.shield = false;
								main_char.inv_time = PLAYER_INVULNERABLE_TIME;
							} else if(!main_char.inv_time) {
								setState(GAME_STATE_GAMEOVER);
								break;
							}
						}
					}
				}
				unsigned int temp_diff = t_diff;
				it->mob->sprite->refresh(temp_diff);
				while(temp_diff > 0) {
					if(it->mob->velx < 0.0f) {
						unsigned int td = (unsigned int)(-it->mob->posx / it->mob->velx * 1000.0f);
						if(td < temp_diff) {
							it->mob->posx = it->mob->velx * td / 1000.0f;
							it->mob->velx = -it->mob->velx;
							temp_diff -= td;
							it->mob->sprite->flip(true, false);
						} else {
							it->mob->posx += it->mob->velx * temp_diff / 1000.0f;
							temp_diff = 0;
						}
					} else {
						unsigned int td = (unsigned int)((it->width - it->mob->posx) / it->mob->velx * 1000.0f);
						if(td < temp_diff) {
							temp_diff -= td;
							it->mob->posx = it->width + it->mob->velx * td / 1000.0f;
							it->mob->velx = -it->mob->velx;
							it->mob->sprite->flip(true, false);
						} else {
							it->mob->posx += it->mob->velx * temp_diff / 1000.0f;
							temp_diff = 0;
						}
					}
				}
			}
		}
		if(state == GAME_STATE_PLAYING) {	
			main_char.sprite->refresh(t_diff);
			main_char.pos.x = npos.x;
			main_char.pos.y = npos.y;
			if(!coll && main_char.fall) main_char.vel.y += PLAYER_GRAVITY * fsec;
			else main_char.vel.y = 0.0f;
			while(main_char.pos.x < 0.0f) main_char.pos.x = width + main_char.pos.x;
			while(main_char.pos.x > width) main_char.pos.x -= width;
			if(main_char.pos.y - main_char.sprite->getSize().y / 2.0f < scroll_pos || main_char.pos.y - main_char.sprite->getSize().y / 2.0f > scroll_pos + height) {
				if(main_char.shield) {
					main_char.shield = false;
					main_char.inv_time = PLAYER_INVULNERABLE_TIME;
					main_char.pos.x = ters.back().pos.x;
					main_char.pos.y = ters.back().pos.y - TERRAIN_SPACE_Y;
					main_char.fall = true;
				} else if(!main_char.inv_time) setState(GAME_STATE_GAMEOVER);
			}
		}
	}
	if(main_char.atk_time > t_diff) main_char.atk_time -= t_diff;
	else main_char.atk_time = 0;
}

void game::draw(void) {
	glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	if(state == GAME_STATE_MENU) {
		glPushMatrix();
		glLoadIdentity();
		menu_bg->draw();
		glTranslatef(menu_play.pos.x, menu_play.pos.y, 0.0f);
		menu_play.sprite->draw();
		glLoadIdentity();
		glTranslatef(menu_music.pos.x, menu_music.pos.y, 0.0f);
		menu_music.sprite->draw();
		glLoadIdentity();
		glTranslatef(menu_score.pos.x, menu_score.pos.y, 0.0f);
		menu_score.sprite->draw();
		glLoadIdentity();
		glTranslatef(menu_about.pos.x, menu_about.pos.y, 0.0f);
		menu_about.sprite->draw();
		glPopMatrix();
	} else if(state == GAME_STATE_PLAYING || state == GAME_STATE_START || state == GAME_STATE_GAMEOVER || state == GAME_STATE_PAUSE) {
		glBegin(GL_TRIANGLES);
		glColor4f(c_top.r, c_top.g, c_top.b, 1.0f);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(width, 0.0f);
		glColor4f(c_bot.r, c_bot.g, c_bot.b, 1.0f);
		glVertex2f(width, height);
		
		glColor4f(c_top.r, c_top.g, c_top.b, 1.0f);
		glVertex2f(0.0f, 0.0f);
		glColor4f(c_bot.r, c_bot.g, c_bot.b, 1.0f);
		glVertex2f(0.0f, height);
		glVertex2f(width, height);
		glEnd();
		
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTranslatef(0.0f, -scroll_pos, 0.0f);
		//TERRAIN
		std::list<terrain>::iterator it;
		for(it = ters.begin(); it != ters.end(); it++) {
			glPushMatrix();
			//glLoadIdentity();
			glTranslatef(it->pos.x - it->width / 2.0f, it->pos.y, 0.0f);
			
			glPushMatrix();
			platform_edge->flipSet(false, false);
			platform_edge->draw();
			glTranslatef(TERRAIN_EDGE_WIDTH, 0.0f, 0.0f);
			for(unsigned char i = 0; i < it->size; i++) {
				platform_tile->draw();
				glTranslatef(TERRAIN_TILE_WIDTH, 0.0f, 0.0f);
			}
			platform_edge->flipSet(true, false);
			platform_edge->draw();
			glPopMatrix();
			
			//MONSTER
			if(it->mob) {
				glPushMatrix();
				glTranslatef(it->mob->posx - it->mob->sprite->getSize().x / 2.0f, -it->mob->sprite->getSize().y, 0.0f);
				it->mob->sprite->draw();
				glPopMatrix();
			}
			
			glPopMatrix();
		}
		std::list<buff>::iterator bit;
		for(bit = buffs.begin(); bit != buffs.end(); bit++) {
			glPushMatrix();
			//glLoadIdentity();
			glTranslatef(bit->pos.x - (float)BUFF_SIZE / 2.0f, bit->pos.y - (float)BUFF_SIZE / 2.0f, 0.0f);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			if(bit->type == BUFF_TYPE_SHIELD) shield->draw();
			else if(bit->type == BUFF_TYPE_CANDY) candy->draw();
			glPopMatrix();
		}
		
		if(state == GAME_STATE_PLAYING || state == GAME_STATE_START || state == GAME_STATE_PAUSE) {
			glPushMatrix();
			glTranslatef(main_char.pos.x - main_char.sprite->getSize().x / 2.0f, main_char.pos.y - main_char.sprite->getSize().y, 0.0f);
			if(main_char.shield) glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			else if(main_char.inv_time) glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			else glColor4f(1.0f, 0.5f, 0.5f, 1.0f);
			main_char.sprite->draw();
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glPopMatrix();
		}
		
		if(state == GAME_STATE_START) {
			glLoadIdentity();
			glTranslatef(width / 2.0f, height / 2.0f, 0.0f);
			cd_font->drawCenter(itos(countdown / 1000 + 1));
		}
		
		glPushMatrix();
		glLoadIdentity();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		score_font->drawLeft(itos(main_char.score));
		glTranslatef(width, 0.0f, 0.0f);
		score_font->drawRight(itos(main_char.level));
		glPopMatrix();
		
		if(state == GAME_STATE_GAMEOVER || state == GAME_STATE_PAUSE) {
			glLoadIdentity();
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			glBegin(GL_TRIANGLES);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(0.0f, height);
			glVertex2f(width, height);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(width, 0.0f);
			glVertex2f(width, height);
			glEnd();
			if(state == GAME_STATE_GAMEOVER) {
				glTranslatef(width / 2.0f, height / 2.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				score_font->drawCenter("Game Over!");
			} else if(state == GAME_STATE_PAUSE) {
				glTranslatef(width / 2.0f, height / 2.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				score_font->drawCenter("Pause");
			}
			glLoadIdentity();
			glTranslatef(width / 2.0f, height / 2.0f + 100.0f, 0.0f);
			txt_font->drawCenter("Press Enter to go to the main menu");
		}
	}
	
	SDL_GL_SwapBuffers();
}
