#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <thread>
#include "src/image.h"
#include "src/planet.h"

using namespace std;
using namespace cv;

unsigned const num_planets = 10;
unsigned const num_planets_moons = 16;

//Estrella
planet sol((char*)"Sol",5,0,0,0,0,0);

//Planetas
planet mer((char*)"Mercurio",1.0,7,0,4.79,2.11,0);
planet ven((char*)"Venus",1.5,11,0,3.5,177.0,0);
planet tir((char*)"Tierra",2,16,0,2.98,23.44,0);
planet mar((char*)"Marte",1.2,21,0,2.41,25,0);
planet jup((char*)"Jupiter",3.5,28,0,1.31,3.13,0);
planet sat((char*)"Saturno",3.0,37,0,0.96,26.70,0);
planet ura((char*)"Urano",2.5,45.5,0,0.68,97.77,0);
planet nep((char*)"Neptuno",2.3,53.6,0,0.54,28.32,0);
planet plu((char*)"Pluton",0.3,59,0,0.47,119.6,0);

//Lunas
planet lun((char*)"Luna",0.4,3,0,5.4,0,0);			//Tierra
planet eur((char*)"Europa",0.24,4,0,4.4,0,0);		//Jupiter
planet gan((char*)"Ganymedes",0.24,4.7,0,5,0,0);	//Jupiter
planet cal((char*)"Callisto",0.24,5.3,0,2.3,0,0);	//Jupiter
planet tit((char*)"Titan",0.75,3.7,0,2.4,0,0);		//Saturno
planet tri((char*)"Triton",0.36,3.2,0,3.4,0,0);		//Neptuno

planet Plan[] = {sol,mer,ven,tir,mar,jup,sat,ura,nep,plu,lun,eur,gan,cal,tit,tri};
GLuint Texture[num_planets+1];

float light[] = {0,0,-75,1};
float spdirec[] = {1,0,0};
static float spangle = 40;
static float spexp = 1;

bool draw_orbit = false;
bool planet_name = false;
bool planet_velocity = false;
bool move_planet = true;
int zoom = 50;

void load_moon(){
	Plan[3].insert_moon(10);
	Plan[5].insert_moon(11);
	Plan[5].insert_moon(12);
	Plan[5].insert_moon(13);
	Plan[6].insert_moon(14);
	Plan[8].insert_moon(15);
}

GLuint loadTexture(Image *im){
	GLuint texture_id;
	glGenTextures(1,&texture_id);
	glBindTexture(GL_TEXTURE_2D,texture_id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im->width,im->height,0,GL_RGB,GL_UNSIGNED_BYTE,im->pixels);
	return texture_id;
}

void setup(){
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	Image* tsta = loadBMP("img/stars.bmp");		Texture[0] = loadTexture(tsta);		delete tsta;
	Image* tsol = loadBMP("img/sun.bmp");		Texture[1] = loadTexture(tsol);		delete tsol;
	Image* tmer = loadBMP("img/mercury.bmp");	Texture[2] = loadTexture(tmer);		delete tmer;
	Image* tven = loadBMP("img/venus.bmp");		Texture[3] = loadTexture(tven);		delete tven;
	Image* ttie = loadBMP("img/earth.bmp");		Texture[4] = loadTexture(ttie);		delete ttie;
	Image* tmar = loadBMP("img/mars.bmp");		Texture[5] = loadTexture(tmar);		delete tmar;
	Image* tjup = loadBMP("img/jupiter.bmp");	Texture[6] = loadTexture(tjup);		delete tjup;
	Image* tsat = loadBMP("img/saturn.bmp");	Texture[7] = loadTexture(tsat);		delete tsat;
	Image* tura = loadBMP("img/uranus.bmp");	Texture[8] = loadTexture(tura);		delete tura;
	Image* tnep = loadBMP("img/neptune.bmp");	Texture[9] = loadTexture(tnep);		delete tnep;
	Image* tplu = loadBMP("img/pluto.bmp");	Texture[10] = loadTexture(tplu);		delete tplu;

	glEnable(GL_LIGHTING);
	float light_amb[] = {0,0,0,1};
	float light_dif[] = {1,1,1,1};
	float gl_amb[] = {0.5,0.5,0.5,1};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_dif);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gl_amb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, light);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spangle);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spdirec);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spexp);
}

void planet_trails(void){
	glPushMatrix();
	glColor3ub(255,255,255);
	glRotatef(90,1,0,0);
	for(unsigned i=1; i<num_planets; i++)
		glutWireTorus(0.001,Plan[i].m_distance,100,100);
	glPopMatrix();
}

