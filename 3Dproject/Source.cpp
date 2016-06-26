#include "zombie.h"
#include <windows.h> 
#include <wingdi.h> 
#include<time.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>


#define DEFAULT_SHADER 0
#define MY_SHADER 1
#define STEP 1
// Maximum data buffers we will need.
#define NUM_BUFFERS 5

// Maximum emissions we will need.
#define NUM_SOURCES 5

// These index the buffers and sources.
#define SOURCE  0
#define SOURCE1 1
#define SOURCE2 2
#define SOURCE3 3
#define SOURCE4 4


//Directional Light
float light_amb[4] = { 1.0, 1.0, 1.0, 1 };
float light_dif[4] = { 1.0, 1.0, 1.0, 1 };
float light_spe[4] = { 1.0, 1.0, 1.0, 1 };
float light_dir[4] = { 0, 100, 100, 1 };
//spot light
float  lit2_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };
float  lit2_specular[] = { 0.0, 0.0, 0.0, 1.0 };
float  lit3_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
float  lit3_specular[] = { 0.0, 1.0, 0.0, 1.0 };
float  lit2_cutoff = 60;
float  lit2_position[] = { 0, 30, 0, 1.0 };
float  lit2_direction[] = { 0.0, -1.0, 0.0 };
bool light_color = 0;
//Polygon Material
float material_amb[4] = { 0.1, 0.1, 0.1, 1 };
float material_dif[4] = { 0.6, 0.6, 0.6, 1 };
float material_spe[4] = { 0.3, 0.3, 0.3, 1 };
float material_spe0[4] = { 0.0, 0.0, 0.0, 1 };
float shininess = 64.0f;
int shader_effect = DEFAULT_SHADER;

/*-----Define a unit box--------*/
/* Vertices of the box */
float  points[][3] = { { -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 },
{ 0.5, 0.5, -0.5 }, { -0.5, 0.5, -0.5 },
{ -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 },
{ 0.5, 0.5, 0.5 }, { -0.5, 0.5, 0.5 } };
/* face of box, each face composing of 4 vertices */
int    face[][4] = { { 0, 3, 2, 1 }, { 0, 1, 5, 4 }, { 1, 2, 6, 5 },
{ 4, 5, 6, 7 }, { 2, 3, 7, 6 }, { 0, 4, 7, 3 } };
int    cube[6] = { 0, 1, 2, 3, 4, 5 };
double color[6][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
, { 0.0, 0.0, 0.0 } };
/*----Define normals of faces ----*/
float vertex[][3] = { { -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 }, { 0.5, 0.5, -0.5 }, { -0.5, 0.5, -0.5 }, { -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 }, { 0.5, 0.5, 0.5 }, { -0.5, 0.5, 0.5 } };

float  normal[][4] = { { 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 }, { 1.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }, { -1.0, 0.0, 0.0 } };

//texture
unsigned char *image0;         //得到圖案，是能直接讓OpenGL使用的資料了 
unsigned char *image1;
unsigned char *image2;
unsigned char *image3;
unsigned char *image4;
unsigned char *image5;
unsigned char *image6;
unsigned char *image7;
unsigned int TextureID[8];

/*-------scene-------*/
int gameTime = 0;
float grassTextureOffset = 0;
float fog_color[] = { 0.15, 0.15, 0.15, 0.50 };

/*-------move-------*/
float eyeX = 0.0;
float eyeY = 10.0;

unsigned char *LoadBitmapFile(char *fileName, BITMAPINFO *bitmapInfo)
{
	FILE            *fp;
	BITMAPFILEHEADER   bitmapFileHeader;   // Bitmap file header 
	unsigned char       *bitmapImage;      // Bitmap image data 
	unsigned int      lInfoSize;         // Size of information 
	unsigned int      lBitSize;         // Size of bitmap 

	unsigned char change;
	int pixel;
	int p = 0;

	fp = fopen(fileName, "rb");
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);         //讀取 bitmap header 

	lInfoSize = bitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);   //Info的size 
	fread(bitmapInfo, lInfoSize, 1, fp);


	lBitSize = bitmapInfo->bmiHeader.biSizeImage;                  //配置記憶體 
	bitmapImage = new BYTE[lBitSize];
	fread(bitmapImage, 1, lBitSize, fp);                        //讀取影像檔 

	fclose(fp);

	//此時傳回bitmapImage的話，顏色會是BGR順序，下面迴圈會改順序為RGB 
	pixel = (bitmapInfo->bmiHeader.biWidth)*(bitmapInfo->bmiHeader.biHeight);

	for (int i = 0; i<pixel; i++, p += 3)
	{
		//交換bitmapImage[p]和bitmapImage[p+2]的值 
		change = bitmapImage[p];
		bitmapImage[p] = bitmapImage[p + 2];
		bitmapImage[p + 2] = change;
	}

	return bitmapImage;
}

