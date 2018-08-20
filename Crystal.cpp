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

#include "Crystal.h"
#include "defaults.h"
#include "mat3x3.h"
#include <iostream>
#include "Tinker.h"
#include <QtCore/qcoreapplication.h>
	

vec3 Crystal::_cube[] = 
{{-0.5, -0.5, -0.5},
{-0.5, -0.5, 0.5},
{-0.5, 0.5, -0.5},
{-0.5, 0.5, 0.5},
{0.5, -0.5, -0.5},
{0.5, -0.5, 0.5},
{0.5, 0.5, -0.5},
{0.5, 0.5, 0.5}};

bool Crystal::isSysabs(int a, int b, int c)
{
    if (_latticeType == BravaisLatticePrimitive)
    {
        return false;
    }
    else if (_latticeType == BravaisLatticeBody)
    {
        if (abs(a + b + c) % 2 != 0)
        {
            return true;
        }
    }
    else if (_latticeType == BravaisLatticeFace)
    {
        if (abs(a + b) % 2 == 0 && abs(b + c) % 2 == 0
            && abs(c + a) % 2 == 0)
        {
            return false;
        }
        
        return true;
    }
    else if (_latticeType == BravaisLatticeBase)
    {
        if (abs(a + b) % 2 == 1)
        {
            return true;
        }
    }
    
    return false;
}

Crystal::Crystal()
{
    _unitCell = make_mat3x3();
    _rotation = make_mat3x3();
    _fixedAxis = {0, 0, 0};

    _horiz = 0;
    _vert = 0;
    
    for (int i = 0; i < 3; i++) {_cellDims.push_back(1);};
    for (int i = 0; i < 3; i++) {_cellDims.push_back(90);};

    _resolution = STARTING_RESOLUTION;
    _rlpSize = 0.0015;
    _wavelength = STARTING_WAVELENGTH;
    _latticeType = BravaisLatticePrimitive;
}

void Crystal::setUnitCell(mat3x3 unitCell)
{
	_unitCell = unitCell;
	mat3x3 real = mat3x3_inverse(unitCell);
	
	mat3x3 trans = mat3x3_transpose(real);
	mat3x3 mult = mat3x3_mult_mat3x3(real, trans);

	_cellDims = std::vector<double>();
	_cellDims.resize(6);
	_cellDims[0] = sqrt(mult.vals[0]);
	_cellDims[1] = sqrt(mult.vals[4]);
	_cellDims[2] = sqrt(mult.vals[8]);
	_cellDims[3] = rad2deg(acos(mult.vals[5] / (_cellDims[1] * _cellDims[2])));
	_cellDims[4] = rad2deg(acos(mult.vals[2] / (_cellDims[0] * _cellDims[2])));
	_cellDims[5] = rad2deg(acos(mult.vals[1] / (_cellDims[0] * _cellDims[1])));
	
	std::cout << "Cell dimensions: ";

	for (int i = 0; i < 6; i++)
	{
		std::cout << _cellDims[i] << " ";
	}

	std::cout << std::endl;
}

void Crystal::setUnitCell(std::vector<double> cellDims)
{
    _cellDims = cellDims;
    mat3x3 mat = mat3x3_from_unit_cell(&cellDims[0]);
    _unitCell = mat3x3_inverse(mat);
    
    populateMillers();
}

void Crystal::quickCheckMillers()
{
/* duplicated code - work out best fix */

    std::cout << "Checking " << _reflections.size() << " stored Millers." << std::endl;

    vec3 samplePos = make_vec3(0, 0, - 1 / _wavelength);
    double minLength = 1 / _wavelength - _rlpSize;
    double maxLength = 1 / _wavelength + _rlpSize;
    double minLengthSq = minLength * minLength;
    double maxLengthSq = maxLength * maxLength;
    
	mat3x3 three = getNudge(_horiz, _vert, 0);

    for (unsigned int i = 0; i < _reflections.size(); i++)
    {
		Reflection *refl = &(_reflections[i]);
        vec3 abc = make_vec3(refl->h, refl->k, refl->l);

		mat3x3_mult_vec(_unitCell, &abc);
		mat3x3_mult_vec(_rotation, &abc);
		mat3x3_mult_vec(three, &abc);
        
        vec3 diff = vec3_subtract_vec3(abc, samplePos);
        
        double sqLength = vec3_sqlength(diff);
        
        if (sqLength < minLengthSq || sqLength > maxLengthSq)
        {
			refl->onImage = false;
            continue;
        }
        
        double length = sqrt(sqLength);
        double size = fabs(1 / _wavelength - length) / (_rlpSize);
        if (size < 0) size = 0;
        if (size > 1) size = 1;
		refl->onImage = true;
		refl->weight = size;
		refl->miller = abc;
    }
}

