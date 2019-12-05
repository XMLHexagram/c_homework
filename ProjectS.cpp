#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include<conio.h>
#include<time.h>
#include<Windows.h>

#define _CRT_SECURE_NO_WARNINGS 1
//绘制游戏信息,卡关(未实现),分数(实现);
void DrawGameInfo(int level);

//设置光标位置
void goto_xy(int x, int y);

//游戏运行
int StartGame(int iLevel);

//游戏胜负界面
void DrawWinShell(int iScore, int iNextLevel);
bool DrawLoseShell(int iScore);


HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
CONSOLE_CURSOR_INFO cInfo;

//控制台宽,高
int CONSOLE_WIDTH;
int CONSOLE_HEIGHT;

//正确,错误的分数
int iRightScore=100;
int iErrorScore=90;

//胜利/失败 次数
int iWinTimes = 2;
int iLoseTimes = 2;

//int iSuccessScore = iRightScore* iWinTimes;
//int iLoseScore = iLoseTime * iErrorScore;

double dNotTypePunishRate=0.5;

//
int iScore = 0;
int iScoreOnNewLevel = 0;

//每个卡关实时胜利/失败分数
int iMission = 0;
int iDeadline = 0;



//初始卡关时间(毫秒)
int iInitialTime = 2000;
int iNewAlphaTime;

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
	//游戏开始
	for (int iLevel = 1, iLastLevelScore = 0; iScore>=0; )
	{

		iLastLevelScore= StartGame(iLevel);
		
		if (iLastLevelScore >= iMission)
		{
			DrawWinShell(iScore, ++iLevel);
		}
		else 
		{
			if (!DrawLoseShell(iScore))
			{
				system("cls");

				printf("Game Over");
				break;
			}
		}
	};
	printf("Thanks for playing!");
}

int StartGame(int iLevel)
{
	system("cls");
	iNewAlphaTime = (int)iInitialTime * (pow(0.8, iLevel));

	//读取控制台宽度
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	CONSOLE_WIDTH = bInfo.dwSize.X;


	printf("%60s", "GAME START\n");
	start = clock();
	int i = 0;

	
	goto_xy(0, 5);


	//胜利/失败分数
	iScoreOnNewLevel = iScore;
	iMission = iRightScore * iWinTimes + iScoreOnNewLevel;
	iDeadline = iScoreOnNewLevel - iErrorScore * iLoseTimes;
	if (iDeadline < 0) iDeadline = 0;
	//上次系统生成的字母
	char cLastAlpha = 0;


	//上次的字母是否已经敲入
	bool bHasTyped = false;

	//开始生成字母
	DrawGameInfo(iLevel);
	while (1) {
		now = clock();





		//检测是否敲入字母
		if (_kbhit()&& cLastAlpha)
		{

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
				DrawGameInfo(iLevel);

			}

			

		}


		//生成新的字母
		if ((now - start) / (CLOCKS_PER_SEC/1000) - i>=iNewAlphaTime) {


			i = (now - start) / (CLOCKS_PER_SEC / 1000);
			//i-=1000;

			//若上个字母没有反应,则扣分
			//扣分规则:错误扣分*无反应惩罚倍率
			if (!bHasTyped && cLastAlpha)
			{
				iScore -=(int) (iErrorScore * dNotTypePunishRate);
			}


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
			DrawGameInfo(iLevel);
		}
		//游戏结束判断
		if (iScore < iDeadline) {
			return iScore;
		}
		else if (iScore >= iMission)
		{

			return iScore;
		}

	}

	return 0;
}


void DrawGameInfo(int level) {
	//SaveCurrentCursorPos
	GetConsoleScreenBufferInfo(hOut, &bInfo);


	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 1 : 0);

	printf("╔");
	for (size_t i = 1; i < CONSOLE_WIDTH-1; i++)
	{
		// ╦ ╩ ╝ ╔ ╗ ╚ ═ ║ 
		//putchar('╦╩╝╔╗╚═║');
		printf("═");
	}
	printf("╗");


	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 2 : 1);
	printf("%120c", ' ');

	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 3 : 2);
	//此处可能会影响后面的字母
	//输出卡关,分数,速度,胜利/失败分数
	printf("LEVEL:%3d SCORE:%5d SPEED:%4dms Mission:%4d Deadline:%4d %100c",
		level, iScore, iNewAlphaTime, iMission,iDeadline , ' ');

	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 4 : 3);
	printf("%120c", ' ');

	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 5 : 4);


	printf("╚");
	for (size_t i = 1; i < CONSOLE_WIDTH-1; i++)
	{
		printf("═");
	}
	printf("╝");

	//ResetCurPos
	goto_xy(bInfo.dwCursorPosition.X, bInfo.dwCursorPosition.Y);

}
void goto_xy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);
}


void DrawWinShell(int iScore,int iNextLevel)
{
	system("cls");
	printf("Congratulation!\nYour Score:%d\n\n\nAre you ready for next level %d?\nPRESS Y TO GO", iScore,iNextLevel);
	while (1)
	{
		char cInput = getchar();
		if (cInput == 'y' || cInput == 'Y')
		{
			break;
		}

	}
}





bool DrawLoseShell(int iScore)
{
	if (iScore < 0) 
	{
		return false;
	}
	system("cls");
	printf("Oh! You Failed!\nDo you want to play this level again?\nPress Y for Yes or press N for no", iScore);
	while (1)
	{
		char cInput = getchar();
		if (cInput == 'y' || cInput == 'Y')
		{
			return true;
		}
		else if(cInput == 'n' || cInput == 'N')
		{
			return false;
		}
	}
}