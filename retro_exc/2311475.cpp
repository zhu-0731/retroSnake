
//#define DEBUG
//#define SCREENSHOT
//#define DEBUGautomove
//#define DEBUGINI
#include<iostream>
#include <graphics.h>
#include <conio.h>
#include<vector>
#include<algorithm>
#include<random>
using namespace std;

#define INIX 50 
#define INIY 50 
#define BLOCKWID 21
#define BK_LEN 840
#define BK_WID 630
#define N_LEN 40
#define N_WID 30

enum {RIGHT='d',UP='w',LEFT='a',DOWN='s' };
enum {GOOD=1,BAD=0};
class DIRECT//方向变量
{
public:
	char direc = LEFT;
	char oppo_direc = RIGHT;
	char dr_mem;
}dr;
class TIME
{
public:
	int sleep_t=500;//移动速度，初始为五百，每次变化背景*=0.8，最小为150；
	int time_record;//记录游戏时间
}ti;
class CONTROL
{
public:
	int increase = 0;//为0pop
	

}con;


class SnakeBody
{
public:
	int x;
	int y;
};
vector<SnakeBody>snake;

class FOOD
{

public:
	class FOODINFO
	{
	public:
		int x, y;
	};

	vector <FOODINFO>food_good_location;
	vector <FOODINFO>food_bad_location;

	//生成食物坐标
	void genefood(int bord_x1, int bord_y1, int bord_x2,int bord_y2,int food_type)
	{
		// 使用随机设备作为种子
		std::random_device rd;

		// 使用 Mersenne Twister 引擎和随机设备种子初始化
		std::mt19937 gen(rd());

		// 定义随机数分布
		std::uniform_int_distribution<> dis_x(bord_x1+2, bord_x2-3);
		std::uniform_int_distribution<> dis_y(bord_y1+2, bord_y2-3);

		// 生成随机数
		int newfood_x = dis_x(gen);
		int newfood_y = dis_y(gen);

		// 加入新食物
		if (food_type == GOOD)
		{
			food_good_location.push_back({newfood_x,newfood_y});
		}
		else if (food_type == BAD)
		{
			food_bad_location.push_back({ newfood_x,newfood_y });
		}
	}
	//绘制食物
	void draw_food(void)//绘制食物
	{
		IMAGE food;
		loadimage(&food, _T("food.png"));
		IMAGE shit;
		loadimage(&shit, _T("shit.png"));
		for (int i = 0; i < food_good_location.size(); i++)
		{
			putimage(food_good_location.at(i).x * BLOCKWID, food_good_location.at(i).y * BLOCKWID, &food);
		}
		for (int i = 0; i < food_bad_location.size(); i++)
		{
			putimage(food_bad_location.at(i).x * BLOCKWID, food_bad_location.at(i).y * BLOCKWID, &shit);
		}

	}
	
	void initfood(int n,int i,int j)
	{
		
		genefood(n * i/BLOCKWID, n * i / BLOCKWID ,(BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j )/ BLOCKWID, GOOD);
		genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, GOOD);
		genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, BAD);

	}
	void judge_food(void)//吃食物判定
	{
		int eaten_food_location_good = findFood(this->food_good_location);
		int eaten_food_location_bad = findFood(this->food_bad_location);
		if (eaten_food_location_good >= 0)
		{
			con.increase = 1;//增长开关调节为1，停止popback一次后在automove模块中被设为0
			this->food_good_location.erase(eaten_food_location_good);
		}
	}


private:
	int findFood(const vector<FOODINFO>& f)
	{
		for (int i = 0; i < f.size(); i++)
		{
			if (f[i].x == snake[0].x && f[i].y == snake[0].y)
				return i;
		}

		return-1;
	}

}foods;


int snake_ini[9][2] =
{
	{20,13},{21,13},{21,14},{21,15},{20,15},{19,15},{19,16},{19,17},{20,17}
};

