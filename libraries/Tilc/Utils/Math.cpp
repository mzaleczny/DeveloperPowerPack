#include "Tilc/Utils/Math.h"

#include <stdio.h>
#include <string.h>

/*
static REAL f_tolerance = 0.0001f;

vector3 vector3_new(REAL x, REAL y, REAL z) {
    vector3 v;

    v.x = x;
    v.y = y;
    v.z = z;

    return v;
}

void vector3_set(vector3 *v, REAL x, REAL y, REAL z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

REAL vector3_magnitude(vector3 *v) {
    return (REAL)sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

void vector3_normalize(vector3 *v) {
    REAL m = (REAL)sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    if (m < f_tolerance) {
        m = 1.0f;
    }
    v->x /= m;
    v->y /= m;
    v->z /= m;

    if (fabs(v->x) < f_tolerance) { v->x = 0.0f; }
    if (fabs(v->y) < f_tolerance) { v->y = 0.0f; }
    if (fabs(v->z) < f_tolerance) { v->z = 0.0f; }
}

void vector3_reverse(vector3 *v_in, vector3 *v_out) {
    v_out->x = -v_in->x;
    v_out->y = -v_in->y;
    v_out->z = -v_in->z;
}

void vector3_add(vector3 *v1_in, vector3 *v2_in, vector3 *v_out) {
    v_out->x = v1_in->x + v2_in->x;
    v_out->y = v1_in->y + v2_in->y;
    v_out->z = v1_in->z + v2_in->z;
}

void vector3_sub(vector3 *v1_in, vector3 *v2_in, vector3 *v_out) {
    v_out->x = v1_in->x - v2_in->x;
    v_out->y = v1_in->y - v2_in->y;
    v_out->z = v1_in->z - v2_in->z;
}

void vector3_mul(vector3 *v, REAL s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

void vector3_div(vector3 *v, REAL s) {
    v->x /= s;
    v->y /= s;
    v->z /= s;
}

void vector3_neg(vector3 *v) {
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
}

void vector3_cross_product(vector3 *v1_in, vector3 *v2_in, vector3 *v_out) {
    v_out->x = v1_in->y * v2_in->z - v1_in->z * v2_in->y;
    v_out->y = -v1_in->x * v2_in->z + v1_in->z * v2_in->x;
    v_out->z = v1_in->x * v2_in->y - v1_in->y * v2_in->x;
}

REAL vector3_dot_product(vector3 *v1, vector3 *v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

REAL vector3_triple_dot_product(vector3 *u, vector3 *v, vector3 *w) {
    return (u->x * (v->y * w->z - v->z * w->y)) +
            (u->y * (-v->x * w->z + v->z * w->x)) +
            (u->z * (v->x * w->y - v->y * w->x));
}


void vector3_set_f(REAL v[3], REAL x, REAL y, REAL z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}


void vector3_set_fv(REAL v[3], REAL v_source[3]) {
	v[0] = v_source[0];
	v[1] = v_source[1];
	v[2] = v_source[2];
}

REAL vector3_magnitude_fv(REAL v[3]) {
    return (REAL)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void vector3_normalize_fv(REAL v[3]) {
    REAL m = (REAL)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (m < f_tolerance) {
        m = 1.0f;
    }
    v[0] /= m;
    v[1] /= m;
    v[2] /= m;

    if (fabs(v[0]) < f_tolerance) { v[0] = 0.0f; }
    if (fabs(v[1]) < f_tolerance) { v[1] = 0.0f; }
    if (fabs(v[2]) < f_tolerance) { v[2] = 0.0f; }
}

void vector3_reverse_fv(REAL v_in[3], REAL v_out[3]) {
    v_out[0] = -v_in[0];
    v_out[1] = -v_in[1];
    v_out[2] = -v_in[2];
}

void vector3_add_fv(REAL v1_in[3], REAL v2_in[3], REAL v_out[3]) {
    v_out[0] = v1_in[0] + v2_in[0];
    v_out[1] = v1_in[1] + v2_in[1];
    v_out[2] = v1_in[2] + v2_in[2];
}

void vector3_sub_fv(REAL v1_in[3], REAL v2_in[3], REAL v_out[3]) {
    v_out[0] = v1_in[0] - v2_in[0];
    v_out[1] = v1_in[1] - v2_in[1];
    v_out[2] = v1_in[2] - v2_in[2];
}

void vector3_mul_fv(REAL v[3], REAL s) {
    v[0] *= s;
    v[1] *= s;
    v[2] *= s;
}

void vector3_div_fv(REAL v[3], REAL s) {
    v[0] /= s;
    v[1] /= s;
    v[2] /= s;
}

void vector3_neg_fv(REAL v[3]) {
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

void vector3_mul_vector3_fv(REAL output[3], REAL v1[3], REAL v2[3]) {
	output[0] = v1[0] * v2[0];
	output[1] = v1[1] * v2[1];
	output[2] = v1[2] * v2[2];
}

void vector3_cross_product_fv(REAL v1_in[3], REAL v2_in[3], REAL v_out[3]) {
    v_out[0] = v1_in[1] * v2_in[2] - v1_in[2] * v2_in[1];
    v_out[1] = -v1_in[0] * v2_in[2] + v1_in[2] * v2_in[0];
    v_out[2] = v1_in[0] * v2_in[1] - v1_in[1] * v2_in[0];
}

REAL vector3_dot_product_fv(REAL v1[3], REAL v2[3]) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

REAL vector3_triple_dot_product_fv(REAL u[3], REAL v[3], REAL w[3]) {
    return (u[0] * (v[1] * w[2] - v[2] * w[1])) +
            (u[1] * (-v[0] * w[2] + v[2] * w[0])) +
            (u[2] * (v[0] * w[1] - v[1] * w[0]));
}


void dump_matrix4x4(REAL *matrix) {
    fprintf(stdout, "%8.4f  %8.4f  %8.4f  %8.4f\n"
                    "%8.4f  %8.4f  %8.4f  %8.4f\n"
                    "%8.4f  %8.4f  %8.4f  %8.4f\n"
                    "%8.4f  %8.4f  %8.4f  %8.4f\n",
                    matrix[0], matrix[1], matrix[2], matrix[3],
                    matrix[4], matrix[5], matrix[6], matrix[7],
                    matrix[8], matrix[9], matrix[10], matrix[11],
                    matrix[12], matrix[13], matrix[14], matrix[15]);
}

void dump_vector4(REAL *vector) {
    fprintf(stdout, "%8.2f\n"
                    "%8.2f\n"
                    "%8.2f\n"
                    "%8.2f\n",
                    vector[0], vector[1], vector[2], vector[3]);
}

void dump_matrix3x3(REAL *matrix) {
    fprintf(stdout, "%8.2f  %8.2f  %8.2f\n"
                    "%8.2f  %8.2f  %8.2f\n"
                    "%8.2f  %8.2f  %8.2f\n",
                    matrix[0], matrix[1], matrix[2],
                    matrix[3], matrix[4], matrix[5],
                    matrix[6], matrix[7], matrix[8]);
}

void dump_vector3(REAL *vector) {
    fprintf(stdout, "%8.2f\n"
                    "%8.2f\n"
                    "%8.2f\n",
                    vector[0], vector[1], vector[2]);
}

void dump_coords_3d(REAL *coords, unsigned long coords_size) {
	printf("[\n");
	for (int i = 0; i < coords_size / sizeof(REAL); i += 3) {
		printf("x: %8.2f,  y: %8.2f,  z: %8.2f\n", coords[i], coords[i+1], coords[i+2]);
	}
	printf("]\n");
}

void set_z_coordinate_of_points_array(REAL z, REAL *coords, unsigned long coords_size) {
    for (int i = 0; i < coords_size / sizeof(REAL); i += 3) {
        coords[i+2] = z;
    }
}

void mat3x3_from_mat4x4(REAL out_mat3x3[9], REAL in_mat4x4[16]) {
    out_mat3x3[0] = in_mat4x4[0];
    out_mat3x3[1] = in_mat4x4[1];
    out_mat3x3[2] = in_mat4x4[2];

    out_mat3x3[3] = in_mat4x4[4];
    out_mat3x3[4] = in_mat4x4[5];
    out_mat3x3[5] = in_mat4x4[6];

    out_mat3x3[6] = in_mat4x4[8];
    out_mat3x3[7] = in_mat4x4[9];
    out_mat3x3[8] = in_mat4x4[10];
}

void mult_matrices(REAL *output_matrix, REAL *matrix1, REAL *matrix2, int matrix_size) {
    int i, j, k;
    REAL val;

    for (i = 0; i < matrix_size; i++) {
        for (j = 0; j < matrix_size; j++) {
            val = 0.0f;
            for (k = 0; k < matrix_size; k++) {
                val += matrix1[j * matrix_size + k] * matrix2[i + k * matrix_size];
            }
            output_matrix[i + (j * matrix_size)] = val;
        }
    }
}

void mult_matrices4x4(REAL output_matrix[16], REAL m1[16], REAL m2[16]) {
	output_matrix[0] = m1[0]*m2[0] + m1[1]*m2[4] + m1[2]*m2[8] + m1[3]*m2[12];
	output_matrix[1] = m1[0]*m2[1] + m1[1]*m2[5] + m1[2]*m2[9] + m1[3]*m2[13];
	output_matrix[2] = m1[0]*m2[2] + m1[1]*m2[6] + m1[2]*m2[10] + m1[3]*m2[14];
	output_matrix[3] = m1[0]*m2[3] + m1[1]*m2[7] + m1[2]*m2[11] + m1[3]*m2[15];

	output_matrix[4] = m1[4]*m2[0] + m1[5]*m2[4] + m1[6]*m2[8] + m1[7]*m2[12];
	output_matrix[5] = m1[4]*m2[1] + m1[5]*m2[5] + m1[6]*m2[9] + m1[7]*m2[13];
	output_matrix[6] = m1[4]*m2[2] + m1[5]*m2[6] + m1[6]*m2[10] + m1[7]*m2[14];
	output_matrix[7] = m1[4]*m2[3] + m1[5]*m2[7] + m1[6]*m2[11] + m1[7]*m2[15];

	output_matrix[8] = m1[8]*m2[0] + m1[9]*m2[4] + m1[10]*m2[8] + m1[11]*m2[12];
	output_matrix[9] = m1[8]*m2[1] + m1[9]*m2[5] + m1[10]*m2[9] + m1[11]*m2[13];
	output_matrix[10] = m1[8]*m2[2] + m1[9]*m2[6] + m1[10]*m2[10] + m1[11]*m2[14];
	output_matrix[11] = m1[8]*m2[3] + m1[9]*m2[7] + m1[10]*m2[11] + m1[11]*m2[15];

	output_matrix[12] = m1[12]*m2[0] + m1[13]*m2[4] + m1[14]*m2[8] + m1[15]*m2[12];
	output_matrix[13] = m1[12]*m2[1] + m1[13]*m2[5] + m1[14]*m2[9] + m1[15]*m2[13];
	output_matrix[14] = m1[12]*m2[2] + m1[13]*m2[6] + m1[14]*m2[10] + m1[15]*m2[14];
	output_matrix[15] = m1[12]*m2[3] + m1[13]*m2[7] + m1[14]*m2[11] + m1[15]*m2[15];
}

void mult_matrices4x4_3(REAL output_matrix[16], REAL m3[16], REAL m2[16], REAL m1[16]) {
    REAL tmp1[16];
    mult_matrices4x4(tmp1, m2, m1);
    mult_matrices4x4(output_matrix, m3, tmp1);
}

void mult_matrices4x4_4(REAL output_matrix[16], REAL m4[16], REAL m3[16], REAL m2[16], REAL m1[16]) {
    REAL tmp1[16], tmp2[16];

    mult_matrices4x4(tmp1, m2, m1);
    mult_matrices4x4(tmp2, m3, tmp1);
    mult_matrices4x4(output_matrix, m4, tmp2);
}

void mult_matrix_and_scalar(REAL *output_matrix, REAL *input_matrix, REAL scalar, int matrix_cols, int matrix_rows) {
    int i, j;

    for (i = 0; i < matrix_rows; i++) {
        for (j = 0; j < matrix_cols; j++) {
            output_matrix[i * matrix_rows + j] = input_matrix[i * matrix_rows + j] * scalar;
        }
    }
}

REAL mat2x2_det(REAL mat[4]) {
    return mat[0] * mat[3] - mat[2] * mat[2];
}

REAL mat3x3_det(REAL mat[9]) {
    return mat[0] * mat[4] * mat[8] +
       mat[1] * mat[5] * mat[6] +
       mat[2] * mat[3] * mat[7] -
       mat[2] * mat[4] * mat[6] -
       mat[1] * mat[3] * mat[8] -
       mat[0] * mat[5] * mat[7];
}

void mat4x4_to_mat3x3(REAL output_mat3x3[9], REAL input_mat4x4[16]) {
    output_mat3x3[0] = input_mat4x4[0];
    output_mat3x3[1] = input_mat4x4[1];
    output_mat3x3[2] = input_mat4x4[2];
    output_mat3x3[3] = input_mat4x4[4];
    output_mat3x3[4] = input_mat4x4[5];
    output_mat3x3[5] = input_mat4x4[6];
    output_mat3x3[6] = input_mat4x4[8];
    output_mat3x3[7] = input_mat4x4[9];
    output_mat3x3[8] = input_mat4x4[10];
}
void transform_vector(REAL *output_vector, REAL *input_matrix, REAL *input_vector, int vec_matrix_size) {
    int i,j;
    REAL val;

    for (i = 0; i < vec_matrix_size; i++) {
        val = 0;
        for (j = 0; j < vec_matrix_size; j++) {
            val += input_matrix[i*vec_matrix_size + j] * input_vector[j];
        }
        output_vector[i] = val;
    }
}

void get_rotation_x_matrix4x4(REAL *output_matrix, REAL angle) {
    REAL c = cos(angle);
    REAL s = sin(angle);

    output_matrix[0] = 1.0f;
    output_matrix[1] = 0.0f;
    output_matrix[2] = 0.0f;
    output_matrix[3] = 0.0f;

    output_matrix[4] = 0.0f;
    output_matrix[5] = c;
    output_matrix[6] = -s;
    output_matrix[7] = 0.0f;

    output_matrix[8] = 0.0f;
    output_matrix[9] = s;
    output_matrix[10] = c;
    output_matrix[11] = 0.0f;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_rotation_y_matrix4x4(REAL *output_matrix, REAL angle) {
    REAL c = cos(angle);
    REAL s = sin(angle);

    output_matrix[0] = c;
    output_matrix[1] = 0.0f;
    output_matrix[2] = s;
    output_matrix[3] = 0.0f;

    output_matrix[4] = 0.0f;
    output_matrix[5] = 1.0f;
    output_matrix[6] = 0.0f;
    output_matrix[7] = 0.0f;

    output_matrix[8] = -s;
    output_matrix[9] = 0.0f;
    output_matrix[10] = c;
    output_matrix[11] = 0.0f;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_rotation_z_matrix4x4(REAL *output_matrix, REAL angle) {
    REAL c = cos(angle);
    REAL s = sin(angle);

    output_matrix[0] = c;
    output_matrix[1] = -s;
    output_matrix[2] = 0.0f;
    output_matrix[3] = 0.0f;

    output_matrix[4] = s;
    output_matrix[5] = c;
    output_matrix[6] = 0.0f;
    output_matrix[7] = 0.0f;

    output_matrix[8] = 0.0f;
    output_matrix[9] = 0.0f;
    output_matrix[10] = 1.0f;
    output_matrix[11] = 0.0f;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_rotation_matrix4x4_fv(REAL *output_matrix, REAL angle, REAL v[3]) {
    REAL c = cos(angle);
    REAL s = sin(angle);
    REAL axis[3], temp[3];

    axis[0] = v[0];
    axis[1] = v[1];
    axis[2] = v[2];
    vector3_normalize_fv(axis);

    temp[0] = axis[0];
    temp[1] = axis[1];
    temp[2] = axis[2];
    vector3_mul_fv(temp, 1 - c);

    output_matrix[0] = c + temp[0] * axis[0];
    output_matrix[4] = 0.0f + temp[0] * axis[1] + s * axis[2];
    output_matrix[8] = 0.0f + temp[0] * axis[2] - s * axis[1];
    output_matrix[12] = 0.0f;

    output_matrix[1] = 0.0f + temp[1] * axis[0] - s * axis[2];
    output_matrix[5] = c + temp[1] * axis[1];
    output_matrix[9] = 0.0f + temp[1] * axis[2] + s * axis[0];
    output_matrix[13] = 0.0f;

    output_matrix[2] = 0.0f + temp[2] * axis[0] + s * axis[1];
    output_matrix[6] = 0.0f + temp[2] * axis[1] - s * axis[0];
    output_matrix[10] = c + temp[2] * axis[2];
    output_matrix[14] = 0.0f;

    output_matrix[3] = 0.0f;
    output_matrix[7] = 0.0f;
    output_matrix[11] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_translation_matrix4x4v(REAL *output_matrix, REAL *v) {
    get_translation_matrix4x4(output_matrix, v[0], v[1], v[2]);
}

void get_translation_matrix4x4(REAL *output_matrix, REAL x, REAL y, REAL z) {
    output_matrix[0] = 1.0f;
    output_matrix[1] = 0.0f;
    output_matrix[2] = 0.0f;
    output_matrix[3] = x;

    output_matrix[4] = 0.0f;
    output_matrix[5] = 1.0f;
    output_matrix[6] = 0.0f;
    output_matrix[7] = y;

    output_matrix[8] = 0.0f;
    output_matrix[9] = 0.0f;
    output_matrix[10] = 1.0f;
    output_matrix[11] = z;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_translation_matrix4x4v_rev(REAL *output_matrix, REAL *v) {
    get_translation_matrix4x4(output_matrix, -v[0], -v[1], -v[2]);
}

void get_translation_matrix4x4_rev(REAL *output_matrix, REAL x, REAL y, REAL z) {
    get_translation_matrix4x4(output_matrix, -x, -y, -z);
}

void get_scale_matrix4x4(REAL *output_matrix, REAL scale_x, REAL scale_y, REAL scale_z) {
    output_matrix[0] = scale_x;
    output_matrix[1] = 0.0f;
    output_matrix[2] = 0.0f;
    output_matrix[3] = 0.0f;

    output_matrix[4] = 0.0f;
    output_matrix[5] = scale_y;
    output_matrix[6] = 0.0f;
    output_matrix[7] = 0.0f;

    output_matrix[8] = 0.0f;
    output_matrix[9] = 0.0f;
    output_matrix[10] = scale_z;
    output_matrix[11] = 0.0f;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_scale_matrix4x4_rev(REAL *output_matrix, REAL scale_x, REAL scale_y, REAL scale_z) {
    get_scale_matrix4x4(output_matrix, 1/scale_x, 1/scale_y, 1/scale_z);
}



void get_ortho_matrix(REAL *output_matrix, unsigned char force_depth_zero_to_one, REAL left, REAL right, REAL bottom, REAL top, REAL near, REAL far) {
    if (!force_depth_zero_to_one) {
        // for opengl where depth is in range [-1,1]
        output_matrix[0] = 2.0f / (right - left);
        output_matrix[1] = 0.0f;
        output_matrix[2] = 0.0f;
        output_matrix[3] = -(right + left) / (right - left);

        output_matrix[4] = 0.0f;
        output_matrix[5] = 2.0f / (top - bottom);
        output_matrix[6] = 0.0f;
        output_matrix[7] = -(top + bottom) / (top - bottom);

        output_matrix[8] = 0.0f;
        output_matrix[9] = 0.0f;
        output_matrix[10] = -2.0f / (far - near);
        output_matrix[11] = -(far + near) / (far - near);

        output_matrix[12] = 0.0f;
        output_matrix[13] = 0.0f;
        output_matrix[14] = 0.0f;
        output_matrix[15] = 1.0f;
    } else {
        // for vulkan where depth is in range [0,1]
        output_matrix[0] = 2.0f / (right - left);
        output_matrix[1] = 0.0f;
        output_matrix[2] = 0.0f;
        output_matrix[3] = -(right + left) / (right - left);

        output_matrix[4] = 0.0f;
        output_matrix[5] = 2.0f / (top - bottom);
        output_matrix[6] = 0.0f;
        output_matrix[7] = -(top + bottom) / (top - bottom);

        output_matrix[8] = 0.0f;
        output_matrix[9] = 0.0f;
        output_matrix[10] = 1.0f / (near - far);
        output_matrix[11] = - near / (far - near);

        output_matrix[12] = 0.0f;
        output_matrix[13] = 0.0f;
        output_matrix[14] = 0.0f;
        output_matrix[15] = 1.0f;
    }
}


void get_frustum_matrix(REAL *output_matrix, REAL left, REAL right, REAL bottom, REAL top, REAL near, REAL far) {
    output_matrix[0] = 2.0f * near / (right - left);
    output_matrix[1] = 0.0f;
    output_matrix[2] = (right + left) / (right - left);
    output_matrix[3] = 0.0f;

    output_matrix[4] = 0.0f;
    output_matrix[5] = 2.0f * near / (top - bottom);
    output_matrix[6] = (top + bottom) / (top - bottom);
    output_matrix[7] = 0.0f;

    output_matrix[8] = 0.0f;
    output_matrix[9] = 0.0f;
    output_matrix[10] = -(far + near) / (far - near);
    output_matrix[11] = -2.0f * far * near / (far - near);

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = -1.0f;
    output_matrix[15] = 0.0f;
}

void get_perspective_matrix(REAL *output_matrix, REAL fovy, REAL aspect, REAL near, REAL far) {
    REAL radians = DEG_TO_RAD(fovy);
    REAL tan_half_fovy = tan(radians/2.0f);

    output_matrix[0] = 1.0f/(tan_half_fovy*aspect);
    output_matrix[1] = 0.0f;
    output_matrix[2] = 0.0f;
    output_matrix[3] = 0.0f;

    output_matrix[4] = 0.0f;
    output_matrix[5] = 1.0f/tan_half_fovy;
    output_matrix[6] = 0.0f;
    output_matrix[7] = 0.0f;

    output_matrix[8] = 0.0f;
    output_matrix[9] = 0.0f;
    output_matrix[10] = -(far + near) / (far - near);
    output_matrix[11] = -2.0f * far * near / (far - near);;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = -1.0f;
    output_matrix[15] = 0.0f;
}

/*
void get_lookat_matrix(REAL *output_matrix, REAL eye_x, REAL eye_y, REAL eye_z,
                        REAL center_x, REAL center_y, REAL center_z,
                        REAL up_x, REAL up_y, REAL up_z) {
    vector3 f, up, s, s_normalized, u;
    REAL translation_matrix[16], orientation_matrix[16];
    f.x = center_x - eye_x;
    f.y = center_y - eye_y;
    f.z = center_z - eye_z;

    up.x = up_x;
    up.y = up_y;
    up.z = up_z;

    vector3_normalize(&f);
    vector3_normalize(&up);
    vector3_cross_product(&f, &up, &s);

    s_normalized = s;
    vector3_normalize(&s_normalized);
    vector3_cross_product(&s_normalized, &f, &u);

    orientation_matrix[0] = s.x;
    orientation_matrix[1] = s.y;
    orientation_matrix[2] = s.z;
    orientation_matrix[3] = 0;

    orientation_matrix[4] = u.x;
    orientation_matrix[5] = u.y;
    orientation_matrix[6] = u.z;
    orientation_matrix[7] = 0;

    orientation_matrix[8] = -f.x;
    orientation_matrix[9] = -f.y;
    orientation_matrix[10] = -f.z;
    orientation_matrix[11] = 0;

    orientation_matrix[12] = 0;
    orientation_matrix[13] = 0;
    orientation_matrix[14] = 0;
    orientation_matrix[15] = 1;

    get_translation_matrix4x4(translation_matrix, -eye_x, -eye_y, -eye_z);
    mult_matrices(output_matrix, orientation_matrix, translation_matrix, 4);
}
*-/

void get_identity_matrix(REAL *output_matrix) {
    output_matrix[0] = 1.0f;
    output_matrix[1] = 0.0f;
    output_matrix[2] = 0.0f;
    output_matrix[3] = 0.0f;

    output_matrix[4] = 0.0f;
    output_matrix[5] = 1.0f;
    output_matrix[6] = 0.0f;
    output_matrix[7] = 0.0f;

    output_matrix[8] = 0.0f;
    output_matrix[9] = 0.0f;
    output_matrix[10] = 1.0f;
    output_matrix[11] = 0.0f;

    output_matrix[12] = 0.0f;
    output_matrix[13] = 0.0f;
    output_matrix[14] = 0.0f;
    output_matrix[15] = 1.0f;
}

void get_identity_matrix3x3(REAL *output_matrix) {
    output_matrix[0] = 1.0f;
    output_matrix[1] = 0.0f;
    output_matrix[2] = 0.0f;

    output_matrix[3] = 0.0f;
    output_matrix[4] = 1.0f;
    output_matrix[5] = 0.0f;

    output_matrix[6] = 0.0f;
    output_matrix[7] = 0.0f;
    output_matrix[8] = 1.0f;
}

void transpose_matrix(REAL *output_matrix, REAL *input_matrix, int matrix_size) {
    int i, j;

    for (i = 0; i < matrix_size; i++) {
        for (j = 0; j < matrix_size; j++) {
            output_matrix[i*matrix_size + j] = input_matrix[i + j*matrix_size];
        }
    }
}

unsigned char inverse_matrix_2x2(REAL output_matrix[4], REAL input_matrix[4]) {
    REAL det = mat2x2_det(input_matrix);
    if (fabsf(det) < 0.001) {
        memset(output_matrix, 0, sizeof(REAL) * 4);
        return 0;
    }

    output_matrix[0] =  input_matrix[3] / det;
    output_matrix[1] = -input_matrix[1] / det;

    output_matrix[2] = -input_matrix[2] / det;
    output_matrix[3] =  input_matrix[0] / det;

    return 1;
}

unsigned char inverse_matrix_3x3(REAL output_matrix[9], REAL input_matrix[9]) {
    REAL det = mat3x3_det(input_matrix);
    // for shortening
    REAL *in = input_matrix;

    int row1 = 0;
    int row2 = 3;
    int row3 = 6;

    int col1 = 0;
    int col2 = 1;
    int col3 = 2;

    //printf("%.2f\n", det);
    if (fabsf(det) < 0.001) {
        memset(output_matrix, 0, sizeof(REAL) * 9);
        return 0;
    }

    //printf("input_matrix:\n");
    //dump_matrix3x3(input_matrix);
    //printf("\n\n");

    output_matrix[0] =  (in[row2+col2] * in[row3+col3] - in[row2+col3] * in[row3+col2]) / det;
    output_matrix[1] = (in[row1+col3] * in[row3+col2] - in[row1+col2] * in[row3+col3]) / det;
    output_matrix[2] = (in[row1+col2] * in[row2+col3] - in[row1+col3] * in[row2+col2]) / det;

    output_matrix[3] =  (in[row2+col3] * in[row3+col1] - in[row2+col1] * in[row3+col3]) / det;
    output_matrix[4] =  (in[row1+col1] * in[row3+col3] - in[row1+col3] * in[row3+col1]) / det;
    output_matrix[5] =  (in[row1+col3] * in[row2+col1] - in[row1+col1] * in[row2+col3]) / det;

    output_matrix[6] =  (in[row2+col1] * in[row3+col2] - in[row2+col2] * in[row3+col1]) / det;
    output_matrix[7] =  (in[row1+col2] * in[row3+col1] - in[row1+col1] * in[row3+col2]) / det;
    output_matrix[8] =  (in[row1+col1] * in[row2+col2] - in[row1+col2] * in[row2+col1]) / det;

    return 1;
}

unsigned char inverse_matrix_4x4(REAL output_matrix[16], REAL input_matrix[16]) {
    double inv[16], det;
    int i;

    inv[0] = input_matrix[5]  * input_matrix[10] * input_matrix[15] -
             input_matrix[5]  * input_matrix[11] * input_matrix[14] -
             input_matrix[9]  * input_matrix[6]  * input_matrix[15] +
             input_matrix[9]  * input_matrix[7]  * input_matrix[14] +
             input_matrix[13] * input_matrix[6]  * input_matrix[11] -
             input_matrix[13] * input_matrix[7]  * input_matrix[10];

    inv[4] = -input_matrix[4]  * input_matrix[10] * input_matrix[15] +
              input_matrix[4]  * input_matrix[11] * input_matrix[14] +
              input_matrix[8]  * input_matrix[6]  * input_matrix[15] -
              input_matrix[8]  * input_matrix[7]  * input_matrix[14] -
              input_matrix[12] * input_matrix[6]  * input_matrix[11] +
              input_matrix[12] * input_matrix[7]  * input_matrix[10];

    inv[8] = input_matrix[4]  * input_matrix[9] * input_matrix[15] -
             input_matrix[4]  * input_matrix[11] * input_matrix[13] -
             input_matrix[8]  * input_matrix[5] * input_matrix[15] +
             input_matrix[8]  * input_matrix[7] * input_matrix[13] +
             input_matrix[12] * input_matrix[5] * input_matrix[11] -
             input_matrix[12] * input_matrix[7] * input_matrix[9];

    inv[12] = -input_matrix[4]  * input_matrix[9] * input_matrix[14] +
               input_matrix[4]  * input_matrix[10] * input_matrix[13] +
               input_matrix[8]  * input_matrix[5] * input_matrix[14] -
               input_matrix[8]  * input_matrix[6] * input_matrix[13] -
               input_matrix[12] * input_matrix[5] * input_matrix[10] +
               input_matrix[12] * input_matrix[6] * input_matrix[9];

    inv[1] = -input_matrix[1]  * input_matrix[10] * input_matrix[15] +
              input_matrix[1]  * input_matrix[11] * input_matrix[14] +
              input_matrix[9]  * input_matrix[2] * input_matrix[15] -
              input_matrix[9]  * input_matrix[3] * input_matrix[14] -
              input_matrix[13] * input_matrix[2] * input_matrix[11] +
              input_matrix[13] * input_matrix[3] * input_matrix[10];

    inv[5] = input_matrix[0]  * input_matrix[10] * input_matrix[15] -
             input_matrix[0]  * input_matrix[11] * input_matrix[14] -
             input_matrix[8]  * input_matrix[2] * input_matrix[15] +
             input_matrix[8]  * input_matrix[3] * input_matrix[14] +
             input_matrix[12] * input_matrix[2] * input_matrix[11] -
             input_matrix[12] * input_matrix[3] * input_matrix[10];

    inv[9] = -input_matrix[0]  * input_matrix[9] * input_matrix[15] +
              input_matrix[0]  * input_matrix[11] * input_matrix[13] +
              input_matrix[8]  * input_matrix[1] * input_matrix[15] -
              input_matrix[8]  * input_matrix[3] * input_matrix[13] -
              input_matrix[12] * input_matrix[1] * input_matrix[11] +
              input_matrix[12] * input_matrix[3] * input_matrix[9];

    inv[13] = input_matrix[0]  * input_matrix[9] * input_matrix[14] -
              input_matrix[0]  * input_matrix[10] * input_matrix[13] -
              input_matrix[8]  * input_matrix[1] * input_matrix[14] +
              input_matrix[8]  * input_matrix[2] * input_matrix[13] +
              input_matrix[12] * input_matrix[1] * input_matrix[10] -
              input_matrix[12] * input_matrix[2] * input_matrix[9];

    inv[2] = input_matrix[1]  * input_matrix[6] * input_matrix[15] -
             input_matrix[1]  * input_matrix[7] * input_matrix[14] -
             input_matrix[5]  * input_matrix[2] * input_matrix[15] +
             input_matrix[5]  * input_matrix[3] * input_matrix[14] +
             input_matrix[13] * input_matrix[2] * input_matrix[7] -
             input_matrix[13] * input_matrix[3] * input_matrix[6];

    inv[6] = -input_matrix[0]  * input_matrix[6] * input_matrix[15] +
              input_matrix[0]  * input_matrix[7] * input_matrix[14] +
              input_matrix[4]  * input_matrix[2] * input_matrix[15] -
              input_matrix[4]  * input_matrix[3] * input_matrix[14] -
              input_matrix[12] * input_matrix[2] * input_matrix[7] +
              input_matrix[12] * input_matrix[3] * input_matrix[6];

    inv[10] = input_matrix[0]  * input_matrix[5] * input_matrix[15] -
              input_matrix[0]  * input_matrix[7] * input_matrix[13] -
              input_matrix[4]  * input_matrix[1] * input_matrix[15] +
              input_matrix[4]  * input_matrix[3] * input_matrix[13] +
              input_matrix[12] * input_matrix[1] * input_matrix[7] -
              input_matrix[12] * input_matrix[3] * input_matrix[5];

    inv[14] = -input_matrix[0]  * input_matrix[5] * input_matrix[14] +
               input_matrix[0]  * input_matrix[6] * input_matrix[13] +
               input_matrix[4]  * input_matrix[1] * input_matrix[14] -
               input_matrix[4]  * input_matrix[2] * input_matrix[13] -
               input_matrix[12] * input_matrix[1] * input_matrix[6] +
               input_matrix[12] * input_matrix[2] * input_matrix[5];

    inv[3] = -input_matrix[1] * input_matrix[6] * input_matrix[11] +
              input_matrix[1] * input_matrix[7] * input_matrix[10] +
              input_matrix[5] * input_matrix[2] * input_matrix[11] -
              input_matrix[5] * input_matrix[3] * input_matrix[10] -
              input_matrix[9] * input_matrix[2] * input_matrix[7] +
              input_matrix[9] * input_matrix[3] * input_matrix[6];

    inv[7] = input_matrix[0] * input_matrix[6] * input_matrix[11] -
             input_matrix[0] * input_matrix[7] * input_matrix[10] -
             input_matrix[4] * input_matrix[2] * input_matrix[11] +
             input_matrix[4] * input_matrix[3] * input_matrix[10] +
             input_matrix[8] * input_matrix[2] * input_matrix[7] -
             input_matrix[8] * input_matrix[3] * input_matrix[6];

    inv[11] = -input_matrix[0] * input_matrix[5] * input_matrix[11] +
               input_matrix[0] * input_matrix[7] * input_matrix[9] +
               input_matrix[4] * input_matrix[1] * input_matrix[11] -
               input_matrix[4] * input_matrix[3] * input_matrix[9] -
               input_matrix[8] * input_matrix[1] * input_matrix[7] +
               input_matrix[8] * input_matrix[3] * input_matrix[5];

    inv[15] = input_matrix[0] * input_matrix[5] * input_matrix[10] -
              input_matrix[0] * input_matrix[6] * input_matrix[9] -
              input_matrix[4] * input_matrix[1] * input_matrix[10] +
              input_matrix[4] * input_matrix[2] * input_matrix[9] +
              input_matrix[8] * input_matrix[1] * input_matrix[6] -
              input_matrix[8] * input_matrix[2] * input_matrix[5];

    det = input_matrix[0] * inv[0] + input_matrix[1] * inv[4] + input_matrix[2] * inv[8] + input_matrix[3] * inv[12];

    if (det == 0) {
        return 0;
    }

    det = 1.0 / det;

    for (i = 0; i < 16; i++) {
        output_matrix[i] = inv[i] * det;
    }

    return 1;
}

unsigned char calc_normal_matrix(REAL output[9], REAL m[16]) {
    REAL d = m[0] * m[5] * m[10]
            - m[0] * m[6] * m[8]
            + m[1] * m[6] * m[8]
            - m[1] * m[4] * m[10]
            + m[2] * m[4] * m[9]
            - m[2] * m[5] * m[8];
    if (fabs(d) > 0.0f) {
        output[0] = -m[6] * m[9] + m[5] * m[10];
        output[1] = m[6] * m[8] - m[4] * m[10];
        output[2] = -m[5] * m[8] + m[4] * m[9];
        output[3] = m[2] * m[9] - m[1] * m[10];
        output[4] = -m[2] * m[8] + m[0] * m[10];
        output[5] = m[1] * m[8] - m[0] * m[9];
        output[6] = -m[2] * m[5] + m[1] * m[6];
        output[7] = m[2] * m[4] - m[0] * m[6];
        output[8] = -m[1] * m[4] + m[0] * m[5];

        return 1;
    }

    get_identity_matrix3x3(output);

    return 0;
}

char is_point_inside_polygon2(point2 *polygon_vertices, int polygon_vertices_count, point2 *point) {
    int i;
    vector2 v1, v2;
    REAL result_z;
    char is_point_in_polygon = 1;

    for (i = 0; i < polygon_vertices_count; i++) {
        v1.x = polygon_vertices[(i+1) % polygon_vertices_count].x - polygon_vertices[i].x;
        v1.y = polygon_vertices[(i+1) % polygon_vertices_count].y - polygon_vertices[i].y;

        v2.x = point->x - polygon_vertices[i].x;
        v2.y = point->y - polygon_vertices[i].y;

        result_z = v1.x * v2.y - v1.y * v2.x;
        if (result_z < 0.0f) {
            is_point_in_polygon = 0;
            break;
        }
    }

    return is_point_in_polygon;
}

inline REAL get_interpolated_value(REAL Ax, REAL Ay, REAL Bx, REAL By, REAL Cx) {
    return Ay + (((Cx - Ax) / (Bx - Ax)) * (By - Ay));
}

void get_normal(point3 *p1, point3 *p2, point3 *p3, vector3 *result) {
    vector3 v1, v2;

    v1.x = p1->x - p2->x;
    v1.y = p1->y - p2->y;
    v1.z = p1->z - p2->z;

    v2.x = p1->x - p3->x;
    v2.y = p1->y - p3->y;
    v2.z = p1->z - p3->z;

    result->x = v1.y * v2.z - v1.z * v2.y;
    result->y = v1.z * v2.x - v1.x * v2.z;
    result->z = v1.x * v2.y - v1.y * v2.x;

    vector3_normalize(result);
}

void get_normal_fv(const REAL p1[3], const REAL p2[3], const REAL p3[3], REAL result[3]) {
    REAL v1[3];
    REAL v2[3];

    v1[0] = p1[0] - p2[0];
    v1[1] = p1[1] - p2[1];
    v1[2] = p1[2] - p2[2];

    v2[0] = p1[0] - p3[0];
    v2[1] = p1[1] - p3[1];
    v2[2] = p1[2] - p3[2];

    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];

    vector3_normalize_fv(result);
}

unsigned char check_if_picked_triangle(vector3 *v0, vector3 *v1, vector3 *v2, vector3 *o, vector3 *d) {
    vector3 t1, t2, n, p;
    REAL front_facing, dist;
    REAL mat3x3[9], output_matrix[9];
    // uvw_vector - wspolrzedne barycentryczne
    REAL vector_p[3], uvw_vector[3];

    vector3_sub(v1, v0, &t1);
    vector3_sub(v2, v0, &t2);
    vector3_cross_product(&t1, &t2, &n);

    front_facing = -vector3_dot_product(&n, d);
    dist = (vector3_dot_product(&n, o) - vector3_dot_product(v0, &n)) / front_facing;

    if (dist < 0.0f) {
        return 0;
    }

    // punkt przeciecia prostej z plaszczyczyzna
    t1 = *d;
    vector3_mul(&t1, dist);
    vector3_add(o, &t1, &p);


    mat3x3[0] = v0->x;
    mat3x3[1] = v1->x;
    mat3x3[2] = v2->x;

    mat3x3[3] = v0->y;
    mat3x3[4] = v1->y;
    mat3x3[5] = v2->y;

    mat3x3[6] = v0->z;
    mat3x3[7] = v1->z;
    mat3x3[8] = v2->z;

    inverse_matrix_3x3(output_matrix, mat3x3);
    vector_p[0] = p.x;
    vector_p[1] = p.y;
    vector_p[2] = p.z;

    transform_vector(uvw_vector, output_matrix, vector_p, 3);

    if (uvw_vector[0] < 0.0f || uvw_vector[0] > 1.0f || uvw_vector[1] < 0.0f || uvw_vector[0] + uvw_vector[1] > 1.0f) {
        return 0;
    }

    return 1;
}

// poniższa funkcja jest nieoptymalna
unsigned int recursive_fib(unsigned int n) {
    if (n <= 1) {
        return n;
    }
    return recursive_fib(n - 1) + recursive_fib(n - 2);
}

// poniższa funkcja jest nieoptymalna
unsigned long recursive_fact(unsigned int n) {
    return ((n <= 1) ? 1 : (n * recursive_fact(n - 1)));
}

REAL sin_ex(REAL x) {
    REAL sqr = x * x;
    return x * (1.0f - sqr / 2.0f / 3.0f
             * (1.0f - sqr / 4.0f / 5.0f
             * (1.0f - sqr / 6.0f / 7.0f
             * (1.0f - sqr / 8.0f / 9.0f
             * (1.0f - sqr / 10.0f / 11.0f
             * (1.0f - sqr / 12.0f / 13.0f
             * (1.0f - sqr / 14.0f / 15.0f
             * (1.0f - sqr / 16.0f / 17.0f
             * (1.0f - sqr / 18.0f / 19.0f
             * (1.0f - sqr / 20.0f / 21.0f
             ) ) ) ) ) ) ) ) ) );
}

unsigned long int sum_series_simple(unsigned int max) {
    unsigned long int s;
    
    s = 0;
    for (unsigned int i = 1; i <= max; i++) {
        s += i;
    }
    
    return s;
}

long int sum_array(int *array, size_t array_elements_count) {
    unsigned long int s;
    
    s = 0;
    for (unsigned int i = 0; i < array_elements_count; i++) {
        s += array[i];
    }
    
    return s;
}

float average_array(int *array, size_t array_elements_count) {
    float s;
    
    s = 0.0f;
    for (unsigned int i = 0; i < array_elements_count; i++) {
        s += array[i];
    }
    
    return s / array_elements_count;
}





void trig_init(t_trig *trig, int type) {
    trig->fn.is_zero = (int (*)(struct _function *fn))trig_is_zero;
    trig->fn.evaluate = (REAL (*)(struct _function *fn, REAL x))trig_evaluate;
    trig->fn.differentiate = (void (*)(struct _function *fn))trig_differentiate;

    trig->func = NULL;
    switch (type) {
        case 1:
            trig->func = sinf;
            break;
        case 2:
            trig->func = cosf;
            break;
        default:
            break;
    };
    trig->sign = 1;
}

int trig_is_zero(t_trig *trig) {
    return 0;
}
float trig_evaluate(t_trig *trig, float x) {
    return trig->func(x) * trig->sign;
}
void trig_differentiate(t_trig *trig) {
    if (trig->func == sinf) {
        trig->func = cosf;
    } else if (trig->func == cosf) {
        trig->func = sinf;
        trig->sign = -trig->sign;
    }
}



int polynomial_init(t_polynomial *poly, int degree, REAL a0) {
    poly->fn.is_zero = (int (*)(struct _function *fn))polynomial_is_zero;
    poly->fn.evaluate = (REAL (*)(struct _function *fn, REAL x))polynomial_evaluate_poly;
    poly->fn.differentiate = (void (*)(struct _function *fn))polynomial_differentiate;

    poly->degree = degree;
    poly->max_degree = degree;
    poly->coefficients = malloc((poly->max_degree+1) * sizeof(REAL));
    if (!poly->coefficients) {
        memset(poly, 0, sizeof(t_polynomial));
        return 0;
    }
    poly->coefficients[0] = a0;
    return 1;
}
void polynomial_free(t_polynomial *poly) {
    if (poly->coefficients) {
        free(poly->coefficients);
        memset(poly, 0, sizeof(t_polynomial));
        return;
    }
}
void polynomial_print(t_polynomial *poly) {
    for (int i = poly->degree; i >= 0; i--) {
        if (i == poly->degree && i > 0) {
            printf("%.6f*x^%d %c ", poly->coefficients[i], i, (poly->coefficients[i-1] >= 0.0f) ? '+' : '-');
        } else if (i > 1) {
            printf("%.6f*x^%d %c ", fabs(poly->coefficients[i]), i, (poly->coefficients[i-1] >= 0.0f) ? '+' : '-');
        } else if (i == 1) {
            printf("%.6f*x %c ", fabs(poly->coefficients[i]), (poly->coefficients[i-1] >= 0.0f) ? '+' : '-');
        } else if (i == 0) {
            if (poly->degree > 0) {
                printf("%.6f", fabs(poly->coefficients[i]));
            } else {
                printf("%.6f", poly->coefficients[i]);
            }
        }
    }
    printf("\n");
}
int polynomial_set_degree(t_polynomial *poly, int degree) {
    if (degree > poly->max_degree) {
        REAL *coeff = poly->coefficients;
        poly->coefficients = realloc(poly->coefficients, (degree+1) * sizeof(REAL));
        if (!poly->coefficients) {
            poly->coefficients = coeff;
            return 0;
        }
        for (int i = poly->max_degree + 1; i <= degree; i++) {
            poly->coefficients[i] = 0.0f;
        }
        poly->max_degree = degree;
    } else {
        for (int i = degree + 1; i <= poly->max_degree; i++) {
            poly->coefficients[i] = 0.0f;
        }
    }
    poly->degree = degree;
    return 1;
}
REAL polynomial_evaluate_poly(t_polynomial *poly, REAL x) {
    REAL powx = 1.0f;
    REAL sum = 0.0f;

    for (int i = 0; i <= poly->degree; i++) {
        sum += powx * poly->coefficients[i];
        powx *= x;
    }

    return sum;
}
void polynomial_change_input_units(t_polynomial *poly, REAL old_units, REAL new_units) {
    REAL mult = old_units / new_units;
    REAL powmult = 1.0f;

    for (int i = 0; i <= poly->degree; i++) {
        poly->coefficients[i] *= powmult;
        powmult *= mult;
    }
}
void polynomial_change_output_units(t_polynomial *poly, REAL old_units, REAL new_units) {
    REAL mult = new_units / old_units;
    //REAL powmult = 1.0f;

    for (int i = 0; i <= poly->degree; i++) {
        poly->coefficients[i] *= mult;
    }
}
void polynomial_differentiate(t_polynomial *poly) {
    for (int i = 1; i <= poly->degree; i++) {
        poly->coefficients[i-1] = poly->coefficients[i] * i;
    }
    poly->coefficients[poly->degree] = 0.0f;
    if (poly->degree > 0) {
        poly->degree -= 1;
    }
}
inline int polynomial_is_zero(t_polynomial *poly) {
    return poly->degree == 0 && fabs(poly->coefficients[0]) < 0.000001f;
}
int polynomial_copy(t_polynomial *dst_poly, t_polynomial *src_poly) {
    REAL nfact = 1.0f;

    if (dst_poly->degree < src_poly->degree) {
        if (!polynomial_set_degree(dst_poly, src_poly->degree)) {
            return 0;
        }
    }

    dst_poly->degree = 0;
    dst_poly->coefficients[0] = polynomial_evaluate_poly(src_poly, 0.0f);
    polynomial_differentiate(src_poly);

    while (!polynomial_is_zero(src_poly)) {
        dst_poly->degree++;
        nfact *= dst_poly->degree;
        dst_poly->coefficients[dst_poly->degree] = polynomial_evaluate_poly(src_poly, 0.0f) / nfact;
        polynomial_differentiate(src_poly);
    }

    return 1;
}
int polynomial_copy_fast(t_polynomial *dst_poly, t_polynomial *src_poly) {
    if (dst_poly->degree < src_poly->degree) {
        if (!polynomial_set_degree(dst_poly, src_poly->degree)) {
            return 0;
        }
    } else {
        dst_poly->degree = src_poly->degree;
    }
    memcpy(dst_poly->coefficients, src_poly->coefficients, (src_poly->degree+1) * sizeof(REAL));
    return 1;
}
void polynomial_force_odd(t_polynomial *poly) {
    for (int i = 0; i <= poly->degree; i+=2) {
        poly->coefficients[i] = 0;
    }
}
void polynomial_force_even(t_polynomial *poly) {
    for (int i = 1; i <= poly->degree; i+=2) {
        poly->coefficients[i] = 0;
    }
}
void polynomial_multiply_by_factor(t_polynomial *poly, REAL a) {
    polynomial_set_degree(poly, poly->degree + 1);
    for (int i = poly->degree; i > 0; i--) {
        poly->coefficients[i] = poly->coefficients[i-1] + poly->coefficients[i] * a;
    }
    poly->coefficients[0] = poly->coefficients[0] * a;
}
void polynomial_add(t_polynomial *dst_poly, t_polynomial *src_poly) {
    if (dst_poly->degree < src_poly->degree) {
        polynomial_set_degree(dst_poly, src_poly->degree);
    }

    for (int i = 0; i <= dst_poly->degree; i++) {
        dst_poly->coefficients[i] += src_poly->coefficients[i];
    }
}
void polynomial_add_scaled(t_polynomial *dst_poly, t_polynomial *src_poly, REAL scale) {
    if (dst_poly->degree < src_poly->degree) {
        polynomial_set_degree(dst_poly, src_poly->degree);
    }

    for (int i = 0; i <= dst_poly->degree; i++) {
        dst_poly->coefficients[i] += scale * src_poly->coefficients[i];
    }
}
inline void polynomial_offset_range(t_polynomial *poly, REAL offset) {
    poly->coefficients[0] += offset;
}
void polynomial_offset_domain(t_polynomial *poly, REAL offset) {
    t_polynomial ppow, result;

    polynomial_init(&ppow, 0, 1);
    polynomial_init(&result, 0, 0);

    for (int i = 0; i < poly->degree; i++) {
        polynomial_add_scaled(&result, &ppow, poly->coefficients[i]);
        polynomial_multiply_by_factor(&ppow, -offset);
    }

    polynomial_set_degree(poly, result.degree);
    memcpy(poly->coefficients, result.coefficients, result.degree * sizeof(REAL));
    //memcpy(poly->coefficients, result.coefficients, (result.degree+1) * sizeof(REAL));
    // for (int i = 0; i < poly->degree; i++) {
    //     poly->coefficients[i] = result.coefficients[i];
    // }

    polynomial_free(&ppow);
    polynomial_free(&result);
}
void polynomial_make_taylor_series(t_polynomial *out_poly, t_function *in_function, REAL offset, int max_degree) {
    REAL nfact = 1;

    polynomial_set_degree(out_poly, 0);
    out_poly->coefficients[0] = in_function->evaluate(in_function, offset);
    in_function->differentiate(in_function);

    while (!in_function->is_zero(in_function) && out_poly->degree < max_degree) {
        polynomial_set_degree(out_poly, out_poly->degree + 1);
        nfact *= (REAL)out_poly->degree;
        out_poly->coefficients[out_poly->degree] = in_function->evaluate(in_function, offset) / nfact;
        in_function->differentiate(in_function);
    }

    polynomial_offset_domain(out_poly, offset);
}
void polynomial_make_lagrange_series(t_polynomial *out_poly, int num_points, REAL *x, REAL *y) {
    polynomial_set_degree(out_poly, 0);
    out_poly->coefficients[0] = 0.0f;

    t_polynomial temp;
    polynomial_init(&temp, 0, 1.0f);
    for (int j = 0; j < num_points; j++) {
        polynomial_set_degree(&temp, 0);
        temp.coefficients[0] = 1.0f;
        for (int i = 0; i < num_points; i++) {
            if (i != j) {
                polynomial_multiply_by_factor(&temp, -x[i]);
            }
        }
        polynomial_change_output_units(&temp, temp.fn.evaluate((t_function*)&temp, x[j]), y[j]);
        polynomial_add(out_poly, &temp);
    }
    polynomial_free(&temp);
}

*/

bool Tilc::TMath::IsPrime(long long n)
{
    if (n <= 1)
    {
        return false;
    }
    if (n == 2 || n == 3)
    {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0)
    {
        return false;
    }
    long long k = 1;
    while ((6 * k - 1) * (6 * k - 1) <= n)
    {
        if (n % (6 * k - 1) == 0 || n % (6 * k + 1) == 0)
        {
            return false;
        }
        ++k;
    }
    return true;
}

long long int Tilc::TMath::Round(double v)
{
    if ((v + 0.5) >= (int(v) + 1))
    {
        return int(v) + 1;
    }
    return int(v);
}
