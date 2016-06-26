/***************************************************************
*  操作方法：                                                  *
*  1. ↑、↓、←、→：控制汽車前進、後退、左轉、右轉和煞車燈。  *
*  2. u、j、h、k：控制鏡頭上、下、左、右。                      *
*  3. 8、2：pitch。                                             *
*  4. 4、6：yaw。                                               *
*  5. 7、9：roll。                                              *
*  6. 3、1：zoom in、zoom out。                                 *
*  7. 0：切換四種視角。                                         *
*  8. l：控制車頭燈。                                           *
*  9. i、y：控制尾翼升降。                                      *
*  10. r：汽車返回原點。                                         *
*  11. q：離開。                                                 *
****************************************************************/


#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <GL/glut.h>


#define   PI   3.141592653
#define   Step  1
#define   TSIZE  64   /* define texture dimension */
#define   ON      1
#define   OFF     0

/*-----Define a unit box--------*/
/* Vertices of the box */
float  points[][3] = { { -0.5, -0.5, -0.5 }, { 0.5, -0.5, -0.5 },
{ 0.5, 0.5, -0.5 }, { -0.5, 0.5, -0.5 },
{ -0.5, -0.5, 0.5 }, { 0.5, -0.5, 0.5 },
{ 0.5, 0.5, 0.5 }, { -0.5, 0.5, 0.5 } };
/* face of box, each face composing of 4 vertices */
int    face[][4] = { { 0, 3, 2, 1 }, { 0, 1, 5, 4 }, { 1, 2, 6, 5 },
{ 4, 5, 6, 7 }, { 2, 3, 7, 6 }, { 0, 4, 7, 3 } };

/* indices of the box faces */
int    cube[6] = { 0, 1, 2, 3, 4, 5 };

/*----Define normals of faces ----*/
float  normals[6][3] = { { 0.0, 0.0, -1.0 }, { 0.0, -1.0, 0.0 }, { 1.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0 }, { -1.0, 0.0, 0.0 } };


/*----Define light source properties -------*/
/*lit0 平行光*/
float  lit0_position[] = { 100.0, 100.0, 100.0, 0.0 };
float  lit0_ambient[] = { 0.4, 0.4, 0.4, 1.0 };//環境
float  lit0_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };//散射
float  lit0_specular[] = { 0.8, 0.8, 0.8, 1.0 };//反射

/*lit1 R/Y/G 點光源*/
float  litR_position[] = { -15.0, 15.0, -38.85 };
float  litR_ambient[] = { 0.2, 0.0, 0.0, 1.0 };
float  litR_diffuse[] = { 0.2, 0.0, 0.0, 1.0 };
float  litR_specular[] = { 0.0, 0.0, 0.0, 1.0 };

float  litY_position[] = { -12.0, 15.0, -38.85 };
float  litY_ambient[] = { 0.2, 0.2, 0.0, 1.0 };
float  litY_diffuse[] = { 0.2, 0.2, 0.0, 1.0 };
float  litY_specular[] = { 0.0, 0.0, 0.0, 1.0 };

float  litG_position[] = { -9.0, 15.0, -38.85 };
float  litG_ambient[] = { 0.0, 0.2, 0.0, 1.0 };
float  litG_diffuse[] = { 0.0, 0.2, 0.0, 1.0 };
float  litG_specular[] = { 0.0, 0.0, 0.0, 1.0 };

/*lit2 頭燈聚光燈*/
float  lit2_position[] = { 8.0, 0.0, 0.0, 1.0 };
float  lit2_direction[] = { -1.0, 0.0, 0.0, 0.0 };
float  lit2_diffuse[] = { 0.9, 0.9, 0.5, 1.0 };
float  lit2_specular[] = { 0.9, 0.9, 0.5, 1.0 };
float  lit2_cutoff = 20.0;

/*lit3 尾燈聚光燈*/
float  lit3_position[] = { -7.5, 0.0, 0.0, 1.0 };
float  lit3_direction[] = { 1.0, 0.0, 0.0, 0.0 };
float  lit3_diffuse[] = { 0.9, 0.5, 0.5, 1.0 };
float  lit3_specular[] = { 0.9, 0.5, 0.5, 1.0 };
float  lit3_cutoff = 40.0;



/*----Some global variables for transformation------*/
float  lit_angle = 0.0;
float  cube_angle = 0.0;
float  head = 0.0, roll = 0.0, Epitch = 0.0, Wpitch = 0.0;
float  up = 0.0, right = 0.0, zoom = 0.0;



static int front_light = -1, back_light = 1;     //控制車燈
static float zoomAngel = 0; //zoom in/out
static int light = 1; //太陽光
static int modulate = 0;
static int fog = 0;

/*---- the axes of billboard ----*/
float  a[3], b[3];

float    mtx[16];     /* the modelview matrix */

/*-Declare GLU quadric objects, sphere, cylinder, and disk --*/
GLUquadricObj  *sphere = NULL, *cylind = NULL, *disk;


/*-Declare car position, orientation--*/
float  self_ang = -90.0, glob_ang = 0.0;
float  position[3] = { 8.0, 0.0, 0.0 };

/*-----Define window size----*/
int width = 800, height = 800;



/*-----Translation and rotations of eye coordinate system---*/
float   eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float   eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double  Eye[3] = { 0.0, 10.0, 30.0 }, Focus[3] = { 0.0, 0.0, 0.0 },
Vup[3] = { 0.0, 1.0, 0.0 };

float   u[3][3] = { { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 } };
float   eye[3];
float   cv, sv; /* cos(5.0) and sin(5.0) */


/*-----Create image space for textures -----*/
unsigned char  lawn[TSIZE][TSIZE][4];         /* lawn textures */
unsigned char  asphalt[TSIZE][TSIZE][4];      /* asphalt road textures */
unsigned char  stick[TSIZE][TSIZE][4];         /* building stick textures */
unsigned char  checkboard[TSIZE][TSIZE][4];	/* checkboard textures */
unsigned int   textName[4];                   /* declare two texture maps*/

