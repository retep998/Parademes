#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <string>
#include <bitset>
using namespace std;

sf::Window window;
bool fullscreen = false;
const int width = 1024, height = 768;
const int cw = width/8, ch = height/16;
const int pixels = width*width/8;
uint8_t data[pixels];
GLfloat red[2] = {0, 0};
GLfloat green[2] = {0.25, 1};
GLfloat blue[2] = {0, 0};
sf::Clock timer;
sf::Time last;
double fps;
uint8_t* font[0x10000];
bitset<0x10000> charfw;
#define TEX
#ifdef TEX
GLuint texture;
#endif

int main() {
	{//Imitialize a few things
		window.Create(sf::VideoMode(width, height), "Land of Parademes", sf::Style::Close, sf::ContextSettings(0, 0, 0, 2, 1));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, red);
		glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, green);
		glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, blue);
		timer.Restart();
		last = timer.GetElapsedTime();
		fps = 0;
#ifdef TEX
		glEnable(GL_TEXTURE_3D);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, width, 0, GL_COLOR_INDEX, GL_BITMAP, 0);
		glLoadIdentity();
		glOrtho(0, 1, 1, 0, -1, 1);
#endif
	}
	while (true) {
		{//First lets obtain our events
			sf::Event e;
			while (window.PollEvent(e)) {
				switch (e.Type) {
				case sf::Event::Closed:
					exit(0);
					break;
				case sf::Event::GainedFocus:
					break;
				case sf::Event::JoystickButtonPressed:
					break;
				case sf::Event::JoystickButtonReleased:
					break;
				case sf::Event::JoystickConnected:
					break;
				case sf::Event::JoystickDisconnected:
					break;
				case sf::Event::JoystickMoved:
					break;
				case sf::Event::KeyPressed:
					switch (e.Key.Code) {
					case sf::Keyboard::Return:
						if (e.Key.Alt) {
							fullscreen = !fullscreen;
							if (fullscreen) {
								window.Create(sf::VideoMode(width, height), "Land of Parademes", sf::Style::Fullscreen);
							} else {
								window.Create(sf::VideoMode(width, height), "Land of Parademes", sf::Style::Close);
							}
							glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, red); 
							glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, green);
							glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, blue);
						} else {
							//Parse command
						}
						break;
					default:
						break;
					}
					break;
				case sf::Event::KeyReleased:
					break;
				case sf::Event::LostFocus:
					break;
				case sf::Event::MouseButtonPressed:
					break;
				}
			}
		}
		{//Display the final result
			generate(data, data+pixels, rand);
#ifdef TEX
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_COLOR_INDEX, GL_BITMAP, &data[0]);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_COLOR_INDEX, GL_BITMAP, &data[0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			glTexCoord2f(1, 0);
			glVertex2f(1, 0);
			glTexCoord2f(1, 0.75);
			glVertex2f(1, 1);
			glTexCoord2f(0, 0.75);
			glVertex2f(0, 1);
			glEnd();
#else
			glDrawPixels(width, height, GL_COLOR_INDEX, GL_BITMAP, &data[0]);
#endif
			sf::Time next = timer.GetElapsedTime();
			fps = fps*0.9 + 100./max(1, (next-last).AsMilliseconds());
			last = next;
			window.SetTitle("Land of Parademes running at "+to_string((long long)fps)+" fps");
			window.Display();
		}
	}
}