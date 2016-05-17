#include "StdAfx.h"
#include "Game.h"
#include "gdiplus.h"
#include "sstream"
#include "time.h"
#include <iostream>

using namespace Gdiplus;
using namespace std;
class Shot;
class Boom;
class Spawn;
class Pole;
class Tanks;
// загрузка текстур из файлов
void Game::loadTextures()
{
	textures[0] = Bitmap::FromFile(L"textures\\tanks.png");
	textures[1] = Bitmap::FromFile(L"textures\\gameover.png");
}
// удаление текстур
void Game::freeTextures()
{
	delete textures[0];
	delete textures[1];
}

class GameDraw:public Game
{
private:
	long gameTime; // время игры
protected:
	Graphics *g;
public:
	GameDraw();
	~GameDraw();
	void drawScorePanel();
	void frame(Pole &pole,Shot &p,Spawn &s,Tanks &T,Boom &b,int dt);
    void Draw();
};
class Tanks:public GameDraw
{
protected: 
	Tank tank1, tank2;
public:
	friend class Shot;
	friend class Spawn;
	friend class Boom;
	Tanks();
	void initTank(int TankNumber,Tank *spawn);
	void Draw(int i);
	bool rectsIntersect(RectF* r1, int x, int y);
	void killTank(Boom &b,Spawn &s,Tank *tank);
	void moveTank(Shot &sh,Pole &p,Spawn &s,Boom &b,int i,int dt);
	Tank* Tanks::getTank(int x, int y);
	void animateTank(int TankNumber,int dt);
	void processKeys(Shot &s);
};
class Shot:public Tanks
{
	friend class Tanks;
private:
	Tank shot1[SHOTS], shot2[SHOTS];
public:
	Shot();
	~Shot();
	void Draw(int number,int i);
	void moveShot(Pole &p, Boom &b, Spawn &s,int TankNumber,int number, int dt);
	Tank* getShot(Tank *tank,int x, int y);
	void shoot(Tank* tank, Tank* shot);
	void tankshoot(int TankNumber);
};
class Spawn:public Tanks
{
	friend class Tanks;
protected: 
	Tank spawn1, spawn2;
public:
	Spawn();
	~Spawn();
	void initSpawn(Tank *spawn,int x, int y, int dir);
	void Draw(int i);
	void spawnTank(Tank *tank);
	void animateSpawn(int TankNumber,Tank *spawn,int dt);
	Tank* getSpawn1();
	Tank* getSpawn2();
};
class Boom:public Tanks
{
	friend class Tanks;
private:
	Tank Booms[SHOTS*2];
public:
	Boom();
	~Boom();
	void Draw(int i);
	void makeBoom(REAL x, REAL y);
	void animateBoom(int i,int dt);
};
class Pole:public GameDraw
{
	friend class Tanks;
public:
	Pole();
	~Pole();
	void poleCreate();
	void Draw(int x, int y);
	int countHealth();
	int countBronya();
};
Shot::Shot()
{
	for (int i = 0; i < SHOTS; i++)
	{
		shot1[i].alive = false;
		shot2[i].alive = false;
	}
}
Shot::~Shot()
{
}
Boom::Boom()
{
	for (int i = 0; i < SHOTS * 2; i++)
	{
		Booms[i].alive = false;
	}

}
Boom::~Boom()
{
}
Spawn::Spawn()
{
	initSpawn(&spawn1, 1, 1, 1);
	initSpawn(&spawn2, WIDTH - 2, HEIGHT - 2, 3);
}
Spawn::~Spawn()
{
}
GameDraw::GameDraw()
{
	SolidBrush blackBrush(Color::Black);
	SolidBrush whiteBrush(Color::White);
	
	// Размер стороны временного буфера
	int bbx = WIDTH * CELL_SIZE;// Ширина
	int bby = HEIGHT * CELL_SIZE + 16;// Высота
    // Временный буфер
	Bitmap backBuffer(bbx, bby);
	// Временный графический контекст
    Graphics bbg(&backBuffer);
	// Включаем сглаживание
	bbg.SetSmoothingMode(SmoothingModeAntiAlias);
	// Заливка всего окна - фон
	bbg.Clear(Color::Black); // Черным цветом
	this->g=&bbg;
	gameTime=0;
}
GameDraw::~GameDraw()
{
}
void GameDraw::drawScorePanel()
{
	SolidBrush grayBrush(Color::Gray);
	SolidBrush blackBrush(Color::Black);
	g->FillRectangle(&grayBrush, 0, HEIGHT * CELL_SIZE, WIDTH * CELL_SIZE, 16);

	RectF p1(8.0, REAL(HEIGHT * CELL_SIZE), 16, 16);
	g->DrawImage(textures[0], p1, 0.0, 16.0, 15.0, 15.0, UnitPixel);

	RectF p2(REAL(WIDTH * CELL_SIZE - 24), REAL(HEIGHT * CELL_SIZE), 16, 16);
	g->DrawImage(textures[0], p2, 0.0, 32.0, 15.0, 15.0, UnitPixel);

	Font font(L"Verdana", 10, FontStyleBold);

	wstringstream wss;
	wss << p1frags;
	PointF p1point(30, HEIGHT * CELL_SIZE);
	g->DrawString(wss.str().c_str(), wss.str().length(), &font, p1point, &blackBrush); 

	wss.str(L"");
	wss << p2frags;
	PointF p2point(WIDTH * CELL_SIZE - 54, HEIGHT * CELL_SIZE);
	g->DrawString(wss.str().c_str(), wss.str().length(), &font, p2point, &blackBrush); 

	wss.str(L"");
	int s = gameTime / 1000;
	int m = s / 60;
	s = s % 60;
	wss << L"Время игры: " << m << ":";
	if (s < 10) wss << "0";
	wss << s;
	PointF tpoint(REAL((WIDTH * CELL_SIZE - wss.str().length() * 8) / 2), HEIGHT * CELL_SIZE);
	g->DrawString(wss.str().c_str(), wss.str().length(), &font, tpoint, &blackBrush); 
}
// новый кадр
void GameDraw::frame(Pole &pole,Shot &p,Spawn &s,Tanks &T,Boom &b,int dt)
{
	T.processKeys(p);

	p1ShotTime += dt;
	p2ShotTime += dt;
	gameTime += dt;
	if (gameTime > TIME_LIMIT * 60 * 1000)
	{
		game = false;
		return;
	}

	for (int i = 0; i < SHOTS; i++)
	{
		p.moveShot(pole,b,s,1,i, dt);
		p.moveShot(pole,b,s,2,i, dt);
	}

	T.moveTank(p,pole,s,b,1,dt);
	T.moveTank(p,pole,s,b,2,dt);

	T.animateTank(1, dt);
	T.animateTank(2, dt);
	Tank *spawn;
	spawn=s.getSpawn1();
	s.animateSpawn(1,spawn, dt);
	spawn=s.getSpawn2();
	s.animateSpawn(2,spawn, dt);
	delete spawn;

	for (int i = 0; i < SHOTS * 2; i++)
	{
		b.animateBoom(i, dt);
	}
}



