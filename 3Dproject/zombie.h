#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "zombie.h"
/*---- the axes of billboard ----*/
extern  a[3], b[3];
extern    mtx[16];     /* the modelview matrix */
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
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textName[0]);

	draw_billboard(5.0*4.0, 3.0*4.0, 5.0, 8.0);
	draw_billboard(6.0*4.0, 5.0*4.0, 5.0, 8.0);
	draw_billboard(3.0*4.0, 6.0*4.0, 5.0, 8.0);
	draw_billboard(2.0*4.0, 7.0*4.0, 5.0, 8.0);
	draw_billboard(7.0*4.0, 2.0*4.0, 5.0, 8.0);

	glDisable(GL_TEXTURE_2D);
}

#endif 