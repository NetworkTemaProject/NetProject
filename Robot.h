#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Part {
	glm::mat4 T1, T2;
	glm::mat4 R;
	glm::mat4 S;
	glm::mat4 TRS;
	float r, g, b;

	void makePart(int type, bool leftPart);
};

struct Robot {
	float angle_turn;
	float x, y, z;
	float dx, dy, dz;
	float angle_swing;
	bool liftend;
	bool fall;

	Part head;
	Part nose;
	Part body;
	Part arm_l, arm_r;
	Part leg_l, leg_r;
	glm::mat4 Turn;
	glm::mat4 Move;
	glm::mat4 Swing_l, Swing_r;

	Robot();

	void Update();
	void Locate();
	void Swing();
	void Walk_anim();
	void Jump();
	void Fall();
	void get_angle();
};