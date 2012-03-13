#include <SDL.h>
#include <SDL_main.h>
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
using namespace std;

bool fullscreen = false;
const int width = 1024, height = 768;
const int cw = width/8, ch = height/16;
SDL_Surface* font;
bitset<0x10000> charfw;
SDL_Color colors[2] = {{0, 15, 0, 255}, {0, 255, 0, 255}};
SDL_Surface* window;
clock_t last;
array<array<char16_t, ch>, cw> text;
u16string etext;
double fps = 0;

int main(int argc, char* args[]) {
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
	}
	{//SDL init
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE);
		SDL_SetColors(font, colors, 0, 2);
		SDL_EnableUNICODE(true);
		last = clock();
	}
	while (true) {
		{//First lets obtain our events
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch(e.type) {
				case SDL_QUIT:
					SDL_Quit();
					exit(0);
					break;
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_RETURN) {
						if (e.key.keysym.mod & KMOD_ALT) {
							fullscreen = !fullscreen;
							if (fullscreen) window = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE|SDL_FULLSCREEN);
							else window = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE);
							SDL_SetPalette(window, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 2);
						} else {
							//Parse command
						}
					} else if (e.key.keysym.sym == SDLK_BACKSPACE) {
						etext.pop_back();
					} else if (e.key.keysym.unicode) {
						etext.push_back(e.key.keysym.unicode);
					}
					break;
				}
			}
		}
		{//Display the final result
			transform(text.begin(), text.end(), text.begin(), [](array<char16_t, ch> text)->array<char16_t, ch>{transform(text.begin(), text.end(), text.begin(), [](char16_t)->char16_t{return rand();}); return text;});
			for (int y = 0; y < ch; ++y) {
				for (int x = 0; x < cw; ++x) {
					char16_t c = text[x][y];
					SDL_Rect srect = {(c&0xFF)<<1, (c&0xFF00)>>4, 8, 16};
					SDL_Rect drect = {x*8, y*16, 8, 16};
					if (charfw[c]) {
						srect.w = 16;
						drect.w = 16;
					}
					SDL_BlitSurface(font, &srect, window, &drect);
				}
			}
			SDL_Flip(window);
		}
		{//Time
			clock_t next = clock();
			SDL_WM_SetCaption(to_string((long long)fps).c_str(), "Land of Parademes");
			fps = 0.9*fps + 0.1*(double)CLOCKS_PER_SEC/max(1, (int)(next-last));
			last = next;
		}
	}
}