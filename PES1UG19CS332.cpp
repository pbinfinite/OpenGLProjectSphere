#include<windows.h>
#include<random>
#include<gl/glut.h>
#include<cstdlib>
#include<math.h>
#define DEF_D 5.0
#define PI 3.14
#define cosd(k) cos(PI/180*(k))
#define sind(k) sin(PI/180*(k))

double rad;

float theta = 0;
int rotAngle = 0;
float camx = 0, camy = 0, camz = 0;
float deltaAngle = 0.0f;
float xold = -1;
double r = 15.0;

int isten = 0;
int i, j;
int conv = -1;
int justpaused = 0;

float rx, ry, rz;
float tx, ty, tz;

/*translation, rotation, scaling coordinates of all 3 objects*/
float cdx[3], cdy[3], cdz[3];
int rdx[3], rdy[3], rdz[3];
float sclx[3], scly[3], sclz[3];

int clickx, clicky;

int selectWire = 0, wcount = 0;
int scalecount = 0;

int rchoice, tchoice;
int negt, negtcoll = 0;
float dist;
float distimer[3];
int rotspd = 0;


/*function to find distance from origin to that point*/
float dist3d(float x, float y, float z) {
	float s = float(sqrtf(float(x * x) + float(y * y) + float(z * z)));
	return s;
}

/*To handle collision with walls of sphere*/
void maxt() {
	float n1 = fabs(cdx[i]);
	float n2 = fabs(cdy[i]);
	float n3 = fabs(cdz[i]);

	if (n1 >= n2 && n1 >= n3) {
		if (tx > 0) {
			tx = tx - 3;
		}
		else if (tx < 0) {
			tx = tx + 3;
		}
	}

	if (n2 >= n1 && n2 >= n3) {
		if (ty > 0) {
			ty = ty - 3;
		}
		else if (ty < 0) {
			ty = ty + 3;
		}
	}

	if (n3 >= n1 && n3 >= n2) {
		if (tz > 0) {
			tz = tz - 3;
		}
		else if (tz < 0) {
			tz = tz + 3;
		}
	}
}

/*To set random translation coordinate values of objects*/
void setcd() {
	if (conv == -1) {
		cdx[0] = tx; cdy[0] = ty; cdz[0] = tz;
		cdx[1] = ty + 1; cdy[1] = tx - 1; cdz[1] = tz + 1;
		cdx[2] = tz - 1; cdy[2] = tx + 1; cdz[2] = ty - 1;
		return;
	}
	if (conv != 1) {
		cdx[0] = tx; cdy[0] = ty; cdz[0] = tz;
	}
	if (conv != 2) {
		cdx[1] = ty + 1; cdy[1] = tx - 1; cdz[1] = tz + 1;
	}
	if (conv != 3) {
		cdx[2] = tz - 1; cdy[2] = tx + 1; cdz[2] = ty - 1;
	}
}

/*To generate random axis of rotation*/
void randRot() {
	rchoice = rand() % 3;
	if (rchoice == 0) {
		rx = 1; ry = 0; rz = 0;
	}
	else if (rchoice == 1) {
		rx = 0; ry = 1; rz = 0;
	}
	else if (rchoice == 2) {
		rx = 0; ry = 0; rz = 1;
	}
}

/*To set rotation coordinates (x,y,z) of objects*/
void setRot() {
	rdx[0] = rx; rdy[0] = ry; rdz[0] = rz;
	rdx[1] = ry; rdy[1] = rz; rdz[1] = rx;
	rdx[2] = rz; rdy[2] = rx; rdz[2] = ry;
}

/*To generate random coordinate values for translation of objects*/
void randTransl() {
	tx = (rand() % 13);
	ty = (rand() % 13);
	tz = (rand() % 13);
	negt = rand() % 2;
	if (negt == 1) {
		tx = tx * -1;
		ty = ty * -1;
		tz = tz * -1;
	}
	tchoice = rand() % 3;

	if (tchoice == 0) tx = 0;
	if (tchoice == 1) ty = 0;
	if (tchoice == 2) tz = 0;
	dist = dist3d(tx, ty, tz);
	if (dist > 10.5) {
		randTransl();
	}
}

/*To handle collision of objects with walls of sphere*/
void randInter() {
	negt = rand() % 2;
	if (negt == 0) {
		//positive
		tx = tx + 0.5;
		ty = ty + 0.5;
		tz = tz + 0.5;
		dist = dist3d(tx, ty, tz);
		if (dist > 10.5) {
			negt = 1;
			maxt();
		}
	}
	else if (negt == 1) {
		tx = tx - 0.5;
		ty = ty - 0.5;
		tz = tz - 0.5;
		dist = dist3d(tx, ty, tz);
		if (dist > 10.5) {
			negt = 0;
			maxt();
		}
	}
}

