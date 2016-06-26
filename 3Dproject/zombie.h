#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "zombie.h"
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <string>

using namespace std;


unsigned char *LoadBitmapFile(char *fileName, BITMAPINFO *bitmapInfo);

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
float light_dir[4] = { -100, 100, -100, 1 };
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


/*-----Define a unit box--------*/
/* Vertices of the box */
float  points[][3] = { { -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 },
{ 0.5, 0.5, -0.5 }, { -0.5, 0.5, -0.5 },
{ -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 },
{ 0.5, 0.5, 0.5 }, { -0.5, 0.5, 0.5 } };

/*---- the axes of billboard ----*/
float  a[3], b[3];
float    mtx[16];     /* the modelview matrix */
/*----Define normals of faces ----*/
float vertex[][3] = { { -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 }, { 0.5, 0.5, -0.5 }, { -0.5, 0.5, -0.5 }, { -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 }, { 0.5, 0.5, 0.5 }, { -0.5, 0.5, 0.5 } };

float  normal[][4] = { { 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 }, { 1.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }, { -1.0, 0.0, 0.0 } };
/* face of box, each face composing of 4 vertices */
int    face[][4] = { { 0, 3, 2, 1 }, { 0, 1, 5, 4 }, { 1, 2, 6, 5 },
{ 4, 5, 6, 7 }, { 2, 3, 7, 6 }, { 0, 4, 7, 3 } };
int    cube[6] = { 0, 1, 2, 3, 4, 5 };
double color[6][3] = { { 0.7, 0.7, 0.7 }, { 0.7, 0.7, 0.7 }
, { 0.7, 0.7, 0.7 }, { 0.7, 0.7, 0.7 }, { 0.7, 0.7, 0.7 }
, { 0.7, 0.7, 0.7 } };


