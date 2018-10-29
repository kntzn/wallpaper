#include <SFML/Graphics.hpp>
#include <ctime>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>

int main()
	{
    const int sizeX = 1920;
    const int sizeY = 1080;
    const int sinLen = sizeX * 3 / 4;

    while (true)
        {
        sf::Image generated_img;
        generated_img.create (sizeX, sizeY, sf::Color::Black);
    
        // coords of sun/moon
        int pos = rand()%sinLen;
        int y = sizeY / 2 + float (sizeY / 8) * cos (float ((pos) * 2 * 3.14159f) / float (sinLen));

        // draws a sine wave
        for (int i = 0; i < sinLen; i++)
            {
            uint8_t bright = 255 - 255 * pow (float (abs (pos - i)) / float (sinLen), 0.125);
            sf::Color col (bright, bright, bright);
            generated_img.setPixel (i + sizeX / 8, sizeY / 2 + float (sizeY / 8) * cos (float ((i) * 2 * 3.14159f) / float (sinLen)), col);
            }

        generated_img.saveToFile ("C:\\wp\\bgr.png");

        SystemParametersInfo (SPI_SETDESKWALLPAPER, 0, L"C:\\wp\\bgr.png", SPIF_UPDATEINIFILE);

        Sleep (1000);
        }

    
    system ("pause");
    
	return 0;
	}