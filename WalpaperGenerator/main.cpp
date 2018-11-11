#define _CRT_SECURE_NO_WARNINGS

#define EARTH_AXIS 23.44f
#define LAT 56.686452f
#define LON 37.317652f
#define GMT 3
#define N_BELTS 24
#define MIN_PER_DEGREE (60.f / (360.f / N_BELTS))
#define MIN_PER_DAY 1440
#define UPM 3
#define USAGE_VISUALIZATION_MINUTES (MIN_PER_DAY / 4) * UPM

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
    obj_color.r = sf::Uint8 (255);
    obj_color.g = obj_color.b = sf::Uint8 (127) + sf::Uint8 (128 * sqrt (bright));

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
    // Parameters
    const int sizeX = 1920;
    const int sizeY = 1080;
    const int centerOffset = sizeY / 2;
    const int xOffset = sizeX/8;

    const int sinLen = sizeX - 2*xOffset;
    const float sinAmpl = float (sizeY) / 8.f;
    
    const int sunSize = 25;
    
    // Gets the current dir
    // I sure that there is much easier way to do it
    std::string thisDirectory = _pgmptr;
    while (thisDirectory [(thisDirectory.size () - 1)] != '\\')
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

    // Array of usage
    int usage [MIN_PER_DAY] = {};
    
    // Time of last usage update
    int lastUsageTime = 0;

    while (true)
        {
        // Execution time timer
        clock_t exec_start = clock ();

        // Gets current pc time
        time_t now = time (0);
        tm* Time = localtime (&now);

        // Creates target image
        sf::Image generated_img;
        generated_img.create (sizeX, sizeY, sf::Color::Black);

        // Current time in minutes since 0:00
        int currentTime = Time->tm_hour * 60 + Time->tm_min;
        // Local (astronomical) time in minutes in this time belt
        float currentLocalTime = (float)(currentTime) + float (LON - GMT * 15.f)*MIN_PER_DEGREE;
        // Astronomical time in minutes by module MIN_PER_DAY
        int currentLocalTimeByMod = int (currentLocalTime + MIN_PER_DAY) % MIN_PER_DAY;

        // sets usage array to max value at current point
        usage [int (currentLocalTime + 24 * 60) % (24 * 60)] = true;

        // Coords of sun/moon
        int pos = int (float (sinLen)*(currentLocalTime / float (24 * 60)));
        
        int y = int (sinAmpl * cos (float ((pos) * 2 * 3.14159f) / float (sinLen)));
        int yMax = - sizeY / 8;
        int yMin =   sizeY / 8;

        // Zero-angle line height
        int h = int (-sinAmpl * cos ((Time->tm_yday + 5)*(2.f*3.14159f) / 365) * fabs (EARTH_AXIS / (90.f - LAT)));

        // Value that represents current sun position
        bool isDay = (y < h);

        // draws a sine wave
        for (int i = 0; i < sizeX; i++)
            {
            sf::Uint8 bright = 255 - sf::Uint8 (255 * pow (float (abs (sizeX / 2 - (i))) / float (sizeX), 0.08));
            sf::Color col (bright, bright, bright);

            // Draws the line
            generated_img.setPixel (i, centerOffset + h, col);
            
            // Colors the sine if pc was used
            
            // Scaled array index
            int timeIndexFromIter = (i - sizeX / 2) * MIN_PER_DAY / sinLen;
            
            if (usage [(timeIndexFromIter + pos + MIN_PER_DAY) % MIN_PER_DAY])
                col = sf::Color (100, 0, 255);
                
            // Draws the sine
            generated_img.setPixel (i, centerOffset - int (sinAmpl * cos (float ((i + pos - xOffset) * 2 * 3.14159f) / float (sinLen))), col);
            }
        
        // Sets current usage
        usage [currentLocalTimeByMod] = USAGE_VISUALIZATION_MINUTES;

        for (int i = 0; i < MIN_PER_DAY; i++)
            if (usage [i] > 0)
                usage [i]--;

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

        clock_t dt = clock () - exec_start;

        if (dt < 60 * 1000 / UPM)
            Sleep (60 * 1000 / UPM - dt);
        }

    
    system ("pause");
    
	return 0;
	}