float    ds = 0.0, dt = 0.0;
//Texture mapping status
int status = ON;

/*---Define fog color----*/
float fog_color[] = { 0.15, 0.20, 0.20, 0.50 };
float fog_color2[] = { 0.30, 0.20, 0.0, 0.50 };

void draw_scene(void);
float norm2(float v[]);

/*----------------------------------------------------------
* Procedure to make a checkboard texture.
*/
void make_check()
{
	int   i, j, c;

	for (i = 0; i<TSIZE; i++)
		for (j = 0; j<TSIZE; j++){
			if (j>i / 2 && j <= 64 - i / 2) c = 255;
			else c = 0;
			checkboard[i][j][0] = c / 8;
			checkboard[i][j][1] = c * 2 / 3;
			checkboard[i][j][2] = c / 4;
			if (c == 255) checkboard[i][j][3] = 255;
			else checkboard[i][j][3] = 0;
		}
}

/*-------------------------------------------------------
* Procedure to compute the a[] and b[] axes of billboard
* after eye parameters are specified.
*/
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
}

/*----------------------------------------------------------
* Procedure to make a lawn texture.
*/
void make_lawn()
{
	int x, y, i;

	for (y = 0; y<32; y++)
		for (x = 0; x<32; x++)
		{
			lawn[x][y][0] = 240;
			lawn[x][y][1] = 240;
			lawn[x][y][2] = 240;
			lawn[x][y][3] = 255;
		}


	for (x = 24; x<32; x++)
	{
		lawn[x][0][0] = 200;
		lawn[x][0][1] = 255;
		lawn[x][0][2] = 200;
		lawn[x][0][3] = 255;

		lawn[0][x][0] = 200;
		lawn[0][x][1] = 255;
		lawn[0][x][2] = 200;
		lawn[0][x][3] = 255;
	}
	for (x = 26; x<32; x++)
	{
		lawn[x][1][0] = 255;
		lawn[x][1][1] = 200;
		lawn[x][1][2] = 200;
		lawn[x][1][3] = 255;

		lawn[1][x][0] = 200;
		lawn[1][x][1] = 255;
		lawn[1][x][2] = 200;
		lawn[1][x][3] = 255;
	}
	for (x = 27; x<32; x++)
	{
		for (y = 2; y<4; y++)
		{
			lawn[x][y][0] = 200;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 200;
			lawn[x][y][3] = 255;

			lawn[y][x][0] = 200;
			lawn[y][x][1] = 255;
			lawn[y][x][2] = 200;
			lawn[y][x][3] = 255;
		}
	}
	for (x = 28; x<32; x++)
	{
		for (y = 4; y<7; y++)
		{
			lawn[x][y][0] = 200;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 200;
			lawn[x][y][3] = 255;

			lawn[y][x][0] = 200;
			lawn[y][x][1] = 255;
			lawn[y][x][2] = 200;
			lawn[y][x][3] = 255;
		}
	}
	for (x = 29; x<32; x++)
	{
		for (y = 7; y<9; y++)
		{
			lawn[x][y][0] = 200;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 200;
			lawn[x][y][3] = 255;

			lawn[y][x][0] = 200;
			lawn[y][x][1] = 255;
			lawn[y][x][2] = 200;
			lawn[y][x][3] = 255;
		}
	}
	for (x = 30; x<32; x++)
	{
		for (y = 9; y<12; y++)
		{
			lawn[x][y][0] = 200;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 200;
			lawn[x][y][3] = 255;

			lawn[y][x][0] = 200;
			lawn[y][x][1] = 255;
			lawn[y][x][2] = 200;
			lawn[y][x][3] = 255;
		}
	}
	for (x = 31; x<32; x++)
	{
		for (y = 12; y<16; y++)
		{
			lawn[x][y][0] = 200;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 200;
			lawn[x][y][3] = 255;

			lawn[y][x][0] = 200;
			lawn[y][x][1] = 255;
			lawn[y][x][2] = 200;
			lawn[y][x][3] = 255;
		}
	}

	for (y = 16; y<32; y++)
		for (x = 16; x<32; x++)
		{
			lawn[x][y][0] = 200;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 160;
			lawn[x][y][3] = 255;
		}
	for (y = 20; y<32; y++)
		for (x = 20; x<32; x++)
		{
			lawn[x][y][0] = 160;
			lawn[x][y][1] = 255;
			lawn[x][y][2] = 160;
			lawn[x][y][3] = 255;
		}

	for (x = 63; x>31; x--)
	{
		for (y = 0; y<32; y++)
			for (i = 0; i<4; i++)
				lawn[x][y][i] = lawn[63 - x][y][i];
	}

	/* Create the rest half part */
	for (y = 32; y<64; y++)
		for (x = 0; x<64; x++)
			for (i = 0; i<4; i++)
				lawn[x][y][i] = lawn[x][63 - y][i];
}

