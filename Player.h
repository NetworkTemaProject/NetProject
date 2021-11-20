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

class CPlayer {

public:
	float angle_turn;
	float x, y, z;
	float dx, dy, dz;
	float angle_swing;
	bool liftend;
	bool fall;
	int m_nScore = 0;

	Part head;
	Part nose;
	Part body;
	Part arm_l, arm_r;
	Part leg_l, leg_r;
	glm::mat4 Turn;
	glm::mat4 Move;
	glm::mat4 Swing_l, Swing_r;

public:
	CPlayer();

	void Update();
	void Locate();
	void Swing();
	void Walk_anim();
	void Jump();
	void Fall();
	void get_angle();
};

//////////////////////////////////////////////////////////////////
#define CLIENT_NUM 2
#define BUFSIZE 256

struct InputData {
	bool bUp = false;
	bool bRight = false;
	bool bLeft = false;
	bool bDown = false;
	bool bSpace = false;
};

struct PlayerMgr {
	DWORD threadId;
	CPlayer player;
	bool Win;
	bool mine = false;
};

struct SendPlayerData {
	InputData Input = {};
	clock_t ClientTime = {};
};