Pole::Pole(){}
Pole::~Pole(){}
void Pole::poleCreate()
{
	// инициализируем генератор случайных чисел
	srand((unsigned)time(NULL));
	// очищаем игровое поле
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			pole[i][j] = EMPTY;
		}
	}
	int x, y;
	// генерируем случайные кирпичи и бетон
	for (int i = 0; i < WIDTH + HEIGHT; i++)
	{
		// генерируем координаты таким образом, чтобы
		// не замуровать углы с игроками
		do {
			x = rand() % WIDTH;
			y = rand() % HEIGHT;
		} while (x < 3 && y < 3 || x > WIDTH - 4 && y > HEIGHT - 4
			|| x > WIDTH - 4 && y < 3 || x < 3 && y > HEIGHT - 4);
		// ставим кирпич
		pole[x][y] = KIRPICH;
		do {
			x = rand() % WIDTH;
			y = rand() % HEIGHT;
		} while (x < 3 && y < 3 || x > WIDTH - 4 && y > HEIGHT - 4
			|| x > WIDTH - 4 && y < 3 || x < 3 && y > HEIGHT - 4);//(x + y < 4 || x + y > (WIDTH - 3) + (HEIGHT - 3));
		// ставим бетон
		pole[x][y] = BETON;
		do {
			x = rand() % WIDTH;
			y = rand() % HEIGHT;
		} while (x < 3 && y < 3 || x > WIDTH - 4 && y > HEIGHT - 4
			|| x > WIDTH - 4 && y < 3 || x < 3 && y > HEIGHT - 4);
		pole[x][y] = NET;
	}
}
// отрисовка клетки игрового поля
void Pole::Draw(int x, int y)
{
	RectF rect(REAL(x * CELL_SIZE), REAL(y * CELL_SIZE), CELL_SIZE, CELL_SIZE);
	int num = -1;
	if (pole[x][y] == KIRPICH) num = 0;
	if (pole[x][y] == BETON) num = 1;
	if (pole[x][y] == ARMOR) num = 2;
	if (pole[x][y] == HEALTH) num = 7;
	if (num >= 0)
	{
		REAL x = REAL(16 * num);
		g->DrawImage(textures[0], rect, x, 0.0, 15.0, 15.0, UnitPixel);
	}
	if (pole[x][y] == NET) 
	{
		g->DrawImage(textures[0], rect, 112, 48, 15.0, 15.0, UnitPixel);
	}
}
// подсчет брони на игровом поле
int Pole::countBronya()
{
	int cnt = 0;
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			if (pole[i][j] == ARMOR) cnt++;
		}
	}
	return cnt;
}
int Pole::countHealth()
{
	int cnt = 0;
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			if (pole[i][j] == HEALTH) cnt++;
		}
	}
	return cnt;
}

