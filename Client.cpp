﻿#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <cstdlib>
#include "Foothold.h"
#include"filetobuf.h"
#include "Player.h"
#define pi 3.141592


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
//void Mouse(int button, int state, int x, int y);
//void MouseMove(GLint x, GLint y);
void Timerfunction(int value);
void make_vertexShader();
void make_fragmentShader();

void renderBitmapCharacher(float, float , float, void*, char* );
void Print_word(float, float, float, float, int, char*);
void check_GameOver();
void check_Bonus();

void check_collide();
bool collide_box(Foothold, CPlayer&);
void Time_score();

void Init_Game();

using namespace std;

int g_window_w;
int g_window_h;

GLuint vao, vbo[2];
GLuint s_program;

GLchar* vertexsource, * fragmentsource; 
GLuint vertexshader, fragmentshader; 
float theta = 0;
float cx = 1, cy = 1, cz = 1;

int font = (int)GLUT_BITMAP_TIMES_ROMAN_24;
int score = 0;
int cnt = 0;
int p_cnt = 0;
int tine = 0;
int p_time = 0;

char char_score[256];
char word1[10] = "score:";
char word2[11] = "life time:";
char over[10] = "Game Over";
clock_t past;
clock_t present;
clock_t start;
bool game_over = false;

DWORD WINAPI ClientMain(LPVOID arg);

GLfloat	box[][3] = {
	{ -0.5, 0, 0.5 },
	{ 0.5, 0, 0.5 },
	{ 0.5,1, 0.5 },	

	{ 0.5, 1, 0.5 },
	{ -0.5, 1, 0.5 },
	{ -0.5, 0, 0.5 }, // 앞면

	{ -0.5, 0, -0.5 },
	{ 0.5, 0, -0.5 },
	{ -0.5, 0, 0.5 },

	{ 0.5, 0, -0.5 },
	{ 0.5, 0, 0.5 },
	{ -0.5, 0, 0.5 }, // 밑면

	{ -0.5,0, 0.5 }, 
	{ -0.5, 1, 0.5 },
	{ -0.5, 0, -0.5 },

	{ -0.5, 1, 0.5 },
	{ -0.5, 1, -0.5 },
	{ -0.5, 0, -0.5 }, //왼옆면 (바라보는기준)

	{ -0.5, 0, -0.5 }, 
	{ 0.5, 0, -0.5 },
	{ -0.5, 1, -0.5 },

	{ 0.5, 0, -0.5 },
	{ 0.5, 1, -0.5 },
	{ -0.5, 1, -0.5 }, //뒷면

	{ -0.5, 1, 0.5 },
	{ 0.5, 1, 0.5 },
	{ -0.5, 1, -0.5 },

	{ -0.5, 1, -0.5 },
	{ 0.5, 1, 0.5 },
	{ 0.5,1, -0.5 }, //윗면

	{ 0.5, 1, -0.5 },
	{ 0.5, 1, 0.5 },
	{ 0.5,0, -0.5 },

	{ 0.5, 1, 0.5 },
	{ 0.5, 0, 0.5 },
	{ 0.5, 0, -0.5 }, //오른옆면
};
GLfloat	boxN[][3] = {
	 0.0f,  0.0f,  1.0f, 
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,

	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f, // 앞면

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f, // 밑면

	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,

	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f, //옆면

	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,

	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f, //뒷면

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f, //윗면

	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,

	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  0.0f,
};

std::vector<Foothold> Bottom;
CPlayer player;
///////////////////////////////////////////////////////////////////////////////////////
struct SendGameData {
	PlayerMgr* PMgr;
	clock_t ServerTime;
	std::vector<Foothold>& Bottom;
};

SendPlayerData myPlayer;
PlayerMgr players[CLIENT_NUM];
SOCKET sock;
SOCKADDR_IN peeraddr;
SOCKADDR_IN serveraddr;
SendGameData* ServerDatas;

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

