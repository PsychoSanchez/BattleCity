#pragma once
#include "gdiplus.h"
using namespace Gdiplus;

#define FRAME_TIMER 1 // таймер кадров экрана
#define FRAME_PERIOD 10 // период одного кадра
#define ARMOR_TIMER 2 // таймер брони
#define ARMOR_PERIOD 10000 // период появления брони
#define HEALTH_TIMER 2 //таймер хп
#define HEALTH_PERIOD 10000 //период спавна хп
#define FC_TIME 100 // время смены кадров (скорость анимации)
#define CELL_SIZE 32 // размер ячейки игрового поля

#define FRAG_LIMIT 15 // ограничение по фрагам
#define TIME_LIMIT 5 // ограничение по времени
#define MAX_ARMOR 5 // макисмальное количество брони на поле
#define MAX_HEALTH 3 //макисмальное количеств хп на поле

#define TANK_SPEED 150 // скорость танка
#define SHOT_SPEED 300 // скорость выстрела
#define SHOTS 3 // количество одновременных выстрелов
#define SHOT_PAUSE 330 // пауза между выстрелами

#define EMPTY 0 // пустая клетка
#define KIRPICH 1 // кирпич (пробивается)
#define BETON 2 // бетон (не пробивается)
#define ARMOR 3 // броня
#define HEALTH 4
#define NET 5

#define WIDTH 15 // размер поля по горизонтали
#define HEIGHT 10 // размер поля по вертикали

#define DIR_UP 0 // направление движеия - вверх
#define DIR_RIGHT 1 // направление движеия - вправо
#define DIR_DOWN 2 // направление движеия - вниз
#define DIR_LEFT 3 // направление движеия - влево

// структура - Танк (используется для хранения
// всех подвижных или анимированных объектов)
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
	int fps; // количество кадров в секунду
	DWORD lastTick; // последний замер времени
protected:
	bool noGamesPlayed; // сыграна ли хоть одна игра
	bool game;
	int p1frags; // счет игрока 1
	int p2frags; // счет игрока 2
	int p1ShotTime; // время после выстрела игрока 1
	int p2ShotTime; // время после выстрела игрока 2
	Bitmap* textures[2]; // текстуры
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