void Crystal::populateMillers()
{
    std::cout << "Populating millers" << std::endl;
    _reflections.clear();
    int aMax = _cellDims[0] / _resolution;
    int bMax = _cellDims[1] / _resolution;
    int cMax = _cellDims[2] / _resolution;
    vec3 samplePos = make_vec3(0, 0, - 1 / _wavelength);
    double minLength = 1 / _wavelength - _rlpSize;
    double maxLength = 1 / _wavelength + _rlpSize;
    double minLengthSq = minLength * minLength;
    double maxLengthSq = maxLength * maxLength;
    maxLength += _rlpSize * 2;
    minLength -= _rlpSize * 2;
    double minBuffer = minLength * minLength;
    double maxBuffer = maxLength * maxLength; 

    std::cout << minLengthSq << " " << maxLengthSq << std::endl;
    std::cout << "To maximum resolution: " << _resolution << std::endl;
    
    for (int a = -aMax; a <= aMax; a++)
    {
        for (int b = -bMax; b <= bMax; b++)
        {
            for (int c = -cMax; c <= cMax; c++)
            {
                vec3 abc = make_vec3(a, b, c);
                
                bool sysabs = isSysabs(a, b, c);
               
                if (sysabs) continue;

                mat3x3_mult_vec(_unitCell, &abc);
				double length = vec3_length(abc);

				if (length > 1 / _resolution)
				{
					continue;
				}

                mat3x3_mult_vec(_rotation, &abc);
                
                vec3 diff = vec3_subtract_vec3(abc, samplePos);
                
                double sqLength = vec3_sqlength(diff);

                if (sqLength < minBuffer || sqLength > maxBuffer)
                {
					continue;
                }

				Reflection refl;
				refl.miller = abc;
				refl.h = a;
				refl.k = b;
				refl.l = c;
				refl.weight = 0;
				refl.onImage = false;
				refl.watched = false;
				_reflections.push_back(refl);
			}
        }
    }
    
    quickCheckMillers();
    
    std::cout << "Found " << _reflections.size() << " reflections." << std::endl;
}

mat3x3 Crystal::getNudge(double diffX, double diffY, double diffZ)
{
    vec3 xAxis = {1, 0, 0};
    vec3 yAxis = {0, 1, 0};
    vec3 zAxis = {0, 0, 1};
    
    if (vec3_length(_fixedAxis) > 0.5)
    {
        xAxis = _fixedAxis;
        yAxis = vec3_cross_vec3(_fixedAxis, zAxis);
        vec3_set_length(&yAxis, 1);
    }
    
    mat3x3 xRot = mat3x3_unit_vec_rotation(yAxis, diffX);
    mat3x3 yRot = mat3x3_unit_vec_rotation(xAxis, diffY);
    mat3x3 zRot = mat3x3_unit_vec_rotation(zAxis, diffZ);
    
    mat3x3 both = mat3x3_mult_mat3x3(yRot, xRot);
    mat3x3 three = mat3x3_mult_mat3x3(zRot, both);
    
    return three;
}

void Crystal::applyRotation(double diffX, double diffY, double diffZ)
{
    mat3x3 three = getNudge(diffX, diffY, diffZ);
    
    _rotation = mat3x3_mult_mat3x3(three, _rotation);
    
    quickCheckMillers();
    
    std::cout << mat3x3_desc(_rotation) << std::endl;
}

void Crystal::bringAxisToScreen(std::vector<double> newAxis)
{
    vec3 beamAxis = {0, 0, 1};
    vec3 axis = make_vec3(newAxis[0], newAxis[1], newAxis[2]);
    mat3x3_mult_vec(_unitCell, &axis);
    vec3 axis2 = {0, 0, 1};

    mat3x3 angle_diff_mat = make_mat3x3();
    
    bool extraAxis = false;
    if (newAxis.size() >= 6)
    {
        axis2 = make_vec3(newAxis[3], newAxis[4], newAxis[5]);
        mat3x3_mult_vec(_unitCell, &axis);
        extraAxis = true;
        double angle = vec3_angle_with_vec3(axis, axis2);
        angle_diff_mat = mat3x3_unit_vec_rotation(beamAxis, angle);
    }
    
    vec3 screenAxis = make_vec3(1, 0, 0);
    
        
    std::cout << "Mapping reciprocal axis " << vec3_desc(axis)
    << " to screen axis " << vec3_desc(screenAxis) << std::endl;
    
    _rotation = mat3x3_map_vec_to_vec(axis, screenAxis);
    
    if (extraAxis)
    {
        vec3 screenAxis2 = make_vec3(1, 0, 0);
        mat3x3_mult_vec(angle_diff_mat, &screenAxis2);
        mat3x3 twizzle = mat3x3_closest_rot_mat(axis2, screenAxis2, screenAxis);
        _rotation = mat3x3_mult_mat3x3(twizzle, _rotation);
    }

    
    std::cout << "Rotation: " << mat3x3_desc(_rotation) << std::endl;
    
    populateMillers();
}

mat3x3 Crystal::getScaledBasisVectors()
{
    mat3x3 both = mat3x3_mult_mat3x3(_rotation, _unitCell);
    mat3x3 inverse = mat3x3_inverse(both);
    
    return inverse;
}

bool Crystal::isBeingWatched(int i)
{
	return _reflections[i].watched;
}

double Crystal::ewaldSphereCloseness()
{
    quickCheckMillers();
    
    double sizeSum = 0;
	int count = 0;

	for (unsigned int i = 0; i < _reflections.size(); i++)
	{
		if (!_reflections[i].watched)
		{
			continue;
		}

		sizeSum += _reflections[i].weight;
		count++;
	}

	if (count == 0)
	{
		return 0;
	}

	sizeSum /= (double)count;

    std::cout << "Ewald sphere closeness check " << sizeSum <<
	" across " << count << " reflections." << std::endl;

    _tinker->drawPredictions();
	QCoreApplication::processEvents();
    
	return sizeSum;
}

void Crystal::clearUpRefinement()
{
    mat3x3 three = getNudge(_horiz, _vert, 0);
    _rotation = mat3x3_mult_mat3x3(three, _rotation);
    _horiz = 0;
    _vert = 0;

	for (unsigned int i = 0; i < _reflections.size(); i++)
	{
		_reflections[i].watched = false;
	}
}
