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
#include "Detector.h"
#include "defaults.h"
#include <iostream>
#include "float.h"

Detector::Detector()
{
    _beamCentre = make_vec3(-1, -1, STARTING_DISTANCE);
    _wavelength = STARTING_WAVELENGTH;
	_lookupTree = NULL;
}

void Detector::calculatePositions()
{
	vec3 samplePos = make_vec3(0, 0, - 1 / _wavelength);

	for (size_t i = 0; i < _xtal->millerCount(); i++)
	{
		vec3 miller = _xtal->miller(i);

		vec3 diff = vec3_subtract_vec3(miller, samplePos);
		double mult = _beamCentre.z / diff.z;
        vec3_mult(&diff, mult);
        
		_xtal->setPositionForMiller(i, diff);
    }
}

double Detector::distToMiller(int i, int x, int y)
{
	vec3 pos = _xtal->position(i);
	vec3 clickPos = make_vec3(x, y, pos.z);
	vec3 diff = vec3_subtract_vec3(clickPos, pos);
	
	return vec3_length(diff);
}

bool Detector::nearMiller(int i, int x, int y)
{
	bool onImage = _xtal->shouldDisplayMiller(i);

	if (!onImage) return false;

	vec3 pos = _xtal->position(i);

	if (x >= pos.x - CLOSENESS && x <= pos.x + CLOSENESS &&
	    y >= pos.y - CLOSENESS && y <= pos.y + CLOSENESS)
	{
		return true;
	}
	
	return false;
}

int Detector::positionNearCoord(int x, int y)
{
	x -= _beamCentre.x;
	y -= _beamCentre.y;

	if (!_lookupTree)
	{
		for (size_t i = 0; i < _xtal->millerCount(); i++)
		{
			if (nearMiller(i, x, y))
			{
				return i;
			}
		}

		return -1;
	}
	
	Node *node = _lookupTree;
	int drills = 0;
	
	/* Some non-exit condition and I'm too lazy to fix properly */
	int max_drills = 50;

	while (true && drills < max_drills)
	{
		for (int i = 0; i < 4; i++)
		{
			Node *trial = node->nextNodes[i];

			if (!trial)
			{
				goto finished_node_search;
			}	

			if (x > trial->xMin && x < trial->xMax && 
			    y > trial->yMin && y < trial->yMax)
			{
				node = trial;
				break;
			}
		}
		
		drills++;
	}
	
finished_node_search:
	
	std::cout << "Drilled down " << drills << " nodes." << std::endl;
	
	int best_n = -1;
	double distance = FLT_MAX;
	
	for (size_t i = 0; i < node->nRefls; i++)
	{
		int n = node->reflPtrs[i];

		if (nearMiller(n, x, y))
		{
			double thisDist = distToMiller(n, x, y);
			
			if (thisDist < distance)
			{
				distance = thisDist;
				best_n = n;	
			}
		}
	}
	
	return best_n;
}

void Detector::prepareLookupTable()
{
	delete_node(_lookupTree);
	_lookupTree = NULL;
	
	size_t count = _xtal->millerCount();
	if (!count) return;

	Reflection *start = _xtal->refl(0);
	_lookupTree = make_node();
	prepare_node(_lookupTree, start, count);
	recursive_split_node(_lookupTree);
}

Detector::~Detector()
{
	if (_lookupTree)
	{
		delete_node(_lookupTree);
	}
}