void texture(void)
{
	
	int width;
	int height;


	BITMAPINFO bmpinfo;            //用來存放HEADER資訊 
	glEnable(GL_TEXTURE_2D);
	glGenTextures(8, TextureID);
	//grass
	image0 = LoadBitmapFile("grass.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image0);
	
	//sky
	image1 = LoadBitmapFile("sky.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
	/*
	//dice2
	image2 = LoadBitmapFile("dice2.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	//dice3
	image3 = LoadBitmapFile("dice3.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);
	//dice4
	image4 = LoadBitmapFile("dice4.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);
	//dice5
	image5 = LoadBitmapFile("dice5.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);
	//dice6
	image6 = LoadBitmapFile("dice6.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image6);
	//deck
	image7 = LoadBitmapFile("brick.bmp", &bmpinfo);
	width = bmpinfo.bmiHeader.biWidth;
	height = bmpinfo.bmiHeader.biHeight;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, TextureID[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image7);
	*/
}

void Draw_UnitCube(int index)
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, TextureID[index]);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	for (int i = 0; i<6; i++)
	{
		glNormal3fv(normal[i]);
		glColor3dv(color[i]);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3fv(vertex[face[i][0]]);
		glTexCoord2f(0.0, 1.0);
		glVertex3fv(vertex[face[i][1]]);
		glTexCoord2f(1.0, 1.0);
		glVertex3fv(vertex[face[i][2]]);
		glTexCoord2f(1.0, 0.0);
		glVertex3fv(vertex[face[i][3]]);
		glEnd();
	}
}

void Draw_UnitDice()
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	for (int i = 0; i<6; i++)
	{
		if (i == 0)glBindTexture(GL_TEXTURE_2D, TextureID[3]);
		if (i == 1)glBindTexture(GL_TEXTURE_2D, TextureID[2]);
		if (i == 2)glBindTexture(GL_TEXTURE_2D, TextureID[1]);
		if (i == 3)glBindTexture(GL_TEXTURE_2D, TextureID[4]);
		if (i == 4)glBindTexture(GL_TEXTURE_2D, TextureID[5]);
		if (i == 5)glBindTexture(GL_TEXTURE_2D, TextureID[6]);
		glNormal3fv(normal[i]);
		glColor3dv(color[i]);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3fv(vertex[face[i][0]]);
		glTexCoord2f(0.0, 1.0);
		glVertex3fv(vertex[face[i][1]]);
		glTexCoord2f(1.0, 1.0);
		glVertex3fv(vertex[face[i][2]]);
		glTexCoord2f(1.0, 0.0);
		glVertex3fv(vertex[face[i][3]]);
		glEnd();
	}
}

void draw_sence(){
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, TextureID[0]);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	
	glNormal3f(0.0, 1.0, 0.0);		//草地
	glColor3f(1.0, 1.0, 1.0);      
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0 + grassTextureOffset);
	glVertex3f(500.0, 0.0, 100.0);
	glTexCoord2f(0.0, 20.0 + grassTextureOffset);
	glVertex3f(500.0, 0.0, -500.0);
	glTexCoord2f(20.0, 20.0 + grassTextureOffset);
	glVertex3f(-500.0, 0.0, -500.0);
	glTexCoord2f(20.0, 0.0 + grassTextureOffset);
	glVertex3f(-500.0, 0.0, 100.0);
	glEnd();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, TextureID[1]);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1.0, 1.0, 1.0);		//天空
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(eyeX + 230, -20.0 + eyeY, -500.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(eyeX + 230, 250.0 + eyeY, -500.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(eyeX - 230, 250.0 + eyeY, -500.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(eyeX - 230, -20.0 + eyeY, -500.0);
	glEnd();
		
}



void Light()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_spe);
	glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
	//glDisable(GL_LIGHT0);
	//glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, lit2_cutoff);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lit2_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lit2_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lit2_position);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lit2_direction);
	//glDisable(GL_LIGHT1);
}

