#pragma once
#include "gdiplus.h"
using namespace Gdiplus;

#define FRAME_TIMER 1 // ������ ������ ������
#define FRAME_PERIOD 10 // ������ ������ �����
#define ARMOR_TIMER 2 // ������ �����
#define ARMOR_PERIOD 10000 // ������ ��������� �����
#define HEALTH_TIMER 2 //������ ��
#define HEALTH_PERIOD 10000 //������ ������ ��
#define FC_TIME 100 // ����� ����� ������ (�������� ��������)
#define CELL_SIZE 32 // ������ ������ �������� ����

#define FRAG_LIMIT 15 // ����������� �� ������
#define TIME_LIMIT 5 // ����������� �� �������
#define MAX_ARMOR 5 // ������������ ���������� ����� �� ����
#define MAX_HEALTH 3 //������������ ��������� �� �� ����

#define TANK_SPEED 150 // �������� �����
#define SHOT_SPEED 300 // �������� ��������
#define SHOTS 3 // ���������� ������������� ���������
#define SHOT_PAUSE 330 // ����� ����� ����������

#define EMPTY 0 // ������ ������
#define KIRPICH 1 // ������ (�����������)
#define BETON 2 // ����� (�� �����������)
#define ARMOR 3 // �����
#define HEALTH 4
#define NET 5

#define WIDTH 15 // ������ ���� �� �����������
#define HEIGHT 10 // ������ ���� �� ���������

#define DIR_UP 0 // ����������� ������� - �����
#define DIR_RIGHT 1 // ����������� ������� - ������
#define DIR_DOWN 2 // ����������� ������� - ����
#define DIR_LEFT 3 // ����������� ������� - �����

// ��������� - ���� (������������ ��� ��������
// ���� ��������� ��� ������������� ��������)
struct Tank
{
	bool alive;
	bool armor;
	bool moving;
	bool health;
	REAL x;
	REAL y;
	int dir;
	int frameNum;
	int frameTime;
}; 
class Game
{
private:
	int fps; // ���������� ������ � �������
	DWORD lastTick; // ��������� ����� �������
protected:
	bool noGamesPlayed; // ������� �� ���� ���� ����
	bool game;
	int p1frags; // ���� ������ 1
	int p2frags; // ���� ������ 2
	int p1ShotTime; // ����� ����� �������� ������ 1
	int p2ShotTime; // ����� ����� �������� ������ 2
	Bitmap* textures[2]; // ��������
	int pole[WIDTH][HEIGHT];
public:
	Game():noGamesPlayed (true),game(false),fps(0),lastTick(-1),p1frags(0),p2frags(0){
	};
	~Game()
	{
	};
	void startNewGame(HWND hWnd);
	void loadTextures();
	void freeTextures();
	void paint(HDC hdc);
	void processTimers(HWND hWnd, int timerId);
};


