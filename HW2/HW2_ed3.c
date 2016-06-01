#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <windows.h>
#include <math.h>
#define GLfloat float
#define GLint int
#define CHAR_NUM 10
#define LINE_NUM 21
#define DEGREE 3.14159/180.0

// Type definition

// point_t (x, y)
typedef struct {
    GLfloat x, y;
} point_t;

// ball_t center, radius, theta, speed
typedef struct {
    point_t center;
    GLfloat radius;
    GLfloat theta;
} ball_t;

// board_t center, width, height, bgColor[3]
typedef struct {
    point_t center;
    GLfloat width;
    GLfloat height;
    GLfloat bgColor[3];
} board_t;

// block_t hp, props;
typedef struct {
    int hp;
    int props;
    int row;
    int column;
} block_t;

// declare global variables
int windowSizeX = 600;
int windowSizeY = 800;
GLfloat movement;

ball_t  ball0;
board_t board;
block_t blocks[21][10];
GLfloat bgColor[4] = {0.95, 1.0, 0.95, 1.0};

// Game states and keyboard detection
int keyLeft;
int keyUp;
int keyDown;
int keyRight;
int keyD;
int keyW;
int keyS;
int keyA;
int restart = 0;
int playing = 0;
int effectCount = 0;
int win = 1;
int lose = 0;
int ball = 0;
block_t* blockEffect1;
block_t* blockEffect2;

void myinit();
void display();
void idle();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardS(int key, int x, int y);
void keyboardSUp(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion (int x, int y);

int collideBlock();
double distance(double x1, double y1, double x2, double y2);
double distance2(double a, double b);

void readMap();
void changeBrightness(GLfloat color[], GLfloat newColor[], int level);
void drawBlockColor(GLfloat color[], GLfloat pos_x, GLfloat pos_y, GLfloat width, GLfloat height);
void drawBlocks();
void drawBall_NTUST();
void drawBall();
void drawBoard(board_t board);
void drawEffect(block_t block);
void drawWin();
void drawText();

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowSizeX, windowSizeY);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("B9730208_HW2");
    
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	
	glutSpecialFunc(keyboardS);
	glutSpecialUpFunc(keyboardSUp);
	
    glutMouseFunc(mouse);
	glutMotionFunc(motion); 
	
    glutDisplayFunc(display);
	glutIdleFunc(idle); 
    myinit();
    glutMainLoop();
}