void draw_scene(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//camera 1
	gluLookAt(0.0, zoom, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	if(draw_orbit) planet_trails();

	GLUquadric *quadric;
	quadric = gluNewQuadric();

	for(unsigned i=0; i<num_planets; i++){
		glPushMatrix();
		glRotatef(Plan[i].m_orbit, 0.0, 1.0, 0.0);
		glTranslatef(Plan[i].m_distance, 0.0, 0.0);
		if(planet_name){
			glRasterPos3f(-1.2, 7.0, 0.0);
			glColor3ub(255, 255, 255);
			writeBitmapString(GLUT_BITMAP_HELVETICA_12, Plan[i].m_name);
		}
		else if(planet_velocity){
			glRasterPos3f(-1.2, 7.0, 0.0);
			glColor3ub(255, 255, 255);
			writeBitmapString(GLUT_BITMAP_HELVETICA_12, Plan[i].m_velocity);	
		}
		glPushMatrix();
		glRotatef(Plan[i].m_axist, 1.0, 0.0, 0.0);
		glRotatef(Plan[i].m_axisa, 0.0, 1.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Texture[i+1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		gluQuadricTexture(quadric, 1);
		gluSphere(quadric, Plan[i].m_rd, 20.0, 20.0);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		for(unsigned j=0; j<Plan[i].moons.size(); j++){
			Plan[Plan[i].moons[j]].moon_orbit();
			Plan[Plan[i].moons[j]].moon();
		}
		glPopMatrix();
	}

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_POLYGON);
	glTexCoord2f(-1.0, 0.0); glVertex3f(-200, -200, -100);
	glTexCoord2f(2.0, 0.0); glVertex3f(200, -200, -100);
	glTexCoord2f(2.0, 2.0); glVertex3f(200, 200, -100);
	glTexCoord2f(-1.0, 2.0); glVertex3f(-200, 200, -100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-200, -83, 200);
	glTexCoord2f(8.0, 0.0); glVertex3f(200, -83, 200);
	glTexCoord2f(8.0, 8.0); glVertex3f(200, -83, -200);
	glTexCoord2f(0.0, 8.0); glVertex3f(-200, -83, -200);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glutSwapBuffers();
}

void resize(int _w, int _h){
	glViewport(0, 0, _w, _h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
}

void planet_moving(int n){
	if(move_planet){
		for(unsigned i=1; i<num_planets_moons; i++){
			Plan[i].m_orbit += Plan[i].m_orbsd;
			if(Plan[i].m_orbit > 360.0)	Plan[i].m_orbit -= 360.0;

			if(i<num_planets){
				Plan[i].m_axisa += 10.0;
				if(Plan[i].m_axisa > 360.0)	Plan[i].m_axisa -= 360.0;
			}
		}

		glutPostRedisplay();
		glutTimerFunc(30, planet_moving, 1);
	}
}

void key(unsigned char key, int x, int y){
	switch (key){
		case 27: exit(0); break;
		case 's': 
			if(move_planet) move_planet = false;
			else{	move_planet = true; planet_moving(1);	}
			break;
		case 'o': 
			if(draw_orbit) draw_orbit = false;
			else{	draw_orbit = true; glutPostRedisplay();	}
			break;
		case 'l': 
			planet_velocity = false;
			if(planet_name) planet_name = false;
			else{	planet_name = true; glutPostRedisplay();	}
			break;
		case 'v': 
			planet_name = false;
			if(planet_velocity) planet_velocity = false;
			else{	planet_velocity = true; glutPostRedisplay();	}
			break;
	}
}

void draw(){
	draw_scene();
}

void intructions(void){
	cout << "[s]\t start|stop" << endl;
	cout << "[o]\t orbita" << endl;
	cout << "[v]\t velocidad" << endl;
	cout << "[l]\t nombre" << endl;
	cout << "[esc]\t salir!" << endl;
}

void info(){
	Mat src = imread("img/info.png", CV_LOAD_IMAGE_UNCHANGED);
	if (src.empty()){
	  	cout << "Error : Image cannot be loaded..!!" << endl;
		return;
	}

	namedWindow("window", CV_WINDOW_NORMAL);
	imshow("window", src);
	waitKey(0);
	destroyWindow("window");
}

int main(int argc, char *argv[]){
	load_moon();
	intructions();
	thread a(&info);
	glutInit(&argc, argv);

	//glutInitContextVersion(4, 2);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000, 500);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Sistema Solar");
	glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutKeyboardFunc(key);
	glewInit();

	setup();
	glutMainLoop();

	a.join();
	return 0;
}

//((cos(exp(x*y))*exp(x*y)*(y+(z*x))*((2*y)-(x*cos(exp(x*y)))))-(sin(exp(x*y))*((2*z)-cos(exp(x*y))+(x*sin(exp(x*y))))*exp(x*y)*(y+(z*x))))/power((2*y)-(x*cos(exp(x*y))),2)