Tank* Spawn::getSpawn1()
{
	return &spawn1;
}
Tank* Spawn::getSpawn2()
{
	return &spawn2;
}


void Boom::Draw(int i)
{
	if (Booms[i].alive)
	{
		RectF rect(Booms[i].x, Booms[i].y, CELL_SIZE, CELL_SIZE);
		REAL x = REAL(16 * Booms[i].frameNum);
		g->DrawImage(textures[0], rect, x, 48.0, 15.0, 15.0, UnitPixel);
	}
}
void Boom::makeBoom(REAL x, REAL y)
{
	for (int i = 0; i < SHOTS * 2; i++)
	{
		if (!Booms[i].alive)
		{
			Booms[i].x = x;
			Booms[i].y = y;
			Booms[i].frameTime = 0;
			Booms[i].frameNum = 0;
			Booms[i].alive = true;
			break;
		}
	}
}
// появление танка
Tanks::Tanks()
{
	tank1.alive = false;
	tank2.alive = false;
}
bool Tanks::rectsIntersect(RectF* r1, int x, int y)
{
	RectF* r2 = new RectF(REAL(x * CELL_SIZE), REAL(y * CELL_SIZE), CELL_SIZE, CELL_SIZE);
	bool intersect = !(
		r2->X >= r1->X + r1->Width
		|| r2->X + r2->Width <= r1->X
		|| r2->Y >= r1->Y + r1->Height
		|| r2->Y + r2->Height <= r1->Y
	);
	delete r2;
	return intersect;
}
// получение снаряда в клетке (x, y)
Tank* Shot::getShot(Tank *tank,int x, int y)
{
	int nxs1, nys1, nxs2, nys2;
	for (int i = 0; i < SHOTS; i++)
	{
		if (shot1[i].alive && tank==&tank2)
		{
			nxs1 = int((shot1[i].x + CELL_SIZE / 2) / CELL_SIZE);
			nys1 = int((shot1[i].y + CELL_SIZE / 2) / CELL_SIZE);
			if (nxs1 == x && nys1 == y) return &shot1[i];
		}
		if (shot2[i].alive && tank==&tank1)
		{
			nxs2 = int((shot2[i].x + CELL_SIZE / 2) / CELL_SIZE);
			nys2 = int((shot2[i].y + CELL_SIZE / 2) / CELL_SIZE);
			if (nxs2 == x && nys2 == y) return &shot2[i];
		}
	}
	return NULL;
}

