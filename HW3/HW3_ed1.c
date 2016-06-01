#include <windows.h>
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>
#define GLfloat float
#define GLint int
#define PI 3.1415926
#define DEG 3.14159/180.0
#define COLOR GLfloat[3]
#define BLOCK_L 60
#define BLOCK_W 20
#define BLOCK_H 20

/*-------------------------- Type definition -------------------------------*/

// point_t (x, y, z)
typedef struct {
    GLfloat x, y, z;
} point_t;

typedef struct {
	GLfloat r, g, b;
} color_t;

// length_t (l, w, h)
typedef struct {
    GLfloat l, w, h;
} length_t;

// ball_t center, radius, theta, movement
typedef struct {
    point_t center;
    GLfloat radius;
    GLfloat theta;
    GLfloat alpha;
    GLfloat movement;
} ball_t;

// board_t center, width, height, bgColor[3]
typedef struct {
	color_t  color;
    point_t  center;
    length_t length;
} board_t;

// block_t hp, center
typedef struct {
    int hp;
    point_t center;
} block_t;

// wall_t
typedef struct {
	color_t  color;
	point_t  center;
	length_t length;
} wall_t; 

/*------------------------- Type definition end ----------------------------*/

GLfloat v[4][3] = {{0.0, 0.0, 1.0}, {0.0, 0.942809, -0.333333},
				   {-0.816497, -0.471405, -0.333333},
				   {0.816497, -0.471405, -0.333333}};

//物件 
board_t board;
wall_t  wall[5];
block_t blocks[5][15][12];
ball_t  ball0;

//鍵盤控制
char keyW;
char keyS;
char keyA;
char keyD;
char keyUp;
char keyDown;
char keyLeft;
char keyRight;
char keySpace;

//視角控制
float cameraAtX;
float cameraAtY;
float cameraAtZ;
float lookAtX;
float lookAtY;
float lookAtZ;
float moveX;
float moveY;

//遊戲控制
char over;
char restart;
int  effectCount[6];
GLfloat transparency[6];

void readMap() {
	FILE *mapFile = fopen("HW3.map", "r");
	char temp;
	int i = 0, j = 0, k = 0;
	while (1) {
		fscanf(mapFile, "%c", &temp);
		if (temp == '\n') {
			j++;
			k = 0;
		} else if (temp == '#') {
			i++;
			j = -1;
		} else if (temp == '/') {
			break;
		} else {
			blocks[i][j][k].hp = temp - '0';
			blocks[i][j][k].center.x =  k * 0.6 - 3.3;
			blocks[i][j][k].center.y =  3.5 - j * 0.5;
			blocks[i][j][k].center.z = 2.25 + i * 0.5;
			k++;
		}
	}
	
	fclose(mapFile);
}

void settingObject() {
	//鍵盤設定
	keyW = 0;
	keyS = 0;
	keyA = 0;
	keyD = 0;
	keyUp = 0;
	keyDown = 0;
	keyLeft = 0;
	keyRight = 0;

	//視角控制
	moveX = 0;
	moveY = 1;

	char over = 0;
	char restart = 1;
	int i;
	for (i = 0; i < 5; i++) {
		effectCount[i] = 0;
		transparency[i] = 0.2;
	}
	effectCount[5] = 0;
	transparency[5] = 0.5;

	readMap();
	// set board
	board.color.r  = 0.9; board.color.g  = 0.45; board.color.b = 0.225;
	board.center.x = 0.0; board.center.y = 0.0; board.center.z = 9.96;
	board.length.l = 1.8; board.length.h = 1.8; board.length.w = 0.08;
	
	// set ball
	ball0.center.x = 0.0; ball0.center.y = 3.0; ball0.center.z = 8.0;
	ball0.movement = 0.1; ball0.theta    = 60 ; ball0.alpha    = 240;
	ball0.radius   = 0.1;
	
	// set wall
	// right wall
	wall[0].color.r  =  0.0; wall[0].color.g  =  1.0; wall[0].color.b  =  1.0;
	wall[0].center.x =  5.0; wall[0].center.y =  0.0; wall[0].center.z =  5.0;
	wall[0].length.l = 0.08; wall[0].length.h = 10.0; wall[0].length.w = 10.0;
	//left wall
	wall[1].color.r  =  1.0; wall[1].color.g  =  0.0; wall[1].color.b  =  1.0;
	wall[1].center.x = -5.0; wall[1].center.y =  0.0; wall[1].center.z =  5.0;
	wall[1].length.l = 0.08; wall[1].length.h = 10.0; wall[1].length.w = 10.0;
	// top wall
	wall[2].color.r  =  1.0; wall[2].color.g  =  1.0; wall[2].color.b  =  0.0;
	wall[2].center.x =  0.0; wall[2].center.y =  5.0; wall[2].center.z =  5.0;
	wall[2].length.l = 10.0; wall[2].length.h = 0.08; wall[2].length.w = 10.0;
	// bottom wall
	wall[3].color.r  =  0.0; wall[3].color.g  =  0.0; wall[3].color.b  =  1.0;
	wall[3].center.x =  0.0; wall[3].center.y = -5.0; wall[3].center.z =  5.0;
	wall[3].length.l = 10.0; wall[3].length.h = 0.08; wall[3].length.w = 10.0;
	// back wall
	wall[4].color.r  =  0.0; wall[4].color.g  =  1.0; wall[4].color.b  =  0.0;
	wall[4].center.x =  0.0; wall[4].center.y =  0.0; wall[4].center.z =  0.0;
	wall[4].length.l = 10.0; wall[4].length.h = 10.0; wall[4].length.w = 0.08;
}

