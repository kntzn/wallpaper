#define _CRT_SECURE_NO_WARNINGS

#define EARTH_AXIS 23.44f
#define LAT 56.f


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

    const int sunSize = 25;
    sf::Image sun;
    sun.loadFromFile ("sun.png");


    while (true)
        {
        // Gets time
        time_t now = time (0);
        tm* Time = localtime (&now);

        // creates image
        sf::Image generated_img;
        generated_img.create (sizeX, sizeY, sf::Color::Black);

        // coords of sun/moon
        int pos = (Time->tm_hour*60 + Time->tm_min)*sinLen/(24*60);
        int y = sizeY / 2 + float (sizeY / 8) * cos (float ((pos) * 2 * 3.14159f) / float (sinLen));

        int h = -float (sizeY / 8) * cos ((Time->tm_yday + 9)*(2.f*3.14159f) / 365) * fabs (EARTH_AXIS / (90 - LAT));

        // draws a sine wave
        for (int i = 0; i < sinLen; i++)
            {
            uint8_t bright = 255 - 255 * pow (float (abs (pos - i)) / float (sinLen), 0.125);
            sf::Color col (bright, bright, bright);
            generated_img.setPixel (i + sizeX / 8, sizeY / 2 + float (sizeY / 8) * cos (float ((i) * 2 * 3.14159f) / float (sinLen)), col);
            generated_img.setPixel (i + sizeX / 8, sizeY / 2 + h, col);
            }

        // draws a sun
        for (int i = pos; i < pos + sunSize; i++)
            for (int j = y; j < y + sunSize; j++)
                generated_img.setPixel (i - sunSize / 2 + sizeX / 8, j - sunSize / 2, sun.getPixel (i - pos, j - y));

        generated_img.saveToFile ("C:\\wp\\bgr.png");

        SystemParametersInfo (SPI_SETDESKWALLPAPER, 0, L"C:\\wp\\bgr.png", SPIF_UPDATEINIFILE);

        Sleep (60000);
        }

    
    system ("pause");
    
	return 0;
	}