// получение танка в клетке (x, y)
Tank* Tanks::getTank(int x, int y)
{
	int nxt1 = int((tank1.x + CELL_SIZE / 2) / CELL_SIZE);
	int nyt1 = int((tank1.y + CELL_SIZE / 2) / CELL_SIZE);
	int nxt2 = int((tank2.x + CELL_SIZE / 2) / CELL_SIZE);
	int nyt2 = int((tank2.y + CELL_SIZE / 2) / CELL_SIZE);
	if (tank1.alive && nxt1 == x && nyt1 == y) return &tank1;
	if (tank2.alive && nxt2 == x && nyt2 == y) return &tank2;
	return NULL;
}

void Tanks::initTank(int i,Tank *spawn)
{
	Tank* tank = &tank2;
	if(i==1) tank=&tank1;
	tank->alive = true;
	tank->armor = false;
	tank->moving = false;
	tank->health = false;
	tank->x = spawn->x;
	tank->y = spawn->y;
	tank->dir = spawn->dir;
	tank->frameNum = 0;
	tank->frameTime = 0;
}
// появление точки появления
void Spawn::initSpawn(Tank *spawn,int x, int y, int dir)
{
	spawn->alive = true;
	spawn->armor = false;
	spawn->moving = false;
	spawn->health=false;
	spawn->x = REAL(x * CELL_SIZE);
	spawn->y = REAL(y * CELL_SIZE);
	spawn->dir = dir;
	spawn->frameNum = 0;
	spawn->frameTime = 0;
}
void Spawn::spawnTank(Tank *tank)
{
	Tank* spawn = &spawn2;
	if (tank == &tank1) spawn = &spawn1;
	int corner = rand() % 4;
	switch(corner)
	{
	case 0: initSpawn(spawn,1, 1, 1); break;
	case 1: initSpawn(spawn,WIDTH - 2, 1, 2); break;
	case 2: initSpawn(spawn,WIDTH - 2, HEIGHT - 2, 3); break;
	case 3: initSpawn(spawn,1, HEIGHT - 2, 0); break;
	}
}
// обработка попадания по танку
void Tanks::killTank(Boom &b,Spawn &s,Tank *tank)
{
	b.makeBoom(tank->x, tank->y);
	if (tank->armor) tank->armor = false;
	else
	{
		tank->alive = false;
		if (tank == &tank1) p2frags++; else p1frags++;
		if (p1frags == FRAG_LIMIT || p2frags == FRAG_LIMIT)
		{
			game = false;
			return;
		}
		s.spawnTank(tank);
	}
}
void Tanks::Draw(int i)
{
	Tank *tank;
	if (i==1) tank=&tank1;
	else tank=&tank2;
	if (tank->alive)
	{
		if(tank->armor)
		{
			RectF rect(tank->x, tank->y, CELL_SIZE, CELL_SIZE);
			REAL x, y;
			x = REAL(16 * 2 * tank->dir + tank->frameNum * 16);
			y = 64.0;
			if (tank == &tank2) y = 80.0;
			g->DrawImage(textures[0], rect, x, y, 15.0, 15.0, UnitPixel);
		}
		else
		{
			RectF rect(tank->x, tank->y, CELL_SIZE, CELL_SIZE);
			REAL x, y;
			x = REAL(16 * 2 * tank->dir + tank->frameNum * 16);
			y = 16.0;
			if (tank == &tank2) y = 32.0;
			g->DrawImage(textures[0], rect, x, y, 15.0, 15.0, UnitPixel);
		}
	}
}

