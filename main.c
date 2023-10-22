#include <curses.h>
#include <locale.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define PI 3.14159265

const char* ASCII = " .:-=+*#%@";
// const char* ASCII = " .`:,;'_^\"></-!~=)(j?}{[ti+l7v1%yrfcJ32uIC$zwo96sngaT5qpkYVOL40&mG8*xhedbZUSAQPFDXWK#RNEHBM@";
const size_t GRAYSCALE = sizeof(ASCII) / sizeof(char);

typedef struct point {
    float x;
    float y;
    float z;
} point;

void draw_point(WINDOW* w, point p) {
    mvwaddch(w, p.y, p.x*2, 'o');
}

void anti_aliased_draw_point(WINDOW* w, point p) {
    float rounded_x = roundf(p.x);
    float rounded_y = roundf(p.y);

    float percent_x = 1.0f - fabs(p.x - rounded_x);
    float percent_y = 1.0f - fabs(p.y - rounded_y);

    float percent = percent_x * percent_y;

    char ascii = ASCII[(int) roundf(percent * GRAYSCALE) - 1];

    mvwaddch(w, rounded_y, rounded_x*2, ascii);
}

int main(void) {
    printf("hello world\n");
    setlocale(LC_ALL, "");

    WINDOW* win = initscr();
    cbreak();
    // raw();
    noecho();
    intrflush(win, FALSE);
    curs_set(0);

    const int WIDTH = (int) (COLS/2);
    const int HEIGHT = LINES;

    const int CENTER_Y = (int) (HEIGHT/2); 
    const int CENTER_X = (int) (WIDTH/2);

    // point points[] = {
    //
    //     (point){ 1,  1,  1},
    //     (point){-1,  1,  1},
    //     (point){-1,  1, -1},
    //     (point){ 1,  1, -1},
    //     (point){ 1, -1,  1},
    //     (point){-1, -1,  1},
    //     (point){-1, -1, -1},
    //     (point){ 1, -1, -1},
    //
    //     // (point){ 1, -1,  0},
    //     // (point){-1, -1,  0},
    //     // (point){ 0, -1, -1},
    //     // (point){ 0, -1,  1},
    //     //
    //     // (point){ 1, 1,  0},
    //     // (point){-1, 1,  0},
    //     // (point){ 0, 1, -1},
    //     // (point){ 0, 1,  1},
    //     //
    //     // (point){ 1, 0,  1},
    //     // (point){-1, 0,  1},
    //     // (point){-1, 0, -1},
    //     // (point){ 1, 0, -1},
    // };

    int r = 2;
    point* points = malloc(sizeof(point));
    if (!points) {
        fprintf(stderr, "points = malloc()!\n");
        return 1;
    }
    size_t points_amount = 1;

    for (float tetha = 0; tetha < PI; tetha += 0.1) {
        for (float phi = 0; phi < PI*2; phi += 0.1) 
        {
            points[points_amount - 1] = (point) {  
                r * sinf(tetha) * cosf(phi),
                r * sinf(tetha) * sinf(phi),
                r * cosf(tetha)
            };
            points = realloc(points, ++points_amount * sizeof(point));
        }
    }

    // printf("%ld\n", points_amount);

    // endwin();
    // return 0;



    float fov = 50;

    for (size_t i = 0; i < points_amount; i++) {
        points[i].x *= 1.5;
        points[i].y *= 1.5;
        points[i].z *= 1.5;
    }

    char* buf = malloc(200);
    if (!buf) {
        fprintf(stderr, "char* buf = malloc()!\n");
        return 1;
    }
    
    for (float angle = 0;; angle += 0.1) {
        clear();

        for (size_t i = 0; i < points_amount; i++) {

            point c = points[i];

            // rodar ao entorno do eixo Y
            c.x = points[i].x * cosf(angle) + points[i].z * sinf(angle);
            c.z = -points[i].x * sinf(angle) + points[i].z * cosf(angle);
            c.z += 8;

            point d = c;

            // rodar ao entorno do eixo Z
            d.x = c.x * cosf(angle) - c.y * sinf(angle);
            d.y = c.x * sinf(angle) + c.y * cosf(angle);


            // cria o efeito de perspectiva
            point projected = (point)
            {   .x = d.x * fov / d.z,
                .y = d.y * fov / d.z,
            };

            projected.x += CENTER_X;
            projected.y += CENTER_Y;

            anti_aliased_draw_point(win, projected);
            sprintf(buf, "%f", c.x);
            mvwaddstr(win, 0, 0, buf);
            
        }

        refresh();
        usleep(.1 * 1000 * 1000);
    }

    free(buf);
    endwin();
    
    return 0;
}
