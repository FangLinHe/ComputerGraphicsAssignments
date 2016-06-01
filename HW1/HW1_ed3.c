#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#define GLfloat float
#define GLint int
#define CHAR_NUM 11

GLfloat bgColor[4] = {0.95, 1.0, 0.95, 1.0};

void myinit() {
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
     glColor3fv(bgColor);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluOrtho2D(0, 600, 0, 800);
     glMatrixMode(GL_MODELVIEW);
}

// change brightness to produce gradient effect
// level: 2, 1, 0, -1, -2, -3 to represent different brightness
//     light  <------>  dark
// level 0 represents the original color
void changeBrightness(GLfloat color[], GLfloat newColor[], int level) {
     GLfloat YIQ[3] = {0.0, 0.0, 0.0},
             rgbToYiq[3][3] = {{0.299,     0.587,     0.114},
                               {0.595716, -0.274453, -0.321263},
                               {0.211456, -0.522591,  0.311135}},
             yiqToRgb[3][3] = {{1,  0.9563,  0.6210},
                               {1, -0.2721, -0.6474},
                               {1, -1.1070,  1.7046}};
     int i, j;
     for (i = 0; i < 3; i++)
         for (j = 0; j < 3; j++)
             YIQ[i] += rgbToYiq[i][j] * color[j];

     switch (level) {
     case 2:
          YIQ[0] += (1 - YIQ[0]) * 0.8;
          break;
     case 1:
          YIQ[0] += (1 - YIQ[0]) * 0.2;
          break;
     case -1:
          YIQ[0] -= (1 - YIQ[0]) * 0.2;
          break;
     case -2:
          YIQ[0] -= (1 - YIQ[0]) * 0.8;
          break;
     case -3:
          YIQ[0] -= (1 - YIQ[0]) * 0.95;
     }

     for (i = 0; i < 3; i++)
         for (j = 0; j < 3; j++)
             newColor[i] += yiqToRgb[i][j] * YIQ[j];
}

// draw color of the block with gradient effect
void drawBlockColor(GLfloat color[], GLint pos_x, GLint pos_y, GLint width, GLint length) {
     GLfloat color2[3] = {0.0, 0.0, 0.0},   // color of brightness level  2
             color1[3] = {0.0, 0.0, 0.0},   // color of brightness level  1
             colorN1[3] = {0.0, 0.0, 0.0},  // color of brightness level -1
             colorN2[3] = {0.0, 0.0, 0.0},  // color of brightness level -2
             colorN3[3] = {0.0, 0.0, 0.0};  // color of brightness level -3
     changeBrightness(color, color2, 2);
     changeBrightness(color, color1, 1);
     changeBrightness(color, colorN1, -1);
     changeBrightness(color, colorN2, -2);
     changeBrightness(color, colorN3, -3);

     // GL_POLYGON -- Background
     // upper part
     glBegin(GL_POLYGON);
       glColor3fv(color2);
       glVertex2i(pos_x, pos_y);
       glVertex2i(pos_x + width, pos_y);
       glColor3fv(color);
       glVertex2i(pos_x + width - 10, pos_y - length*7/20);
       glVertex2i(pos_x + 10, pos_y - length*7/20);
     glEnd();

     // left part
     glBegin(GL_POLYGON);
       glColor3fv(color1);
       glVertex2i(pos_x, pos_y);
       glVertex2i(pos_x, pos_y - length);
       glColor3fv(colorN2);
       glVertex2i(pos_x + 10, pos_y - length*7/20);
     glEnd();

     // right part
     glBegin(GL_POLYGON);
       glColor3fv(color1);
       glVertex2i(pos_x + width, pos_y);
       glVertex2i(pos_x + width, pos_y - length);
       glColor3fv(colorN2);
       glVertex2i(pos_x + width - 10, pos_y - length*7/20);
     glEnd();

     // lower part
     glBegin(GL_POLYGON);
       glColor3fv(colorN1);
       glVertex2i(pos_x + 10, pos_y - length*7/20);
       glVertex2i(pos_x + width - 10, pos_y - length*7/20);
       glColor3fv(colorN3);
       glVertex2i(pos_x + width, pos_y - length);
       glVertex2i(pos_x, pos_y - length);
     glEnd();
     // End GL_POLYGON -- Background

     // GL_LINE_LOOP -- White Border
     glColor3f(1.0, 1.0, 1.0);
     glBegin(GL_LINE_LOOP);
       glVertex2i(pos_x, pos_y);
       glVertex2i(pos_x + width, pos_y);
       glVertex2i(pos_x + width, pos_y - length);
       glVertex2i(pos_x, pos_y - length);
     glEnd();
}

