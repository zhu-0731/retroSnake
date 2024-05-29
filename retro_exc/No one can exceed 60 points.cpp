
//#define DEBUG
//#define SCREENSHOT
//#define DEBUGautomove
//#define DEBUGINI
#define SHOWMODE
#include<iostream>
#include <graphics.h>
#include <conio.h>
#include<vector>
#include<algorithm>
#include<random>
#include<time.h>
#include<chrono>
#include<thread>
#include<windows.h>
using namespace std;

#define INIX 50 
#define INIY 50 
#define BLOCKWID 21
#define BK_LEN 840
#define BK_WID 630
#define N_LEN 40
#define N_WID 30

std::chrono::time_point<std::chrono::high_resolution_clock> endTime ; // 结束计时
std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

void Automove(char direction, int& n, const int& i, const int& j);
void BKjudge(int& n, const int i, const int j);

struct Button {
	int left;
	int top;
	int right;
	int bottom;
};

class Start_graph
{
public:
}sta;

class SnakeBody
{
public:
	int x;
	int y;
};
vector<SnakeBody>snake;
enum {RIGHT='d',UP='w',LEFT='a',DOWN='s' };
enum {GOOD=1,BAD=0,SPEED=2,STEALTH=4};
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
	int sleep_t=300;//移动速度，初始为三百，每次变化背景*=0.8，最小为150；
	int time_record;//记录游戏时间
}ti;
class UI
{
public:
	void print_score(void)
	{
		settextstyle(30, 20, _T("Microsoft YaHei")); // 设置文字样式
		setcolor(YELLOW); // 设置文字颜色
		TCHAR text[10] ;
		_stprintf(text,__T("%d"),snake.size()); // 在指定位置显示文字
		outtextxy(910, 120, text);
	}
	void print_time(void)
	{
		//time_t now = time(0);
		//tm* ltm = localtime(&now);
		std::chrono::duration<double> elapsed_seconds = endTime - startTime;

		TCHAR timeStr[32];
		_stprintf(timeStr, _T("%.0fs"), elapsed_seconds.count());

		// 设置字体样式
		settextstyle(30, 20, _T("Microsoft YaHei"));

		// 在指定位置显示时间
		outtextxy(1040, 140,timeStr );

	}
	void print_pause()
	{

		setbkmode(WHITE);
		settextstyle(90, 30, _T("Microsoft YaHei")); // 设置文字样式
		setcolor(RGB(255, 0, 0)); // 设置文字颜色
		RECT r1 = { 0, 0, BK_LEN, BK_WID };

		TCHAR text[] = _T("Pause...\nPress enter to continue");
		drawtext(_T("Pause...\nPress enter to continue\n'q/Q' to exit"), &r1, DT_CENTER | DT_VCENTER | DT_WORDBREAK);

		FlushBatchDraw();
	}
	
}ui;

class SNAKE_Stealth
{
	int times = 0;
	int moving=0;
public:
	bool state = 1;
	bool needMove()
	{
		int move_times = max(0, 3 - (snake.size() - 20) / 10);
		if (moving <move_times)
		{
			moving++;
			return 0;
		}
		else
		{
			moving = 0;
			return 1;
		}
	}
	void stealth()
	{
		if (times < 3+0.1*snake.size())
		{
			times++;
		}
		else
		{
			times = 0;
			state = 1;
		}
	}
}steal;

class CONTROL
{
public:
	int increase = 0;//为0pop
	int snakeFound(int xed, int yed,int start_k)
	{
		for (int k = start_k; k < snake.size(); k++)
		{
			if (xed == snake[k].x && yed == snake[k].y)
				return k;

		}
		return -1;
	}

}con;




class FOOD
{

public:
	class FOODINFO
	{
	public:
		int x, y;
	};