void Shot::Draw(int number,int i)
{
	Tank *shot;
	if(number==1) shot=&shot1[i];
	else shot=&shot2[i];
	if (shot->alive)
	{
		RectF rect(shot->x, shot->y, CELL_SIZE, CELL_SIZE);
		REAL x = REAL(48.0 + 16 * shot->dir);
		g->DrawImage(textures[0], rect, x, 0.0, 15.0, 15.0, UnitPixel);
	}
	delete shot;
}

void Spawn::Draw(int i)
{
	Tank *spawn;
	if(i==1) spawn=&spawn1;
	else spawn=&spawn2;
	if (spawn->alive)
	{
		RectF rect(spawn->x, spawn->y, CELL_SIZE, CELL_SIZE);
		REAL x = REAL(64.0 + 16 * spawn->frameNum);
		g->DrawImage(textures[0], rect, x, 48.0, 15.0, 15.0, UnitPixel);
	}
	delete spawn;
}
void Tanks::moveTank(Shot &sh,Pole &p,Spawn &s,Boom &b,int i,int dt)
{
	Tank *tank;
	if(i==1) tank=&tank1;
	else tank=&tank2;
	if (!tank->moving || !tank->alive) return;

	int dirx = 0;
	int diry = 0;
	if (tank->dir == 0) diry = -1;
	if (tank->dir == 1) dirx = 1;
	if (tank->dir == 2) diry = 1;
	if (tank->dir == 3) dirx = -1;

	REAL dx = TANK_SPEED * dirx * (dt / 1000.0f);
	REAL dy = TANK_SPEED * diry * (dt / 1000.0f);

	int cnx = int((tank->x + CELL_SIZE / 2) / CELL_SIZE);
	int cny = int((tank->y + CELL_SIZE / 2) / CELL_SIZE);

	RectF* moveRect;
	if (tank->dir == 0 || tank->dir == 3)
	{
		moveRect = new RectF(tank->x + dx, tank->y + dy, CELL_SIZE - dx, CELL_SIZE - dy);
	}
	else
	{
		moveRect = new RectF(tank->x, tank->y, CELL_SIZE + dx, CELL_SIZE + dy);
	}

	if (tank->dir == 1 || tank->dir == 3)
	{
		int i = cnx + dirx;
		while (rectsIntersect(moveRect, i, cny))
		{
			if (i < 0 || i >= WIDTH ||
				p.pole[i][cny] == KIRPICH ||
				p.pole[i][cny] == BETON ||
				getTank(i, cny) != NULL ||
				sh.getShot(tank, i, cny) != NULL)
			{
				Tank* shot = sh.getShot(tank, i, cny);
				if (getTank(i, cny) != NULL || shot == NULL)
				{
					REAL cdx = tank->x + dx - (i - dirx) * CELL_SIZE;
					dx -= cdx;
				}
				else
				{
					shot->alive = false;
					killTank(b,s,tank);
				}
				break;
			}
			i += dirx;
		}
	}
	else
	{
		int i = cny + diry;
		while (rectsIntersect(moveRect, cnx, i))
		{
			if (i < 0 || i >= HEIGHT ||
				p.pole[cnx][i] == KIRPICH ||
				p.pole[cnx][i] == BETON ||
				getTank(cnx, i) != NULL ||
				sh.getShot(tank, cnx, i) != NULL)
			{
				Tank* shot = sh.getShot(tank, cnx, i);
				if (getTank(cnx, i) != NULL || shot == NULL)
				{
					REAL cdy = tank->y + dy - (i - diry) * CELL_SIZE;
					dy -= cdy;
				}
				else
				{
					shot->alive = false;
					killTank(b,s,tank);
				}
				break;
			}
			i += diry;
		}
	}

	delete moveRect;

	if (p.pole[cnx][cny] == ARMOR)
	{
		tank->armor = true;
		p.pole[cnx][cny] = EMPTY;
	}
	if (p.pole[cnx][cny] == HEALTH)
	{
		if(tank==&tank1) p2frags-=1;
		else p1frags-=1;
		p.pole[cnx][cny] = EMPTY;
	}

	if (tank->dir == DIR_LEFT || tank->dir == DIR_RIGHT)
	{
		tank->y = REAL(cny * CELL_SIZE);
		tank->x += dx;
	}
	if (tank->dir == DIR_UP || tank->dir == DIR_DOWN)
	{
		tank->x = REAL(cnx * CELL_SIZE);
		tank->y += dy;
	}
}
void Shot::moveShot(Pole &p, Boom &b, Spawn &s, int number,int i, int dt)
{
	Tank *shot;
	if(number==1) shot=&shot1[i];
	else shot=&shot2[i];
	if (!shot->alive) return;

	int dirx = 0;
	int diry = 0;
	if (shot->dir == 0) diry = -1;
	if (shot->dir == 1) dirx = 1;
	if (shot->dir == 2) diry = 1;
	if (shot->dir == 3) dirx = -1;

	REAL dx = SHOT_SPEED * dirx * (dt / 1000.0f);
	REAL dy = SHOT_SPEED * diry * (dt / 1000.0f);

	int cnx = int((shot->x + CELL_SIZE / 2) / CELL_SIZE);
	int cny = int((shot->y + CELL_SIZE / 2) / CELL_SIZE);

	RectF* moveRect;
	if (shot->dir == 0 || shot->dir == 3)
	{
		moveRect = new RectF(shot->x + dx + CELL_SIZE / 2, shot->y + dy + CELL_SIZE / 2, CELL_SIZE - dx, CELL_SIZE - dy);
	}
	else
	{
		moveRect = new RectF(shot->x, shot->y, CELL_SIZE + dx - CELL_SIZE / 2, CELL_SIZE + dy - CELL_SIZE / 2);
	}

	if (shot->dir == 1 || shot->dir == 3)
	{
		int i = cnx + dirx;
		while (rectsIntersect(moveRect, i, cny))
		{
			if (i < 0 || i >= WIDTH ||
				pole[i][cny] == KIRPICH ||
				pole[i][cny] == BETON ||
				getTank(i, cny) != NULL)
			{
				shot->alive = false;
				Tank* t = getTank(i, cny);
				if (t == NULL)
				{
					if (pole[i][cny] == KIRPICH) pole[i][cny] = EMPTY;
					b.makeBoom(REAL(i * CELL_SIZE), REAL(cny * CELL_SIZE));
				}
				else killTank(b,s,t);
				break;
			}
			i += dirx;
		}
	}
	else
	{
		int i = cny + diry;
		while (rectsIntersect(moveRect, cnx, i))
		{
			if (i < 0 || i >= HEIGHT ||
				pole[cnx][i] == KIRPICH ||
				pole[cnx][i] == BETON ||
				getTank(cnx, i) != NULL)
			{
				shot->alive = false;
				Tank* t = getTank(cnx, i);
				if (t == NULL)
				{
					if (pole[cnx][i] == KIRPICH) pole[cnx][i] = EMPTY;
					b.makeBoom(REAL(cnx * CELL_SIZE), REAL(i * CELL_SIZE));
				}
				else killTank(b,s,t);
				break;
			}
			i += diry;
		}
	}

	delete moveRect;

	shot->x += dx;
	shot->y += dy;
	delete shot;
}
void Shot::shoot(Tank* tank, Tank* shot)
{
	shot->dir = tank->dir;
	shot->x = tank->x;
	shot->y = tank->y;
	shot->moving = true;
	shot->alive = true;
}
void Shot::tankshoot(int number)
{
	Tank* tank;
	if(number==1) 
	{
		tank=&tank1;
		if (p1ShotTime > SHOT_PAUSE && tank->alive)
		{
			for (int i = 0; i < SHOTS; i++)
			{
				if (!shot1[i].alive)
				{
					shoot(tank,&shot1[i]);
					p1ShotTime = 0;
					break;
				}
			}
		}
	}
	else
	{
		tank=&tank2;
		if (p1ShotTime > SHOT_PAUSE && tank->alive)
		{
			for (int i = 0; i < SHOTS; i++)
			{
				if (!shot2[i].alive)
				{
					shoot(tank,&shot2[i]);
					p1ShotTime = 0;
					break;
				}
			}
		}
	}

}
void Tanks::animateTank(int i,int dt)
{
	if(i==1)
	{
		// Определение кадра анимации
		if (tank1.moving)
		{
			tank1.frameTime += dt;
			if (tank1.frameTime > (FC_TIME / 2))
			{
				tank1.frameTime %= (FC_TIME / 2);
				tank1.frameNum = (tank1.frameNum + 1) % 2;
			}
		}
	}
	else 
		if (tank2.moving)
		{
			tank2.frameTime += dt;
			if (tank2.frameTime > (FC_TIME / 2))
			{
				tank2.frameTime %= (FC_TIME / 2);
				tank2.frameNum = (tank2.frameNum + 1) % 2;
			}
		}
}
// анимация взрыва
void Boom::animateBoom(int i,int dt)
{
	// Определение кадра анимации
	if (Booms[i].alive)
	{
		Booms[i].frameTime += dt;
		if (Booms[i].frameTime > FC_TIME)
		{
			Booms[i].frameTime %= FC_TIME;
			Booms[i].frameNum = Booms[i].frameNum + 1;
		}
		if (Booms[i].frameNum > 2) Booms[i].alive = false;
	}
}