void drawBlocks() {
     // Load map
     FILE *map;
     map = fopen("HW1.map", "r");
     char line[CHAR_NUM],
          *status;

     GLint pos_x = 0, pos_y = 800, width = 60, length = 20;
     // Read characters line by line
     for (status = fgets(line, CHAR_NUM, map); status != 0; status = fgets(line, CHAR_NUM, map)) {
         int i, j, k;
         GLfloat color[3];
         for (i = 0; i < CHAR_NUM; i++)
             if (line[i] == '\n')
                line[i] = '\0';

         if (strlen(line) == 0)
            continue;

         // Process character by character
         for (i = 0; i < strlen(line); i++) {

             switch (line[i]) {
             // Yellow blocks
             case '1':
                  color[0] = 0.95;
                  color[1] = 0.95;
                  color[2] = 0.0;
                  drawBlockColor(color, pos_x, pos_y, width, length);
                  break;

             // Red blocks
             case '2':
                  color[0] = 1.0;
                  color[1] = 0.2;
                  color[2] = 0.2;
                  drawBlockColor(color, pos_x, pos_y, width, length);
                  break;

             // Skin color blocks
             case '3':
                  color[0] = 1.0;
                  color[1] = 0.9;
                  color[2] = 0.6;
                  drawBlockColor(color, pos_x, pos_y, width, length);
                  break;

             // Dark red blocks
             case '4':
                  color[0] = 0.67;
                  color[1] = 0.25;
                  color[2] = 0.0;
                  drawBlockColor(color, pos_x, pos_y, width, length);
                  break;
             // Black blocks
             case '5':
                  color[0] = 0.5;
                  color[1] = 0.5;
                  color[2] = 0.5;
                  drawBlockColor(color, pos_x, pos_y, width, length);
                  break;
             } // End switch

             // Move to right
             pos_x += width;
         } // End for-loop: Process character by character
         // Move to left and move down
         pos_x = 0;
         pos_y -= length;
     } // End for-loop: Read line by line
} // End drawBlocks()