	int is_food_here[40][30];
	vector <FOODINFO>food_good_location;
	vector <FOODINFO>food_bad_location;
	vector <FOODINFO>food_speed_location;
	FOODINFO food_steal_location{ 0,0 };

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
		while (con.snakeFound(newfood_x,newfood_y,0)!=-1||is_food_here[newfood_x][newfood_y])
		{
	
			newfood_x += 1;
			newfood_y += 1;
			
		
		}
		// 加入新食物
		if (food_type == GOOD)
		{
			food_good_location.push_back({newfood_x,newfood_y});
		}
		else if (food_type == BAD)
		{
			food_bad_location.push_back({ newfood_x,newfood_y });
		}
		else if (food_type == SPEED)
		{
			food_speed_location.push_back({ newfood_x,newfood_y });
		}
		//隐身食物只重置坐标
		//更改食物存在状态
		is_food_here[newfood_x][newfood_y] = 1;
	}
	//绘制食物
	void draw_food(void)//绘制食物
	{
		IMAGE food;
		loadimage(&food, _T("food.png"));
		IMAGE shit;
		loadimage(&shit, _T("shit.png"));
		IMAGE speed_food;
		loadimage(&speed_food, _T("speed_food.jpg"));
		IMAGE steal_food;
		loadimage(&steal_food, _T("steal.jpg"));


		for (int i = 0; i < food_good_location.size(); i++)
		{
			putimage(food_good_location.at(i).x * BLOCKWID, food_good_location.at(i).y * BLOCKWID, &food);
		}
		if(snake.size()>7)
		for (int i = 0; i < food_bad_location.size(); i++)
		{
			putimage(food_bad_location.at(i).x * BLOCKWID, food_bad_location.at(i).y * BLOCKWID, &shit);
		}
		for (int i = 0; i < food_speed_location.size(); i++)
		{
			putimage(food_speed_location.at(i).x * BLOCKWID, food_speed_location.at(i).y * BLOCKWID, &speed_food);
		}
		
		putimage(food_steal_location.x * BLOCKWID, food_steal_location.y * BLOCKWID, &steal_food);
	}
	
	void initfood(int n,int i,int j)
	{
		
		genefood(n * i/BLOCKWID, n * i / BLOCKWID ,(BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j )/ BLOCKWID, GOOD);
		genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, GOOD);
		genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, BAD);

	}
	void judge_food(int n, const int i, const int j)//吃食物判定
	{
		int eaten_food_location_good = findFood(this->food_good_location);
		int eaten_food_location_bad = findFood(this->food_bad_location);
		int eaten_food_location_speed = findFood(this->food_speed_location);
		//int eaten_food_location_streal = findFood(this->food_speed_location);


		if (food_speed_location.size() == 0 && snake.size() > 10)
		{
			genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, SPEED);
		}
		if (eaten_food_location_good >= 0)
		{
			con.increase = 1;//增长开关调节为1，停止popback一次后在automove模块中被设为0
			this->food_good_location.erase(food_good_location.begin() + eaten_food_location_good);//删除食物
			genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, GOOD);
		}
		
		else if (eaten_food_location_bad >= 0)
		{
			if (snake.size() > 7)
			{
				con.increase = -1;
				this->food_bad_location.erase(food_bad_location.begin() + eaten_food_location_bad);
				genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, BAD);
			}
		}
		else if (eaten_food_location_speed >= 0)
		{
			this->food_speed_location.erase(food_speed_location.begin() + eaten_food_location_speed);
			for (int t = 0; t < 3+(2-n); t++)
			{
				Automove(dr.direc,n,i,j);
			}
			if(food_speed_location.size()<snake.size() && n == 0)
			{
				int sp_size = food_speed_location.size();
				while(food_speed_location.size()<2*sp_size)//分裂函数，因后续还要加入1个故为小于
				{
					genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, SPEED);
				}
			}
			genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, SPEED);
		}
		else if (n<2&&food_steal_location.x == snake[0].x&& food_steal_location.y == snake[0].y)
		{
			steal.state = 0;
			food_steal_location.x = 0;
			food_steal_location.y = 0;
		}
		
	}

	void food_to_pop(int &n,int i,int j)
	{
		judge_food(n, i, j);


		if (con.increase == 0)
			snake.pop_back();
		else if (con.increase == -1)
		{
			int pop_size;
			if (snake.size() <= 30)
			{
				pop_size = 2;
			}
			else
				pop_size = 2 + snake.size() / 15;
			cout << pop_size << endl;
			for (int k = 0; k < pop_size; k++)
			{
				snake.pop_back();
			}
		}
		else if (con.increase == 1)
		{
			BKjudge(n, i, j);
		}
		con.increase = 0;
	}

	void stealFoodMove()
	{
		if (snake[0].x > food_steal_location.x)
		{
			food_steal_location.x++;
		}
		else if(snake[0].x < food_steal_location.x)
		{
			food_steal_location.x--;
		}
		if (snake[0].y > food_steal_location.y)
		{
			food_steal_location.y++;
		}
		else if(snake[0].y < food_steal_location.y)
		{
			food_steal_location.y--;
		}
	}

