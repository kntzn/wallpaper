#define _CRT_SECURE_NO_WARNINGS

#define EARTH_AXIS 23.44f
#define LAT 56.f


#include <SFML/Graphics.hpp>
#include <ctime>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>

sf::Color multiplex (sf::Color c1, sf::Color c2)
    {
    sf::Color result;
    result.r = sf::Uint8 (int (c1.r) * int (c2.r) / 255);
    result.g = sf::Uint8 (int (c1.g) * int (c2.g) / 255);
    result.b = sf::Uint8 (int (c1.b) * int (c2.b) / 255);

    return result;
    }

int main()
	{
    const int sizeX = 1920;
    const int sizeY = 1080;
    const int sinLen = sizeX * 3 / 4;

    const int sunSize = 25;
    sf::Image sun;
    if (sun.loadFromFile ("sun.png"))
        std::cout << "Loaded sun.png\n";
    else
        std::cout << "Failed to load sun.png\n";

    sf::Image moon;
    if (moon.loadFromFile ("moon.png"))
        std::cout << "Loaded moon.png\n";
    else
        std::cout << "Failed to load moon.png\n";

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
        int yMax = sizeY / 2 - sizeY / 8;
        int yMin = sizeY / 2 + sizeY / 8;


        int h = -float (sizeY / 8) * cos ((Time->tm_yday + 9)*(2.f*3.14159f) / 365) * fabs (EARTH_AXIS / (90 - LAT));
        
        // true if sun mode, false if moon mode
        bool isDay = (y < h + sizeY / 2);

        // draws a sine wave
        for (int i = -sizeX / 8; i < sinLen + sizeX / 8; i++)
            {
            uint8_t bright = 255 - 255 * pow (float (abs (pos - i)) / float (sizeX), 0.1);
            sf::Color col (bright, bright, bright);
            generated_img.setPixel (i + sizeX / 8, sizeY / 2 + float (sizeY / 8) * cos (float ((i) * 2 * 3.14159f) / float (sinLen)), col);
            generated_img.setPixel (i + sizeX / 8, sizeY / 2 + h, col);
            }

        
        // brightness factor
        float bright = 0;
        if (isDay)
            {
            bright = float ((sizeY / 2 + h) - y) / float ((sizeY / 2 + h) - yMax);
            }
        else
            {
            bright = float (y - (sizeY / 2 + h)) / float (yMin - (sizeY / 2 + h));
            }
        
        // generates color from height of sun / moon
        sf::Color obj_color;
        obj_color.r = 255;
        obj_color.g = obj_color.b = 127 + 128 * sqrt (bright);

        // draws a sun or moon
        for (int i = pos; i < pos + sunSize; i++)
            for (int j = y; j < y + sunSize; j++)
                {
                if (isDay)
                    generated_img.setPixel (i - sunSize / 2 + sizeX / 8, j - sunSize / 2, multiplex (obj_color, sun.getPixel (i - pos, j - y)));
                else
                    generated_img.setPixel (i - sunSize / 2 + sizeX / 8, j - sunSize / 2, multiplex (obj_color, moon.getPixel (i - pos, j - y)));
                }

        generated_img.saveToFile ("C:\\wp\\bgr.png");

        SystemParametersInfo (SPI_SETDESKWALLPAPER, 0, L"C:\\wp\\bgr.png", SPIF_UPDATEINIFILE);

        Sleep (10000);
        }

    
    system ("pause");
    
	return 0;
	}