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

#ifndef __Windexing__Crystal__
#define __Windexing__Crystal__
#include "mat3x3.h"
#include <iostream>
#include "shared_ptrs.h"

#define STARTING_WAVELENGTH 1.000
#define STARTING_DISTANCE 500.000

typedef struct
{
	vec3 miller; // the transformed coordinates in reciprocal space
	vec3 position; // updated by detector when needed
	int h;
	int k;
	int l; // before transformation on a integer grid
	double weight; // proportional to closeness to Ewald sphere
	bool onImage; // whether it is to be displayed on overlay
	bool watched;
} Reflection;

class Tinker;

class Crystal
{
public:
    Crystal();

    void setUnitCell(std::vector<double> cellDims);
    void bringAxisToScreen(std::vector<double> axis);
    
    void applyRotation(double diffX, double diffY, double diffZ);
    mat3x3 getScaledBasisVectors();
    mat3x3 getNudge(double diffX, double diffY, double diffZ);
    void clearUpRefinement();
    bool isBeingWatched(int i);
    void quickCheckMillers();
    void populateMillers();
    

    static double ewaldSphereClosenessScore(void *crystal)
    {
        return static_cast<Crystal *>(crystal)->ewaldSphereCloseness();
    }
   
    static void setHorizontal(void *crystal, double horiz)
    {
        static_cast<Crystal *>(crystal)->_horiz = horiz;
    }
    
    static void setVertical(void *crystal, double vert)
    {
        static_cast<Crystal *>(crystal)->_vert = vert;
    }
    
    static double getVertical(void *crystal)
    {
        return static_cast<Crystal *>(crystal)->_vert;
    }
    
    static double getHorizontal(void *crystal)
    {
        return static_cast<Crystal *>(crystal)->_horiz;
    }
    
    
    void setResolution(double resolution)
    {
        _resolution = resolution;
    }
    
    Reflection *refl(int i)
    {
        return &_reflections[i];
    }
    
    size_t millerCount()
    {
        return _reflections.size();
    }
    
    vec3 miller(int i)
    {
        //Transformed into reciprocal space. Already fractional.
        return _reflections[i].miller;
    }

	void setPositionForMiller(int i, vec3 pos)
	{
		_reflections[i].position = pos;
	}

	vec3 position(int i)
	{
		return _reflections[i].position;
	}

	void toggleWatched(int i)
	{
		_reflections[i].watched = ((_reflections[i].watched == 0) ? 1 : 0);
	}
    
    void getMillerHKL(int i, int *h, int *k, int *l)
    {
        *h = _reflections[i].h;
        *k = _reflections[i].k;
        *l = _reflections[i].l;
    }
    

    double weightForMiller(int i)
    {
        return _reflections[i].weight;
    }

	bool shouldDisplayMiller(int i)
	{
		return _reflections[i].onImage;
	}
    
    void setFixedAxis(vec3 axis)
    {
        _fixedAxis = axis;
    }
    
    vec3 getFixedAxis()
    {
        return _fixedAxis;
    }
    
    void setWavelength(double wavelength)
    {
        _wavelength = wavelength;
    }
    
    double getRlpSize()
    {
	return _rlpSize;
    }
    
    void setRlpSize(double rlpSize)
    {
        _rlpSize = rlpSize;
    }
    
    
    void setTinker(Tinker *tinker)
    {
        _tinker = tinker;
    }
    
    mat3x3 getRotation()
    {
        return _rotation;
    }
    
    void setRotation(mat3x3 rot)
    {
        _rotation = rot;
    }
    
    mat3x3 getUnitCell()
    {
    	return _unitCell;
    }
    
    void setUnitCell(mat3x3 unitCell);

    void setBravaisLattice(BravaisLatticeType type)
    {
        _latticeType = type;
    }

private:
    double ewaldSphereCloseness();
    bool isSysabs(int a, int b, int c);
    Tinker *_tinker;

    std::vector<double> _cellDims;
    mat3x3 _rotation;
    mat3x3 _unitCell;

	std::vector<Reflection> _reflections;

    double _resolution;
    double _rlpSize;
    double _wavelength;
    
    double _horiz;
    double _vert;
    BravaisLatticeType _latticeType;
    
    static vec3 _cube[8];
    vec3 _fixedAxis;
};


#endif
