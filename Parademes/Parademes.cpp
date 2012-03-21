#include <SDL/SDL.h>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <string>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <array>
#include <vector>
#include <random>
using namespace std;

bool fullscreen = false;
const int width = 1024, height = 768;
const int cw = width/8, ch = height/16;
SDL_Surface* font;
bitset<0x10000> charfw;
SDL_Color colors[2] = {{0, 0, 0, 255}, {0, 255, 0, 255}};
SDL_Surface* window;
clock_t last;
clock_t lsave;
array<array<char16_t, ch>, cw> text;
u16string etext;
double fps = 0;
bool editor = false;
array<array<char16_t, 16>, 64> palette;
int offset = 0;
char16_t selected = 0;
int sx = 0;
int sy = 0;
bool ldown = false;
bool rdown = false;
mt19937 engine;
uniform_int_distribution<char16_t> dist(0, 0xFFFF);

void save() {
	if (editor) {

	} else {

	}
}

void load() {
	if (editor) {

	} else {

	}
}

void quit() {
	save();
	SDL_Quit();
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc >= 2) if (string(argv[1]) == "editor") editor = true;
	load();
	{//Font stuff
		font = SDL_CreateRGBSurface(SDL_SWSURFACE, 0x1000, 0x1000, 1, 0, 0, 0, 0);
		ifstream fontf("unifont.hex");
		int lid = 0;
		while (!fontf.eof()) {
			string sid(4, '0');
			fontf.get(&sid[0], 5);
			int id = stoi(sid, 0, 16);
			for (int i = id; i < lid; ++i) {
				charfw[id] = false;
			}
			lid = id;
			fontf.ignore(1);
			string s;
			getline(fontf, s);
			if (s.size() == 32) {
				charfw[id] = false;
				uint8_t* c = (uint8_t*)font->pixels+(id&0xFF)*0x2+(id&0xFF00)*0x20;
				for (int i = 0; i < 16; ++i) {
					c[i*0x200] = stoi(s.substr(i*2, 2), 0, 16);
				}
			} else if (s.size() == 64) {
				charfw[id] = true;
				uint8_t* c = (uint8_t*)font->pixels+(id&0xFF)*0x2+(id&0xFF00)*0x20;
				for (int i = 0; i < 16; ++i) {
					c[i*0x200] = stoi(s.substr(i*4, 2), 0, 16);
					c[i*0x200+1] = stoi(s.substr(i*4+2, 2), 0, 16);
				}
			} else {
				throw(273);
			}
			if (id == 0xFFFF) break;
		}
		charfw[0] = false;
	}
	{//SDL init
		SDL_Init(SDL_INIT_VIDEO);
		if (editor) {
			SDL_putenv("SDL_VIDEO_WINDOW_POS=0,0");
			window = SDL_SetVideoMode(0x602, 0x402, 8, SDL_SWSURFACE|SDL_NOFRAME);
		} else {
			window = SDL_SetVideoMode(0x400, 0x300, 8, SDL_SWSURFACE);
		}
		SDL_SetColors(font, colors, 0, 2);
		SDL_EnableUNICODE(true);
		last = clock();
		lsave = last;
	}
	while (true) {//The game loop
		if (editor) {//Events for the editor
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch(e.type) {
				case SDL_QUIT:
					quit();
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_s) {
						save();
					} else if (e.key.keysym.sym == SDLK_l) {
						load();
					} else if (e.key.keysym.sym == SDLK_F4 && e.key.keysym.mod & KMOD_ALT) {
						quit();
					} else if (e.key.keysym.sym == SDLK_ESCAPE) {
						quit();
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						int x = e.button.x;
						int y = e.button.y;
						switch (e.button.button) {
						case SDL_BUTTON_LEFT:
							ldown = true;
							if (x >= 1024) {
								palette[sx][sy] = (x-1024)/16+(y/16+offset)*32;
								selected = palette[sx][sy];
							} else if (y > 770) {
								sx = x/16;
								sy = (y-770)/16;
								selected = palette[sx][sy];
							} else if (y < 768) {
								if (x/8 > 0 && charfw[text[x/8-1][y/16]]) break;
								if (x/8+1 == cw && charfw[selected]) break;
								if (x/8+1 < cw && charfw[selected] && text[x/8+1][y/16]) break;
								text[x/8][y/16] = selected;
							}
							break;
						case SDL_BUTTON_RIGHT:
							rdown = true;
							if (y < 768 && x < 1024) {
								text[x/8][y/16] = 0;
								if (x/8 > 0 && charfw[text[x/8-1][y/16]]) text[x/8-1][y/16] = 0;
							}
							break;
						case SDL_BUTTON_WHEELUP:
							offset = max(0, offset-16);
							break;
						case SDL_BUTTON_WHEELDOWN:
							offset = min(0x10000/32-64, offset+16);
							break;
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
					{
						switch (e.button.button) {
						case SDL_BUTTON_LEFT:
							ldown = false;
							break;
						case SDL_BUTTON_RIGHT:
							rdown = false;
							break;
						}
					}
					break;
				case SDL_MOUSEMOTION:
					{
						int x = e.button.x;
						int y = e.button.y;
						if (ldown && y < 768 && x < 1024) {
							if (x/8 > 0 && charfw[text[x/8-1][y/16]]) break;
							if (x/8+1 == cw && charfw[selected]) break;
							if (x/8+1 < cw && charfw[selected] && text[x/8+1][y/16]) break;
							text[x/8][y/16] = selected;
						}
						if (rdown && y < 768 && x < 1024) {
							text[x/8][y/16] = 0;
							if (x/8 > 0 && charfw[text[x/8-1][y/16]]) text[x/8-1][y/16] = 0;
						}
						break;
					}
					break;
				}
			}
		} else {
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch(e.type) {
				case SDL_QUIT:
					quit();
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_RETURN) {
						if (e.key.keysym.mod & KMOD_ALT) {
							if (!editor) {
								fullscreen = !fullscreen;
								if (fullscreen) window = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE|SDL_FULLSCREEN);
								else window = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE);
								SDL_SetPalette(window, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 2);
							}
						} else {
							//Parse command
						}
					} else if (e.key.keysym.sym == SDLK_BACKSPACE) {
						if (!etext.empty())	etext.erase(etext.end()-1);
					} else if (e.key.keysym.sym == SDLK_F4 && e.key.keysym.mod & KMOD_ALT) {
						quit();
					} else if (e.key.keysym.sym == SDLK_ESCAPE) {
						quit();
					} else if (e.key.keysym.unicode) {
						etext.push_back(e.key.keysym.unicode);
					}
					break;
				}
			}
		}
		{//Display the final result
			if (editor){
				SDL_FillRect(window, nullptr, SDL_MapRGB(window->format, 0, 31, 0));
				SDL_Rect r1 = {0, 768, 1024, 2};
				SDL_FillRect(window, &r1, SDL_MapRGB(window->format, 0, 255, 0));
				SDL_Rect r2 = {1024, 0, 2, 1538};
				SDL_FillRect(window, &r2, SDL_MapRGB(window->format, 0, 255, 0));
				for (int y = 0; y < 64; ++y) {
					for (int x = 0; x < 32; ++x) {
						char16_t c = x+(y+offset)*32;
						SDL_Rect srect = {(int16_t)((c&0xFF)<<1), (int16_t)((c&0xFF00)>>4), 16, 16};
						SDL_Rect drect = {(int16_t)(x*16+1026), (int16_t)(y*16), 16, 16};
						SDL_BlitSurface(font, &srect, window, &drect);
					}
				}
				for (int y = 0; y < 16; ++y) {
					for (int x = 0; x < 64; ++x) {
						char16_t c = palette[x][y];
						SDL_Rect srect = {(int16_t)((c&0xFF)<<1), (int16_t)((c&0xFF00)>>4), 16, 16};
						SDL_Rect drect = {(int16_t)(x*16), (int16_t)(y*16+770), 16, 16};
						SDL_BlitSurface(font, &srect, window, &drect);
					}
				}
				if (clock()%CLOCKS_PER_SEC < CLOCKS_PER_SEC/3) {
					SDL_Rect r = {sx*16, 770+sy*16, 16, 16};
					SDL_FillRect(window, &r, SDL_MapRGB(window->format, 0, 200, 0));
				}
			} else {
				SDL_FillRect(window, nullptr, 0);
				transform(text.begin(), text.end(), text.begin(), [](array<char16_t, ch> text)->array<char16_t, ch>{transform(text.begin(), text.end(), text.begin(), [](char16_t)->char16_t{return dist(engine);}); return text;});
			}
			for (int y = 0; y < ch; ++y) {
				for (int x = 0; x < cw; ++x) {
					char16_t c = text[x][y];
					SDL_Rect srect = {(int16_t)((c&0xFF)<<1), (int16_t)((c&0xFF00)>>4), 8, 16};
					SDL_Rect drect = {(int16_t)(x*8), (int16_t)(y*16), 8, 16};
					if (charfw[c] && x+1 != cw) {
						srect.w = 16;
						drect.w = 16;
					}
					if (c) SDL_BlitSurface(font, &srect, window, &drect);
				}
			}
			SDL_Flip(window);
		}
		{//Time
			clock_t next = clock();
			SDL_WM_SetCaption(to_string((long long)fps).c_str(), "Land of Parademes");
			fps = 0.9*fps + 0.1*(double)CLOCKS_PER_SEC/max(1, (int)(next-last));
			last = next;
			SDL_Delay(1);
		}
	}
}