void TimerFunc();
void UpdateSendData();
bool IsPlayingGame();


////////////////////////////////////////////////////////////////////////////////////

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0) break;

		left -= received;
		ptr += received;
	}

	return (len - left);
}

/////////////////////////////////////////////////////////////////////////////////
void init()
{
	glEnable(GL_DEPTH_TEST);
}

void make_vertexShader()
{
	vertexsource = filetobuf("vertex.glsl");
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	glCompileShader(vertexshader);
	checkCompileErrors(vertexshader, "vertex shader");
}

void make_fragmentShader()
{
	fragmentsource = filetobuf("fragment.glsl");
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentshader);
	checkCompileErrors(fragmentshader, "fragment shader");
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(box), box, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxN), boxN, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void InitShader()
{
	make_vertexShader();
	make_fragmentShader();

	s_program = glCreateProgram();
	glAttachShader(s_program, vertexshader);
	glAttachShader(s_program, fragmentshader);
	glLinkProgram(s_program);
	checkCompileErrors(s_program, "PROGRAM");
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);

	glUseProgram(s_program);
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("기말");

	glewExperimental = GL_TRUE;
	glewInit();
	InitShader();
	InitBuffer();

	init();
	
	Init_Game();

	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMainLoop();

	return 0;
}

void Timerfunction(int value)
{
	switch (value) {
	case 1:
		player.Update();
		check_collide();

		for (size_t i = 0; i < Bottom.size(); ++i) {
			if (Bottom[i].startDel)
				Bottom[i].Delete();
		}

		for (int i = Bottom.size() - 1; i >= 0; --i) {
			if (Bottom[i].Del)
			{
				score += Bottom[i].score;
				++cnt;
				Bottom.erase(Bottom.begin() + i);
			}
		}

		glutPostRedisplay();
		if(!game_over)
			glutTimerFunc(50, Timerfunction, 1);
		break;
	}
}

void renderBitmapCharacher(float x, float y, float z, void* font, char* string)
{
	char* c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}

void Print_word(float word_x, float word_y, float x, float y, int num, char* word)
{
	renderBitmapCharacher(word_x, word_y, 0, (void*)font, word);
	sprintf(char_score, "%d", num);
	renderBitmapCharacher(x, y, 0, (void*)font, char_score);
}

void Time_score()
{
	present = clock() / 1000;
	if (present - past)
			score += 1;
	past = clock() / 1000;
}

float radius = 5, camX=0,camY=0,camZ=0;
float lx = 3.0, ly = 3.5, lz = -1.5, ltheta=0; 
float Lx = 0, Ly = 0, Lz = 0;
float aml = 0.35f;