void DrawBG(int start_x, int start_y, int end_x, int end_y)
{
	//已解锁地图
	setfillcolor(RGB(229, 242, 191));
	solidrectangle(start_x + BLOCKWID, start_y + BLOCKWID, end_x - BLOCKWID, end_y - BLOCKWID);

	//绘制边框

	for (int i = 0; i < BLOCKWID / 2; i++)
	{
		setcolor(RGB(255 - 20 * i, 6 * i, 2 * i));
		rectangle(start_x + BLOCKWID - i, start_y + BLOCKWID - i, end_x - BLOCKWID + i, end_y - BLOCKWID + i);
	}

	setlinecolor(RGB(46, 56, 28));
	setlinestyle(PS_DASH, 1);

	//绘制格子
	for (int i = 0; i <= BK_LEN; i += BLOCKWID)
	{
		line(i, 0, i, BK_WID);
	}
	for (int j = 0; j <= BK_WID; j += BLOCKWID)
	{
		line(0, j, BK_LEN, j);
	}
	//侧边栏绘制
	//setfillcolor(RGB(145, 156, 118));
	//solidrectangle(BK_LEN, 0, BK_LEN + 15 * BLOCKWID, BK_WID);
	IMAGE Card;
	loadimage(&Card, _T("card.png"));
	putimage(BK_LEN, 0, &Card);
}

void snakeinit(void)
{
	IMAGE Head;
	loadimage(&Head, _T("head.png"));
	IMAGE Body;
	loadimage(&Body, _T("body.png"));



	//getimage(&Head, 0, 0, BLOCKWID, BLOCKWID);
	putimage(snake_ini[0][0] * BLOCKWID, snake_ini[0][1] * BLOCKWID, &Head);

	for (int i = 1; i < 9; i++)
	{
		//getimage(&Body, 0, 0, BLOCKWID, BLOCKWID);
		putimage(snake_ini[i][0] * BLOCKWID, snake_ini[i][1] * BLOCKWID, &Body);
	}

}



void DrawSnake(void)
{
	IMAGE Head;
	loadimage(&Head, _T("head.png"));
	IMAGE Body;
	loadimage(&Body, _T("body.png"));



	//getimage(&Head, 0, 0, BLOCKWID, BLOCKWID);
	putimage(snake.at(0).x * BLOCKWID, snake.at(0).y * BLOCKWID, &Head);

	for (int i = 1; i < snake.size(); i++)
	{
		//getimage(&Body, 0, 0, BLOCKWID, BLOCKWID);
		putimage(snake.at(i).x * BLOCKWID, snake.at(i).y * BLOCKWID, &Body);
	}

#ifdef DEBUG

	cout << "move:" << dr.direc << endl;
	cout << "now:" << endl;
	for (int i = 0; i < snake.size(); i++)
	{
		cout << "(" << snake.at(i).x << "," << snake.at(i).y << ")";
		cout << " ";
	}
	cout << endl;

#endif // DEBUG
}

void init_start(int n,int i,int j)
{
	//初始化
	initgraph(BK_LEN + 15 * BLOCKWID, BK_WID);
	setbkcolor(RGB(46, 56, 28));
	cleardevice();

	foods.initfood(n, i, j);
	cout << foods.food_bad_location.size() << foods.food_bad_location[0].x<< foods.food_bad_location[0].y;
	
	for (int c = 0; c < 3; c++)
	{
		cleardevice();
		DrawBG(n * i, n * i, BK_LEN - n * i, BK_WID - n * j);
		Sleep(500);
		
		snakeinit();
		foods.draw_food();
		Sleep(900);
		
	}
	
#ifdef DEBUG

	cout << "the initial one is:" << endl;
	for (int i = 0; i < 9; i++)
	{
		cout << "(" << snake_ini[i][0] << "," << snake_ini[i][1] << ")";
		cout << " ";
	}
	cout << endl;

#endif // DEBUG
	//移动到snakeclass
	for (int i = 0; i < 9; i++)
	{
		snake.push_back({ snake_ini[i][0],snake_ini[i][1] });
	}
#ifdef DEBUGINI

	cout << "the initial one is:" << endl;
	for (int i = 0; i < 9; i++)
	{
		cout << "(" << snake.at(i).x << "," << snake.at(i).y << ")";
		cout << " ";
	}
	cout << endl;

#endif // DEBUG
}

