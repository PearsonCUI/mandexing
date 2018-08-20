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

#ifndef __vagabond__mat3x3__
#define __vagabond__mat3x3__

#include <stdio.h>
#include <vector>
#include "vec3.h"
#include <string.h>

struct mat3x3
{
	double vals[9];
};

std::string mat3x3_desc(mat3x3 mat);

mat3x3 mat3x3_inverse(mat3x3 &mat);
mat3x3 mat3x3_from_unit_cell(double a, double b, double c, double alpha, double beta, double gamma);
mat3x3 mat3x3_from_unit_cell(double *unitCell);

void mat3x3_mult_scalar(mat3x3 *mat, double scale);
void unit_cell_from_mat3x3(mat3x3 mat, double *vals);
mat3x3 make_mat3x3();

std::string computer_friendly_desc(mat3x3 &mat);
mat3x3 mat3x3_from_string(std::vector<std::string> &components);

inline void mat3x3_mult_vec(struct mat3x3 mat, struct vec3 *vec)
{
	struct vec2 v;

	v.x = mat.vals[0] * vec->x + mat.vals[1] * vec->y + mat.vals[2] * vec->z;
	v.y = mat.vals[3] * vec->x + mat.vals[4] * vec->y + mat.vals[5] * vec->z;
	vec->z = mat.vals[6] * vec->x + mat.vals[7] * vec->y + mat.vals[8] * vec->z;

	memcpy(vec, &v.x, sizeof(double) * 2);
}

vec3 mat3x3_axis(mat3x3 me, int i);

vec3 mat3x3_mult_vec(struct mat3x3 mat, struct vec3 vec);

void mat3x3_scale(mat3x3 *mat, double a, double b, double c);
double mat3x3_length(mat3x3 &mat, int index);
mat3x3 mat3x3_transpose(mat3x3 &mat);
double mat3x3_determinant(mat3x3 &mat);
mat3x3 mat3x3_mult_mat3x3(struct mat3x3 m1, struct mat3x3 m2);
mat3x3 mat3x3_unit_vec_rotation(vec3 axis, double radians);
mat3x3 mat3x3_rotate(double alpha, double beta, double gamma);
mat3x3 mat3x3_ortho_axes(vec3 cVec);
mat3x3 mat3x3_rhbasis(vec3 aVec, vec3 cVec);
mat3x3 mat3x3_map_vec_to_vec(vec3 aVec, vec3 bVec);
mat3x3 mat3x3_closest_rot_mat(vec3 vec1, vec3 vec2, vec3 axis,
							  double *best = NULL);
mat3x3 mat3x3_covariance(std::vector<vec3> points);

mat3x3 mat3x3_rot_from_angles(double phi, double psi);
mat3x3 mat3x3_from_2d_array(double **values);
void mat3x3_to_2d_array(mat3x3 mat, double ***values);
void free_2d_array(double **values);

#endif /* defined(__vagabond__mat3x3__) */