/*To handle object-object collision*/
void collisiond() {
	if ((abs(cdx[0] - cdx[1]) <= 1.0) && (abs(cdy[0] - cdy[1]) <= 1.0) && (abs(cdz[0] - cdz[1]) <= 1.0)) {
		cdx[1] = cdx[1] - 0.5; cdy[1] = cdy[1] - 0.5; cdz[1] = cdz[1] - 0.5;
		cdx[0] = cdx[0] + 0.5; cdy[0] = cdy[0] + 0.5; cdz[0] = cdz[0] + 0.5;
	}

	if ((abs(cdx[1] - cdx[2]) <= 1.0) && (abs(cdy[1] - cdy[2]) <= 1.0) && (abs(cdz[1] - cdz[2]) <= 1.0)) {
		cdx[1] = cdx[1] - 0.5; cdy[1] = cdy[1] - 0.5; cdz[1] = cdz[1] - 0.5;
		cdx[2] = cdx[2] + 0.5; cdy[2] = cdy[2] + 0.5; cdz[2] = cdz[2] + 0.5;
	}

	if ((abs(cdx[0] - cdx[2]) <= 1.0) && (abs(cdy[0] - cdy[2]) <= 1.0) && (abs(cdz[0] - cdz[2]) <= 1.0)) {
		cdx[2] = cdx[2] - 0.5; cdy[2] = cdy[2] - 0.5; cdz[2] = cdz[2] - 0.5;
		cdx[0] = cdx[0] + 0.5; cdy[0] = cdy[0] + 0.5; cdz[0] = cdz[0] + 0.5;
	}
	if ((cdx[0] == cdx[1]) || (cdx[1] == cdx[2]) || (cdx[2] == cdx[0])) {

		if (negtcoll == 0) {
			cdx[0] = cdx[0] - 0.5;
			cdx[1] = cdx[1] - 0.5;
			cdx[2] = cdx[2] - 0.5;
		}
		if (negtcoll == 1) {
			cdx[0] = cdx[0] + 0.5;
			cdx[1] = cdx[1] + 0.5;
			cdx[2] = cdx[2] + 0.5;
		}
	}
}

/*3d model of a cone using triangles and random colour for every vertex*/
void mcone() {
	int k;
	glBegin(GL_TRIANGLES);
	for (k = 0; k <= 360; k = k + DEF_D) {
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(0, 0, 1);
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(cosd(k), sind(k), 0);
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(cosd(k + DEF_D), sind(k + DEF_D), 0);
	}
	glEnd();

	glRotatef(90, 1, 0, 0);
	glBegin(GL_TRIANGLES);
	for (k = 0; k <= 360; k += DEF_D) {
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(0, 0, 0);
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(cosd(k), 0, sind(k));
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(cosd(k + DEF_D), 0, sind(k + DEF_D));
	}
	glEnd();
	glFlush();
}

/*3d model of cylinder using triangles and random colours for every vertex*/
void mcylinder() {
	int i, j, k;
	glBegin(GL_QUAD_STRIP);
	for (j = 0; j <= 360; j = j + DEF_D) {
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(cosd(j), 1, sind(j));
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(cosd(j), -1, sind(j));
	}
	glEnd();

	//top and bottom circles
	for (i = 1; i >= -1; i = i - 2) {
		glBegin(GL_TRIANGLE_FAN);
		glColor3ub(rand() % 255, rand() % 255, rand() % 255);
		glVertex3f(0, i, 0);
		for (k = 0; k <= 360; k += DEF_D) {
			glColor3ub(rand() % 255, rand() % 255, rand() % 255);
			glVertex3f(i * cosd(k), i, sind(k));
		}
		glEnd();
	}
	glFlush();
}

/*3d model of tetrahedron and random colours for every vertex*/
void mtetrahedron() {
	glBegin(GL_TRIANGLE_STRIP);
	glColor3ub(rand() % 255, rand() % 255, rand() % 255); glVertex3f(0, 2, 0);
	glColor3ub(rand() % 255, rand() % 255, rand() % 255); glVertex3f(-1, 0, 1);
	glColor3ub(rand() % 255, rand() % 255, rand() % 255); glVertex3f(1, 0, 1);
	glColor3ub(rand() % 255, rand() % 255, rand() % 255); glVertex3f(0, 0, -1.4);
	glColor3ub(rand() % 255, rand() % 255, rand() % 255); glVertex3f(0, 2, 0);
	glColor3ub(rand() % 255, rand() % 255, rand() % 255); glVertex3f(-1, 0, 1);
	glEnd();

	glFlush();
}