private:
	int findFood(const vector<FOODINFO>& f)//仅用于判断吃食物
	{
		for (int i = 0; i < f.size(); i++)
		{
			if (f[i].x == snake[0].x && f[i].y == snake[0].y)
			{
				is_food_here[snake[0].x][snake[0].y] = 0;//被吃了
				return i;
			}
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

	IMAGE Card;
	loadimage(&Card, _T("card.png"));
	putimage(BK_LEN, 0, &Card);
	ui.print_score();
	ui.print_time();
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



	

	for (int i = 1; i < snake.size(); i++)
	{
		putimage(snake.at(i).x * BLOCKWID, snake.at(i).y * BLOCKWID, &Body);
	}
	putimage(snake.at(0).x * BLOCKWID, snake.at(0).y * BLOCKWID, &Head);

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
#ifdef SHOWMODE
	DrawBG(n * i, n * i, BK_LEN - n * i, BK_WID - n * j);
	settextstyle(30, 10, _T("Microsoft YaHei")); // 设置文字样式
	setcolor(YELLOW); // 设置文字颜色
	TCHAR text1[] = _T("任意键开始");
	TCHAR text2[] = _T("空格键暂停 回车继续");
	TCHAR text3[] = _T("q键退出");
	outtextxy(240, 240, text1); // 在指定位置显示文字
	outtextxy(200, 280, text2);
	outtextxy(260, 320, text3);
	_getch();

#endif // SHOWMODE
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

void BKjudge(int& n, const int i, const int j)
{
	if (con.increase && (snake.size() % 3==0))
		foods.genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, BAD);
	if (con.increase && (snake.size() % 6==0))
		foods.genefood(n * i / BLOCKWID, n * i / BLOCKWID, (BK_LEN - n * i) / BLOCKWID, (BK_WID - n * j) / BLOCKWID, GOOD);

	if (con.increase && n > 0 && (snake.size() == 20||snake.size() == 45))
	{
		n--;

	}
}

void Automove(char direction,int &n,const int &i,const int&j)
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
			foods.food_to_pop(n, i, j);
			dr.oppo_direc = LEFT;
			break;
	
		case LEFT:
			snake.insert(snake.begin(), { previous_head_x - 1,previous_head_y });
			
			foods.food_to_pop(n, i, j);
			dr.oppo_direc = RIGHT;
			break;

		case UP:
			snake.insert(snake.begin(), { previous_head_x ,previous_head_y-1 });
			foods.food_to_pop(n, i, j);
			dr.oppo_direc = DOWN;
			break;

		case DOWN:
			snake.insert(snake.begin(), { previous_head_x ,previous_head_y+1 });
			//DrawSnake();
			foods.food_to_pop(n, i, j);
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
		char temp_tolower = tolower(temdirec);
		if (tolower(temdirec) == 'q')
		{
			return 1;
		}
		else if (tolower(temdirec) == ' ')
		{
			dr.dr_mem = dr.direc;
			cout << "已暂停，按回车继续" << endl;
			ui.print_pause();

			cin.clear();
			cin.ignore(1000, '\n');
			cin.clear();

			return 0;
		}
		else if (temdirec != dr.oppo_direc&&(temp_tolower==RIGHT||
			temp_tolower==LEFT||temp_tolower==UP||temp_tolower==DOWN))
		{
			dr.direc = temp_tolower;//oppo的改变在移动模块中
		}
		return 0;
	}
}

int diedJudge(int start_x, int start_y, int end_x, int end_y)
{
	if (snake[0].x <= start_x || snake[0].y <= start_y || snake[0].x >= end_x-1 || snake[0].y >= end_y-1)
		return -1;
	if (con.snakeFound(snake[0].x, snake[0].y,1) >0)
		return -1;
	return 0;
}

int main()
{

	
	int i = 4 * BLOCKWID;//边框缩放单位
	int j = 3 * BLOCKWID;
	int n = 2;//单位缩放倍率,应随蛇长而递减，最小为0
	int logic_i = 4;
	int logic_j = 3;

#ifdef SHOWMODE
	//Sleep(5000);

#endif // SHOWMODE

	
	

	init_start(n,i,j);
	PlaySound(TEXT("better-day.wav"), NULL, SND_FILENAME | SND_ASYNC);

#ifdef SCREENSHOT
	Sleep(10000);
#endif // SCREENSHOT 

	startTime = chrono::high_resolution_clock::now(); // 开始计时

	BeginBatchDraw();
	while (1)
	{
		
		endTime = std::chrono::high_resolution_clock::now();//获取时间
		//方向防丢失
		/*cout << dr_mem;
		cout << direc;*/
		cin.clear();
		if (dr.direc!='w'&& dr.direc != 's'&& dr.direc != 'a'&& dr.direc != 'd')
			dr.direc = dr.dr_mem;
		
		//移动模组
		if(move_judge())
			break;

		Automove(dr.direc,n,i,j);
		if (n <2&&steal.needMove())
		{
			foods.stealFoodMove();
		}
		foods.judge_food(n,i,j);
		
		//绘制模组
		cleardevice();
		DrawBG(n * i, n * i, BK_LEN - n * i, BK_WID - n * j);//参数为边框四点坐标
		if(steal.state)
		{
			DrawSnake();
		}
		else
		{
			steal.stealth();
		}
		foods.draw_food();
		//死亡判断模组
		if (diedJudge(n * logic_i, n * logic_i, BK_LEN / BLOCKWID - n * logic_i, BK_WID / BLOCKWID - n * logic_j) == -1)
		{
			Sleep(300);
			cleardevice();
			DrawBG(n * i, n * i, BK_LEN - n * i, BK_WID - n * j);
			DrawSnake();
			setbkmode(WHITE);
			settextstyle(90, 30, _T("Microsoft YaHei")); // 设置文字样式
			setcolor(RED); // 设置文字颜色
			TCHAR text[] = _T("Y O U   D I E D !");
			outtextxy(180, 240,text); // 在指定位置显示文字
			//Sleep(2000);
			break;
		}

		//控制速度
		Sleep(ti.sleep_t);
		if (ti.sleep_t >= 150)
			ti.sleep_t = 300 - (snake.size() - 9) * 10;

		FlushBatchDraw();
	}
	EndBatchDraw();


	Sleep(10000);
	_getch();

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	main();
}