class Zombie{
	public: 
		static unsigned int zombieTexture[6]; //存所有需要用到的texture
		static unsigned char *zombieImage[6]; //存所有需要的殭屍貼圖
		float position[3];
		string state;
		bool alive = 1;
		//做出所有殭屍所需的texture
		static void getZombieTexture(){
			int width;
			int height;
			BITMAPINFO bmpinfo;            //用來存放HEADER資訊 
			glEnable(GL_TEXTURE_2D);
			glGenTextures(6, zombieTexture);
			//臉
			zombieImage[0] = LoadBitmapFile("檔名", &bmpinfo);
			width = bmpinfo.bmiHeader.biWidth;
			height = bmpinfo.bmiHeader.biHeight;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, zombieTexture[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, zombieImage[0]);
			//
			zombieImage[1] = LoadBitmapFile("檔名", &bmpinfo);
			width = bmpinfo.bmiHeader.biWidth;
			height = bmpinfo.bmiHeader.biHeight;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, zombieTexture[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, zombieImage[1]);
			//
			zombieImage[2] = LoadBitmapFile("檔名", &bmpinfo);
			width = bmpinfo.bmiHeader.biWidth;
			height = bmpinfo.bmiHeader.biHeight;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, zombieTexture[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, zombieImage[2]);
			//
			zombieImage[3] = LoadBitmapFile("檔名", &bmpinfo);
			width = bmpinfo.bmiHeader.biWidth;
			height = bmpinfo.bmiHeader.biHeight;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, zombieTexture[3]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, zombieImage[3]);
			//
			zombieImage[4] = LoadBitmapFile("檔名", &bmpinfo);
			width = bmpinfo.bmiHeader.biWidth;
			height = bmpinfo.bmiHeader.biHeight;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, zombieTexture[4]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, zombieImage[4]);
			//
			zombieImage[5] = LoadBitmapFile("檔名", &bmpinfo);
			width = bmpinfo.bmiHeader.biWidth;
			height = bmpinfo.bmiHeader.biHeight;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, zombieTexture[5]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, zombieImage[5]);
		}
		//畫殭屍的頭
		static void drawFaceCube(){
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			for (int i = 0; i<6; i++)
			{
				//if (i == 0)glBindTexture(GL_TEXTURE_2D, TextureID[3]);
				//if (i == 1)glBindTexture(GL_TEXTURE_2D, TextureID[2]);
				//if (i == 2)glBindTexture(GL_TEXTURE_2D, TextureID[1]);
				//if (i == 3)glBindTexture(GL_TEXTURE_2D, TextureID[4]);
				//if (i == 4)glBindTexture(GL_TEXTURE_2D, TextureID[5]);
				//if (i == 5)glBindTexture(GL_TEXTURE_2D, TextureID[6]);
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
		//畫殭屍的身體
		static void drawBodyCube(){
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			for (int i = 0; i<6; i++)
			{
				//if (i == 0)glBindTexture(GL_TEXTURE_2D, TextureID[3]);
				//if (i == 1)glBindTexture(GL_TEXTURE_2D, TextureID[2]);
				//if (i == 2)glBindTexture(GL_TEXTURE_2D, TextureID[1]);
				//if (i == 3)glBindTexture(GL_TEXTURE_2D, TextureID[4]);
				//if (i == 4)glBindTexture(GL_TEXTURE_2D, TextureID[5]);
				//if (i == 5)glBindTexture(GL_TEXTURE_2D, TextureID[6]);
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
		//畫殭屍的手
		static void drawHandCube(){
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			for (int i = 0; i<6; i++)
			{
				//if (i == 0)glBindTexture(GL_TEXTURE_2D, TextureID[3]);
				//if (i == 1)glBindTexture(GL_TEXTURE_2D, TextureID[2]);
				//if (i == 2)glBindTexture(GL_TEXTURE_2D, TextureID[1]);
				//if (i == 3)glBindTexture(GL_TEXTURE_2D, TextureID[4]);
				//if (i == 4)glBindTexture(GL_TEXTURE_2D, TextureID[5]);
				//if (i == 5)glBindTexture(GL_TEXTURE_2D, TextureID[6]);
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
		//畫殭屍的角
		static void drawLegCube(){
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			for (int i = 0; i<6; i++)
			{
				//if (i == 0)glBindTexture(GL_TEXTURE_2D, TextureID[3]);
				//if (i == 1)glBindTexture(GL_TEXTURE_2D, TextureID[2]);
				//if (i == 2)glBindTexture(GL_TEXTURE_2D, TextureID[1]);
				//if (i == 3)glBindTexture(GL_TEXTURE_2D, TextureID[4]);
				//if (i == 4)glBindTexture(GL_TEXTURE_2D, TextureID[5]);
				//if (i == 5)glBindTexture(GL_TEXTURE_2D, TextureID[6]);
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
		//function to draw a zombie
		void draw(){
			glPushMatrix();
			//先畫身體
			glScaled(4, 6, 2);
			drawBodyCube();
			glPopMatrix();
			//畫頭
			glPushMatrix();
			glTranslated(0, 5, 0);
			glScaled(4, 4, 4);
			drawFaceCube();
			glPopMatrix();
			//畫手
			glPushMatrix();
				//右手
			glTranslated(2.8, 0, 0);
			glTranslated(0, 2, 3);//向前擺的姿態
			glRotated(90, 1, 0, 0);//向前擺的姿態
				glPushMatrix();
				//glRotated(5, 0, 0, 1);//向前擺的姿態
				glScalef(1.6, 6, 1.6);
				drawHandCube();
				glPopMatrix();
				//左手
			glTranslated(-5.4, 0, 0);	
			//glTranslated(0.8, 0, 0);//向前擺的姿態
				glPushMatrix();
				//glRotated(-5, 0, 0, 1);//向前擺的姿態
				glScalef(1.6, 6, 1.6);
				drawHandCube();
				glPopMatrix();
			glPopMatrix();
			//畫腳
			glPushMatrix();
			glTranslated(1, -6, 0);
				//右腳
				glPushMatrix();
				glScalef(2, 6, 2);
				drawHandCube();
				glPopMatrix();
				//左腳
				glTranslated(-2, 0, 0);
				glPushMatrix();
				glScalef(2, 6, 2);
				drawHandCube();
				glPopMatrix();
			glPopMatrix();

		}


};


void compute_ab_axes(void)
{
	float  w0, w2;
	double len;

	/*----Get w0 and w2 from the modelview matrix mtx[] ---*/
	w0 = mtx[2]; w2 = mtx[10];

	len = sqrt(w0*w0 + w2*w2);
	/*---- Define the a and b axes for billboards ----*/
	b[0] = 0.0; b[1] = 1.0; b[2] = 0.0;
	a[0] = w2 / len; a[1] = 0.0; a[2] = -w0 / len;
	//float  a[3], b[3];
}
//x是左右 + 右 -左
//y是前後 +靠近鏡頭 -遠離鏡頭
//w是bb寬度
//h是bb高度
void draw_billboard(float x, float z, float w, float h)
{
	float  v0[3], v1[3], v2[3], v3[3];

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*----Compute the 4 vertices of the billboard ----*/
	v0[0] = x - (w / 2)*a[0];  v0[1] = 0.0;  v0[2] = z - (w / 2)*a[2];
	v1[0] = x + (w / 2)*a[0];  v1[1] = 0.0;  v1[2] = z + (w / 2)*a[2];
	v2[0] = x + (w / 2)*a[0];  v2[1] = h;    v2[2] = z + (w / 2)*a[2];
	v3[0] = x - (w / 2)*a[0];  v3[1] = h;    v3[2] = z - (w / 2)*a[2];

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3fv(v0);
	glTexCoord2f(1.0, 0.0); glVertex3fv(v1);
	glTexCoord2f(1.0, 1.0); glVertex3fv(v2);
	glTexCoord2f(0.0, 1.0); glVertex3fv(v3);
	glEnd();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}
void testBillboard(){
	/*-------Draw the billboard ----*/
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glBindTexture(GL_TEXTURE_2D, textName);

	draw_billboard(5.0*4.0, 3.0*4.0, 5.0, 8.0);
	draw_billboard(6.0*4.0, 5.0*4.0, 5.0, 8.0);
	draw_billboard(3.0*4.0, 6.0*4.0, 5.0, 8.0);
	draw_billboard(2.0*4.0, 7.0*4.0, 5.0, 8.0);
	draw_billboard(7.0*4.0, 2.0*4.0, 5.0, 8.0);

	glDisable(GL_TEXTURE_2D);
}

#endif 