/*Timer call back function*/
void timerCallBack() {
	rotAngle++;
	rotspd++;
	if (rotspd == 3) {
		randRot();
		setRot();
	}
	if (negt == 0) {
		if (tchoice == 0) {
			tx = tx + 0.5;
		}
		if (tchoice == 1) {
			ty = ty + 0.5;
		}
		if (tchoice == 2) {
			tz = tz + 0.5;
		}
	}
	else {
		if (tchoice == 0) {
			tx = tx - 0.5;
		}
		if (tchoice == 1) {
			ty = ty - 0.5;
		}
		if (tchoice == 2) {
			tz = tz - 0.5;
		}
	}

	dist = dist3d(tx, ty, tz);

	if (dist > 10.5) {
		randInter();
		tchoice = rand() % 3;
	}
	else if (tx > 10.5 || ty > 10.5 || tz > 10.5) {
		randInter();
		tchoice = rand() % 3;
	}

	setcd();
	//collision detection
	if (conv == -1) {
		collisiond();
		justpaused = 0;
	}

	if (justpaused != 0 && conv != -1) {
		cdx[justpaused - 1] = cdx[justpaused - 1] + 0.5;
		if (dist3d(cdx[justpaused - 1], cdy[justpaused - 1], cdz[justpaused - 1]) > 11.0) {
			if (cdx[justpaused - 1] >= 0) {
				cdx[justpaused - 1] = cdx[justpaused - 1] - 3;
			}
			else {
				cdx[justpaused - 1] = cdx[justpaused - 1] + 3;
			}
		}
	}
	Sleep(90);
	glutPostRedisplay();
}

/*To set camera coordinate (x,y,z) values*/
void setCam() {
	camx = r * cos(rad);
	camy = 0.0;
	camz = r * sin(rad);
}

/*To set material of sphere*/
void setMaterial(float ar, float ag, float ab,
	float dr, float dg, float db,
	float sr, float sg, float sb, float shine)
{
	float ambient[] = { ar,ag,ab };
	float diffuse[] = { dr,dg,db };
	float specular[] = { sr,sg,sb };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	glMaterialf(GL_FRONT, GL_SHININESS, shine);

}

/*Display function: Loads all models,sphere and environment*/
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camx, camy, camz, 0, 0, 0, 0, 1, 0);

	isten = 0;
	glPushMatrix();
	glColor3f(0.5, 0.5, 0.8);
	if (selectWire == 0) {
		setMaterial(0.2, -0.5, -5.5, 0.19225, 0.19225, 0.50754, 0.50754, 0.508273, 0.508273, 50.0);
		glutSolidSphere(15, 100, 100);
	}
	else if (selectWire == 1) {
		glutWireSphere(15, 100, 100);
	}
	glPopMatrix();

	glPushMatrix();
	glStencilFunc(GL_ALWAYS, isten + 1, -1);
	glScalef(sclx[0], scly[0], sclz[0]);
	glTranslatef(cdx[0], cdy[0], cdz[0]);
	glRotatef(rotAngle, rdx[0], rdy[0], rdz[0]);
	mcone();
	glPopMatrix();

	isten = isten + 1;
	glPushMatrix();
	glStencilFunc(GL_ALWAYS, isten + 1, -1);
	glScalef(sclx[1], scly[1], sclz[1]);
	glTranslatef(cdx[1], cdy[1], cdz[1]);
	glRotatef(rotAngle, rdx[1], rdy[1], rdz[1]);
	mcylinder();
	glPopMatrix();

	isten = isten + 1;
	glPushMatrix();
	glStencilFunc(GL_ALWAYS, isten + 1, -1);
	glScalef(sclx[2], scly[2], sclz[2]);
	glTranslatef(cdx[2], cdy[2], cdz[2]);
	glRotatef(rotAngle, rdx[2], rdy[2], rdz[2]);
	mtetrahedron();
	glPopMatrix();


	glutSwapBuffers();
	glFlush();
}