void Automove(char direction)
{
	int previous_head_x = snake.at(0).x;
	int previous_head_y = snake.at(0).y;
	direction = tolower(direction);
	switch (direction)
	{
#ifdef DEBUG
		cout << "move:"<< direction << endl;
#endif // DEBUG

	case RIGHT:
			snake.insert(snake.begin(), { previous_head_x + 1,previous_head_y });
			//DrawSnake();
			if(!con.increase)
				snake.pop_back();
			dr.oppo_direc = LEFT;
			break;
	
		case LEFT:
			snake.insert(snake.begin(), { previous_head_x - 1,previous_head_y });
			if (!con.increase)
				snake.pop_back();
			//DrawSnake();
			con.increase = 0;
			dr.oppo_direc = RIGHT;
			break;
		case UP:
			snake.insert(snake.begin(), { previous_head_x ,previous_head_y-1 });
			//DrawSnake();
			if (!con.increase)
				snake.pop_back();
			con.increase = 0;
			dr.oppo_direc = DOWN;
			break;
		case DOWN:
			snake.insert(snake.begin(), { previous_head_x ,previous_head_y+1 });
			//DrawSnake();
			if (!con.increase)
				snake.pop_back();
			con.increase = 0;
			dr.oppo_direc = UP;
			break;
	}
#ifdef DEBUGautomove

	cout << "now move :" << direction<<endl;
	for (int i = 0; i < 9; i++)
	{
		cout << "(" << snake.at(i).x << "," << snake.at(i).y << ")";
		cout << " ";
	}
	cout << endl;

#endif // DEBUG
}

int  move_judge(void)
{
	//移动模组
	if (_kbhit())
	{
		char temdirec = _getch();
		if (tolower(temdirec) == 'q')
		{
			return 1;
		}
		if (tolower(temdirec) == ' ')
		{
			dr.dr_mem = dr.direc;
			cout << "已暂停，按回车继续" << endl;
			/*cleardevice();
			DrawBG(n * i, n * i, BK_LEN - n * i, BK_WID - n * j);
			DrawSnake();*/
			cin.clear();
			cin.ignore(1000, '\n');
			cin.clear();
			//_getch();
			//Sleep(300);
			//cout << "pause" << endl;
			//continue;
			//direc = dr_mem;
			return 0;
		}
		if (temdirec != dr.oppo_direc)
		{
			dr.direc = temdirec;//oppo的改变在移动模块中
		}
		return 0;
	}
}


int main()
{


	int i = 4 * BLOCKWID;//边框缩放单位
	int j = 3 * BLOCKWID;
	int n = 2;//单位缩放倍率,应随蛇长而递减，最小为0
	


	init_start(n,i,j);

#ifdef SCREENSHOT
	Sleep(10000);
#endif // SCREENSHOT 

	BeginBatchDraw();
	while (1)
	{
		//方向防丢失
		/*cout << dr_mem;
		cout << direc;*/
		if (dr.direc!='w'&& dr.direc != 's'&& dr.direc != 'a'&& dr.direc != 'd')
			dr.direc = dr.dr_mem;
		
		//移动模组
		if(move_judge())
			break;
			
		Automove(dr.direc);
		
		cleardevice();
		DrawBG(n * i, n * i, BK_LEN - n * i, BK_WID - n * j);//参数为边框四点坐标
		DrawSnake();
		foods.draw_food();
		
		

		Sleep(ti.sleep_t);
		FlushBatchDraw();
	}
	EndBatchDraw();



	_getch();

}