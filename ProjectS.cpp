#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include<conio.h>
#include<time.h>
#include<Windows.h>

#define _CRT_SECURE_NO_WARNINGS 1
//绘制游戏信息,卡关(未实现),分数(实现);
void DrawGameInfo(int level, int score);

//设置光标位置
void goto_xy(int x, int y);

//游戏运行
void StartGame(int level);



HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
CONSOLE_CURSOR_INFO cInfo;

//控制台宽,高
int CONSOLE_WIDTH;
int CONSOLE_HEIGHT;

//正确,错误的分数
int iRightScore=1000;
int iErrorScore=500;


//时间相关
time_t curtime;
clock_t start;
clock_t now;

int main()
{
	//初始化
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hOut, &bInfo);

	system("cls");
	SetConsoleTitle(TEXT("TYPE GAME"));

	//printf("Current Console Width:%d %d\n", bInfo.dwSize.X, bInfo.dwSize.Y);
	//获取控制台宽度
	CONSOLE_WIDTH = bInfo.dwSize.X;

	//控制台高度,暂时无法获取
	CONSOLE_HEIGHT = 30;


	//GetCurrentTime

	time(&curtime);
	//Init Rand
	srand(curtime);

	printf("Welcome To Type Game!\n\n\n");

	printf("Please Set The Game Shell Width By Scrolling The Console!!!\n\n\n");


	printf("Type Any Key To Start Game\n\n\n");



	getchar();
	system("cls");


	//读取控制台宽度
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	CONSOLE_WIDTH = bInfo.dwSize.X;


	printf("%60s", "GAME START\n");
	start = clock();
	int i = 0;
	int iScore = 0;

	DrawGameInfo(1, 0);
	goto_xy(0, 5);

	//上次系统生成的字母
	char cLastAlpha = 0;


	//上次的字母是否已经敲入
	bool bHasTyped = false;

	while (1) {
		now = clock();

		



		//检测是否敲入字母
		if (_kbhit()) {

			//cTypedAplha 上次敲入的字母
			char cTypedAplha = _getch();

			//若上次的字母没有敲入
			if (!bHasTyped)
			{
				if (cTypedAplha == cLastAlpha || cTypedAplha == cLastAlpha + ('a' - 'A')) {
					iScore += iRightScore;
				}
				else
				{
					iScore -= iErrorScore;
				}

				bHasTyped = 1;
				DrawGameInfo(1, iScore);

			}

		}


		//生成新的字母
		if ((now - start) / CLOCKS_PER_SEC - i) {


			i = (now - start) / CLOCKS_PER_SEC;

			//产生新字母
			char cRandChar = rand() % 26 + 'A';
			cLastAlpha = cRandChar;

			//iPos 字母的位置
			int iPos = rand() % CONSOLE_WIDTH;
			for (size_t i = 0; i < iPos; i++)
			{
				printf(" ");
			}
			
			printf("%c\n\n", cRandChar);
			bHasTyped = 0;
			DrawGameInfo(1, iScore);
		}


	}


}




void DrawGameInfo(int level, int score) {
	//SaveCurrentCursorPos
	GetConsoleScreenBufferInfo(hOut, &bInfo);


	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 1 : 0);
	for (size_t i = 0; i < CONSOLE_WIDTH; i++)
	{
		putchar('-');
	}
	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 2 : 1);
	printf("%120c", ' ');

	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 3 : 2);
	//此处可能会影响后面的字母
	printf("LEVEL:%3d SCORE:%5d%100c", level, score, ' ');

	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 4 : 3);
	printf("%120c", ' ');

	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 5 : 4);
	for (size_t i = 0; i < CONSOLE_WIDTH; i++)
	{
		putchar('-');
	}

	//ResetCurPos
	goto_xy(bInfo.dwCursorPosition.X, bInfo.dwCursorPosition.Y);

}
void goto_xy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);
}