/*Initialize all lighting*/
void initLight() {

	float ambient0[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float ambient1[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	float diffuse0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuse1[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	float specular0[] = { 0.1f, 0.5f, 0.1f, 1.0f };
	float specular1[] = { 0.5f, 0.1f,0.1f, 0.8f };

	float position0[] = { -2,0.5,4,1 };
	float position1[] = { 0, 13, 0, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//tell opengl to allow us to change color of material
	glEnable(GL_COLOR_MATERIAL);
}

/*Keyboard function*/
void myKeyboard(unsigned char key, int x, int y) {
	unsigned char index = 0;
	switch (key) {
	case 'x': {
		rotspd = 0;
		rdx[conv - 1] = 1; rdy[conv - 1] = 0; rdz[conv - 1] = 0;
	}
			break;
	case 'y': {
		rotspd = 0;
		rdx[conv - 1] = 0; rdy[conv - 1] = 1; rdz[conv - 1] = 0;
	}
			break;
	case 'z': {
		rotspd = 0;
		rdx[conv - 1] = 0; rdy[conv - 1] = 0; rdz[conv - 1] = 1;
	}
			break;
	case '+': {
		scalecount++;
		if (scalecount >= 3) {
			scalecount = 3;
			break;
		}
		sclx[conv - 1] = 1.25 * sclx[conv - 1];
		scly[conv - 1] = 1.25 * scly[conv - 1];
		sclz[conv - 1] = 1.25 * sclz[conv - 1];
	}
			break;
	case '-': {
		scalecount--;
		if (scalecount <= -3) {
			scalecount = -3;
			break;
		}
		sclx[conv - 1] = sclx[conv - 1] / 1.25;
		scly[conv - 1] = scly[conv - 1] / 1.25;
		sclz[conv - 1] = sclz[conv - 1] / 1.25;
	}
			break;
	case 'w': {
		wcount++;
		if (wcount == 1) {
			r = 23;
			setCam();
			selectWire = 1;
		}
		else if (wcount == 2) {
			wcount = 0;
			r = 15;
			setCam();
			selectWire = 0;
		}
	}
			break;
	case 'c': {
		justpaused = conv;
		conv = -1;
	}
			break;
	default:break;
	}
	Sleep(120);
	glutPostRedisplay();
}

/*Function to recognize and act on arrow keys in keyboard*/
void arrowFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: {
		if (cdy[conv - 1] < 10.0 && dist3d(cdx[conv - 1], cdy[conv - 1], cdz[conv - 1]) < 10.0) {
			cdy[conv - 1] = cdy[conv - 1] + 0.5;
		}
		else {

		}
	}
					break;
	case GLUT_KEY_DOWN: {
		if (cdy[conv - 1] < 10.0 && dist3d(cdx[conv - 1], cdy[conv - 1], cdz[conv - 1]) < 10.0) {
			cdy[conv - 1] = cdy[conv - 1] - 0.5;
		}
	}
					  break;
	case GLUT_KEY_RIGHT: {
		if (cdx[conv - 1] < 10.0 && dist3d(cdx[conv - 1], cdy[conv - 1], cdz[conv - 1]) < 10.0) {
			cdx[conv - 1] = cdx[conv - 1] + 0.5;
		}
	}
					   break;
	case GLUT_KEY_LEFT: {
		if (cdx[conv - 1] < 10.0 && dist3d(cdx[conv - 1], cdy[conv - 1], cdz[conv - 1]) < 10.0) {
			cdx[conv - 1] = cdx[conv - 1] - 0.5;
		}
	}
					  break;
	default:break;
	}
}

/*To handle mouse clicks*/
void mouseButton(int button, int state, int x, int y) {

	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_UP) {
			theta = theta + deltaAngle;
			xold = -1;
		}
		else {
			xold = x;
		}
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		unsigned char index = 0;
		glReadPixels(x, 640 - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &index);
		clickx = x; clicky = y;
		conv = index;
	}
}

/*To handle camera rotation using right button click of the mouse*/
void mouseMove(int x, int y) {

	if (xold >= 0) {
		deltaAngle = (x - xold) * 0.01f;

		camx = r * cos(theta + deltaAngle);
		camz = r * sin(theta + deltaAngle);
	}
	glutPostRedisplay();
}

/*To handle window resize*/
void resizew(int w, int h) {
	if (!h) {
		h = 1;
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, 1.0 * w / h, 0.1, 100);
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);

	glutCreateWindow("OpenGL Assignment - PES1UG19CS332");
	glClearColor(0, 0, 0, 0);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glCullFace(GL_FRONT_AND_BACK);
	glClearStencil(0);

	initLight();

	/*calculate position of camera*/
	rad = theta * PI / 180;
	camx = r * cos(rad);
	camy = 0.0;
	camz = r * sin(rad);

	for (j = 0; j < 3; j++) {
		sclx[j] = 1;
		scly[j] = 1;
		sclz[j] = 1;
	}

	//change axis of rot after collision
	randRot();
	randTransl();

	setcd();
	setRot();

	PlaySound(TEXT("fvarbgmusic.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	glutDisplayFunc(display);
	glutReshapeFunc(resizew);

	glutIdleFunc(timerCallBack);


	//keyboard
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(arrowFunc);

	//mouse 
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.1, 100);

	glutMainLoop();

}