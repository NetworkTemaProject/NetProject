#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
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

GLfloat	box[][3] = {
	{ -0.5, 0, 0.5 },
	{ 0.5, 0, 0.5 },
	{ 0.5,1, 0.5 },	

	{ 0.5, 1, 0.5 },
	{ -0.5, 1, 0.5 },
	{ -0.5, 0, 0.5 }, // ¾Õ¸é

	{ -0.5, 0, -0.5 },
	{ 0.5, 0, -0.5 },
	{ -0.5, 0, 0.5 },

	{ 0.5, 0, -0.5 },
	{ 0.5, 0, 0.5 },
	{ -0.5, 0, 0.5 }, // ¹Ø¸é

	{ -0.5,0, 0.5 }, 
	{ -0.5, 1, 0.5 },
	{ -0.5, 0, -0.5 },

	{ -0.5, 1, 0.5 },
	{ -0.5, 1, -0.5 },
	{ -0.5, 0, -0.5 }, //¿Þ¿·¸é (¹Ù¶óº¸´Â±âÁØ)

	{ -0.5, 0, -0.5 }, 
	{ 0.5, 0, -0.5 },
	{ -0.5, 1, -0.5 },

	{ 0.5, 0, -0.5 },
	{ 0.5, 1, -0.5 },
	{ -0.5, 1, -0.5 }, //µÞ¸é

	{ -0.5, 1, 0.5 },
	{ 0.5, 1, 0.5 },
	{ -0.5, 1, -0.5 },

	{ -0.5, 1, -0.5 },
	{ 0.5, 1, 0.5 },
	{ 0.5,1, -0.5 }, //À­¸é

	{ 0.5, 1, -0.5 },
	{ 0.5, 1, 0.5 },
	{ 0.5,0, -0.5 },

	{ 0.5, 1, 0.5 },
	{ 0.5, 0, 0.5 },
	{ 0.5, 0, -0.5 }, //¿À¸¥¿·¸é
};
GLfloat	boxN[][3] = {
	 0.0f,  0.0f,  1.0f, 
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,

	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f,
	 0.0f,  0.0f,  1.0f, // ¾Õ¸é

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,

	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f,
	  0.0f, -1.0f,  0.0f, // ¹Ø¸é

	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,

	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f,
	  -1.0f,  0.0f,  0.0f, //¿·¸é

	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,

	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f,
	  0.0f,  0.0f, -1.0f, //µÞ¸é

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,

	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f,
	  0.0f,  1.0f,  0.0f, //À­¸é

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
#define CLINET_NUM 3

struct InputData{
	bool bUp;
	bool bRight;
	bool bLeft;
	bool bDown;
	bool bSpace;
	bool bEnter;
};

struct PlayerMgr {
	DWORD portnum;
	CPlayer player;
};

struct SendPlayerData {
	InputData Input;
	clock_t ClientTime;
};

struct SendGameData {
	PlayerMgr players;
	clock_t ServerTime;
	bool Win;
	std::vector<Foothold> Bottom;
};

SendPlayerData myPlayer;
PlayerMgr Players[CLINET_NUM];
SOCKET sock;
SOCKADDR_IN peeraddr;
SOCKADDR_IN serveraddr;

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

void TimerFunc();
void UpdateSendData();
bool IsPlayingGame();

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

void main(int argc, char** argv)
{
	srand(time(0));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("±â¸»");

	glewExperimental = GL_TRUE;
	glewInit();
	InitShader();
	InitBuffer();

	init();
	
	Init_Game();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMainLoop();
}

void Timerfunction(int value)
{
	switch (value) {
	case 1:
		player.Update();
		check_collide();

		for (int i = 0; i < Bottom.size(); ++i) {
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

	Lx = (float)cos(ltheta / 180 * 3.141592) * lx;			// Á¶¸í È¸Àü
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
	for (int i = 0; i < Bottom.size(); ++i)
	{
		Bottom[i].Draw_Start();
		glUniform3f(color_location, Bottom[i].r, Bottom[i].g, Bottom[i].b);
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(Bottom[i].Drawing)));
		glDrawArrays(GL_TRIANGLES, 0, Bottom[i].size);
	}

	// head
	glUniform3f(color_location, player.head.r, player.head.g, player.head.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.head.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// nose
	glUniform3f(color_location, player.nose.r, player.nose.g, player.nose.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.nose.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// body
	glUniform3f(color_location, player.body.r, player.body.g, player.body.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.body.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// arm
	glUniform3f(color_location, player.arm_l.r, player.arm_l.g, player.arm_l.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.arm_l.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform3f(color_location, player.arm_r.r, player.arm_r.g, player.arm_r.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.arm_r.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// leg
	glUniform3f(color_location, player.leg_l.r, player.leg_l.g, player.leg_l.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.leg_l.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform3f(color_location, player.leg_r.r, player.leg_r.g, player.leg_r.b);
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(glm::mat4(player.leg_r.TRS)));
	glDrawArrays(GL_TRIANGLES, 0, 36);

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
			player.dz = -0.1;
			break;
		}
		case 'a':
		{
			player.dx = -0.1;
			break;
		}
		case 's':
		{
			player.dz = 0.1;
			break;
		}
		case 'd':
		{
			player.dx = 0.1;
			break;
		}
		case 32:
		{
			player.Jump();
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
	case 'w': case 's':
	{
		player.dz = 0;
		break;
	}
	case 'a': case'd':
	{
		player.dx = 0;
		break;
	}
	}
	glutPostRedisplay();
}

void check_collide()
{
	player.fall = true;
	for (int i = 0; i < Bottom.size(); ++i) {
		if (collide_box(Bottom[i], player)) {
			player.fall = false;
			player.dy = 0;
			Bottom[i].startDel = true;
			break;
		}
	}

	for (int i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].Del)
			Bottom.erase(Bottom.begin() + i);
	}
}

bool collide_box(Foothold bottom, CPlayer& player)
{
	float b_maxX, b_minX, p_maxX, p_minX;
	float b_maxY, b_minY, p_maxY, p_minY;
	float b_maxZ, b_minZ, p_maxZ, p_minZ;
	p_maxX = player.x + 0.15; p_minX = player.x - 0.15;
	p_maxY = player.y + 0.1; p_minY = player.y - 0.1;
	p_maxZ = player.z + 0.15; p_minZ = player.z - 0.15;

	b_maxX = bottom.mx + 0.4; b_minX = bottom.mx - 0.4;
	b_maxY = bottom.my + 0.35; b_minY = bottom.my + 0.25;
	b_maxZ = bottom.mz + 0.4; b_minZ = bottom.mz - 0.4;

	if (b_maxX < p_minX || b_minX > p_maxX)
		return false;  
	if (b_maxZ < p_minZ || b_minZ > p_maxZ)
		return false;
	if (b_maxY < p_minY || b_minY > p_maxY)
		return false;
	player.y = bottom.my + 0.3;
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