GLvoid drawScene()
{
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(s_program);

	GLUquadricObj * armL, * armR;
	armL = gluNewQuadric();
	armR = gluNewQuadric();

	glm::mat4 ptrans = glm::mat4(1.0f);
	glm::mat4 vtrans = glm::mat4(1.0f);

	Lx = (float)cos(ltheta / 180 * 3.141592) * lx;			// 조명 회전
	Ly = ly;
	Lz = (float)sin(ltheta / 180 * 3.141592) * (-lz);

	unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos");
	glUniform3f(lightPosLocation, Lx,Ly,Lz);
	unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor");
	glUniform3f(lightColorLocation, cx, cy, cz);
	unsigned int amlight = glGetUniformLocation(s_program, "ambientLight");
	glUniform1f(amlight, aml);
	

	unsigned int color_location = glGetUniformLocation(s_program, "objectColor");
	unsigned int model = glGetUniformLocation(s_program, "model");

	camX = (float)sin(theta / 180 * 3.141592) * radius;
	camY = +0.0;
	camZ = -1 * (float)cos(theta / 180 * 3.141592) * radius;
	vtrans = glm::lookAt(glm::vec3(player.x, player.y + 2, player.z + 2), glm::vec3(player.x, player.y, player.z), glm::vec3(0.0f, 1.0f, 0.0f));

	unsigned int view = glGetUniformLocation(s_program, "view");
	glUniformMatrix4fv(view, 1, GL_FALSE, &vtrans[0][0]);

	unsigned int projection = glGetUniformLocation(s_program, "projection");
	ptrans = glm::perspective(glm::radians(45.0f), (float)g_window_w / (float)g_window_h, 0.1f, 100.0f);
	ptrans = glm::translate(ptrans, glm::vec3(0, 0, -5.0));

	glUniformMatrix4fv(projection, 1, GL_FALSE, &ptrans[0][0]);

	glBindVertexArray(vao);
	for (size_t i = 0; i < ServerDatas->Bottom.size(); ++i)
	{
		ServerDatas->Bottom[i].Draw_Start();
		glUniform3f(color_location, ServerDatas->Bottom[i].r, ServerDatas->Bottom[i].g, ServerDatas->Bottom[i].b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(ServerDatas->Bottom[i].Drawing)));
		glDrawArrays(GL_TRIANGLES, 0, ServerDatas->Bottom[i].size);
	}

	for (size_t i = 0; i < CLIENT_NUM; ++i) {
		// head
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.head.r, (ServerDatas->PMgr[i]).player.head.g, (ServerDatas->PMgr[i]).player.head.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.head.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// nose
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.nose.r, (ServerDatas->PMgr[i]).player.nose.g, (ServerDatas->PMgr[i]).player.nose.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.nose.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// body
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.body.r, (ServerDatas->PMgr[i]).player.body.g, (ServerDatas->PMgr[i]).player.body.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.body.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// arm
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.arm_l.r, (ServerDatas->PMgr[i]).player.arm_l.g, (ServerDatas->PMgr[i]).player.arm_l.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.arm_l.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.arm_r.r, (ServerDatas->PMgr[i]).player.arm_r.g, (ServerDatas->PMgr[i]).player.arm_r.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.arm_r.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// leg
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.leg_l.r, (ServerDatas->PMgr[i]).player.leg_l.g, (ServerDatas->PMgr[i]).player.leg_l.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.leg_l.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform3f(color_location, (ServerDatas->PMgr[i]).player.leg_r.r, (ServerDatas->PMgr[i]).player.leg_r.g, (ServerDatas->PMgr[i]).player.leg_r.b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4((ServerDatas->PMgr[i]).player.leg_r.TRS)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	check_GameOver();

	if(!game_over)
		Time_score();

	tine = present - start;
	Print_word(0.5f, 0.8f, 0.7f, 0.8f, score,word1);
	Print_word(0.5f, 0.7f, 0.8f, 0.7f,tine, word2);
	check_Bonus();

	glutSwapBuffers();
}

void check_GameOver()
{
	if(game_over)
		renderBitmapCharacher(-0.2f,0.0f, 0, (void*)font, over);
	else
	{
		if (player.y < UNDER)
			game_over = true;
	}
}

void check_Bonus()
{
	if(cnt-p_cnt)
		if (!(cnt % 10))
			score += cnt*5;

	if (tine - p_time)
		if (!(tine% 10))
			score += tine;

	p_cnt = cnt;
	p_time = present - start;
}

GLvoid Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if (game_over) {
		switch (key) {
			case 'R':
			{
				Init_Game();
				break;
			}
			case 'Q':
			{
				exit(0);
				break;
			}
		}
	}
	else {
		switch (key)
		{
			case 'w':
			{
				myPlayer.Input.bUp = true;
				break;
			}
			case 'a':
			{
				myPlayer.Input.bLeft = true;
				break;
			}
			case 's':
			{
				myPlayer.Input.bDown = true;
				break;
			}
			case 'd':
			{
				myPlayer.Input.bRight = true;
				break;
			}
			case 32:
			{
				myPlayer.Input.bSpace = true;
				break;
			}
			case 13:
			{
				myPlayer.Input.bEnter = true;
				break; 
			}
			case 'p':
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case 'P':
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case 'Q':
				exit(0);
				break;
		}
	}
	glutPostRedisplay();
}