GLfloat distance(GLfloat a, GLfloat b) {
	if (a > b)
		return a - b;
	else
		return b - a;
}

void moveBall() {
	int i;
	for (i = 0; i < 5; i++) {
		if (effectCount[i] > 0)
			effectCount[i]++;
		if (effectCount[i] >= 5) {
			effectCount[i] = 0;
			transparency[i] = 0.2;
		}
	}
	if (effectCount[5] > 0)
		effectCount[5]++;
	if (effectCount[5] >= 5) {
		effectCount[5] = 0;
		transparency[5] = 0.5;
	}
	
	srand(time(0));
	// collides right wall
	if (distance(wall[0].center.x, ball0.center.x) <= wall[0].length.l / 2.0 + ball0.radius &&
		distance(wall[0].center.y, ball0.center.y) <= wall[0].length.h / 2.0 + ball0.radius &&
		distance(wall[0].center.z, ball0.center.z) <= wall[0].length.w / 2.0 + ball0.radius){
		ball0.alpha = 180.0 - ball0.alpha;
		ball0.theta = 180.0 - ball0.theta;
		effectCount[0] = 1;
		transparency[0] = 0.1;
	}
	
	// collides left wall
	if (distance(wall[1].center.x, ball0.center.x) <= wall[1].length.l / 2.0 + ball0.radius &&
		distance(wall[1].center.y, ball0.center.y) <= wall[1].length.h / 2.0 + ball0.radius &&
		distance(wall[1].center.z, ball0.center.z) <= wall[1].length.w / 2.0 + ball0.radius){
		ball0.alpha = 180.0 - ball0.alpha;
		ball0.theta = 180.0 - ball0.theta;
		effectCount[1] = 1;
		transparency[1] = 0.1;
	}

	// collides top wall
	if (distance(wall[2].center.x, ball0.center.x) <= wall[2].length.l / 2.0 + ball0.radius &&
		distance(wall[2].center.y, ball0.center.y) <= wall[2].length.h / 2.0 + ball0.radius &&
		distance(wall[2].center.z, ball0.center.z) <= wall[2].length.w / 2.0 + ball0.radius){
		ball0.theta = 0.0 - ball0.theta;
		effectCount[2] = 1;
		transparency[2] = 0.1;
	}
	
	// collides bottom wall
	if (distance(wall[3].center.x, ball0.center.x) <= wall[3].length.l / 2.0 + ball0.radius &&
		distance(wall[3].center.y, ball0.center.y) <= wall[3].length.h / 2.0 + ball0.radius &&
		distance(wall[3].center.z, ball0.center.z) <= wall[3].length.w / 2.0 + ball0.radius){
		ball0.theta = 0.0 - ball0.theta;
		effectCount[3] = 1;
		transparency[3] = 0.1;
	}

	// collides back wall
	if (distance(wall[4].center.x, ball0.center.x) <= wall[4].length.l / 2.0 + ball0.radius &&
		distance(wall[4].center.y, ball0.center.y) <= wall[4].length.h / 2.0 + ball0.radius &&
		distance(wall[4].center.z, ball0.center.z) <= wall[4].length.w / 2.0 + ball0.radius){
		ball0.alpha = 180.0 - ball0.alpha;
		effectCount[4] = 1;
		transparency[4] = 0.1;
	}
	
	// collides board
	if (distance(board.center.x, ball0.center.x) <= board.length.l / 2.0 + ball0.radius &&
		distance(board.center.y, ball0.center.y) <= board.length.h / 2.0 + ball0.radius &&
		distance(board.center.z, ball0.center.z) <= board.length.w / 2.0 + ball0.radius) {
		ball0.alpha = 180.0 - ball0.alpha;
		effectCount[5] = 1;
		transparency[5] = 0.1;
		if (effectCount[5] >= 5) {
			effectCount[5] = 0;
			transparency[5] = 0.3;
		}
	}

	if (ball0.center.z - wall[3].center.z >= wall[3].length.w / 1.5)
		over = 1;

	ball0.center.x += ball0.movement * cos(ball0.theta * DEG) * sin(ball0.alpha * DEG);
	ball0.center.y += ball0.movement * sin(ball0.theta * DEG);
	ball0.center.z += ball0.movement * cos(ball0.theta * DEG) * cos(ball0.alpha * DEG);
	
}

