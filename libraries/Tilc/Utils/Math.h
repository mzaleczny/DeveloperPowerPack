#pragma once

#define _USE_MATH_DEFINES 1
#include <math.h>
#include <stdlib.h>
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"

namespace Tilc
{
    #define DEG_TO_RAD(degrees) ((degrees) * M_PI / 180.0)
    #define RAD_TO_DEG(rad) ((rad) * 180.0 / M_PI)
    #define MIN2(a, b) ((a) < (b) ? (a) : (b))
    #define MIN3(a, b, c) (MIN2(MIN2((a), (b)), (c)))
    #define MAX2(a, b) ((a) > (b) ? (a) : (b))
    #define MAX3(a, b, c) (MAX2(MAX2((a), (b)), (c)))

    #ifndef REAL
    #define REAL float
    #endif

    class DECLSPEC TMath
    {
        static bool IsPrime(long long n);
        static long long int Round(double v);
    };
    /* poniższe dwie funkcje inicjalizują wektor pdanymi wartościami *-/
    vector3 vector3_new(REAL x, REAL y, REAL z);
    void vector3_set(vector3 *v, REAL x, REAL y, REAL z);
    /* oblicza wartość skalarną wektora *-/
    REAL vector3_magnitude(vector3 *v);
    /* normalizuje podany wektor, czyli zamienia go w wektor tak samo skierowany, ale o wartości skalarnej równej 1 *-/
    void vector3_normalize(vector3 *v);
    /* zwraca wektor przeciwny do podanego *-/
    void vector3_reverse(vector3 *v_in, vector3 *v_out);
    /* dodawanie wektorów *-/
    void vector3_add(vector3 *v1_in, vector3 *v2_in, vector3 *v_out);
    /* odejmowanie wektorów *-/
    void vector3_sub(vector3 *v1_in, vector3 *v2_in, vector3 *v_out);
    /* mnożenie wektora przez skalar *-/
    void vector3_mul(vector3 *v, REAL s);
    /* dzielenie wektora przez skalar *-/
    void vector3_div(vector3 *v, REAL s);
    /* zmiana znaku wektora *-/
    void vector3_neg(vector3 *v);

    /* Oblicza iloczyn wektorowy (cross product) podanych wektorów. Wynikowy wektor jest prostopadły do obu wektorów,
     * a więc prostopadły do płaszczyzny, w której leżą oba mnożone wektory *-/
    void vector3_cross_product(vector3 *v1_in, vector3 *v2_in, vector3 *v_out);
    /* Oblicza iloczyn skalarny (dot product) wektorów. Jest to skalar *-/
    REAL vector3_dot_product(vector3 *v1, vector3 *v2);
    /* Oblicza skalarny iloczyn mieszany trzech wektorów: u * (v x w). Jest to skalar *-/
    REAL vector3_triple_dot_product(vector3 *u, vector3 *v, vector3 *w);


    // ustawia składowe podanego wektora
    void vector3_set_f(REAL v[3], REAL x, REAL y, REAL z);
    // kopiuje składowe wektora v_source do wektora v
    void vector3_set_fv(REAL v[3], REAL v_source[3]);
    /* oblicza wartość skalarną wektora *-/
    REAL vector3_magnitude_fv(REAL v[3]);
    /* normalizuje podany wektor, czyli zamienia go w wektor tak samo skierowany, ale o wartości skalarnej równej 1 *-/
    void vector3_normalize_fv(REAL v[3]);
    /* zwraca wektor przeciwny do podanego *-/
    void vector3_reverse_fv(REAL v_in[3], REAL v_out[3]);
    /* dodawanie wektorów *-/
    void vector3_add_fv(REAL v1_in[3], REAL v2_in[3], REAL v_out[3]);
    /* odejmowanie wektorów *-/
    void vector3_sub_fv(REAL v1_in[3], REAL v2_in[3], REAL v_out[3]);
    /* mnożenie wektora przez skalar *-/
    void vector3_mul_fv(REAL v[3], REAL s);
    /* dzielenie wektora przez skalar *-/
    void vector3_div_fv(REAL v[3], REAL s);
    /* zmiana znaku wektora *-/
    void vector3_neg_fv(REAL v[3]);
    /* mnoży oba wektory *-/
    void vector3_mul_vector3_fv(REAL output[3], REAL v1[3], REAL v2[3]);

    /* Oblicza iloczyn wektorowy (cross product) podanych wektorów. Wynikowy wektor jest prostopadły do obu wektorów,
     * a więc prostopadły do płaszczyzny, w której leżą oba mnożone wektory *-/
    void vector3_cross_product_fv(REAL v1_in[3], REAL v2_in[3], REAL v_out[3]);
    /* Oblicza iloczyn skalarny (dot product) wektorów. Jest to skalar *-/
    REAL vector3_dot_product_fv(REAL v1[3], REAL v2[3]);
    /* Oblicza skalarny iloczyn mieszany trzech wektorów: u * (v x w). Jest to skalar *-/
    REAL vector3_triple_dot_product_fv(REAL u[3], REAL v[3], REAL w[3]);


    /* Ustawia współrzędne z na podaną wartość we wszystkich punktach (x,y,z) tablicy *-/
    void set_z_coordinate_of_points_array(REAL z, REAL *coords, unsigned long coords_size);

    // **************************** UWAGA **********************************
    // Macierze (wejściowe i wyjściowe) przechowywane są w orientacji wierszowej:
    // wiersz pierwszy: matrix[0] matrix[1] matrix[2] matrix[3]
    // wiersz drugi: matrix[4] matrix[5] matrix[6] matrix[7]
    // wiersz trzeci: matrix[8] matrix[9] matrix[10] matrix[11]
    // wiersz czwarty: matrix[12] matrix[13] matrix[14] matrix[15]
    void dump_matrix4x4(REAL *matrix);
    void dump_vector4(REAL *vector);
    void dump_matrix3x3(REAL *matrix);
    void dump_vector3(REAL *vector);
    void dump_coords_3d(REAL *coords, unsigned long coords_size);
    void mat3x3_from_mat4x4(REAL out_mat3x3[9], REAL in_mat4x4[16]);
    // macierze są mnożone w kolejności matrix1 * matrix2
    void mult_matrices(REAL *output_matrix, REAL *matrix1, REAL *matrix2, int matrix_size);
    void mult_matrices4x4(REAL output_matrix[16], REAL m1[16], REAL m2[16]);
    // Mnoży trzy macierze 4x4 w kolejności od prawej do lewej, a więc:
    // output_matrix = m3 * (m2 * m1)
    void mult_matrices4x4_3(REAL output_matrix[16], REAL m3[16], REAL m2[16], REAL m1[16]);
    // Mnoży trzy macierze 4x4 w kolejności od prawej do lewej, a więc:
    // output_matrix = m4 * (m3 * (m2 * m1))
    void mult_matrices4x4_4(REAL output_matrix[16], REAL m4[16], REAL m3[16], REAL m2[16], REAL m1[16]);
    // Mnoży macierz wejściową o podanych wymiarach przez skalar i zwraca wynik w podanej macierzy wyjściowej.
    // Macierz wyjściowa jest dokładnie tego samego rozmiaru co macierz wejściowa.
    void mult_matrix_and_scalar(REAL *output_matrix, REAL *input_matrix, REAL scalar, int matrix_cols, int matrix_rows);
    // macierz i wektor są mnożone w kolejności: input_matrix * input_vector
    void transform_vector(REAL *output_vector, REAL *input_matrix, REAL *input_vector, int vec_matrix_size);


    // Poniżej kąty są w radianach
    void get_rotation_x_matrix4x4(REAL *output_matrix, REAL angle);
    void get_rotation_y_matrix4x4(REAL *output_matrix, REAL angle);
    void get_rotation_z_matrix4x4(REAL *output_matrix, REAL angle);
    void get_rotation_matrix4x4_fv(REAL *output_matrix, REAL angle, REAL v[3]);

    void get_translation_matrix4x4v(REAL *output_matrix, REAL *v);
    void get_translation_matrix4x4(REAL *output_matrix, REAL x, REAL y, REAL z);
    void get_translation_matrix4x4v_rev(REAL *output_matrix, REAL *v);
    void get_translation_matrix4x4_rev(REAL *output_matrix, REAL x, REAL y, REAL z);

    void get_scale_matrix4x4(REAL *output_matrix, REAL scale_x, REAL scale_y, REAL scale_z);
    void get_scale_matrix4x4_rev(REAL *output_matrix, REAL scale_x, REAL scale_y, REAL scale_z);

    // Transponuje podaną macierz wejściową.
    void transpose_matrix(REAL *output_matrix, REAL *input_matrix, int matrix_size);
    // Odwraca macierz 2x2 i zwraca 1, jeśli macierz odwrotna dla podanej macierzy wejściowej istnieje.
    // W przeciwnym razie zwraca 0.
    unsigned char inverse_matrix_2x2(REAL output_matrix[4], REAL input_matrix[4]);
    // Odwraca macierz 3x3 i zwraca 1, jeśli macierz odwrotna dla podanej macierzy wejściowej istnieje.
    // W przeciwnym razie zwraca 0.
    unsigned char inverse_matrix_3x3(REAL output_matrix[9], REAL input_matrix[9]);
    // Odwraca macierz 4x4 i zwraca 1, jeśli macierz odwrotna dla podanej macierzy wejściowej istnieje.
    // W przeciwnym razie zwraca 0.
    unsigned char inverse_matrix_4x4(REAL output_matrix[16], REAL input_matrix[16]);
    // calculates normal matrix 3x3 for given matrix 4x4
    unsigned char calc_normal_matrix(REAL output[9], REAL m[16]);

    // Poniższe trzy funkcje: get_ortho_matrix, get_frustum_matrix i get_perspective_matrix umieszczają obserwatora
    // w początku układu współrzędnych i jest on skierowany w stronę ujemnej półosi OZ. Obserwator jest tak zorientowany
    // w przestrzeni, że kierunek "do góry" pokrywa się z kierunkiem dodatniej osi OY.
    void get_ortho_matrix(REAL *output_matrix, unsigned char force_depth_zero_to_one, REAL left, REAL right, REAL bottom, REAL top, REAL near, REAL far);
    void get_frustum_matrix(REAL *output_matrix, REAL left, REAL right, REAL bottom, REAL top, REAL near, REAL far);
    // fovy - kąt w stopniach widzenia sceny przez obserwatora
    void get_perspective_matrix(REAL *output_matrix, REAL fovy, REAL aspect, REAL near, REAL far);

    /* -> mało optymalna funkcja
    void get_lookat_matrix(REAL *output_matrix, REAL eye_x, REAL eye_y, REAL eye_z,
                            REAL center_x, REAL center_y, REAL center_z,
                            REAL up_x, REAL up_y, REAL up_z);
    *-/

    void get_identity_matrix(REAL *output_matrix);
    void get_identity_matrix3x3(REAL *output_matrix);

    // Oblicza wyznacznik macierzy 2x2
    REAL mat2x2_det(REAL mat[4]);
    // Oblicza wyznacznik macierzy 3x3
    REAL mat3x3_det(REAL mat[9]);
    // Zwraca lewą górną część macierzy 4x4 w postaci macierzy 3x3
    void mat4x4_to_mat3x3(REAL output_mat3x3[9], REAL input_mat4x4[16]);

    // **************************** UWAGA **********************************

    // wierzchołki wielokąta muszą być umieszczone w kolejności przeciwnej do ruchu wskazówek zegara, wielokąt musi być wypukły
    char is_point_inside_polygon2(point2 *polygon_vertices, int polygon_vertices_count, point2 *point);

    REAL get_interpolated_value(REAL Ax, REAL Ay, REAL Bx, REAL By, REAL Cx);

    // zwraca wektor normalny dla płaszczyzny wyznaczonej przez trzy punkty p1, p2 i p3 (jest to znormalizowany wektor prostopadły do tej płaszczyzny)
    void get_normal(point3 *p1, point3 *p2, point3 *p3, vector3 *v_result);
    void get_normal_fv(const REAL p1[3], const REAL p2[3], const REAL p3[3], REAL result[3]);

    /* Zwraca 1 jeśli odcinek opisany przez punkt początkowy o i wektor kierunkowy d przecina
     * trójkąt opisany wierzchołkami v0, v1, v2.
     * Uwaga! w aktualnej wersji funkcja ta zwraca nieprawidłowe wyniki, należy ją jeszcze sprawdzić i poprawić.
     *-/
    unsigned char check_if_picked_triangle(vector3 *v0, vector3 *v1, vector3 *v2, vector3 *o, vector3 *d);

    // Wylicza podany wyraz ciągu Fibonacciego
    unsigned int recursive_fib(unsigned int n);

    // Wylicza silnię z podanej liczby
    unsigned long recursive_fact(unsigned int n);

    // Przybliżone wyliczenie sinusa. Jest ok. 2x szybsze od standardowej funkcji sin w math.h
    REAL sin_ex(REAL x);

    // Sumuje wszystkie liczby całkowite od 1 do max i zwraca wynik
    unsigned long int sum_series_simple(unsigned int max);

    // Sumuje wszystkie liczby w podanej tablicy (o podanym rozmiarze)
    long int sum_array(int *array, size_t array_elements_count);

    // Oblicza średnią liczb w podanej tablicy (o podanym rozmiarze)
    float average_array(int *array, size_t array_elements_count);

    // Oblicza odległość pomiędzy dwoma punktami w przestrzeni 2d - na płaszczyźnie
    #define distance2d(x1, y1, x2, y2) sqrt(((x2)-(x1))*((x2)-(x1)) + ((y2)-(y1))*((y2)-(y1)))



    // **************************** funkcje ************************************
    // **************************** functions **********************************
    typedef struct _function {
        int (*is_zero)(struct _function *fn);
        REAL (*evaluate)(struct _function *fn, REAL x);
        void (*differentiate)(struct _function *fn);
    } t_function;
    // **************************** sinus/cosinus function ***********************
    typedef struct _trig {
        t_function fn;
        REAL (*func)(REAL x);
        int sign;
    } t_trig;
    // type == 1 - sinus
    // type == 2 - cosinus
    // struktura t_trig po inicjalizacji nie potrzebuje wywołania funkcji free
    // zwalniającej zasoby
    void trig_init(t_trig *trig, int type);
    int trig_is_zero(t_trig *trig);
    float trig_evaluate(t_trig *trig, float x);
    void trig_differentiate(t_trig *trig);
    // **************************** wielomiany  **********************************
    // **************************** polynomials **********************************
    typedef struct _polynomial {
        t_function fn;
        int degree;
        int max_degree;
        REAL *coefficients;
    } t_polynomial;
    int polynomial_init(t_polynomial *poly, int degree, REAL a0);
    void polynomial_free(t_polynomial *poly);
    void polynomial_print(t_polynomial *poly);
    int polynomial_set_degree(t_polynomial *poly, int degree);
    REAL polynomial_evaluate_poly(t_polynomial *poly, REAL x);
    void polynomial_change_input_units(t_polynomial *poly, REAL old_units, REAL new_units);
    void polynomial_change_output_units(t_polynomial *poly, REAL old_units, REAL new_units);
    void polynomial_differentiate(t_polynomial *poly);
    int polynomial_copy(t_polynomial *dst_poly, t_polynomial *src_poly);
    int polynomial_copy_fast(t_polynomial *dst_poly, t_polynomial *src_poly);
    int polynomial_is_zero(t_polynomial *poly);
    void polynomial_force_odd(t_polynomial *poly);
    void polynomial_force_even(t_polynomial *poly);
    // multiply the polynomial by (x+a)
    void polynomial_multiply_by_factor(t_polynomial *poly, REAL a);
    void polynomial_add(t_polynomial *dst_poly, t_polynomial *src_poly);
    void polynomial_add_scaled(t_polynomial *dst_poly, t_polynomial *src_poly, REAL scale);
    void polynomial_offset_range(t_polynomial *poly, REAL offset);
    void polynomial_offset_domain(t_polynomial *poly, REAL offset);
    // makes fo in_poly polynomial taylor series and retruns it in out_poly
    void polynomial_make_taylor_series(t_polynomial *out_poly, t_function *in_function, REAL offset, int max_degree);
    void polynomial_make_lagrange_series(t_polynomial *out_poly, int num_points, REAL *x, REAL *y);
    // ***************************************************************************
    // ***************************************************************************
    */
}