void make_asphalt()
{
	int  y, x, i, temp;
	for (y = 0; y<64; y++)
	{
		for (x = 0; x<64; x++)
		{
			asphalt[y][x][0] = 100;
			asphalt[y][x][1] = 100;
			asphalt[y][x][2] = 100;
			asphalt[y][x][3] = 255;
		}
	}
	for (y = 16, x = 0; y<48; y++)
	{
		if (y <= 20 && y >= 17)
			x = x + 2;
		else if (y == 22 || y == 25 || y == 31)
			x = x + 1;
		else if (y == 42 || y == 39 || y == 33)
			x = x - 1;
		else if (y <= 47 && y >= 44)
			x = x - 2;
		temp = x;
		for (x = 0; x<temp; x++)
		{
			asphalt[y][x][0] = 150;
			asphalt[y][x][1] = 150;
			asphalt[y][x][2] = 150;
			asphalt[y][x][3] = 255;
		}
		for (x = 64 - temp; x<64; x++)
		{
			asphalt[y][x][0] = 150;
			asphalt[y][x][1] = 150;
			asphalt[y][x][2] = 150;
			asphalt[y][x][3] = 255;
		}
		x = temp;
	}

	for (x = 16, y = 0; x<48; x++)
	{
		if (x <= 20 && x >= 17)
			y = y + 2;
		else if (x == 22 || x == 25 || x == 31)
			y = y + 1;
		else if (x == 42 || x == 39 || x == 33)
			y = y - 1;
		else if (x <= 47 && x >= 44)
			y = y - 2;
		temp = y;
		for (y = 0; y<temp; y++)
		{
			asphalt[y][x][0] = 200;
			asphalt[y][x][1] = 200;
			asphalt[y][x][2] = 200;
			asphalt[y][x][3] = 255;
		}
		for (y = 64 - temp; y<64; y++)
		{
			asphalt[y][x][0] = 200;
			asphalt[y][x][1] = 200;
			asphalt[y][x][2] = 200;
			asphalt[y][x][3] = 255;
		}
		y = temp;
	}
	for (i = 0; i<61; i++)
	{
		for (y = i; y<4 + i; y++)
		{
			for (x = i; x<4 + i; x++)
			{
				asphalt[y][x][0] = 255;
				asphalt[y][x][1] = 255;
				asphalt[y][x][2] = 255;
				asphalt[y][x][3] = 255;
			}
		}
	}
	for (i = 0; i<61; i++)
	{
		for (y = i; y<4 + i; y++)
		{
			for (x = 60 - i; x<64 - i; x++)
			{
				asphalt[y][x][0] = 255;
				asphalt[y][x][1] = 255;
				asphalt[y][x][2] = 255;
				asphalt[y][x][3] = 255;
			}
		}
	}
}

void make_stick()
{
	int  y, x, i;


	for (y = 0; y<12; y++){    /*----Create the first row of sticks----*/
		for (x = 0; x<2; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}

		for (x = 2; x<30; x++){
			stick[y][x][0] = 200;
			stick[y][x][1] = 0;
			stick[y][x][2] = 0;
			stick[y][x][3] = 255;
		}

		for (x = 30; x<34; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}

		for (x = 34; x<62; x++){
			stick[y][x][0] = 200;
			stick[y][x][1] = 0;
			stick[y][x][2] = 0;
			stick[y][x][3] = 255;
		}

		for (x = 62; x<64; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}
	}

	for (y = 12; y<16; y++)     /*--Create the cement between two breick layers */
		for (x = 0; x<64; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}

	for (y = 16; y<28; y++){    /*--Create the 2nd layer of sticks---*/
		for (x = 0; x<14; x++){
			stick[y][x][0] = 200;
			stick[y][x][1] = 0;
			stick[y][x][2] = 0;
			stick[y][x][3] = 255;
		}

		for (x = 14; x<18; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}

		for (x = 18; x<46; x++){
			stick[y][x][0] = 200;
			stick[y][x][1] = 0;
			stick[y][x][2] = 0;
			stick[y][x][3] = 255;
		}

		for (x = 46; x<50; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}

		for (x = 50; x<64; x++){
			stick[y][x][0] = 200;
			stick[y][x][1] = 0;
			stick[y][x][2] = 0;
			stick[y][x][3] = 255;
		}
	}

	for (y = 28; y<32; y++) /*---Add another layer of cement----*/
		for (x = 0; x<64; x++){
			stick[y][x][0] = 140;
			stick[y][x][1] = 140;
			stick[y][x][2] = 140;
			stick[y][x][3] = 255;
		}

	for (y = 32; y<64; y++)
		for (x = 0; x<64; x++)
			for (i = 0; i<4; i++) stick[y][x][i] = stick[y - 32][x][i];
	
}