// анимация точки появления
void Spawn::animateSpawn(int i,Tank* spawn,int dt)
{
	// Определение кадра анимации
	if (spawn->alive)
	{
		spawn->frameTime += dt;
		if (spawn->frameTime > FC_TIME)
		{
			spawn->frameTime %= FC_TIME;
			spawn->frameNum = (spawn->frameNum + 1) % 4;
		}
		if (spawn->frameNum > 2)
		{
			if (!spawn->armor)
			{
				spawn->armor = true;
				spawn->frameNum = 0;
			}
			if (!spawn->health)
			{
				spawn->health = true;
				spawn->frameNum = 0;
			}
			if (!spawn->moving)
			{
				spawn->moving = true;
				spawn->frameNum = 0;
			}
			else
			{
				spawn->alive = false;
				initTank(i,spawn);
			}

		}
	}
}

// обработка нажатия клавиш
void Tanks::processKeys(Shot &s)
{
	tank1.moving = false;
	if (GetKeyState(VK_UP) < 0)
	{
		tank1.dir = DIR_UP;
		tank1.moving = true;
	}
	else if (GetKeyState(VK_DOWN) < 0)
	{
		tank1.dir = DIR_DOWN;
		tank1.moving = true;
	}
	else if (GetKeyState(VK_LEFT) < 0)
	{
		tank1.dir = DIR_LEFT;
		tank1.moving = true;
	}
	else if (GetKeyState(VK_RIGHT) < 0)
	{
		tank1.dir = DIR_RIGHT;
		tank1.moving = true;
	}
	if (GetKeyState(VK_RCONTROL) < 0) s.tankshoot(1);

	tank2.moving = false;
	if (GetKeyState('W') < 0)
	{
		tank2.dir = DIR_UP;
		tank2.moving = true;
	}
	else if (GetKeyState('S') < 0)
	{
		tank2.dir = DIR_DOWN;
		tank2.moving = true;
	}
	else if (GetKeyState('A') < 0)
	{
		tank2.dir = DIR_LEFT;
		tank2.moving = true;
	}
	else if (GetKeyState('D') < 0)
	{
		tank2.dir = DIR_RIGHT;
		tank2.moving = true;
	}
	if (GetKeyState(VK_SPACE) < 0) s.tankshoot(2);
	}