char isCollision(block_t b) {
	if (b.hp != 0)
		if ((distance(ball0.center.x, b.center.x) <= ball0.radius + 0.3) &&
			(distance(ball0.center.y, b.center.y) <= ball0.radius + 0.25) &&
			(distance(ball0.center.z, b.center.z) <= ball0.radius + 0.25))
			if (distance(ball0.center.x, b.center.x) <= 0.3)
				return 1;
			else if (distance(ball0.center.y, b.center.y) <= 0.25)
				return 2;
			else if (distance(ball0.center.z, b.center.z) <= 0.25)
				return 3;
			else
				return 4;
	return 0;
}

void collideBlocks() {
	int i, j, k, c;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 15; j++)
			for (k = 0; k < 12; k++) {
				c = isCollision(blocks[i][j][k]);
				if (c) {
					blocks[i][j][k].hp--;
					if (c == 1)
						ball0.theta = 0.0 - ball0.theta;
					else if (c == 2)
						ball0.alpha = 180.0 - ball0.alpha;
					else if (c == 3) {
						ball0.alpha = 180.0 - ball0.alpha;
						ball0.theta = 180.0 - ball0.theta;
					} else {
						ball0.alpha += 180.0;
						ball0.theta += 180.0;
					}
				}
			}
}

void drawBoard() {
	GLfloat mat_diffuse[] = {board.color.r, board.color.g, board.color.b, transparency[5]};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glPushMatrix();
	  glTranslatef(board.center.x, board.center.y, board.center.z);
	  glScalef(board.length.l, board.length.h, board.length.w);
	  glutSolidCube(1);
	glPopMatrix();
}

void drawWall() {
	int i;
	for (i = 4; i >= 0; i--) {
		GLfloat mat_diffuse[] = {wall[i].color.r, wall[i].color.g, wall[i].color.b, transparency[i]};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glPushMatrix();
		  glTranslatef(wall[i].center.x, wall[i].center.y, wall[i].center.z);
		  glScalef(wall[i].length.l, wall[i].length.h, wall[i].length.w);
		  glutSolidCube(1);
		glPopMatrix();
	}
}

void drawBlocks() {
	int i, j, k;
	GLfloat mat_diffuse[5][4] = {{1,  1,  1, 0.0},
								 {1,  0,  0, 0.7},
								 {.3, .3, .3, 0.7},
								 {0, 0, 0, 0.7},
								 {1, 1, 1, 0.7}};
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 15; j++) {
			for (k = 0; k < 12; k++) {
				if (blocks[i][j][k].hp != 0) {
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse[blocks[i][j][k].hp]);
					glPushMatrix();
				  	  glTranslatef(blocks[i][j][k].center.x, blocks[i][j][k].center.y, blocks[i][j][k].center.z);
					  glScalef(0.6, 0.5, 0.5);
					  glutSolidCube(1);
					glPopMatrix();
				}
			}
		}
	}
}

void normalize(GLfloat *p) {
	GLfloat d = 0.0;
	int i;
	for (i = 0; i < 3; i++) d += p[i] * p[i];
	d = sqrt(d);
	if (d > 0.0) for (i = 0; i < 3; i++) p[i] /= d;
}

