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

#ifndef __vagabond__vec3__
#define __vagabond__vec3__

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>

struct vec3
{
	double x;
	double y;
	double z;
};

struct vec2
{
	double x;
	double y;
};

double vec3_length(vec3 &vec);
void vec3_set_length(vec3 *vec, double length);

struct vec3 empty_vec3();
inline vec3 make_vec3(double x, double y, double z)
{
	struct vec3 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;

	return vec;
}

vec3 make_randomish_axis();

inline vec2 make_vec2(double x, double y)
{
	struct vec2 vec;
	vec.x = x;
	vec.y = y;

	return vec;
}

inline bool vec2_less_vec2(vec2 x, vec2 y)
{
	return x.x > y.x;
}


double vec3_sqlength(vec3 &vec);
struct vec3 vec3_add_vec3(vec3 &aVec, vec3 &bVec);
struct vec3 vec3_subtract_vec3(vec3 &to, vec3 &from);
double vec3_dot_vec3(vec3 &aVec, vec3 &bVec);
double vec3_angle_with_vec3(vec3 &aVec, vec3 &bVec);
double vec3_cosine_with_vec3(vec3 &aVec, vec3 &bVec);
vec3 vec3_cross_vec3(vec3 &aVec, vec3 &bVec);
double vec3_angle_from_three_points(vec3 &aVec, vec3 &bVec, vec3 &cVec);
double ewald_wavelength(vec3 &aVec);

vec3 vec3_from_string(std::vector<std::string> &components);
std::string computer_friendly_desc(vec3 &vec);
std::string vec3_desc(vec3 vec);

inline void vec3_min_each(vec3 *minVec, vec3 &aVec)
{
	if (aVec.x < minVec->x) minVec->x = aVec.x;
	if (aVec.y < minVec->y) minVec->y = aVec.y;
	if (aVec.z < minVec->z) minVec->z = aVec.z;
}

inline void vec3_max_each(vec3 *maxVec, vec3 &aVec)
{
	if (aVec.x > maxVec->x) maxVec->x = aVec.x;
	if (aVec.y > maxVec->y) maxVec->y = aVec.y;
	if (aVec.z > maxVec->z) maxVec->z = aVec.z;
}
inline void vec3_mult(vec3 *aVec, double mult)
{
	aVec->x *= mult;
	aVec->y *= mult;
	aVec->z *= mult;
}


inline void vec3_set_length(vec3 *vec, double length)
{
	double now = vec3_length(*vec);
	vec3_mult(vec, length / now);
}



#endif /* defined(__vagabond__vec3__) */