void myinit() {
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluOrtho2D(0, windowSizeX, 0, windowSizeY);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

void display() {
     glClear(GL_COLOR_BUFFER_BIT);
     if (restart) {
        readMap();
        movement = 5.0;
        ball0.center.x = 200.0;
        ball0.center.y = 200.0;
        ball0.radius = 7.5;
        ball0.theta = 45.0;
        board.center.x = 300;
        board.center.y = 50;
        board.width = 150;
        board.height = 10;
        board.bgColor[0] = 0.5;
        board.bgColor[1] = 0.5;
        board.bgColor[2] = 1.0;

        readMap();
        keyLeft = 0;
        keyUp = 0;
        keyDown = 0;
        keyRight = 0;
        keyD = 0;
        keyW = 0;
        keyS = 0;
        keyA = 0;
        restart = 0;
        effectCount = 0;
        win = 1;
        lose = 0;
        ball = 0;
        blockEffect1 = NULL;
        blockEffect2 = NULL;
     }

     if (playing) {
         if (keyUp)
            movement += 0.05;
         if (keyDown)
            movement -= 0.05;
         if (movement < 0.5)
            movement = 5.0;
         if (movement > 15.0)
            movement = 5.0;

         ball0.center.x += movement * cos(ball0.theta * DEGREE);
         ball0.center.y += movement * sin(ball0.theta * DEGREE);
    
         if (keyLeft && board.center.x - board.width / 2.0 >= 0.0)
            board.center.x -= movement;
         else if (keyRight && board.center.x + board.width / 2.0 <= 600.0 )
              board.center.x += movement;
         
         
         // collides wall
         if (ball0.center.x + ball0.radius >= 600.0 ||
             ball0.center.x - ball0.radius <= 0.0) {
             ball0.theta = 180 - ball0.theta;
             ball++;
             ball %= 2;
         }
             
         else if (ball0.center.y + ball0.radius >= 800.0)
              ball0.theta = 360.0 - ball0.theta;
         
         // collides block
         int collide = collideBlock();
         if (collide == 1)
            ball0.theta = 360 - ball0.theta;
         else if (collide == 2)
              ball0.theta = 180.0 - ball0.theta;
         else if (collide == 3)
              ball0.theta += 180.0;
         
         // collides board
         if (ball0.center.x >= board.center.x - board.width / 2 &&
             ball0.center.x <= board.center.x + board.width / 2 &&
             ball0.center.y - board.center.y <= ball0.radius) {
             ball0.theta = 90 - (ball0.center.x - board.center.x);
             board.bgColor[0] = (double)(rand()%20) / 20;
             board.bgColor[1] = (double)(rand()%20) / 20;
             board.bgColor[2] = (double)(rand()%20) / 20;
         }
         
         if (ball0.theta < 0.0)
            ball0.theta += 360.0;
         else if (ball0.theta > 360.0)
              ball0.theta -= 360.0;
         drawBoard(board);
         drawBlocks();
         if (ball)
            drawBall_NTUST();
         else
             drawBall();
         
         if (blockEffect1 != NULL && effectCount < 10) {
            drawEffect(*blockEffect1);
            if (blockEffect2 != NULL)
               drawEffect(*blockEffect2);
            effectCount++;
         } else {
                blockEffect1 = NULL;
                blockEffect2 = NULL;
                effectCount = 0;
         }
      }

      // press space to start
      if (ball0.center.y <= board.center.y) {
         drawText();
         playing = 0;
      }


      glutSwapBuffers();
      glFlush();
}

void idle(void) {
     Sleep(10);
     glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
     if (key == 'w' || key == 'W')
        keyW = 1;
     if (key == 'd' || key == 'D')
        keyD = 1;
     if (key == 's' || key == 'S')
        keyS = 1;
     if (key == 'a' || key == 'A')
        keyA = 1;
     if (key == ' ') {
        restart = 1;
        playing = 1;
     }
}

void keyboardUp(unsigned char key, int x, int y) {
     if (key == 'w' || key == 'W')
        keyW = 0;
     if (key == 'd' || key == 'D')
        keyD = 0;
     if (key == 's' || key == 'S')
        keyS = 0;
     if (key == 'a' || key == 'A')
        keyA = 0;
}

void keyboardS(int key, int x, int y){
     if (key == GLUT_KEY_RIGHT)
        keyRight = 1;
     if (key == GLUT_KEY_UP)
        keyUp = 1;
     if (key == GLUT_KEY_DOWN)
        keyDown = 1;
     if (key == GLUT_KEY_LEFT)
        keyLeft = 1;
}

void keyboardSUp(int key, int x, int y){
     if (key == GLUT_KEY_RIGHT)
        keyRight = 0;
     if (key == GLUT_KEY_UP)
        keyUp = 0;
     if (key == GLUT_KEY_DOWN)
        keyDown = 0;
     if (key == GLUT_KEY_LEFT)
        keyLeft = 0;
}

void mouse(int button, int state, int x, int y) {
	/*printf("mouse state = %d\n", state);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {	
		printf("mouse down state = %d \n", state);
	}*/
}

void motion (int x, int y) {
	//printf("mouse %d, %d\n", x, y);
}



// return 0: no collision top or bottom: 1, left or right: 2
int collideBlock() {
    int bx = ball0.center.x / 60,
        by = (800 - ball0.center.y) / 20;

    if (by > LINE_NUM) return 0;

    // 左上 
    if (bx > 0 && by > 0 && intersect(ball0, blocks[by - 1][bx - 1], 1)) {
       if (intersect(ball0, blocks[by - 1][bx], 2)) {
          blocks[by - 1][bx].hp--;
          blocks[by - 1][bx - 1].hp--;
          blockEffect1 = &blocks[by - 1][bx];
          blockEffect1 = &blocks[by - 1][bx - 1];
          return 1;
       } else {
              blocks[by - 1][bx - 1].hp--;
              blockEffect1 = &blocks[by - 1][bx - 1];
              if (ball0.theta <= 180.0)
                 return 1;
              else
                  return 2;
       }
    }
    
    //上 
    if (by > 0 && intersect(ball0, blocks[by - 1][bx], 2)) {
       if (bx < CHAR_NUM - 1 && by > 0 && intersect(ball0, blocks[by - 1][bx + 1], 3)) {
          blocks[by - 1][bx].hp--;
          blocks[by - 1][bx + 1].hp--;
          blockEffect1 = &blocks[by - 1][bx];
          blockEffect1 = &blocks[by - 1][bx + 1];
          return 1;
       } else {
              blocks[by - 1][bx].hp--;
              blockEffect1 = &blocks[by - 1][bx];
              return 1;
       }
    }
    
    // 右上 
    if (bx < CHAR_NUM - 1 && by > 0 && intersect(ball0, blocks[by - 1][bx + 1], 3)) {
       if (bx < CHAR_NUM - 1 && intersect(ball0, blocks[by][bx + 1], 4)) {
          blocks[by - 1][bx + 1].hp--;
          blocks[by][bx + 1].hp--;
          blockEffect1 = &blocks[by - 1][bx + 1];
          blockEffect1 = &blocks[by][bx + 1];
          return 2;
       } else {
              blocks[by - 1][bx + 1].hp--;
              blockEffect1 = &blocks[by - 1][bx + 1];
              if (ball0.theta <= 180.0)
                 return 1;
              else
                  return 2;
       }
    }
    
    // 右 
    if (bx < CHAR_NUM - 1 && intersect(ball0, blocks[by][bx + 1], 4)) {
       if (bx < CHAR_NUM - 1 && by < LINE_NUM - 1 && intersect(ball0, blocks[by + 1][bx + 1], 5)) {
          blocks[by + 1][bx + 1].hp--;
          blocks[by][bx + 1].hp--;
          blockEffect1 = &blocks[by + 1][bx + 1];
          blockEffect1 = &blocks[by][bx + 1];
          return 2;
       } else {
              blocks[by][bx + 1].hp--;
              blockEffect1 = &blocks[by][bx + 1];
              return 2;
       }
    }
    
    // 右下 
    if (bx < CHAR_NUM - 1 && by < LINE_NUM - 1 && intersect(ball0, blocks[by + 1][bx + 1], 5)) {
       if (by < LINE_NUM - 1 && intersect(ball0, blocks[by + 1][bx], 6)) {
          blocks[by + 1][bx].hp--;
          blocks[by + 1][bx + 1].hp--;
          blockEffect1 = &blocks[by + 1][bx];
          blockEffect1 = &blocks[by + 1][bx + 1];
          return 1;
       } else {
              blocks[by + 1][bx + 1].hp--;
              blockEffect1 = &blocks[by + 1][bx + 1];
              if (ball0.theta <= 180.0)
                 return 2;
              else
                  return 1;
       }
    }
    
    // 下 
    if (by < LINE_NUM - 1 && intersect(ball0, blocks[by + 1][bx], 6)) {
       if (bx > 0 && by < LINE_NUM - 1 && intersect(ball0, blocks[by + 1][bx - 1], 7)) {
          blocks[by + 1][bx - 1].hp--;
          blocks[by + 1][bx].hp--;
          blockEffect1 = &blocks[by + 1][bx - 1];
          blockEffect1 = &blocks[by + 1][bx];
          return 1;
       } else {
              blocks[by + 1][bx].hp--;
              blockEffect1 = &blocks[by + 1][bx];
              return 1;
       }
    }
    
    // 左下 
    if (bx > 0 && by < LINE_NUM - 1 && intersect(ball0, blocks[by + 1][bx - 1], 7)) {
       if (bx > 0 && intersect(ball0, blocks[by][bx - 1], 8)) {
          blocks[by][bx - 1].hp--;
          blocks[by + 1][bx - 1].hp--;
          blockEffect1 = &blocks[by][bx - 1];
          blockEffect1 = &blocks[by + 1][bx - 1];
          return 2;
       } else {
              blocks[by + 1][bx - 1].hp--;
              blockEffect1 = &blocks[by + 1][bx - 1];
              if (ball0.theta <= 180.0)
                 return 2;
              else
                  return 1;
       }
    }
    
    // 左 
    if (bx > 0 && intersect(ball0, blocks[by][bx - 1], 8)) {
       if (bx > 0 && by > 0 && intersect(ball0, blocks[by - 1][bx - 1], 1)) {
          blocks[by - 1][bx - 1].hp--;
          blocks[by][bx - 1].hp--;
          blockEffect1 = &blocks[by - 1][bx - 1];
          blockEffect1 = &blocks[by][bx - 1];
          return 2;
       } else {
              blocks[by][bx - 1].hp--;
              blockEffect1 = &blocks[by][bx - 1];
              return 2;
       }
    }
    return 0;
}

int intersect(ball_t ball, block_t block, int where) {
    double x1 = block.column * 60,
           x2 = x1 + 60,
           y1 = 800 - block.row * 20,
           y2 = y1 - 20;
    switch (where) {
    // 偵測球是否碰到左上方磚塊： 計算球圓心與磚塊右下角的距離 <= 半徑 
    case 1:
         if (block.hp > 0 && distance(x2, y2, ball.center.x, ball.center.y) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到上方磚塊： 計算球圓心與磚塊下邊的距離 <= 半徑 
    case 2:
         if (block.hp > 0 && distance2(ball.center.y, y2) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到右上方磚塊： 計算球圓心與磚塊左下角的距離 <= 半徑 
    case 3:
         if (block.hp > 0 && distance(x1, y2, ball.center.x, ball.center.y) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到右方磚塊： 計算球圓心與磚塊左邊的距離 <= 半徑 
    case 4:
         if (block.hp > 0 && distance2(ball.center.x, x1) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到右下方磚塊： 計算球圓心與磚塊左上角的距離 <= 半徑 
    case 5:
         if (block.hp > 0 && distance(x1, y1, ball.center.x, ball.center.y) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到下方磚塊： 計算球圓心與磚塊上邊的距離 <= 半徑 
    case 6:
         if (block.hp > 0 && distance2(ball.center.y, y1) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到左下方磚塊： 計算球圓心與磚塊右上角的距離 <= 半徑 
    case 7:
         if (block.hp > 0 && distance(x2, y1, ball.center.x, ball.center.y) <= ball.radius)
             return 1;
         else
             return 0;
         break;
    // 偵測球是否碰到左方磚塊： 計算球圓心與磚塊右邊的距離 <= 半徑 
    case 8:
         if (block.hp > 0 && distance2(ball.center.x, x2) <= ball.radius)
             return 1;
         else
             return 0;
    }
    return 0;
}

double distance(double x1, double y1, double x2, double y2) {
       return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

double distance2(double a, double b) {
       return abs(a - b);
}

// ---- Drawing related functions ----

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
void drawBlockColor(GLfloat color[], GLfloat pos_x, GLfloat pos_y, GLfloat width, GLfloat height) {
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
       glVertex2i(pos_x - width/2, pos_y + height/2);
       glVertex2i(pos_x + width/2, pos_y + height/2);
       glColor3fv(color);
       glVertex2f(pos_x + width/2 - 10, pos_y + 3*height/20);
       glVertex2f(pos_x - width/2 + 10, pos_y + 3*height/20);
     glEnd();

     // left part
     glBegin(GL_POLYGON);
       glColor3fv(color1);
       glVertex2f(pos_x - width/2, pos_y + height/2);
       glVertex2f(pos_x - width/2, pos_y - height/2);
       glColor3fv(colorN2);
       glVertex2f(pos_x - width/2 + 10, pos_y + 3*height/20);
     glEnd();

     // right part
     glBegin(GL_POLYGON);
       glColor3fv(color1);
       glVertex2f(pos_x + width/2, pos_y + height/2);
       glVertex2f(pos_x + width/2, pos_y - height/2);
       glColor3fv(colorN2);
       glVertex2f(pos_x + width/2 - 10, pos_y + 3*height/20);
     glEnd();

     // lower part
     glBegin(GL_POLYGON);
       glColor3fv(colorN1);
       glVertex2f(pos_x - width/2 + 10, pos_y + 3*height/20);
       glVertex2f(pos_x + width/2 - 10, pos_y + 3*height/20);
       glColor3fv(colorN3);
       glVertex2f(pos_x + width/2, pos_y - height/2);
       glVertex2f(pos_x - width/2, pos_y - height/2);
     glEnd();
     // End GL_POLYGON -- Background

     // GL_LINE_LOOP -- White Border
     glColor3f(1.0, 1.0, 1.0);
     glBegin(GL_LINE_LOOP);
       glVertex2f(pos_x - width/2, pos_y + height/2);
       glVertex2f(pos_x + width/2, pos_y + height/2);
       glVertex2f(pos_x + width/2, pos_y - height/2);
       glVertex2f(pos_x - width/2, pos_y - height/2);
     glEnd();
}

void readMap() {
     FILE *mapFile = fopen("HW2.map", "r");
     char temp;
     int i, j;
     for (i = 0; i < LINE_NUM; i++)
         for (j = 0; j < CHAR_NUM; j++) {
             fscanf(mapFile, " %c", &temp);
             blocks[i][j].hp = temp - '0';
             blocks[i][j].props = 0;
             blocks[i][j].row = i;
             blocks[i][j].column = j;
         }
     fclose(mapFile);
}

void drawBlocks() {
     GLfloat width = 60.0, height = 20.0,
             pos_x = width / 2.0, pos_y = 800 - height / 2.0;
     // Read characters line by line
     int i, j;
     for (i = 0; i < 21; i++) {
         GLfloat color[3];
         // Process character by character
         for (j = 0; j < 10; j++) {
             switch (blocks[i][j].hp) {
             // Yellow blocks
             case 1:
                  color[0] = 0.95;
                  color[1] = 0.95;
                  color[2] = 0.0;
                  drawBlockColor(color, pos_x, pos_y, width, height);
                  break;

             // Red blocks
             case 2:
                  color[0] = 1.0;
                  color[1] = 0.2;
                  color[2] = 0.2;
                  drawBlockColor(color, pos_x, pos_y, width, height);
                  break;

             // Skin color blocks
             case 3:
                  color[0] = 1.0;
                  color[1] = 0.9;
                  color[2] = 0.6;
                  drawBlockColor(color, pos_x, pos_y, width, height);
                  break;

             // Dark red blocks
             case 4:
                  color[0] = 0.67;
                  color[1] = 0.25;
                  color[2] = 0.0;
                  drawBlockColor(color, pos_x, pos_y, width, height);
                  break;
             // Black blocks
             case 5:
                  color[0] = 0.5;
                  color[1] = 0.5;
                  color[2] = 0.5;
                  drawBlockColor(color, pos_x, pos_y, width, height);
                  break;
             } // End switch
             // Move to right
             pos_x += width;
         } // End for-loop: Process character by character
         // Move to left and move down
         pos_x = width / 2.0;
         pos_y -= height;
     } // End for-loop: Read line by line
} // End drawBlocks()


// Ball of NTUST << It's ignorable!! >>      XDDDD
void drawBall_NTUST() {
     GLfloat theta, x, y;
     
     glBegin(GL_TRIANGLE_FAN);
         glColor3fv(bgColor);
         glVertex2f(ball0.center.x, ball0.center.y);
         for (theta = 0.0; theta <= 180.0; theta += 1.0) {
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();

     glBegin(GL_TRIANGLE_FAN);
         glColor3f(0.0, 0.0, 0.5);
         glVertex2f(ball0.center.x, ball0.center.y);
         for (theta = -225.0; theta <= 45.0; theta += 1.0) {
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();

     glColor3fv(bgColor);
     glBegin(GL_POLYGON);
         glVertex2f(ball0.center.x - 2*ball0.radius/3, ball0.center.y + 2*ball0.radius/3);
         glVertex2f(ball0.center.x + 2*ball0.radius/3, ball0.center.y + 2*ball0.radius/3);
         glVertex2f(ball0.center.x + 2*ball0.radius/3, ball0.center.y);
         glVertex2f(ball0.center.x, ball0.center.y - ball0.radius/2);
         glVertex2f(ball0.center.x - 2*ball0.radius/3, ball0.center.y);
     glEnd();

     glBegin(GL_POLYGON);
         glVertex2f(ball0.center.x - ball0.radius/15, ball0.center.y);
         glVertex2f(ball0.center.x + ball0.radius/15, ball0.center.y);
         glVertex2f(ball0.center.x + ball0.radius/15, ball0.center.y - ball0.radius);
         glVertex2f(ball0.center.x - ball0.radius/15, ball0.center.y - ball0.radius);
     glEnd();
     
     glColor3f(0.0, 0.0, 0.5);
     glBegin(GL_POLYGON);
         glVertex2f(ball0.center.x, ball0.center.y + ball0.radius);
         glVertex2f(ball0.center.x + 8*ball0.radius/15, ball0.center.y + 2*ball0.radius/3);
         glVertex2f(ball0.center.x + 8*ball0.radius/15, ball0.center.y + ball0.radius/20);
         glVertex2f(ball0.center.x, ball0.center.y - ball0.radius/3);
         glVertex2f(ball0.center.x - 8*ball0.radius/15, ball0.center.y + ball0.radius/22);
         glVertex2f(ball0.center.x -8*ball0.radius/15, ball0.center.y + 2*ball0.radius/3);
     glEnd();

     ball0.radius /= 3.0;
     ball0.center.y += ball0.radius;
     glBegin(GL_TRIANGLE_FAN);
         glEnable(GL_BLEND);
         glColor3f(0.95, 1.0, 0.95);
         glVertex2f(ball0.center.x, ball0.center.y);
         for (theta = -180.5; theta < 180.0; theta += 1.0) {
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();
     
     ball0.center.y -= ball0.radius;
     ball0.radius *= 3.0;
     glBegin(GL_TRIANGLE_FAN);
         glColor4f(0.0, 0.0, 0.0, 0.5);
         glVertex2f(ball0.center.x + ball0.radius / 2.5, ball0.center.y + ball0.radius / 2.5);
         for (theta = -135.0; theta < 226.0; theta += 1.0) {
             glColor4f(0.0, 0.0, 0.0, (abs(theta - 45.1)) / 180.0);
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();
     glBegin(GL_TRIANGLE_FAN);
         glColor4f(1.0, 1.0, 1.0, 0.2);
         glVertex2f(ball0.center.x + ball0.radius / 2, ball0.center.y + ball0.radius / 2);
         for (theta = -135.0; theta <= 225.0; theta += 1.0) {
             glColor4f(0.0, 0.0, 0.0, abs(theta-44.4) / 360.0);
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();
}

void drawBall() {
     GLfloat theta, x, y;

     // GL_TRIANGLE_FAN -- Background
     glBegin(GL_TRIANGLE_FAN);
         // Center of the ball
         glColor3f(1.0, 1.0, 1.0);
         glVertex2f(ball0.center.x + ball0.radius / 2.5, ball0.center.y + ball0.radius / 2.5);

         // Circumference
         for (theta = -135.0; theta < 226.0; theta += 1.0) {
             glColor3f(0.0, 0.8 - (GLfloat) abs(theta-44.4) / 360.0, 0.0);
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();

     glBegin(GL_TRIANGLE_FAN);
         glColor4f(0.0, 0.0, 0.0, 0.0);
         glVertex2f(ball0.center.x + ball0.radius / 2.5, ball0.center.y + ball0.radius / 2.5);
         for (theta = -135.0; theta < 226.0; theta += 1.0) {
             glColor4f(0.0, 0.0, 0.0, (abs(theta - 45.1)+180) / 360.0);
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();

     // GL_LINE_LOOP -- Green border
     glBegin(GL_LINE_LOOP);
         for (theta = -180.5; theta < 181.0; theta += 1.0) {
             glColor3f(0.8, 1.0, 0.8);
             x = ball0.center.x + ball0.radius * cos(theta * DEGREE);
             y = ball0.center.y + ball0.radius * sin(theta * DEGREE);
             glVertex2f(x, y);
         }
     glEnd();
}

void drawBoard(board_t b) {
     drawBlockColor(board.bgColor, board.center.x, board.center.y, board.width, board.height);
}

void drawEffect(block_t block) {
     srand(time(0));
     GLfloat color[3] = {(double)(rand()%20) / 20, (double)(rand()%20) / 20, (double)(rand()%20) / 20};
     drawBlockColor(color, block.column * 60 + 30, 800 - block.row * 20 - 10, 60, 20);
}

void drawWin() {
     char string[] = "You win";
     int shift = 0, i;
     glColor3f(0.0,0.0,0.0); 
	 glPushMatrix();
     for (i = 0; i < strlen(string); i++) {
		 glRasterPos2i(250 + shift, 300);
		 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
		 shift += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
      }
	  glPopMatrix();
}

void drawText() {
     char string[] = "Press space to start";
     int shift = 0, i;
     glColor3f(0.0,0.0,0.0); 
	 glPushMatrix();
     for (i = 0; i < strlen(string); i++) {
		 glRasterPos2i(200 + shift, 265);
		 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
		 shift += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
      }
	  glPopMatrix();
}



