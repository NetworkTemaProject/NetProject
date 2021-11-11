#include "Player.h"

void Part::makePart(int type, bool leftPart)
{
	T1 = glm::mat4(1.0f);
	T2 = glm::mat4(1.0f);
	R = glm::mat4(1.0f);
	S = glm::mat4(1.0f);
	TRS = glm::mat4(1.0f);
	switch (type) {
	case 1:	// head
	{
		r = 1.0; g = 1.0; b = 1.0;
		S = glm::scale(S, glm::vec3(0.2, 0.2, 0.2));
		T2 = glm::translate(T2, glm::vec3(0.0f, 0.4f, 0.0f));
		break;
	}
	case 2:	// nose
	{
		r = 0.0; g = 0.0; b = 0.0;
		S = glm::scale(S, glm::vec3(0.05, 0.05, 0.05));
		T2 = glm::translate(T2, glm::vec3(0.0f, 0.5f, 0.1f));
		break;
	}
	case 3:	// body
	{
		r = 0.0; g = 1.0; b = 0.0;
		S = glm::scale(S, glm::vec3(0.3, 0.2, 0.3));
		T2 = glm::translate(T2, glm::vec3(0.0f, 0.2f, 0.0f));
		break;
	}
	case 4:	// arm
	{
		r = 1.0; g = 0.0; b = 0.0;
		S = glm::scale(S, glm::vec3(0.1, 0.2, 0.1));
		if (leftPart) {
			T1 = glm::translate(T1, glm::vec3(0.0f, -0.2f, 0.0f));
			T2 = glm::translate(T2, glm::vec3(-0.2f, 0.4f, 0.0f));
		}
		else {
			T1 = glm::translate(T1, glm::vec3(0.0f, -0.2f, 0.0f));
			T2 = glm::translate(T2, glm::vec3(0.2f, 0.4f, 0.0f));
		}
		break;
	}
	case 5:	// leg
	{
		r = 0.0; g = 0.0; b = 1.0;
		S = glm::scale(S, glm::vec3(0.1, 0.2, 0.1));
		if (leftPart) {
			T1 = glm::translate(T1, glm::vec3(0.0f, -0.2f, 0.0f));
			T2 = glm::translate(T2, glm::vec3(-0.1f, 0.2f, 0.0f));
		}
		else {
			T1 = glm::translate(T1, glm::vec3(0.0f, -0.2f, 0.0f));
			T2 = glm::translate(T2, glm::vec3(0.1f, 0.2f, 0.0f));
		}
		break;
	}
	}
}

CPlayer::CPlayer()
{
	head.makePart(1, true);
	nose.makePart(2, true);
	body.makePart(3, true);
	arm_l.makePart(4, true);
	arm_r.makePart(4, false);
	leg_l.makePart(5, true);
	leg_r.makePart(5, false);

	x = 0, y = 0, z = 0;
	dx = 0, dy = 0, dz = 0;
	angle_turn = 0.0f;
	angle_swing = 0;
	liftend = false;
	fall = false;

	Turn = glm::mat4(1.0f);
	Move = glm::mat4(1.0f);
	Swing_l = glm::mat4(1.0f);
	Swing_r = glm::mat4(1.0f);
}

void CPlayer::Update()
{
	get_angle();
	Fall();
	Walk_anim();
	x += dx;
	y += dy;
	z += dz;
	Locate();
}

void CPlayer::Locate()
{
	Turn = glm::mat4(1.0f);
	Move = glm::mat4(1.0f);
	Swing_l = glm::mat4(1.0f);
	Swing_r = glm::mat4(1.0f);
	Move = glm::translate(Move, glm::vec3(x, y, z));
	Turn = glm::rotate(Turn, glm::radians(angle_turn), glm::vec3(0, 1, 0));
	Swing_l = glm::rotate(Swing_l, glm::radians(angle_swing), glm::vec3(1, 0, 0));
	Swing_r = glm::rotate(Swing_r, glm::radians(-angle_swing), glm::vec3(1, 0, 0));

	head.TRS = Move * Turn * head.T2 * head.S;
	nose.TRS = Move * Turn * nose.T2 * nose.S;
	body.TRS = Move * Turn * body.T2 * body.S;
	arm_l.TRS = Move * Turn * arm_l.T2 * Swing_l * arm_l.T1 * arm_l.S;
	arm_r.TRS = Move * Turn * arm_r.T2 * Swing_r * arm_r.T1 * arm_r.S;
	leg_l.TRS = Move * Turn * leg_l.T2 * Swing_r * leg_l.T1 * leg_l.S;
	leg_r.TRS = Move * Turn * leg_r.T2 * Swing_l * leg_r.T1 * leg_r.S;
}

void CPlayer::Swing()
{
	if (liftend) {
		angle_swing -= 5.0f;
		if (angle_swing <= -20.0f) {
			liftend = false;
		}
	}
	else {
		angle_swing += 5.0f;
		if (angle_swing >= 20.0f) {
			liftend = true;
		}
	}
}

void CPlayer::Walk_anim()
{
	if (dx != 0 || dz != 0)
		Swing();
	else {
		if (angle_swing != 0) {
			Swing();
		}
	}
}

void CPlayer::Jump()
{     
	if (fall == false) {
		fall = true;
		dy = 0.25;
	}
}

void CPlayer::Fall()
{
	if(fall)
		if (dy > -0.2)
			dy -= 0.03;
}

void CPlayer::get_angle()
{
	if (dx == 0 && dz == 0)
		return;
	angle_turn = atan2f(dx, dz) * 180 / 3.141592;
}