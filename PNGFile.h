// Mandexing: a manual indexing program for crystallographic data.
// Copyright (C) 2017-2018 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#ifndef __CellCounter__Image__
#define __CellCounter__Image__

#include <vector>
#include <map>
#include <stdio.h>
#include <png.h>
#include <string>
#include "shared_ptrs.h"

typedef enum
{
    FlagNone, FlagThreshold
} Flag;

class PNGFile
{
private:
    png_bytep data;
    int height;
    int centreX;
    int centreY;
    bool plain;
    std::string filename;
    std::string rootName;
    int bytesPerPixel;
    int pixelsPerRow;
    int writeImage(std::string filename, int width, int height, std::string title);
    void read_png_file(const char *file_name);
    void pixelAt(int x, int y, png_byte **bytes);
    png_byte valueAt(int x, int y);
    bool pixelReachesThreshold(int x, int y);
    void setPixelForChannel(int x, int y, int channel, png_byte value);
    void readImage();
    void moveCoordRelative(int *x, int *y);
public:
    void dropImage();
    void preProcess();
    void writeImageOutput();
    void process();
    void setPixelColour(int x, int y, png_byte red, png_byte green, png_byte blue, float transparency = 1);
    void setPixelColourRelative(int x, int y, png_byte red, png_byte green, png_byte blue);
	void RGB_to_HSB(float red, float green, float blue, float *brightness, float *saturation, float *hue);
	void invertColourRelative(int x, int y);
    void drawLine(int x1, int y1, int x2, int y2, float transparency, png_byte red, png_byte green, png_byte blue);
    void drawCircleAroundPixel(int x, int y, float radius, float transparency, png_byte red, png_byte green, png_byte blue, float thickness = 3);
    void drawText(std::string text, int xCentre, int yCentre, png_byte red = 0, png_byte green = 0, png_byte blue = 0);
    static void HSB_to_RGB(float hue, float sat, float bright,
                           png_byte *red, png_byte *green, png_byte *blue);
	void drawArrow(float xDir, float yDir, float centreX, float centreY, float transparency, png_byte red, png_byte green, png_byte blue);
    
    PNGFile(std::string filename, int width = 2400, int height = 2400);
    
    void setCentre(int newX, int newY)
    {
        centreX = newX;
        centreY = newY;
    }
    
    void getCentre(int *x, int *y)
    {
        *x = centreX;
        *y = centreY;
    }
    
    void setPlain()
    {
        plain = true;
    }
};



#endif /* defined(__CellCounter__Image__) */
