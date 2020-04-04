#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>


struct point {
    float x, y;
};

struct circle {
    struct point cent;
    float r;
};

float **bitmap;
int bitmap_a, bitmap_b;

void create_bitmap(int a, int b) {
    bitmap_a = a;
    bitmap_b = b;
    bitmap = malloc(a * sizeof(float *));
    for (int i = 0; i < a; ++i) {
        bitmap[i] = malloc(b * sizeof(float));
        memset(bitmap[i], 0, b * sizeof(float));
    }
}

void set_bitmap(int i, int j, float value) {
    bitmap[i][j] = value;
}

float get_bitmap(int i, int j) {
    return bitmap[i][j];
}

void free_bitmap() {
    if (bitmap != NULL) {
        for (int i = 0; i < bitmap_a; ++i)
            free(bitmap[i]);
        free(bitmap);
        bitmap = NULL;
    }
}

float **result;
int result_a, result_b;

float get_result(int a, int b) {
    return result[a][b];
}

void set_result(int a, int b, float v) {
    result[a][b] = v;
}

void free_result() {
    if (result != NULL) {
        for (int i = 0; i < result_a; ++i) {
            free(result[i]);
        }
        free(result);
        result = NULL;
    }
}

float roof(float x) {
    return floorf(x + 1) - x;
}

float tile(float x) {
    if (x < 0) {
        return floorf(x + 1);
    }
    return x;
}

float bresenham_super(struct point A, struct point B) {
    float sum = 0;

//    printf("Wybrano:\n");
    if (A.x == B.x) {

        int di = (int) ((B.y - A.y) / fabsf(B.y - A.y));
//        printf("di:%d\n", di);
        for (int i = (int) A.y; di * i < di * B.y; i += di) { //TODO: rozbić na ifa i dwie pętle
//            printf("x: %d\ty: %d\n", (int) A.x, i);
            sum += bitmap[(int) A.x][i];
        }

    } else {

        float a;
        float mul = 1;
//        float b;
        a = (B.y - A.y) / (B.x - A.x);
//        b = A.y - a * A.x;
//        printf("a: %f\n", a);


        struct point act;
//        printf("x: %d\ty: %d\n", (int) A.x, (int) A.y);

        float d_next_horizontal, d_next_vertical;
        if (fabsf(a) <= 1) {
            if (a < 0) {
                mul = -1;
                a *= -1;
                A.y *= mul;
                B.y *= mul;
            }

            act.x = A.x;
            act.y = A.y;

            d_next_horizontal = roof(act.y) / a;
            d_next_vertical = roof(act.x);

            while ((act.x <= floorf(B.x) && act.y <= floorf(B.y))) { //TODO: ?początkowe? i końcowe procenty
//                printf("act: (%.2f, %.2f)\n", act.x, act.y);
//                printf("piksel (%d, %d)", (int) (act.x), (int) (mul * tile(act.y)));

                if (d_next_vertical > d_next_horizontal) {
//                    printf(" na %f\n", d_next_horizontal);
                    sum += d_next_horizontal * bitmap[(int) (act.x)][(int) (mul * tile(act.y))];
                    act.x += d_next_horizontal;
                    act.y += a * d_next_horizontal;
                } else {
//                    printf(" na %f\n", d_next_vertical);
                    sum += d_next_vertical * bitmap[(int) (act.x)][(int) (mul * tile(act.y))];
                    act.x += d_next_vertical;
                    act.y += a * d_next_vertical;
                }

                d_next_horizontal = roof(act.y) / a;
                d_next_vertical = roof(act.x);
            }
        } else {
            a = 1 / a;
            if (a < 0) {
                mul = -1;
                a *= -1;
                A.y *= mul;
                B.y *= mul;
            }

            act.x = A.y;
            act.y = A.x;

            d_next_horizontal = roof(act.y) / a;
            d_next_vertical = roof(act.x);

            while ((act.x <= floorf(B.x) && act.y <= floorf(B.y))) {
//                printf("act: (%.2f, %.2f)\n", act.x, act.y);
//                printf("piksel (%d, %d)", (int) (act.y), (int) (mul * tile(act.x)));

                if (d_next_vertical > d_next_horizontal) {
//                    printf(" na %f\n", d_next_horizontal);
                    sum += d_next_horizontal * bitmap[(int) (act.y)][(int) (mul * tile(act.x))];
                    act.x += d_next_horizontal;
                    act.y += a * d_next_horizontal;
                } else {
//                    printf(" na %f\n", d_next_vertical);
                    sum += d_next_vertical * bitmap[(int) (act.y)][(int) (mul * tile(act.x))];
                    act.x += d_next_vertical;
                    act.y += a * d_next_vertical;
                }

                d_next_horizontal = roof(act.y) / a;
                d_next_vertical = roof(act.x);
            }
        }

    }
    return sum;
}