GLvoid KeyboardUp(unsigned char key, int x, int y)
{
	switch (key){
		case 'w':
		{
			myPlayer.Input.bUp = false;
			break;
		}
		case 's':
		{
			myPlayer.Input.bDown = false;
			break;
		}
		case 'a':
		{
			myPlayer.Input.bLeft = false;
			break;
		}
		case 'd':
		{			
			myPlayer.Input.bRight = false;
			break;
		}
		case 32:
		{
			myPlayer.Input.bSpace = false;
			break;
		}
		case 13:
		{
			myPlayer.Input.bEnter = false;
			break;
		}
	}
	glutPostRedisplay();
}

void check_collide()
{
	player.fall = true;
	for (size_t i = 0; i < Bottom.size(); ++i) {
		if (collide_box(Bottom[i], player)) {
			player.fall = false;
			player.dy = 0;
			Bottom[i].startDel = true;
			break;
		}
	}

	for (size_t i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].Del)
			Bottom.erase(Bottom.begin() + i);
	}
}

bool collide_box(Foothold bottom, CPlayer& player)
{
	float b_maxX, b_minX, p_maxX, p_minX;
	float b_maxY, b_minY, p_maxY, p_minY;
	float b_maxZ, b_minZ, p_maxZ, p_minZ;
	p_maxX = player.x + 0.15f; p_minX = player.x - 0.15f;
	p_maxY = player.y + 0.1f; p_minY = player.y - 0.1f;
	p_maxZ = player.z + 0.15f; p_minZ = player.z - 0.15f;

	b_maxX = bottom.mx + 0.4f; b_minX = bottom.mx - 0.4f;
	b_maxY = bottom.my + 0.35f; b_minY = bottom.my + 0.25f;
	b_maxZ = bottom.mz + 0.4f; b_minZ = bottom.mz - 0.4f;

	if (b_maxX < p_minX || b_minX > p_maxX)
		return false;  
	if (b_maxZ < p_minZ || b_minZ > p_maxZ)
		return false;
	if (b_maxY < p_minY || b_minY > p_maxY)
		return false;
	player.y = bottom.my + 0.3f;
	return true;
}

void Init_Game()
{
	game_over = false;
	score = 0;
	start = clock() / 1000;
	present = start;
	past = start;

	Bottom.clear();
	MakeFoothold(Bottom);

	for (int i = 0; i < 5; ++i) {
		Bottom[rand() % 25].Del = true;
		Bottom[rand() % 25 + 25].Del = true;
		Bottom[rand() % 25 + 50].Del = true;
		Bottom[rand() % 25 + 75].Del = true;
		Bottom[rand() % 25 + 100].Del = true;
	}
	for (int i = Bottom.size() - 1; i >= 0; --i) {
		if (Bottom[i].Del)
		{
			Bottom.erase(Bottom.begin() + i);
		}
	}

	player.x = 0;
	player.y = 5;
	player.z = 0;
	player.dx = 0;
	player.dy = 0;
	player.dz = 0;
	player.fall = true;
	player.Locate();

	glutTimerFunc(50, Timerfunction, 1);
}

DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	//if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));


	int len;
	char buf[BUFSIZE];

	while (1) {
		// myPlayer 송신

		// ServerGameData 수신
		recvn(sock, (char*)&len, sizeof(int), 0);
		recvn(sock, buf, len, 0);
		ServerDatas = reinterpret_cast<SendGameData*>(&buf);
		// 여기서 포트번호로? 어느 인덱스가 자신의 것인지 판단 후 기억해놓기
	}


	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

void TimerFunc()
{

}

void UpdateSendData()
{

}

bool IsPlayingGame()
{
	return false;
}