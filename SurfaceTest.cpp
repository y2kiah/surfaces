//	----==== SURFACETEST.CPP ====----
//
//	Author:			Jeff Kiah
//					y2kiah@hotmail.com
//	Version:		1
//	Date:			6/04
//	Description:	Tests bi-cubic patches
//	-------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN		// this keeps MFC (Microsoft Foundation Classes) from being included

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <time.h>
#include "utilitycode/keyboardmanager.h"
#include "utilitycode/mousemanager.h"
#include "mathcode/spline.h"
#include "mathcode/vector3.h"
#include "utilitycode/glfont.h"


/*---------------
---- DEFINES ----
---------------*/

#define SUBDIVISIONS	10
#define POINTSPERSIDE	8
#define HORZSCALE		2
#define VERTSCALE		1


/*-----------------
---- VARIABLES ----
-----------------*/

extern HDC		hDC;
extern GLFont	*font;

float	heights[POINTSPERSIDE*POINTSPERSIDE];
float	rotateX = 0, rotateY = 0;

bool	drawWireframe = false;


/*-----------------
---- FUNCTIONS ----
-----------------*/


void renderSurface(void)
{
	glColor3f(0,0,0);
	glPointSize(2.5f);

	float hPtr[16];
	float tStep = 1.0f / SUBDIVISIONS;

	for (int z = 0; z < POINTSPERSIDE-3; z++) {

		for (int x = 0; x < POINTSPERSIDE-3; x++) {
			
			// store patch heights in array
			for (int h = 0; h < 4; h++) {
				hPtr[h*4]   = heights[(z+h)*POINTSPERSIDE+x];
				hPtr[h*4+1] = heights[(z+h)*POINTSPERSIDE+x+1];
				hPtr[h*4+2] = heights[(z+h)*POINTSPERSIDE+x+2];
				hPtr[h*4+3] = heights[(z+h)*POINTSPERSIDE+x+3];
			}

			CubicBSpline::preCalcMiddleMatrix(hPtr);
			//CatmullRomSpline::setSplineMatrix(0,0,hPtr,4);

			glBegin(GL_POINTS);

			float v = 0;
			for (int j = 0; j <= SUBDIVISIONS; j++) {
				
				float u = 0;
				for (int i = 0; i <= SUBDIVISIONS; i++) {
					Vector3 p(	x*6 + u*6 - 15,
								CubicBSpline::calcHeightOnPatchMatrix(u,v),
								z*6 + v*6 - 15);

					glColor3f(0,0,0);
					glVertex3fv(p.v);
								
					glColor3f(1,0,0);
					Vector3 n(p);
					n += CubicBSpline::calcNormalOnPatchMatrix(u,v,6);					
					glVertex3fv(n.v);

					u += tStep;
				}

				v += tStep;
			}
			glEnd();			
		}
	}
}


void initSurfacePoints(void)
{
	srand(time(NULL) + (rand() % 100));

	for (int c = 0; c < POINTSPERSIDE*POINTSPERSIDE; c++) {
		heights[c] = (rand() % 12) - 6.0f;
	}
}


void drawSurfaceControlPoints(void)
{
	glPointSize(4.0f);
	glColor3f(0,0,1);
	for (int z = 0; z < POINTSPERSIDE; z++) {
		for (int x = 0; x < POINTSPERSIDE; x++) {
			int index = z * POINTSPERSIDE + x;
			glBegin(GL_POINTS);
				glVertex3f(GLfloat(x*6-15-6), heights[index], GLfloat(z*6-15-6));
			glEnd();
		}
	}
}


void renderScene(void)
{
	// Perform a page flip immediately prior to doing all rendering for best performance
	SwapBuffers(hDC);

	// Clear the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// handle keyboard input
	if (kb.buttonPressed('1')) drawWireframe = !drawWireframe;

	// handle mouse movement
	mouse.updateMousePosition();

	// reset the modelview matrix
	glLoadIdentity();

	// translate the view in the -z direction (straight back away from the screen)
	// so we can see the scene
	glTranslatef(0,0,-50);

	// rotate the scene
	if (mouse.leftButtonDown()) {
		rotateX += mouse.getSensitiveDeltaY();
		rotateY += mouse.getSensitiveDeltaX();
	}

	glRotatef(rotateX,1,0,0);
	glRotatef(rotateY,0,1,0);

	// Draw the scene
	drawSurfaceControlPoints();
	renderSurface();

	// Write text
	glColor3f(0,0,0);
	if (drawWireframe)
		font->print(10,50, "<1> Render Mode WIREFRAME");
	else
		font->print(10,50, "<1> Render Mode FILL");

	font->print(10,64, "<ENTER> Recalculate Points");
	font->print(10,78, "<LEFT MOUSE BUTTON> Rotate Scene");
}