struct point point_on_circle(struct circle C, float a) {
    struct point r = C.cent;
    r.x += C.r * cosf(a);
    r.y += C.r * sinf(a);
    return r;
}

void transform(float step, int decod, float spread) {

    unsigned long width = (unsigned long) floorf(2 * M_PI / step);
    result = malloc(width * sizeof(float *));
    for (unsigned int i = 0; i < width; ++i)
        result[i] = malloc(decod * sizeof(float));
    result_a = (int) width;
    result_b = decod;

    struct circle C;
    C.cent = (struct point) {((float) bitmap_b - 2) / 2, ((float) bitmap_a - 2) / 2};
    C.r = fmaxf(bitmap_b, bitmap_a) / 2 - 1;
    printf("okrag srodek: (%f, %f)\tprom: %f\n", C.cent.x, C.cent.y, C.r);

    struct point E, D;
    float alpha = -step, beta;
    float step_beta = spread / decod;

    for (unsigned int i = 0; i < width; ++i) { //TODO: dodać normalizację
        alpha += step;
        E = point_on_circle(C, alpha);

        beta = alpha - spread / 2 + M_PI;
        for (int j = 0; j < decod; ++j) {
            beta += step_beta;
            D = point_on_circle(C, beta);
//            printf("E: (%.2f, %.2f)\tD: (%.2f, %.2f)\n", E.x, E.y, D.x, D.y);
            if (E.x <= D.x) result[i][j] = bresenham_super(E, D);
            else result[i][j] = bresenham_super(D, E);
        }
    }

}

float line(float x, float y, float A, float B, float C) {
    return A * x + B * y + C;
}

/*void bresenham(struct point P, struct point K) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
        printf("(%d, %.0f)\n", x, y);
        if ( val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void bresenham1(struct point P, struct point K) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
        printf("(%.0f, %d)\n", y, x);
        if ( val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void bresenham_(struct point P, struct point K) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    y = floorf(P.y + 1);
    for (; x <= ex; ++x) {
        printf("(%d, %.0f)\n", x, -y);
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void bresenham1_(struct point P, struct point K) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 0.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
        printf("(%.0f, %d)\n", y, -x);
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void line_bres(float x1, float y1, float x2, float y2) {
    if (x1 != x2) {
        if (y2 - y1 >= 0) {
            if ((y2 - y1) / (x2 - x1) <= 1)
                bresenham((struct point) {x1, y1}, (struct point) {x2, y2});
            else
                bresenham1((struct point) {y1, x1}, (struct point) {y2, x2});
        } else {
            if ((y2 - y1) / (x2 - x1) >= -1)
                bresenham_((struct point) {x1, -y1}, (struct point) {x2, -y2});
            else
                bresenham1_((struct point) {-y1, x1}, (struct point) {-y2, x2});
        }
    } else {
        if(y1>y2) {
            float tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        int y = (int) y1, ey = (int) y2, x = (int) x1;
        for (; y <= ey; ++y)
            printf("(%d, %d)\n", x, y);
    }
}*/

