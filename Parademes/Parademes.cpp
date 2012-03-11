#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

sf::Window window;
bool fullscreen = false;
const int width = 1024, height = 768;
const int cw = width/768, ch = height/16;
const int pixels = width*width/8;
uint8_t data[pixels];
GLfloat red[2] = {0, 0};
GLfloat green[2] = {0.25, 1};
GLfloat blue[2] = {0, 0};
sf::Clock timer;
sf::Time last;
double fps;

int main() {
	{//Imitialize a few things
		window.Create(sf::VideoMode(width, height), "Land of Parademes", sf::Style::Close, sf::ContextSettings(0, 0, 0, 2, 1));
		glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, red); 
		glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, green);
		glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, blue);
		timer.Restart();
		last = timer.GetElapsedTime();
		fps = 0;
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
				}
			}
		}
		{//Display the final result
			generate(data, data+pixels, rand);
			glDrawPixels(width, height, GL_COLOR_INDEX, GL_BITMAP, &data[0]);
			sf::Time next = timer.GetElapsedTime();
			fps = fps*0.9 + 100./max(1, (next-last).AsMilliseconds());
			last = next;
			window.SetTitle("Land of Parademes running at "+to_string((long long)fps)+" fps");
			window.Display();
		}
	}
}