// Ball of NTUST << It's ignorable!! >>      XDDDD
void drawBall_NTUST() {
     GLfloat pi = 3.14159,
             r = 15.0,
             theta, c, x, y,
             cen_x = 300.0,
             cen_y = 200.0;
     c = pi / 180.0;
     
     glBegin(GL_TRIANGLE_FAN);
         glColor3fv(bgColor);
         glVertex2f(cen_x, cen_y);
         for (theta = 0.0; theta <= 180.0; theta += 1.0) {
             x = cen_x + r * cos(theta * c);
             y = cen_y + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();

     glBegin(GL_TRIANGLE_FAN);
         glColor3f(0.0, 0.0, 0.5);
         glVertex2f(cen_x, cen_y);
         for (theta = -225.0; theta <= 45.0; theta += 1.0) {
             x = cen_x + r * cos(theta * c);
             y = cen_y + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();

     glColor3fv(bgColor);
     glBegin(GL_POLYGON);
         glVertex2f(cen_x - 2*r/3, cen_y + 2*r/3);
         glVertex2f(cen_x + 2*r/3, cen_y + 2*r/3);
         glVertex2f(cen_x + 2*r/3, cen_y);
         glVertex2f(cen_x, cen_y - r/2);
         glVertex2f(cen_x - 2*r/3, cen_y);
     glEnd();

     glBegin(GL_POLYGON);
         glVertex2f(cen_x - r/15, cen_y);
         glVertex2f(cen_x + r/15, cen_y);
         glVertex2f(cen_x + r/15, cen_y - r);
         glVertex2f(cen_x - r/15, cen_y - r);
     glEnd();
     
     glColor3f(0.0, 0.0, 0.5);
     glBegin(GL_POLYGON);
         glVertex2f(cen_x, cen_y + r);
         glVertex2f(cen_x + 8*r/15, cen_y + 2*r/3);
         glVertex2f(cen_x + 8*r/15, cen_y + r/20);
         glVertex2f(cen_x, cen_y - r/3);
         glVertex2f(cen_x - 8*r/15, cen_y + r/22);
         glVertex2f(cen_x -8*r/15, cen_y + 2*r/3);
     glEnd();

     r /= 3.0;
     cen_y += r;
     glBegin(GL_TRIANGLE_FAN);
         glEnable(GL_BLEND);
         glColor3f(0.95, 1.0, 0.95);
         glVertex2f(cen_x, cen_y);
         for (theta = -180.5; theta < 180.0; theta += 1.0) {
             x = cen_x + r * cos(theta * c);
             y = cen_y + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();
     
     cen_y -= r;
     r *= 3.0;
     glBegin(GL_TRIANGLE_FAN);
         glColor4f(0.0, 0.0, 0.0, 0.5);
         glVertex2f(cen_x + r / 2.5, cen_y + r / 2.5);
         for (theta = -135.0; theta < 226.0; theta += 1.0) {
             glColor4f(0.0, 0.0, 0.0, (abs(theta - 45.1)) / 180.0);
             x = cen_x + r * cos(theta * c);
             y = cen_y + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();
     glBegin(GL_TRIANGLE_FAN);
         glColor4f(1.0, 1.0, 1.0, 0.2);
         glVertex2f(cen_x + r / 2, cen_y + r / 2);
         for (theta = -135.0; theta <= 225.0; theta += 1.0) {
             glColor4f(0.0, 0.0, 0.0, abs(theta-44.4) / 360.0);
             x = cen_x + r * cos(theta * c);
             y = cen_y + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();
}

void drawBall() {
     GLfloat pi = 3.14159,
             r = 150.0,
             theta, c, x, y,
             cen_x = 200.0,
             cen_y = 300.0;
     c = pi / 180.0;

     // GL_TRIANGLE_FAN -- Background
     glBegin(GL_TRIANGLE_FAN);
         // Center of the ball
         glColor3f(1.0, 1.0, 1.0);
         glVertex2f(cen_x + r / 2.5, cen_y + r / 2.5);

         // Circumference
         for (theta = -135.0; theta < 226.0; theta += 1.0) {
             glColor3f(0.0, 0.8 - (GLfloat) abs(theta-44.4) / 360.0, 0.0);
             x = 200.0 + r * cos(theta * c);
             y = 300.0 + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();

     glBegin(GL_TRIANGLE_FAN);
         glColor4f(0.0, 0.0, 0.0, 0.0);
         glVertex2f(cen_x + r / 2.5, cen_y + r / 2.5);
         for (theta = -135.0; theta < 226.0; theta += 1.0) {
             glColor4f(0.0, 0.0, 0.0, (abs(theta - 45.1)+180) / 360.0);
             x = cen_x + r * cos(theta * c);
             y = cen_y + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();

     // GL_LINE_LOOP -- Green border
     glBegin(GL_LINE_LOOP);
         for (theta = -180.5; theta < 181.0; theta += 1.0) {
             glColor3f(0.8, 1.0, 0.8);
             x = 200.0 + r * cos(theta * c);
             y = 300.0 + r * sin(theta * c);
             glVertex2f(x, y);
         }
     glEnd();
}

void drawBoard() {
     GLfloat color[3] = {0.5, 0.5, 1.0};
     drawBlockColor(color, 225, 45, 150, 10);
}

void display() {
     glClear(GL_COLOR_BUFFER_BIT);
     drawBlocks();
     drawBall_NTUST();
     //drawBall();
     drawBoard();
     glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 800);
    glutInitWindowPosition(700, 0);
    glutCreateWindow("B9730208_HW1");
    glutDisplayFunc(display);
    myinit();
    glutMainLoop();
}
