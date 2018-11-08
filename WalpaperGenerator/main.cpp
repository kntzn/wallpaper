#define _CRT_SECURE_NO_WARNINGS

#define EARTH_AXIS 23.44f
#define LAT 56.686452f
#define LON 37.317652f
#define GMT 3


#include <SFML/Graphics.hpp>
#include <ctime>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>

sf::Color multiplex (sf::Color c1, sf::Color c2)
    {
    sf::Color result;
    result.r = sf::Uint8 (int (c1.r) * int (c2.r) / 255);
    result.g = sf::Uint8 (int (c1.g) * int (c2.g) / 255);
    result.b = sf::Uint8 (int (c1.b) * int (c2.b) / 255);

    return result;
    }

sf::Image makeCopyRed (const int sunSize, sf::Image target, float bright)
    {
    // Generates color from height of sun / moon
    sf::Color obj_color;
    obj_color.r = 255;
    obj_color.g = obj_color.b = 127 + 128 * sqrt (bright);

    // Copy of target
    sf::Image sun_cpy = target;

    // Colors copy of image
    for (int i = 0; i < sunSize; i++)
        for (int j = 0; j < sunSize; j++)
            sun_cpy.setPixel (i, j, multiplex (sun_cpy.getPixel (i, j), obj_color));
            
    return sun_cpy;
    }

int main()
	{
    const int sizeX = 1920;
    const int sizeY = 1080;
    const int centerOffset = sizeY / 2;
    const int xOffset = sizeX / 8;

    const int sinLen = sizeX - 2*xOffset;
    const float sinAmpl = float (sizeY) / 8.f;
    
    const int sunSize = 25;
    
    // Gets the current dir
    // I sure that there is much easier way to do it
    std::string thisDirectory = "";
    thisDirectory.append (_pgmptr);
    assert (thisDirectory [(thisDirectory.size () - 1) - 8] == '\\');
    for (int i = 0; i < 8; i++)
        thisDirectory.pop_back ();
    
    // Loads images
    sf::Image sun;
    if (sun.loadFromFile (std::string (thisDirectory + "sun.png").c_str ()))
        std::cout << "Loaded sun.png\n";
    else
        std::cout << "Failed to load sun.png\n";

    sf::Image moon;
    if (moon.loadFromFile (std::string (thisDirectory + "moon.png").c_str ()))
        std::cout << "Loaded moon.png\n\n";
    else
        std::cout << "Failed to load moon.png\n\n";

    while (true)
        {
        // Gets current pc time
        time_t now = time (0);
        tm* Time = localtime (&now);

        // Creates target image
        sf::Image generated_img;
        generated_img.create (sizeX, sizeY, sf::Color::Black);

        // Coords of sun/moon
        int pos = (Time->tm_hour * 60 + Time->tm_min - float (GMT * 15.f - LON) * (60.f / 15.f))*sinLen / (24 * 60);
        
        int y = sinAmpl * cos (float ((pos) * 2 * 3.14159f) / float (sinLen));
        int yMax = - sizeY / 8;
        int yMin =   sizeY / 8;

        // Zero-angle line height
        int h = -float (sizeY / 8) * cos ((Time->tm_yday + 5)*(2.f*3.14159f) / 365) * fabs (EARTH_AXIS / (90 - LAT));

        // Value that represents current sun position
        bool isDay = (y < h);

        // draws a sine wave
        for (int i = -sizeX / 8; i < sinLen + sizeX / 8; i++)
            {
            uint8_t bright = 255 - 255 * pow (float (abs (sizeX / 2 - (i + xOffset))) / float (sizeX), 0.08);
            sf::Color col (bright, bright, bright);

            generated_img.setPixel (i + xOffset, centerOffset - sinAmpl * cos (float ((i + pos) * 2 * 3.14159f) / float (sinLen)), col);

            // Draws the line
            generated_img.setPixel (i + xOffset, centerOffset + h, col);
            }

        // brightness factor
        float bright = 0;
        if (isDay)
            bright = float (h - y) / float (h - yMax);
        else
            bright = float (y - h) / float (yMin - h);

        // Draws sun or moon
        if (isDay)
            generated_img.copy (makeCopyRed (sunSize, sun, bright), 
                                sizeX / 2     - sunSize / 2, 
                                sizeY / 2 + y - sunSize / 2);
        else
            generated_img.copy (makeCopyRed (sunSize, moon, bright), 
                                sizeX / 2     - sunSize / 2,
                                sizeY / 2 + y - sunSize / 2);


        // Saves result
        if (generated_img.saveToFile (std::string (thisDirectory + "bgr.png").c_str ()))
            std::cout << "Saved bgr.png" << std::endl;
        else
            std::cout << "Failed to save bgr.png" << std::endl;
        
        // DO NOT TOUCH
        if (SystemParametersInfoA (SPI_SETDESKWALLPAPER, 0, (PVOID)((thisDirectory + "bgr.png").c_str ()), SPIF_UPDATEINIFILE))
            std::cout << "Wallpaper set" << std::endl;
        else
            std::cout << "Failed to set wallpaper" << std::endl;

        Sleep (10000);
        }

    
    system ("pause");
    
	return 0;
	}