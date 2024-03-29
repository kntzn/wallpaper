#define _CRT_SECURE_NO_WARNINGS

#define EARTH_AXIS 23.44f
#define LAT 55.911186 //56.686452f 
#define LON 43.791329 //37.317652f
#define GMT 3
#define N_BELTS 24
#define MIN_PER_DEGREE (60.f / (360.f / N_BELTS))
#define MIN_PER_DAY 1440
#define UPM 3
#define USAGE_VISUALIZATION_MINUTES MIN_PER_DAY/2
#define YEARDATE_OFFSET 11

#define ver "1.0.3"

#include <SFML/Graphics.hpp>
#include <ctime>
#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
//#include "ImageGenerator.h"

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

// Current time in minutes since 0:00
float getCurrentTime ()
    {
    // Gets current pc time
    time_t now = time (0);
    tm* Time = localtime (&now);

    return float (Time->tm_hour) * 60.f + 
           float (Time->tm_min)  * 1.f  + 
           float (Time->tm_sec)  / 60.f;
    }

// Local (astronomical) time in minutes in this time belt
float getCurrentLocalTime ()
    {
    return (getCurrentTime ()) + float (LON - GMT * 15.f)*MIN_PER_DEGREE;
    }

int getAstronomicalYearDate ()
    {
    // Gets current pc time
    time_t now = time (0);
    tm* Time = localtime (&now);

    return Time->tm_yday + YEARDATE_OFFSET/2;
    }

float getLineHeight (float sinAmpl)
    {
    return float (-sinAmpl * cos (float (getAstronomicalYearDate ())*(2.f*3.14159f) / 365) * fabs (EARTH_AXIS / (90.f - LAT)));
    }

int main()
	{
    std::cout << "WPGN (Wallpaper generator)\n"
                 "Ver. " << ver << std::endl;

    // ParamEters
    const int sizeX = 1920;
    const int sizeY = 1080;
    const int centerOffset = sizeY / 2;
    const int xOffset = sizeX/8;

    const int sinLen = sizeX - 2*xOffset;
    const float sinAmpl = float (sizeY) / 8.f;
    
    const int sunSize = 25;
    
    // Gets the current dir
    // I'm sure that the much easier way to do this exists
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

        // Creates target image
        sf::Image generated_img;
        generated_img.create (sizeX, sizeY, sf::Color::Black);
      
        // -------- CALCULATIONS -------- //

        // Astronomical time in minutes by module MIN_PER_DAY
        float currentLocalTime = getCurrentLocalTime ();
        float currentLocalTimeByMod = (currentLocalTime > 0)? currentLocalTime : 
                                                              currentLocalTime + float (MIN_PER_DAY);

        // Coords of sun/moon
        float x = float (sinLen)*(currentLocalTimeByMod / float (MIN_PER_DAY));
        float y = sinAmpl * cos (float ((x) * 2 * 3.14159f) / float (sinLen));
        
        // Zero-angle line height
        float h = getLineHeight (sinAmpl);

        // Value that represents current sun position
        bool isDay = (y < h);

        // brightness factor
        float sun_bright = 0;
        if (isDay)
            sun_bright = (h - y) / float (h + sizeY / 8);
        else
            sun_bright = (y - h) / float (sizeY / 8 - h);


        // moon offset from sun position
        float moon_x_offset = (float (getAstronomicalYearDate ()) + 11 + 356.f) / 29.3f * float (sinLen);
        while (moon_x_offset > sinLen / 2.f)
            moon_x_offset -= sinLen;


        // Moon
        float moon_x = x + moon_x_offset;
        
        float moon_y = 200;

        

        // -------- !CALCULATIONS -------- //

        std::cout << "LAT: " << float (-y - (-getLineHeight (sinAmpl))) * ((90.f - LAT) / sinAmpl) << " LON:" << currentLocalTimeByMod / MIN_PER_DAY * 360 << "\n";

        // -------- GRAPHICS -------- //

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
            
            // Colors the pixel 
            if (i <= sizeX / 2)
                if (usage [(timeIndexFromIter + int (x) + MIN_PER_DAY) % MIN_PER_DAY])
                    col = (bright > 63) ? sf::Color (255, 127, 0) : sf::Color (bright * 4, bright * 2, 0);
                
            // Draws the sine
            generated_img.setPixel (i, centerOffset - int (sinAmpl * cos (float ((i + x - xOffset) * 2 * 3.14159f) / float (sinLen))), col);
            }
       
        // Draws sun or moon
        sf::Image* currentImg = nullptr;
        currentImg = &sun;

        generated_img.copy (makeCopyRed (sunSize, *currentImg, sun_bright),
                            sizeX / 2           - sunSize / 2,
                            sizeY / 2 + int (y) - sunSize / 2);


        //if (isDay)

        //else
        //currentImg = &moon;

        //generated_img.copy (moon, moon_x, moon_y);


        // -------- !GRAPHICS -------- //

        // -------- USAGE -------- //

        // Sets current usage
        usage [int (currentLocalTimeByMod)] = USAGE_VISUALIZATION_MINUTES;

        // Reduces old values
        for (int i = 0; i < MIN_PER_DAY; i++)
            if (usage [i] > 0)
                usage [i] -= (MIN_PER_DAY + int (currentLocalTimeByMod) - lastUsageTime) % MIN_PER_DAY;
            else
                usage [i] = 0;

        // Updates last usage
        lastUsageTime = int (currentLocalTimeByMod);

        // -------- !USAGE -------- //

        // -------- WALLPAPER -------- //

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

        // -------- !WALLPAPER -------- //

        // -------- DELAY -------- //
        if (dt < 60 * 1000 / UPM)
            Sleep (60 * 1000 / UPM - dt);
        // -------- !DELAY -------- //
        }

    
    system ("pause");
    
	return 0;
	}