// обработка срабатывания таймеров
void Game::processTimers(HWND hWnd, int timerId)
{
	GameDraw *Draw=new Pole;
	Pole P;
	Tanks T;
	Spawn S;
	Shot Sh;
	Boom B;
	if (!game) return;
	if (timerId == FRAME_TIMER)
	{
		DWORD newTick = GetTickCount();
		int dt;
		if (lastTick != -1) dt = newTick - lastTick; else dt = FRAME_PERIOD;
		if (dt > 0)
		{
			fps = 1000 / dt;
			lastTick = newTick;
			Draw->frame(P,Sh,S,T,B,dt);
		}
	}
	if (timerId == ARMOR_TIMER)
	{
		if (P.countBronya() < MAX_ARMOR)
		{
			// координаты для брони
			int x = rand() % WIDTH;
			int y = rand() % HEIGHT;
			// если попали в пустую клетку, кладем туда броню
			if (pole[x][y] == EMPTY) pole[x][y] = ARMOR;
		}
	}
	if (timerId == HEALTH_TIMER)
	{
		if (P.countHealth() < MAX_HEALTH)
		{
			// координаты для хп
			int x = rand() % WIDTH;
			int y = rand() % HEIGHT;
			// если попали в пустую клетку, кладем туда броню
			if (pole[x][y] == EMPTY) pole[x][y] = HEALTH;
		}
	}
	InvalidateRect(hWnd, NULL, false);
}