void triangle(GLfloat *a, GLfloat *b, GLfloat *c) {
	glBegin(GL_POLYGON);
	  glNormal3fv(a);
	  glVertex3fv(a);
	  glNormal3fv(b);
	  glVertex3fv(b);
	  glNormal3fv(c);
	  glVertex3fv(c);
	glEnd();
}

void divide_triangle(GLfloat *a, GLfloat *b, GLfloat *c, int n) {
    GLfloat v1[3], v2[3], v3[3];
    int j;
    if (n > 0) {
		for (j = 0; j < 3; j++) v1[j] = a[j] + b[j];
        normalize(v1);
        for (j = 0; j < 3; j++) v2[j] = a[j] + c[j];
        normalize(v2);
        for (j = 0; j < 3; j++) v3[j] = b[j] + c[j];
        normalize(v3);
        divide_triangle(a, v2, v1, n - 1);
        divide_triangle(c, v3, v2, n - 1);
        divide_triangle(b, v1, v3, n - 1);
        divide_triangle(v1, v2, v3, n-1);
    }
    else triangle(a, b, c);
}

void tetrahedron(int n) {
	divide_triangle(v[0], v[1], v[2], n);
	divide_triangle(v[3], v[2], v[1], n);
	divide_triangle(v[0], v[3], v[1], n);
	divide_triangle(v[0], v[2], v[3], n);
}

void drawBall() {
	glPolygonMode(GL_BACK, GL_FILL);
	GLfloat d = abs(11.0 - ball0.center.z) / 40.0;
    GLfloat mat_diffuse[]  = {0.2 - d, 0.3 - d, d * 3.0, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	
	glTranslatef(ball0.center.x, ball0.center.y, ball0.center.z);
	glScalef(ball0.radius * 2, ball0.radius * 2, ball0.radius * 2);
	tetrahedron(5);
	glScalef(0.5 / ball0.radius, 0.5 / ball0.radius, 0.5 / ball0.radius);
	glTranslatef(-ball0.center.x, -ball0.center.y, -ball0.center.z);
	glPolygonMode(GL_BACK, GL_LINE);
}

void drawShadow() {
	GLfloat theta, x, y, z;
    GLfloat mat_diffuse[]  = {0.0, 0.0, 0.0, 0.2 + (5.0 - ball0.center.y) * 0.08};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(ball0.center.x, -4.95, ball0.center.z);
	for (theta = -180.5; theta < 181.0; theta += 1.0) {
		x = ball0.center.x + ball0.radius * cos(theta * DEG);
		z = ball0.center.z + ball0.radius * sin(theta * DEG);
		glVertex3f(x, -4.95, z);
    }
	glEnd();

	if (distance(board.center.x, ball0.center.x) <= board.length.l / 2.0 &&
		distance(board.center.y, ball0.center.y) <= board.length.h / 2.0) {
		glBegin(GL_LINE_LOOP);

		for (theta = -180.5; theta < 181.0; theta += 1.0) {
			x = ball0.center.x + ball0.radius * cos(theta * DEG);
			y = ball0.center.y + ball0.radius * sin(theta * DEG);
			glVertex3f(x, y, 10.0);
	    }
		glEnd();
	}
}

void myinit()
{
	//物件設定
	settingObject();

	//背景顏色
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); 

	//3D用
	glEnable(GL_DEPTH_TEST);
	
	//正面填滿
	glPolygonMode(GL_FRONT, GL_FILL);

	//背面畫線
	glPolygonMode(GL_BACK, GL_LINE);
	
	//混色函數
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);  //啟用

	//shading方法
	glShadeModel(GL_SMOOTH);

	//光源 
	glEnable(GL_LIGHTING);

	//透視投影
	float rate = (float)600 / (float)800;//畫面視野變了，但內容不變形   
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, rate, 1.0, 3000.0);

	//攝影機視角
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)
{
	//清除畫面
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	if (restart) {
		settingObject();
		restart = 0;
	}
	
	if(keySpace) {
		restart = 1;
		over = 0;
	}
	
	if (!over) {
		//攝影機移動 & 板子移動 
		if (keyW || keyUp && board.center.y + board.length.h / 2.0 + 0.14 <= 5.0)
		{ 
			moveY += .15;
			board.center.y += .15;
		} 
		if(keyS || keyDown && board.center.y - board.length.h / 2.0 - 0.14 >= -5.0)
		{
			moveY -= .15;
			board.center.y -= .15;
		} 
		if(keyA || keyLeft && board.center.x - board.length.l / 2.0 - 0.14 >= -5.0)
		{ 
			moveX -= .15;
			board.center.x -= .15;
		} 
		if(keyD || keyRight && board.center.x + board.length.l / 2.0 + 0.14 <= 5.0)
		{ 
			moveX += .15;
			board.center.x += .15;
		}
	
		//球移動
		moveBall();

		//偵測是否碰撞到磚塊 
		collideBlocks();
	
		//攝影機設定
		lookAtX = 0;
		lookAtY = 0;
		lookAtZ = 0;
		cameraAtX = moveX;
		cameraAtY = moveY;
		cameraAtZ = 14;
	
	
		//攝影機視角
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt( cameraAtX, cameraAtY, cameraAtZ, lookAtX, lookAtY, lookAtZ, cameraAtX, cameraAtY+1  , cameraAtZ);
	
		//光源 
		GLfloat light_diffuse[] ={1.0, 0.75, 0.5, 0.2};
	    GLfloat light0_pos[] ={0.0 , 2.0, 16.0 , 0.5}; 
		glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	    glEnable(GL_LIGHT0);
	    
		//畫
		drawBall();
		drawShadow();
		drawBlocks();
		drawWall();
		drawBoard();
	}
	
	//動畫用
	glutSwapBuffers();
}