/*----------------------------------------------------------
* Procedure to initialize the working environment.
*/
void  myinit()
{
	glClearColor(0.0, 0.8, 0.8, 1.0);      /*set the background color BLACK */
	/*Clear the Depth & Color Buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	/*---Create quadratic objects---*/
	if (sphere == NULL){
		sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricNormals(sphere, GLU_FLAT);
	}
	if (cylind == NULL){
		cylind = gluNewQuadric();
		gluQuadricDrawStyle(cylind, GLU_FILL);
		gluQuadricNormals(cylind, GLU_FLAT);
	}
	if (disk == NULL){
		disk = gluNewQuadric();
		gluQuadricDrawStyle(disk, GLU_FILL);
		gluQuadricNormals(disk, GLU_FLAT);
	}

	/*---Create a light source----*/
	glEnable(GL_LIGHTING);

	/*-----Define light0平行光 ---------*/
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lit0_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lit0_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lit0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lit0_position);
	glEnable(GL_LIGHT0);      /*Turn on light0 */

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	/*---Enable normalization of normals----*/
	glEnable(GL_NORMALIZE);

	/*-----Create Asphalt Texture -----*/
	make_asphalt();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(4, textName);
	glBindTexture(GL_TEXTURE_2D, textName[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, asphalt);

	/*-----Create Lawn Texture -----*/
	make_lawn();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	
	glBindTexture(GL_TEXTURE_2D, textName[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, lawn);

	/*-----Create stick Texture -----*/
	make_stick();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	
	glBindTexture(GL_TEXTURE_2D, textName[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, stick);
	/*-----Create Texture -----*/
	make_check();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, textName[3]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, checkboard);

	/*----Set up fog conditions ---*/
	//glEnable(GL_FOG);                /*enable fog fade */
	glFogi(GL_FOG_MODE, GL_LINEAR);  /*fog factor=GL_LINEAR,GL_EXP,or GL_EXP2*/
	glFogf(GL_FOG_DENSITY, 0.05);    /*fog opacity(density)= 0.25*/
	glFogf(GL_FOG_START, 10.0);       /*Setup two ends for GL_LINEAR*/
	glFogf(GL_FOG_END, 80.0);
	glFogfv(GL_FOG_COLOR, fog_color);/*set the fog color */

	

	/*---- Compute cos(5.0) and sin(5.0) ----*/
	cv = cos(5.0*PI / 180.0);
	sv = sin(5.0*PI / 180.0);
	/*---- Copy eye position ---*/
	eye[0] = Eye[0];
	eye[1] = Eye[1];
	eye[2] = Eye[2];
}


/*--------------------------------------------------------
* Procedure to draw a 1x1x1 cube. The cube is within
* (-0.5,-0.5,-0.5) ~ (0.5,0.5,0.5)
*/
void draw_cube()
{
	int    i;	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_SPECULAR);
	glMaterialf(GL_FRONT, GL_SHININESS, 6.0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
		for (i = 0; i < 6; i++){
			glNormal3fv(normals[i]);
			glBegin(GL_POLYGON);  /* Draw the face */
			glVertex3fv(points[face[i][0]]);
			glVertex3fv(points[face[i][1]]);
			glVertex3fv(points[face[i][2]]);
			glVertex3fv(points[face[i][3]]);
			glEnd();
		}
	
	glDisable(GL_COLOR_MATERIAL);
}


void draw_cylinder()
{
	static GLUquadricObj  *cylind = NULL;

	if (cylind == NULL){
		cylind = gluNewQuadric();
		gluQuadricDrawStyle(cylind, GLU_FILL);
		gluQuadricNormals(cylind, GLU_SMOOTH);
	}

	

	gluQuadricTexture(cylind, GL_TRUE);
	//glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(cylind, 1.0, 1.0, 20.0, 12, 3);
	//glPopMatrix();
}

void draw_billboard(float x, float z, float w, float h)
{
	float  v0[3], v1[3], v2[3], v3[3];

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*----Compute the 4 vertices of the billboard ----*/
	v0[0] = x - (w / 2)*a[0]; v0[1] = 0.0; v0[2] = z - (w / 2)*a[2];
	v1[0] = x + (w / 2)*a[0]; v1[1] = 0.0; v1[2] = z + (w / 2)*a[2];
	v2[0] = x + (w / 2)*a[0]; v2[1] = h; v2[2] = z + (w / 2)*a[2];
	v3[0] = x - (w / 2)*a[0]; v3[1] = h; v3[2] = z - (w / 2)*a[2];

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3fv(v0);
	glTexCoord2f(1.0, 0.0); glVertex3fv(v1);
	glTexCoord2f(1.0, 1.0); glVertex3fv(v2);
	glTexCoord2f(0.0, 1.0); glVertex3fv(v3);
	glEnd();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}


/*---------------------------------------------------------
* Procedure to draw the floor.
*/
void draw_floor()
{
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);


	glBindTexture(GL_TEXTURE_2D, textName[0]);
	glColor3f(0.5, 0.5, 0.5);      //馬路
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(15.0, -2.5, 60.0);
	glTexCoord2f(0.0, 10.0);
	glVertex3f(15.0, -2.5, -60.0); 
	glTexCoord2f(10.0, 10.0);
	glVertex3f(-15.0, -2.5, -60.0);
	glTexCoord2f(10.0, 0.0);
	glVertex3f(-15.0, -2.5, 60.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textName[3]);
	draw_billboard(80.0, 30.0, 5.0, 8.0);
	draw_billboard(100.0, 20.0, 5.0, 8.0);
	draw_billboard(70.0, 10.0, 5.0, 8.0);
	draw_billboard(60.0, 20.0, 5.0, 8.0);
	draw_billboard(80.0, -10.0, 5.0, 8.0);
	draw_billboard(70.0, -30.0, 5.0, 8.0);
	draw_billboard(50.0, 0.0, 5.0, 8.0);
	draw_billboard(30.0, -20.0, 5.0, 8.0);

	
	glBindTexture(GL_TEXTURE_2D, textName[1]);
	glColor3f(1.0, 1.0, 1.0);            //草地
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(150.0, -2.5, 60.0);
	glTexCoord2f(0.0, 20.0);
	glVertex3f(150., -2.5, -60.0);
	glTexCoord2f(20.0, 20.0);
	glVertex3f(15.0, -2.5, -60.0);
	glTexCoord2f(20.0, 0.0);
	glVertex3f(15.0, -2.5, 60.0);
	glEnd();

	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(150.0, -2.5, 60.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(150.0, -2.5, -60.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(15.0, -2.5, -60.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(15.0, -2.5, 60.0);
	glEnd();

	glColor3f(0.0, 0.8, 0.0);             //斜坡
	glBegin(GL_POLYGON);
	glNormal3f(1.0, sqrt((double)3), 0.0);
	glVertex3f(-15.0, -2.5, 60.0);
	glVertex3f(-15.0, -2.5, -60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), 37.5, -60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), 37.5, 60.0);
	glEnd();

	glColor3f(0.0, 1.0, 0.0);

	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-15.0, -2.5, 60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), -2.5, 60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), 37.5, 60.0);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-15.0, -2.5, -60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), -2.5, -60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), 37.5, -60.0);
	glEnd();

	glColor3f(0.0, 0.6, 0.0);
	glBegin(GL_POLYGON);
	glNormal3f(-1.0, 0.0, .0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), -2.5, 60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), 37.5, 60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), 37.5, -60.0);
	glVertex3f(-15.0 - (40 * sqrt((double)3)), -2.5, -60.0);
	glEnd();


	for (int i = 0; i > -11; i -= 3){//黃線
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(0.5, -2.4, (i + 6)*10.0);
		glVertex3f(0.5, -2.4, (i + 4)*10.0);
		glVertex3f(-0.5, -2.4, (i + 4)*10.0);
		glVertex3f(-0.5, -2.4, (i + 6)*10.0);
		glEnd();
	}

	glColor3f(1.0, 1.0, 1.0);            //右白線
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(14.0, -2.4, 60.0);
	glVertex3f(14.0, -2.4, -60.0);
	glVertex3f(13.0, -2.4, -60.0);
	glVertex3f(13.0, -2.4, 60.0);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);            //左白線
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-14.0, -2.4, 60.0);
	glVertex3f(-14.0, -2.4, -60.0);
	glVertex3f(-13.0, -2.4, -60.0);
	glVertex3f(-13.0, -2.4, 60.0);
	glEnd();

	glPushMatrix();              //大樓
	glTranslatef(25.0, 12.5, -10.0);
	glScalef(10.0, 30.0, 18.0);
	glColor3f(0.3, 0.3, 0.3);
	draw_cube();
	glPopMatrix();

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	

	int i, j;
	
	for (i = 5; i < 25; i += 3){
		for (j = -6.5; j < 8.5; j += 5){//窗戶
			glColor3f(1.0, 1.0, 0.0);
			glDisable(GL_LIGHTING);
			glBegin(GL_POLYGON);
			glVertex3f(19.9, i, j - 10);
			glVertex3f(19.9, i + 2, j - 10);
			glVertex3f(19.9, i + 2, j - 7);
			glVertex3f(19.9, i, j - 7);
			glEnd();
			glEnable(GL_LIGHTING);
		}
	}

	
	glPushMatrix();       //樹幹
	glTranslatef(25.0, -2.5, 30.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
	glColor3ub(135, 51, 36);
	gluCylinder(cylind, 2.0, 2.0, /* radius of top and bottom lawn */
		5.0,             /* height of the cylinder */
		12,               /* use 12-side polygon approximating lawn*/
		3);               /* Divide it into 3 sections */
	glPopMatrix();

	glPushMatrix();       //樹葉
	glTranslatef(25.0, 2.5, 30.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
	glColor3ub(48, 128, 20);
	gluCylinder(cylind, 7.0, 0.0, /* radius of top and bottom lawn */
		20.0,             /* height of the cylinder */
		12,               /* use 12-side polygon approximating lawn*/
		3);               /* Divide it into 3 sections */
	glPopMatrix();

	glPushMatrix();       //蘋果1
	glTranslatef(30.0, 8.0, 30.0);
	glColor3f(1.0, 0.0, 0.0);
	gluSphere(sphere, 1.5, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();       //蘋果2
	glTranslatef(25.0, 15.0, 33.0);
	glColor3f(1.0, 0.0, 0.0);
	gluSphere(sphere, 1.5, 10.0, 10.0);
	glPopMatrix();

	glPushMatrix();       //蘋果3
	glTranslatef(22.0, 11.0, 27.0);
	glColor3f(1.0, 0.0, 0.0);
	gluSphere(sphere, 1.5, 10.0, 10.0);
	glPopMatrix();

	

	glBindTexture(GL_TEXTURE_2D, textName[2]);
	
	glPushMatrix();       //紅綠燈桿
	
	glTranslatef(-17.0, -2.5, -40.0);
//	glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
	glColor3ub(128, 138, 135);
//	gluCylinder(cylind, 1.0, 1.0, /* radius of top and bottom lawn */
//		20.0,             /* height of the cylinder */
//		12,               /* use 12-side polygon approximating lawn*/
//		3);               /* Divide it into 3 sections */
	draw_cylinder();
	glPopMatrix(); 

	
	glPushMatrix();       //紅綠燈框	
	glTranslatef(-12.0, 15.0, -40.0);
	glScalef(9.0, 4.0, 2.0);
	glColor3f(0.5, 0.5, 0.5);
	draw_cube();

	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glNormal3f(0.5, 0.5, 0.5);
	if (time(NULL) % 10 <= 3){
		glPushMatrix();       //紅滅
		glTranslatef(-15.0, 15.0, -38.85);
		glColor3f(0.3, 0.0, 0.0);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glPopMatrix();
		glPushMatrix();       //黃滅
		glTranslatef(-12.0, 15.0, -38.85);
		glColor3f(0.4, 0.4, 0.0);
		gluDisk(disk, 0.0, 1.0, 26, 13);

		glPopMatrix();
		glPushMatrix();       //綠亮
		glTranslatef(-9.0, 15.0, -38.85);
		glColor3f(0.0, 1.0, 0.0);
		glDisable(GL_LIGHTING);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glEnable(GL_LIGHTING);

		/*-----Define lightG綠燈 ---------*/
		glLightfv(GL_LIGHT1, GL_DIFFUSE, litG_diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, litG_ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, litG_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, litG_position);
		glEnable(GL_LIGHT1);      /*Turn on light0 */

		glPopMatrix();
	}
	else if (time(NULL) % 10 > 3 && time(NULL) % 10 <= 5){
		glPushMatrix();       //紅滅
		glTranslatef(-15.0, 15.0, -38.85);
		glColor3f(0.3, 0.0, 0.0);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glPopMatrix();
		glPushMatrix();       //黃亮
		glTranslatef(-12.0, 15.0, -38.85);
		glColor3f(1.0, 1.0, 0.0);
		glDisable(GL_LIGHTING);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glEnable(GL_LIGHTING);

		/*-----Define lightY黃燈 ---------*/
		glLightfv(GL_LIGHT1, GL_DIFFUSE, litY_diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, litY_ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, litY_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, litY_position);
		glEnable(GL_LIGHT1);      /*Turn on light0 */

		glPopMatrix();
		glPushMatrix();       //綠滅
		glTranslatef(-9.0, 15.0, -38.85);
		glColor3f(0.0, 0.3, 0.0);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glPopMatrix();
	}
	else if (time(NULL) % 10 > 5){
		glPushMatrix();       //紅亮
		glTranslatef(-15.0, 15.0, -38.85);
		glColor3f(1.0, 0.0, 0.0);
		glDisable(GL_LIGHTING);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glEnable(GL_LIGHTING);

		/*-----Define lightR紅燈 ---------*/
		glLightfv(GL_LIGHT1, GL_DIFFUSE, litR_diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, litR_ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, litR_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, litR_position);
		glEnable(GL_LIGHT1);      /*Turn on light0 */

		glPopMatrix();
		glPushMatrix();       //黃滅
		glTranslatef(-12.0, 15.0, -38.85);
		glColor3f(0.4, 0.4, 0.0);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glPopMatrix();
		glPushMatrix();       //綠滅
		glTranslatef(-9.0, 15.0, -38.85);
		glColor3f(0.0, 0.3, 0.0);
		gluDisk(disk, 0.0, 1.0, 26, 13);
		glPopMatrix();
	}

	/*		glPushMatrix();   //天空
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glColor3ub(135, 206, 235);
	gluSphere(sphere, 130.0, 10.0, 10.0);
	glPopMatrix();
	*/
	glPushMatrix();   //地洞!	
	glColor3f(0.0, 0.0, 0.0);
	glTranslatef(110.0, -2.4, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);

	gluDisk(disk, 0.0, 10.0, 100.0, 100.0);
	glPopMatrix();

	glDisable(GL_COLOR_MATERIAL);
}
/*-------------------------------------------------------
* Procedure to draw three axes and the orign
*/
void draw_axes()
{

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	if (light == 1){
		glDisable(GL_LIGHT1);
	}
	else if (light == -1){
		glEnable(GL_LIGHT1);
	}

	/*----Draw a white sphere to represent the original-----*/
	glColor3f(0.9, 0.9, 0.9);

	glTranslatef(0.0, 0.0, 0.0);
	gluSphere(sphere, 2.0,   /* radius=2.0 */
		12,            /* composing of 12 slices*/
		12);           /* composing of 8 stacks */

	/*----Draw three axes in colors, yellow, meginta, and cyan--*/
	/* Draw Z axis  */
	glColor3f(0.0, 0.95, 0.95);
	gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom lawn */
		10.0,              /* height of the cylinder */
		12,               /* use 12-side polygon approximating lawn*/
		3);               /* Divide it into 3 sections */

	/* Draw Y axis */
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
	glColor3f(0.95, 0.0, 0.95);
	gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom lawn */
		10.0,             /* height of the cylinder */
		12,               /* use 12-side polygon approximating lawn*/
		3);               /* Divide it into 3 sections */
	glPopMatrix();

	/* Draw X axis */
	glColor3f(0.95, 0.95, 0.0);
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about y  by 90', x becomes z */
	gluCylinder(cylind, 0.5, 0.5,   /* radius of top and bottom lawn */
		10.0,             /* height of the cylinder */
		12,               /* use 12-side polygon approximating lawn*/
		3);               /* Divide it into 3 sections */
	glPopMatrix();
	/*-- Restore the original modelview matrix --*/
	glPopMatrix();
	
	glDisable(GL_COLOR_MATERIAL);
}