void Game::startNewGame(HWND hWnd)
{
	noGamesPlayed = false;
	GameDraw Game;
	Pole P;
	P.poleCreate();
	Tanks T;
	Spawn S;
	Shot Sh;
	Boom B;
	game = true;
	InvalidateRect(hWnd, NULL, false);
}

void Game::paint(HDC hdc)
{
	SolidBrush blackBrush(Color::Black);
	SolidBrush whiteBrush(Color::White);
	
	// Размер стороны временного буфера
	int bbx = WIDTH * CELL_SIZE;//2 * BORDER + 15 * DIEW; // Ширина
	int bby = HEIGHT * CELL_SIZE + 16;//2 * BORDER + 8 * DIEH; // Высота
    // Временный буфер
	Bitmap backBuffer(bbx, bby);
	// Временный графический контекст
    Graphics bbg(&backBuffer);
	// Включаем сглаживание
	bbg.SetSmoothingMode(SmoothingModeAntiAlias);
	// Заливка всего окна - фон
	bbg.Clear(Color::Black); // Черным цветом
	GameDraw Draw;
	Pole P;
	Tanks T;
	Spawn S;
	Shot Sh;
	Boom B;

	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			P.Draw(i,j);
		}
	}
	S.Draw(1);
	S.Draw(2);
	T.Draw(1);
	T.Draw(2);
	for (int i = 0; i < SHOTS; i++)
	{
		Sh.Draw(1,i);
		Sh.Draw(2,i);
	}
	for (int i = 0; i < SHOTS * 2; i++)
	{
		B.Draw(i);
	}

	Draw.drawScorePanel();

	// Шрифт
	Font font(L"Verdana", 6);
	// Вывод fps
	wstringstream wss;
	wss << fps;
	PointF point(1, 1);
	bbg.DrawString(wss.str().c_str(), wss.str().length(), &font, point, &whiteBrush); 

	if (!game && !noGamesPlayed)
	{
		SolidBrush transparentBrush(Color(224, 0, 0, 0));
		bbg.FillRectangle(&transparentBrush, 0, 0, bbx, bby - 16);
		int w = textures[1]->GetWidth();
		int h = textures[1]->GetHeight();
		bbg.DrawImage(textures[1], (bbx - w) / 2, (bby - h) / 4);

		wstringstream wss;
		if (p1frags == p2frags) wss << L"Ничья!";
		else if (p1frags > p2frags) wss << L"Выиграл 1-ый игрок!";
		else wss << L"Выиграл 2-ой игрок!";

		Font font(L"Verdana", 12, FontStyleBold);

		PointF point(REAL((WIDTH * CELL_SIZE - wss.str().length() * 10) / 2), REAL(((bby - h) / 4) + h * 1.5));
		bbg.DrawString(wss.str().c_str(), wss.str().length(), &font, point, &whiteBrush); 
	}

	// Графический контекст окна
    Graphics g(hdc);
	// Вывод содержимого временного буфера на экран
	g.DrawImage(&backBuffer, 0, 0);
}





























