void Init()
{
	texture();
	Light();
	
	/*----Set up fog conditions ---*/
	glEnable(GL_FOG);                /*enable fog fade */
	glFogi(GL_FOG_MODE, GL_LINEAR);  /*fog factor=GL_LINEAR,GL_EXP,or GL_EXP2*/
	glFogf(GL_FOG_DENSITY, 0.25);    /*fog opacity(density)= 0.25*/
	glFogf(GL_FOG_START, 100.0);       /*Setup two ends for GL_LINEAR*/
	glFogf(GL_FOG_END, 800.0);
	glFogfv(GL_FOG_COLOR, fog_color);/*set the fog color */
	
}

void DrawAxis()
{
	glBegin(GL_LINES);
	//x axis
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(500, 0, 0);
	//-x axis
	glColor3f(0.5, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-500, 0, 0);
	//y axis
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 500, 0);
	//-y axis
	glColor3f(0, 0.5, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -500, 0);
	//z axis
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 500);
	//-z axis
	glColor3f(0, 0, 0.5);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, -500);
	glEnd();
}


void Display()
{
	glEnable(GL_DEPTH_TEST);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.15, 0.15, 0.15, 1);
	//projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 5000);
	//Camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eyeX, eyeY, 40.0, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);


//	DrawAxis();




	
	glEnable(GL_LIGHTING);//open lighting
	glEnable(GL_LIGHT0);//open light0

	//set polygon material
	glColor3f(0.0, 0.0, 0.9);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_spe0);

	draw_sence();

	/*
	//畫箱子
	glPushMatrix();
	glTranslated(-15, 0, -20);
	glScaled(20, 20, 20);
	Draw_UnitCube(0);
	glPopMatrix();
	//畫石頭
	glPushMatrix();
	glPushMatrix();
	glTranslated(5.5, 0, -15);
	glScaled(10, 10, 10);
	glRotated(30, 0, 1, 0);
	Draw_UnitCube(7);
	glPopMatrix();
	glPopMatrix();

	//畫骰
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_spe);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glPushMatrix();
	glScaled(5, 5, 5);
	glRotated(dice_y, 1, 0, 0);
	glRotated(dice_x, 0, 1, 0);
	Draw_UnitDice();
	glPopMatrix();
	*/
//	glDisable(GL_LIGHT0);//close light0
//	glDisable(GL_LIGHTING);//close lighting

	glutSwapBuffers();
}


void KeyboardDown(unsigned char c, int x, int y)
{

	glFinish();
	glutPostRedisplay();
}

void KeyboardUp(unsigned char c, int x, int y)
{
	
	glFinish();
}
void my_quit(unsigned char key, int ix, int iy)
{
	if (key == 'Q' || key == 'q') exit(0);


	else if (key == '4'){//鏡頭向左
		eyeX -= STEP;        /* move right */
	}
	else if (key == '6'){//鏡頭向右
		eyeX += STEP;       /* move left */
	}		
	glFinish();
	glutPostRedisplay();
}

void Timer(int c)
{	
	gameTime++;
	printf("%d\n", gameTime);
	if ((gameTime / 5) % 2 == 0){
		eyeY += 0.5;
	}
	else if ((gameTime / 5) % 2 == 1){
		eyeY -= 0.5;
	}
	if (grassTextureOffset < 1){
		grassTextureOffset += 0.25;
	}
	else{
		grassTextureOffset = 0.25;
	}
	glutPostRedisplay();
	glutTimerFunc(50, Timer, 0);
}

int main(int argc, char** argv)
{
	//test cube.obj
	///*
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Zombie");//all opengl api 在此後才有作用
	glutDisplayFunc(Display);
	glutKeyboardFunc(my_quit);
//	glutKeyboardFunc(KeyboardDown);
//	glutKeyboardUpFunc(KeyboardUp);
	glutTimerFunc(50, Timer, 0);
	/**/
	//glewInit();
	Init();
	glutMainLoop();
	//*/
}