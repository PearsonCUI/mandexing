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

#ifndef __Windexing__Detector__
#define __Windexing__Detector__

#include "mat3x3.h"
#include <vector>
#include <iostream>
#include "Node.h"

class Crystal;

class Detector
{
public:
    Detector();
	~Detector();
    
    void calculatePositions();
    int positionNearCoord(int x, int y);
	void prepareLookupTable();

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
    
    double getWavelength()
    {
	return _wavelength;
    }
    
    void setWavelength(double wavelength)
    {
        _wavelength = wavelength;
	std::cout << "Setting wavelength to " << wavelength << std::endl;
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
	bool nearMiller(int i, int x, int y);
	double distToMiller(int i, int x, int y);

	Node *_lookupTree;
};


#endif
