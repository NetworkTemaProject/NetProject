#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <random>
#include <time.h>
#include <iostream>
using namespace std;

#define foothold_sizex 0.8f
#define foothold_sizey 0.3f
#define foothold_sizez 0.8f
#define N 5
#define foothold_vertex 36
#define MAX 32767.0f			// 실수 rand() 위한 변수값
#define UNDER -23.0f

class Foothold {
public:
	float mx, my, mz;
	float cx, cy, cz;
	float rx, ry, rz;
	float r, g, b;
	int model, color_location;
	int size;
	int ani;
	float theta;
	int score;
	int cnt;

	glm::mat4 Move;
	glm::mat4 Scale;
	glm::mat4 Rotate;
	glm::mat4 Drawing;
	bool Del = FALSE;
	bool startDel = false;

public:
	Foothold(float x, float y, float z, float R, float G, float B) :
		mx{ x }, my{ y }, mz{ z },
		cx{ foothold_sizex }, cy{ foothold_sizey }, cz{ foothold_sizez },
		r{ R }, g{ G }, b{ B }, size{ foothold_vertex },
		rx{ FALSE }, ry{ FALSE }, rz{ FALSE }, theta{ 0 }, cnt{ 0 }{
			ani = rand() % 5;

			if (my < 0)
				// 층 낮아질수록 score 값 증가
				score = int(10 * -my);
			else
				score = int(10 * my);
	}

	~Foothold(){}
	void Init();
	void Pos_Drawing();
	void Draw_Start();
	void Delete();
};

void MakeFoothold(vector<Foothold> &Bottom);
void DeleteRandomFoothold(vector<Foothold>& Bottom);