float bresenham(struct point P, struct point K) {
//    char *str = malloc(1000);
    float sum = 0;
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
//        printf("(%d, %.0f)\n", x, y);
        sum += bitmap[x][(int) y];
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
    return sum;
}

float bresenham1(struct point P, struct point K) {
//    char *str = malloc(1000);
    float sum = 0;
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
//        printf("(%.0f, %d)\n", y, x);

        sum += bitmap[(int) y][x];
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
    return sum;
}

float bresenham_(struct point P, struct point K) {
//    char *str = malloc(1000);
    float sum = 0;
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    y = floorf(P.y + 1);
    for (; x <= ex; ++x) {
//        printf("(%d, %.0f)\n", x, -y);
//        if(x>=bitmap_a||(int)(-y)>=bitmap_b) printf("BLADD\n");
        sum += bitmap[x][(int) (-y)];
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
    return sum;
}

float bresenham1_(struct point P, struct point K) {
//    char *str = malloc(1000);
    float sum = 0;
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 0.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
//        printf("(%.0f, %d)\n", y, -x);
//        if((int)y>=bitmap_a||x>=bitmap_b) printf("BLADD\n");
        sum += bitmap[(int) (y)][-x];
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
    return sum;
}

float line_bres(float x1, float y1, float x2, float y2) {
    if (x1 != x2) {
        if (y2 - y1 >= 0) {
            if ((y2 - y1) / (x2 - x1) <= 1)
                return bresenham((struct point) {x1, y1}, (struct point) {x2, y2});
            else
                return bresenham1((struct point) {y1, x1}, (struct point) {y2, x2});
        } else {
            if ((y2 - y1) / (x2 - x1) >= -1)
                return bresenham_((struct point) {x1, -y1}, (struct point) {x2, -y2});
            else
                return bresenham1_((struct point) {-y1, x1}, (struct point) {-y2, x2});
        }
    } else {
        float sum = 0;
        if (y1 > y2) {
            float tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        int y = (int) y1, ey = (int) y2, x = (int) x1;
        for (; y <= ey; ++y) {
//            printf("(%d, %d)\n", x, y);
//            if (x >= bitmap_a || y >= bitmap_b) printf("BLADD\n");
            sum += bitmap[x][y];
        }
        return sum;
    }
}

void transform_bres(float step, int decod, float spread) {

    unsigned long width = (unsigned long) floorf(2 * M_PI / step);
    result = malloc(width * sizeof(float *));
    for (unsigned int i = 0; i < width; ++i)
        result[i] = malloc(decod * sizeof(float));
    result_a = (int) width;
    result_b = decod;

    struct circle C;
    C.cent = (struct point) {((float) bitmap_b - 2) / 2, ((float) bitmap_a - 2) / 2};
    C.r = fminf(bitmap_b, bitmap_a) / 2 - 1;
    printf("okrag srodek: (%f, %f)\tprom: %f\n", C.cent.x, C.cent.y, C.r);
#pragma omp parallel
    {
        struct point E, D;
        float alpha = -step, beta;
        float step_beta = spread / decod;

#pragma omp for
        for (unsigned int i = 0; i < width; ++i) { //TODO: dodać normalizację
            alpha = i * step;
//            alpha += step;
            E = point_on_circle(C, alpha);

            beta = alpha - spread / 2 + M_PI;
//            beta = step * i - spread / 2 + M_PI;
            for (int j = 0; j < decod; ++j) {
                beta += step_beta;
                D = point_on_circle(C, beta);
                if (E.x <= D.x) result[i][j] = line_bres(E.x, E.y, D.x, D.y);
                else result[i][j] = line_bres(D.x, D.y, E.x, E.y);
//            if (i > 90 && i < 110)
//                printf("E: (%.2f, %.2f)\tD: (%.2f, %.2f)\t%f\n", E.x, E.y, D.x, D.y, result[i][j]);
            }
        }
    };

}

void re_bresenham(struct point P, struct point K, float add) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
//        printf("(%d, %.0f)\n", x, y);
//        printf("\t\t\ttest\n");
        bitmap[x][(int) y] += add;
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void re_bresenham1(struct point P, struct point K, float add) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
//        printf("(%.0f, %d)\n", y, x);
//        printf("\t\t\ttest\n");
        bitmap[(int) y][x] += add;
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void re_bresenham_(struct point P, struct point K, float add) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 1.5f) + B * (y + 1.f) + C;
    y = floorf(P.y + 1);
    for (; x <= ex; ++x) {
//        printf("(%d, %.0f)\n", x, -y);
//        if(x>=bitmap_a||(int)(-y)>=bitmap_b) printf("BLADD\n");
//        printf("\t\t\ttest\n");
        bitmap[x][(int) (-y)] += add;
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void re_bresenham1_(struct point P, struct point K, float add) {
//    char *str = malloc(1000);
    float A = K.y - P.y;
    float B = P.x - K.x;
    float C = -A * P.x - B * P.y;
    int x = (int) P.x, ex = (int) K.x;
    float y = floorf(P.y);
    float val = A * ((float) x + 0.5f) + B * (y + 1.f) + C;
    for (; x <= ex; ++x) {
//        printf("(%.0f, %d)\n", y, -x);
//        if((int)y>=bitmap_a||x>=bitmap_b) printf("BLADD\n");
//        printf("\t\t\ttest\n");
        bitmap[(int) (y)][-x] += add;
        if (val >= 0) {
            val += B;
            ++y;
        }
        val += A;
    }
}

void re_line_bres(float x1, float y1, float x2, float y2, float add) {
    if (x1 != x2) {
//        printf("\t\t\tskos\n");
        if (y2 - y1 >= 0) {
            if ((y2 - y1) / (x2 - x1) <= 1)
                re_bresenham((struct point) {x1, y1}, (struct point) {x2, y2}, add);
            else
                re_bresenham1((struct point) {y1, x1}, (struct point) {y2, x2}, add);
        } else {
            if ((y2 - y1) / (x2 - x1) >= -1)
                re_bresenham_((struct point) {x1, -y1}, (struct point) {x2, -y2}, add);
            else
                re_bresenham1_((struct point) {-y1, x1}, (struct point) {-y2, x2}, add);
        }
    } else {
//        printf("\t\t\tpros\n");
        if (y1 > y2) {
            float tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        int y = (int) y1, ey = (int) y2, x = (int) x1;
        for (; y <= ey; ++y) {
//            printf("(%d, %d)\n", x, y);
            if (x >= bitmap_a || y >= bitmap_b) printf("BLADD\n");
            bitmap[x][y] += add;
        }
    }
}

const float rev_pi = -(float) (M_2_PI * M_2_PI);

float filtered(unsigned int row, int col) {
    float sum = 0.0f;
    for (int i = -11; i <= 11; i += 2) {
        if (i + row >= 0 && i + row < result_a)
            sum += (rev_pi / (float) (i * i)) * result[row + i][col];
    }
    sum += result[row][col];
//    printf("%f\n", sum);
    return sum;
}

void reverse_bres(float step, int decod, float spread, int fil) {
    printf("%f\n", rev_pi);
    /*
    result = malloc(width * sizeof(float *));
    for (unsigned int i = 0; i < width; ++i)
        result[i] = malloc(decod * sizeof(float));
    result_a = (int) width;
    result_b = decod;*/
    unsigned long width = (unsigned long) floorf(2 * M_PI / step);

    float div = 0.0;
    for (int i = 0; i < result_a; ++i) {
        for (int j = 0; j < result_b; ++j) {
            if (div < result[i][j]) div = result[i][j];
        }
    }
    for (int i = 0; i < result_a; ++i) {
        for (int j = 0; j < result_b; ++j) {
            result[i][j] /= div;
        }
    }

    struct circle C;
    C.cent = (struct point) {((float) bitmap_a - 2) / 2, ((float) bitmap_b - 2) / 2};
    C.r = fminf((float) bitmap_a, (float) bitmap_b) / 2 - 1;
    printf("okrag srodek: (%f, %f)\tprom: %f\n", C.cent.x, C.cent.y, C.r);

    struct point E, D;
    float alpha = -step, beta;
    float step_beta = spread / decod;

    for (unsigned int i = 0; i < width; ++i) { //TODO: dodać normalizację
        alpha += step;
        E = point_on_circle(C, alpha);

        beta = alpha - spread / 2 + M_PI;
//        printf("\talfa %f\n", alpha);
        for (int j = 0; j < decod; ++j) {
            beta += step_beta;
            D = point_on_circle(C, beta);
//            printf("%f\n", result[i][j]);
//            printf("\t\tbeta %f\n", beta);
            if (result[i][j] > 0.0) {
//                printf("%f\n", result[i][j]);
                if (fil) {
                    if (E.x <= D.x) re_line_bres(E.x, E.y, D.x, D.y, filtered(i, j));
                    else re_line_bres(D.x, D.y, E.x, E.y, filtered(i, j));
                } else {
                    if (E.x <= D.x) re_line_bres(E.x, E.y, D.x, D.y, result[i][j]);
                    else re_line_bres(D.x, D.y, E.x, E.y, result[i][j]);
                }
            }
//            if (i > 90 && i < 110)
//                printf("E: (%.2f, %.2f)\tD: (%.2f, %.2f)\t%f\n", E.x, E.y, D.x, D.y, result[i][j]);
        }
    }

}

/*void test_bresenham() {
    CU_ASSERT(1 + 1 == 2);
    CU_ASSERT(1 + 1 == 21);
//    CU_ASSERT(1);
}*/

/*static MunitResult test_bresenham(const MunitParameter params[], void *user_data) {

}

static float x1_params[] = {};
static float y1_params[] = {};
static float x2_params[] = {};
static float y2_params[] = {};

static MunitTest test_suite_tests[] = {
        {(char *) "test/bresenham", test_bresenham, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};*/


int main() {
//    printf("Hello, World!\n%f", suf(1.4f));
//    printf("%f\n", M_PI);
//    printf("%f\n", floorf(4.5f));

//    line_bres(0.5f, 0.5f,4.5f, 4.5f);
//    line_bres(0.5f, 0.5f,6.74f, 3.17f);
//    line_bres(0.5f, 0.5f,2.8f, 5.53f);
//    line_bres(0.5f, 0.5f, 7.45f, 0.92f);
//    line_bres(0.5f, 0.5f, 7.45f, 0.5f);
//    line_bres(0.5f, 0.5f, 7.45f, 0.2f);
//    line_bres(0.71f,1.82f,7.53f,0.2f);
//    line_bres(1.19f, 4.18f, 7.53f, 0.2f);
//    line_bres(1.19f, 4.18f, 3.23f, 0.16f);
//    line_bres(2.78f, 5.68f, 9.56f, 4.53f);
//    line_bres(2.78f, 0.53f, 2.78f, 5.68f);
//    line_bres(2.78f, 5.68f, 2.78f, 0.53f);
//    line_bres(1.26f,9.76f,2.165f,6.244f);
//    line_bres(0.42f,6.29f,0.88f,9.965f);
//    line_bres(134.21f, 387.16f, 339.52f, 58.09f);

/*    CU_initialize_registry();
    CU_pSuite pSuite = CU_add_suite("test", 0, 0);

    CU_add_test(pSuite, "bresenham", test_bresenham);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();*/


    return 0;
}