//動畫用
void idle(void)
{
	//printf("\n  idle");
	Sleep(10);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	//printf("\nkeyboard key=%c, x=%d, y=%d",key,x,y);
	if(key == 'd' || key == 'D')
		keyD = 1;
	if(key == 'w' || key == 'W')
		keyW = 1;
	if(key == 'a' || key == 'A')
		keyA = 1;
	if(key == 's' || key == 'S')
		keyS = 1;
	if(key == ' ')
		keySpace = 1;
}

void keyboardUp(unsigned char key, int x, int y)
{
	//printf("\nkeyboardUp key=%c, x=%d, y=%d",key,x,y);
	if(key == 'd' || key == 'D')
		keyD = 0;
	if(key == 'w' || key == 'W')
		keyW = 0;
	if(key == 'a' || key == 'A')
		keyA = 0;
	if(key == 's' || key == 'S')
		keyS = 0;
	if(key == ' ')
		keySpace = 0;
}

void keyboardS(int key, int x, int y)
{
	//printf("\nkeyboardS key=%d, x=%d, y=%d",key,x,y);
	if(key == GLUT_KEY_RIGHT)
		keyRight = 1;
	if(key == GLUT_KEY_UP)
		keyUp = 1;
	if(key == GLUT_KEY_LEFT)
		keyLeft = 1;
	if(key == GLUT_KEY_DOWN)
		keyDown = 1;
}
void keyboardSUp(int key, int x, int y)
{
	//printf("\nkeyboardSUp key=%d, x=%d, y=%d",key,x,y);
	if(key == GLUT_KEY_RIGHT)
		keyRight = 0;
	if(key == GLUT_KEY_UP)
		keyUp = 0;
	if(key == GLUT_KEY_LEFT)
		keyLeft = 0;
	if(key == GLUT_KEY_DOWN)
		keyDown = 0;
}

void mouse(int button, int state, int x, int y)
{
	//printf("mouse state = %d\n",state);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	
		//printf("mouse down state = %d \n",state);
	}

}



void motion (int x, int y) 
{
	//printf("mouse %d, %d\n",x,y);
}

int main(int argc, char** argv)
{

	// standard GLUT initialization 
    glutInit(&argc,argv);
    
	//設定畫圖模式
	//3D動畫
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowSize(600, 800); // 600 x 800 pixel window 
    glutInitWindowPosition(120, 0); // place window top left on display 
    glutCreateWindow("homework3"); // window title 
    
	//鍵盤事件
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	
	//鍵盤事件-特殊鍵 
	glutSpecialFunc(keyboardS);
	glutSpecialUpFunc(keyboardSUp);
	
	//滑鼠事件
    glutMouseFunc(mouse);
	glutMotionFunc(motion); 

	//影格事件
	glutDisplayFunc(display);
	//動畫
	glutIdleFunc(idle); 
	
    myinit();
  
    glutMainLoop(); // enter event loop 
}
	