/*------------------------------------------------------
* Procedure to make projection matrix
*/
void make_projection(int x)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (x == 4){
		gluPerspective(90.0 + zoomAngel, (double)width / (double)height, 1.5, 200.0);
	}
	else{
		if (width>height)
			glOrtho(-40.0, 40.0, -40.0*(float)height / (float)width,
			40.0*(float)height / (float)width,
			-0.0 + zoomAngel, 100.0 + zoomAngel);
		else
			glOrtho(-40.0*(float)width / (float)height,
			40.0*(float)width / (float)height, -40.0, 40.0,
			-0.0 + zoomAngel, 100.0 + zoomAngel);
	}
	glMatrixMode(GL_MODELVIEW);
}


/*-------------------------------------------------------
* Display callback func. This func. draws three
* cubes at proper places to simulate a solar system.
*/
void display()
{
	/*Clear previous frame and the depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, width, height);
	gluLookAt(eye[0], eye[1], eye[2],
		eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
		u[1][0], u[1][1], u[1][2]);

	/*----Define Viewing Matrix-----*/
	glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
	compute_ab_axes();

	if (light == 1){
		glClearColor(0.0, 0.8, 0.8, 1.0);
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
	else if (light == -1){
		glDisable(GL_LIGHT0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
	}



	

	make_projection(4);
	draw_scene();


	
	
	/*-------Swap the back buffer to the front --------*/
	glutSwapBuffers();
	return;

}

/*-------------------------------------------------------
* This procedure draw the car
*/
void draw_scene()
{
	static float  ang_self = 0.0;  /*Define the angle of self-rotate */
	static float  angle = 0.0;

	draw_floor();

	draw_axes();

	if (position[0] < -15){
		glTranslatef(position[0], position[1], position[2]);
		glRotatef(-30, 0.0, 0.0, 1.0);

	}
	else{
		position[1] = 0;
		glTranslatef(position[0], position[1], position[2]);
	}

	

}

/*--------------------------------------------------
* Reshape callback function which defines the size
* of the main window when a reshape event occurrs.
*/
void my_reshape(int w, int h)
{

	width = w;
	height = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (double)w / (double)h, 1.5, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*--------------------------------------------------
* Keyboard callback func. When a 'q' key is pressed,
* the program is aborted.
*/
void my_special(int key, int x, int y){
	if (key == GLUT_KEY_UP){
		back_light = -1;//煞車燈滅
		if (position[0] <= 30 + 7.5 && position[0] >= 20 - 7.5 && position[2] <= -1 + 7.5 && position[2] >= -19 - 7.5){//大樓障礙物
			if (position[0] + 1 >= 30 + 7.5)position[0] += 2;
			if (position[0] - 1 <= 20 - 7.5)position[0] -= 2;
			if (position[2] + 1 >= -1 + 7.5)position[2] += 2;
			if (position[2] - 1 <= -19 - 7.5)position[2] -= 2;
			return;
		}
		else if (position[0] <= 27 + 7.5 && position[0] >= 23 - 7.5 && position[2] <= 32 + 7.5 && position[2] >= 28 - 7.5){//樹障礙物
			if (position[0] + 1 >= 27 + 7.5)position[0] += 2;
			if (position[0] - 1 <= 23 - 7.5)position[0] -= 2;
			if (position[2] + 1 >= 32 + 7.5)position[2] += 2;
			if (position[2] - 1 <= 28 - 7.5)position[2] -= 2;
			return;
		}
		else if (position[0] <= -16 + 7.5 && position[0] >= -18 - 7.5 && position[2] <= -39 + 7.5 && position[2] >= -41 - 7.5){//紅綠燈障礙物
			if (position[0] + 1 >= -16 + 7.5)position[0] += 2;
			if (position[0] - 1 <= -18 - 7.5)position[0] -= 2;
			if (position[2] + 1 >= -39 + 7.5)position[2] += 2;
			if (position[2] - 1 <= -41 - 7.5)position[2] -= 2;
			return;
		}
		else if (position[0] <= 120 + 5.5 && position[0] >= 100 - 5.5 && position[2] <= 10 + 5.5 && position[2] >= -10 - 5.5){//地洞!
			if (position[0] + 1 >= 120 + 5.5){ position[0] = 0.0; position[2] = 0.0; }
			if (position[0] - 1 <= 100 - 5.5){ position[0] = 0.0; position[2] = 0.0; }
			if (position[2] + 1 >= 10 + 5.5){ position[0] = 0.0; position[2] = 0.0; }
			if (position[2] - 1 <= -10 - 5.5){ position[0] = 0.0; position[2] = 0.0; }
			return;
		}
		else{
			position[0] -= Step*cos(self_ang*PI / 180.0);
			position[2] += Step*sin(self_ang*PI / 180.0);
			if (position[0] < -15)                               //爬坡
				position[1] = -(position[0] + 15) / sqrt((double)3);
		}
	}

	else if (key == GLUT_KEY_DOWN){
		back_light = 1;//煞車燈亮
		if (position[0] <= 30 + 7.5 && position[0] >= 20 - 7.5 && position[2] <= -1 + 7.5 && position[2] >= -19 - 7.5){  //大樓障礙物
			if (position[0] + 1 >= 30 + 7.5)position[0] += 2;
			if (position[0] - 1 <= 20 - 7.5)position[0] -= 2;
			if (position[2] + 1 >= -1 + 7.5)position[2] += 2;
			if (position[2] - 1 <= -19 - 7.5)position[2] -= 2;
			return;
		}
		else if (position[0] <= 27 + 7.5 && position[0] >= 23 - 7.5 && position[2] <= 32 + 7.5 && position[2] >= 28 - 7.5){//樹障礙物
			if (position[0] + 1 >= 27 + 7.5)position[0] += 2;
			if (position[0] - 1 <= 23 - 7.5)position[0] -= 2;
			if (position[2] + 1 >= 32 + 7.5)position[2] += 2;
			if (position[2] - 1 <= 28 - 7.5)position[2] -= 2;
			return;
		}
		else if (position[0] <= -16 + 7.5 && position[0] >= -18 - 7.5 && position[2] <= -39 + 7.5 && position[2] >= -41 - 7.5){//紅綠燈障礙物
			if (position[0] + 1 >= -16 + 7.5)position[0] += 2;
			if (position[0] - 1 <= -18 - 7.5)position[0] -= 2;
			if (position[2] + 1 >= -39 + 7.5)position[2] += 2;
			if (position[2] - 1 <= -41 - 7.5)position[2] -= 2;
			return;
		}
		else if (position[0] <= 120 + 5.5 && position[0] >= 100 - 5.5 && position[2] <= 10 + 5.5 && position[2] >= -10 - 5.5){//地洞!
			if (position[0] + 1 >= 120 + 5.5){ position[0] = 0.0; position[2] = 0.0; }
			if (position[0] - 1 <= 100 - 5.5){ position[0] = 0.0; position[2] = 0.0; }
			if (position[2] + 1 >= 10 + 5.5){ position[0] = 0.0; position[2] = 0.0; }
			if (position[2] - 1 <= -10 - 5.5){ position[0] = 0.0; position[2] = 0.0; }
			return;
		}
		else{
			position[0] += Step*cos(self_ang*PI / 180.0);
			position[2] -= Step*sin(self_ang*PI / 180.0);
			if (position[0] < -15)                          //爬坡
				position[1] = -(position[0] + 15) / sqrt((double)3);
		}
	}

	else if (key == GLUT_KEY_LEFT){
		self_ang += 5.0;
	}
	else if (key == GLUT_KEY_RIGHT){
		self_ang -= 5.0;
	}

	display();
}

void my_quit(unsigned char key, int ix, int iy)
{
	int    i;
	float  x[3], y[3], z[3];


	if (key == 'R' || key == 'r'){ position[0] = 0; position[1] = 0; position[2] = 0; }  //回原點

	if (key == 'Q' || key == 'q') exit(0);


	else if (key == 'h'){//鏡頭向左
		eyeDx += 0.5;        /* move right */
		for (i = 0; i<3; i++) eye[i] -= 0.5*u[0][i];
	}
	else if (key == 'k'){//鏡頭向右
		eyeDx += -0.5;       /* move left */
		for (i = 0; i<3; i++) eye[i] += 0.5*u[0][i];
	}
	else if (key == 'j'){//鏡頭向下
		eyeDy += 0.5;       /* move up */
		for (i = 0; i<3; i++) eye[i] -= 0.5*u[1][i];
	}
	else if (key == 'u'){//鏡頭向上
		eyeDy += -0.5;       /* move down */
		for (i = 0; i<3; i++) eye[i] += 0.5*u[1][i];
	}
	else if (key == 'm'){
		eyeDz += 0.5;        /* 前進 */
		for (i = 0; i<3; i++) eye[i] -= 0.5*u[2][i];
	}
	else if (key == 'n'){
		eyeDz += -0.5;       /* 後退 */
		for (i = 0; i < 3; i++) eye[i] += 0.5*u[2][i];
	}
	else if (key == '1'){    /* zoom in */
		zoomAngel += 0.5;
	}
	else if (key == '3'){    /* zoom out */
		zoomAngel -= 0.5;
	}
	else if (key == '8'){             /* pitching up */
		eyeAngx += 5.0;
		if (eyeAngx > 360.0) eyeAngx -= 360.0;
		y[0] = u[1][0] * cv + u[2][0] * sv;
		y[1] = u[1][1] * cv + u[2][1] * sv;
		y[2] = u[1][2] * cv + u[2][2] * sv;

		z[0] = u[2][0] * cv - u[1][0] * sv;
		z[1] = u[2][1] * cv - u[1][1] * sv;
		z[2] = u[2][2] * cv - u[1][2] * sv;

		for (i = 0; i<3; i++){
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == '2'){             /* pitching down */
		eyeAngx += 5.0;
		if (eyeAngx > 360.0) eyeAngx -= 360.0;
		y[0] = u[1][0] * cv - u[2][0] * sv;
		y[1] = u[1][1] * cv - u[2][1] * sv;
		y[2] = u[1][2] * cv - u[2][2] * sv;

		z[0] = u[2][0] * cv + u[1][0] * sv;
		z[1] = u[2][1] * cv + u[1][1] * sv;
		z[2] = u[2][2] * cv + u[1][2] * sv;

		for (i = 0; i<3; i++){
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == '4'){            /* heading left*/
		eyeAngy += 5.0;
		if (eyeAngy>360.0) eyeAngy -= 360.0;
		for (i = 0; i<3; i++){
			x[i] = cv*u[0][i] - sv*u[2][i];
			z[i] = sv*u[0][i] + cv*u[2][i];
		}
		for (i = 0; i<3; i++){
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == '6'){            /* heading right*/
		eyeAngy += -5.0;
		if (eyeAngy<0.0) eyeAngy += 360.0;
		for (i = 0; i<3; i++){
			x[i] = cv*u[0][i] + sv*u[2][i];
			z[i] = -sv*u[0][i] + cv*u[2][i];
		}
		for (i = 0; i<3; i++){
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == '7'){            /* rolling */
		eyeAngz += -5.0;
		if (eyeAngz<0.0) eyeAngz += 360.0;
		for (i = 0; i<3; i++){
			x[i] = cv*u[0][i] + sv*u[1][i];
			y[i] = -sv*u[0][i] + cv*u[1][i];
		}
		for (i = 0; i<3; i++){
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
	else if (key == '9'){            /* rolling */
		eyeAngz += 5.0;
		if (eyeAngz>360.0) eyeAngz -= 360.0;
		for (i = 0; i<3; i++){
			x[i] = cv*u[0][i] - sv*u[1][i];
			y[i] = sv*u[0][i] + cv*u[1][i];
		}
		for (i = 0; i<3; i++){
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
	else if (key == 'l'){//車燈控制
		front_light *= -1;
	}
	else if (key == ' '){//太陽開關
		light *= -1;
	}
	else if (key == 'z'){//太陽控制
		lit0_position[0]-=100;
		glLightfv(GL_LIGHT0, GL_POSITION, lit0_position);
	}
	else if (key == 'x'){//太陽控制
		lit0_position[0]+=100;
		glLightfv(GL_LIGHT0, GL_POSITION, lit0_position);
	}
	else if (key == '+'){//車燈cutoff
		lit2_cutoff += 1;
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit2_cutoff);
	}
	else if (key == '-'){//車燈cutoff
		lit2_cutoff -= 1;
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit2_cutoff);
	}
	else if (key == '5'){//MODULATE
		
	}
	else if (key == 'g')
	{
		
		
		fog++;
		if (fog == 0){
			glDisable(GL_FOG);		
		}
		else if (fog == 1){
			glEnable(GL_FOG);
			glFogfv(GL_FOG_COLOR, fog_color);/*set the fog color */
			glFogi(GL_FOG_MODE, GL_LINEAR);	
		}
		else if (fog == 2){
			glFogfv(GL_FOG_COLOR, fog_color);/*set the fog color */
			glFogi(GL_FOG_MODE, GL_EXP);			
		}
		else if (fog == 3){
			glFogfv(GL_FOG_COLOR, fog_color);/*set the fog color */
			glFogi(GL_FOG_MODE, GL_EXP2);
		}
		else if (fog == 4){
			glFogfv(GL_FOG_COLOR, fog_color2);/*set the fog color */
			glFogi(GL_FOG_MODE, GL_LINEAR);		
		}
		if (fog == 4)fog = -1;

	}
	/*fog factor=GL_LINEAR,GL_EXP,or GL_EXP2*/
	

	display();
}

void TimerFunction(int id){     //紅綠燈動畫
	display();
	glutTimerFunc(100, TimerFunction, 1);
}


/*---------------------------------------------------
* Main procedure which defines the graphics environment,
* the operation style, and the callback func's.
*/
void main(int argc, char **argv)
{
	/*-----Initialize the GLUT environment-------*/
	glutInit(&argc, argv);

	/*-----Depth buffer is used, be careful !!!----*/
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(width, height);
	glutCreateWindow("windmill");

	myinit();      /*---Initialize other state varibales----*/

	/*----Associate callback func's whith events------*/
	glutDisplayFunc(display);
	/*  glutIdleFunc(display); */
	glutReshapeFunc(my_reshape);
	glutSpecialFunc(my_special);
	glutKeyboardFunc(my_quit);
	glutTimerFunc(100, TimerFunction, 1);
	

	glutMainLoop();
}

/*------------------------------------------
* Procedure to compute norm of vector v[]
*/
float norm2(float v[])
{
	double  sum;
	float   temp;

	sum = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	temp = (float)sqrt(sum);
	return temp;
}