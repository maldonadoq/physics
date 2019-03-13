#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <string>
#include <cstring>
#include <vector>

class planet{
public:
	float m_rd, m_distance, m_orbit;
	float m_orbsd, m_axist, m_axisa;
	char* m_name;
	char* m_velocity;
	vector<unsigned> moons;
	planet(char* _m_name, float _m_ra, float _m_distance, float _m_orbit,
		float _m_orbsp, float _m_axist, float _m_axisa){
		this->m_name = _m_name;
		this->m_rd = _m_ra;
		this->m_distance = _m_distance;
		this->m_orbit = _m_orbit;
		this->m_orbsd = _m_orbsp;
		this->m_axist = _m_axist;
		this->m_axisa = _m_axisa;
		string tmp = to_string(this->m_orbsd/0.027787).substr(0,6)+" Km/h";
		this->m_velocity = new char[tmp.size()+1];
    	memcpy(this->m_velocity, tmp.c_str(), tmp.size()+1);
	}

	void insert_moon(unsigned im){	this->moons.push_back(im);	}

	void moon_orbit(){
		glPushMatrix();
		glColor3ub(255, 255, 255);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glutWireTorus(0.001, this->m_distance, 100.0, 100.0);
		glPopMatrix();
	}

	void moon(){
		GLUquadricObj *quadric;
		quadric = gluNewQuadric();
		glPushMatrix();
		glColor3ub(255, 255, 255);
		glRotatef(this->m_orbit, 0.0, 1.0, 0.0);
		glTranslatef(this->m_distance, 0.0, 0.0);
		gluSphere(quadric, this->m_rd, 20.0, 20.0);
		glPopMatrix();
	}
	~planet(){		};
};