//
//  Detector.h
//  Windexing
//
//  Created by Helen Ginn on 11/12/2016.
//  Copyright (c) 2017 Helen Ginn. All rights reserved.
//

#ifndef __Windexing__Detector__
#define __Windexing__Detector__

#include "mat3x3.h"
#include <vector>
#include <iostream>

class Crystal;

class Detector
{
public:
    Detector();
    
    void calculatePositions();
    int positionNearCoord(int x, int y);
    
    vec3 getBeamCentre()
    {
        return _beamCentre;
    }
      
    void setBeamCentre(double x, double y)
    {
        _beamCentre.x = x;
        _beamCentre.y = y;
    }
    
    void setDetectorDistance(double z)
    {
        _beamCentre.z = z;
    }
    
    void setWavelength(double wavelength)
    {
        _wavelength = wavelength;
    }
    
    void adjustBeamCentre(double x, double y)
    {
        _beamCentre.x += x;
        _beamCentre.y += y;
        std::cout << "New beam centre " << _beamCentre.x << " "
         << _beamCentre.y << std::endl;
    }

	void setCrystal(Crystal *pointer)
	{
		_xtal = pointer;
	}
private:
	Crystal *_xtal;
    vec3 _beamCentre; // beam X, beam Y, det dist. all pix
    double _wavelength;
    std::vector<vec3> _positions;
    
};


#endif
