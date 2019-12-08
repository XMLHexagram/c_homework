#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include<conio.h>
#include<time.h>
#include<Windows.h>

#define _CRT_SECURE_NO_WARNINGS 1
//绘制游戏信息,卡关(实现),分数(全局);
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
int iWinTimes = 1112;
int iLoseTimes = 1112;

//int iSuccessScore = iRightScore* iWinTimes;
//int iLoseScore = iLoseTime * iErrorScore;

double dNotTypePunishRate=0.5;

//
int iScore = iRightScore;
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
	//设置控制台标题
	SetConsoleTitle(TEXT("TYPE GAME"));

	//隐藏光标
	CONSOLE_CURSOR_INFO cInfo;
	cInfo.bVisible = 0;
	cInfo.dwSize = 1;
	
	SetConsoleCursorInfo(hOut, &cInfo);
	

	//获取控制台宽度
	CONSOLE_WIDTH = bInfo.dwSize.X;

	//控制台高度,暂时无法获取
	CONSOLE_HEIGHT = 30;


	//GetCurrentTime

	time(&curtime);
	//Init Rand
	srand(curtime);

	//游戏欢迎界面
	for (size_t i = 0; i < 100; i++)
	{
		printf("\n");
	}
	goto_xy(0, 0);
	printf("Welcome To Type Game!\n\n\n");
	printf("Please Set The Game Shell Width By Scrolling The Console!!!\n\n\n");


	printf("Type Any Key To Start Game\n\n\n");



	getchar();

	//游戏开始
	for (int iLevel = 1, iLastLevelScore = 0; iScore>=0; )
	{
		//开始游戏
		iLastLevelScore= StartGame(iLevel);
		
		//胜利
		if (iLastLevelScore >= iMission)
		{
			DrawWinShell(iScore, ++iLevel);
		}
		else 
		{
			//分数低于0 或选择放弃游戏
			if (!DrawLoseShell(iScore))
			{
				system("cls");

				printf("Game Over");
				break;
			}
		}
	};

	//结束语
	printf("Thanks for playing!");
}

int StartGame(int iLevel)
{
	system("cls");

	//设置产生字母的间隔时间
	iNewAlphaTime = (int)iInitialTime * (pow(0.8, iLevel));

	//读取控制台宽度
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	CONSOLE_WIDTH = bInfo.dwSize.X;


	printf("%60s", "GAME START\n");

	//开始计时
	start = clock();
	int i = 0;

	
	goto_xy(0, 5);


	//计算 胜利/失败分数
	//iScoreOnNewLevel这个变量似乎没用,可用iScore代替?
	iScoreOnNewLevel = iScore;
	iMission = iRightScore * iWinTimes + iScoreOnNewLevel;
	iDeadline = iScoreOnNewLevel - iErrorScore * iLoseTimes;

	//若失败分数小于0,则设为0(防止出现负分数)
	if (iDeadline < 0) iDeadline = 0;


	//上次系统生成的字母
	char cLastAlpha = 0;


	//上次的字母是否已经敲入
	bool bHasTyped = false;

	//绘制游戏界面
	DrawGameInfo(iLevel);

	//开始生成
	while (1) {
		DrawGameInfo(iLevel);


		//获取运行时间
		now = clock();

		//检测是否敲入字母
		if (_kbhit()&& cLastAlpha)
		{
			//cTypedAplha 上次敲入的字母
			char cTypedAplha = _getch();

			//若当前的字母没有敲入
			if (!bHasTyped)
			{

				//正确输入
				if (cTypedAplha == cLastAlpha || cTypedAplha == cLastAlpha + ('a' - 'A')) {
					iScore += iRightScore;
					//此处可增加反馈:OnInputCorrectly()
					//1.正确输入的字母变绿
					//2.正确输入的音效!

				}
				else
				{
					//错误输入
					iScore -= iErrorScore;
					//此处可增加反馈:OnInputIncorrectly()
					//1.错误输入的字母变红
					//2.错误输入的音效!
				}

				bHasTyped = 1;
				//更新游戏信息
				DrawGameInfo(iLevel);

			}
		}


		//生成新的字母
		if ((now - start) / (CLOCKS_PER_SEC/1000) - i>=iNewAlphaTime) {

			//更新计时器
			i = (now - start) / (CLOCKS_PER_SEC / 1000);



			//若上个字母没有输入,则扣分
			//扣分规则:错误扣分 * 未输入的惩罚倍率
			if (!bHasTyped && cLastAlpha)
			{
				iScore -=(int) (iErrorScore * dNotTypePunishRate);
				//此处增加未输入的反馈
				//1.未输入的字母变灰
				//2.未输入的音效?(生成新字母的音效 和这个重合?)

			}


			//产生新字母
			char cRandChar = rand() % 26 + 'A';
			cLastAlpha = cRandChar;

			//iPos 随机字母的位置
			int iPos = rand() % CONSOLE_WIDTH;
			for (size_t i = 0; i < iPos; i++)
			{
				printf(" ");
			}

			printf("%c\n\n\n\n\n", cRandChar);
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

//绘制游戏信息 : 卡关 , 累计分数 , 目标分数 , 卡关时间
//
//		 ╔══════════════════════════════════════╗
//		 ║										║
//		 ║										║
//		 ║	Info		Info		Info		║
//		 ║										║
//		 ║										║
//		 ║										║
//		 ╚══════════════════════════════════════╝
//
void DrawGameInfo(int level) {

	//获取当前光标位置(备份)
	GetConsoleScreenBufferInfo(hOut, &bInfo);

	//光标移动至最上行
	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 1 : 0);

	//开始绘制
	//制表符: ╦ ╩ ╝ ╔ ╗ ╚ ═ ║ 
	//不要用putchar 会不显示
	printf("╔");
	for (size_t i = 1; i < CONSOLE_WIDTH-1; i++)
	{
		printf("═");
	}
	printf("╗");

	//绘制空行
	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 2 : 1);
	printf("%120c", ' ');

	//开始绘制游戏信息
	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 3 : 2);
	//此处可能会影响后面的字母
	//输出卡关,分数,速度,胜利/失败分数,时间
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

	//恢复光标信息
	goto_xy(bInfo.dwCursorPosition.X, bInfo.dwCursorPosition.Y);

}


//将光标移动到(x,y)
void goto_xy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);
}

//绘制卡关胜利的界面
void DrawWinShell(int iScore,int iNextLevel)
{
	system("cls");
	printf("Congratulation!\n");
	printf("Your Score:%d\n\n\n",iScore);
	printf("Are you ready for next level %d?\nPRESS Y TO GO", iNextLevel);

	//监测玩家抉择
	
	while (1)
	{
		char cInput = _getch();
		if (cInput == 'y' || cInput == 'Y')
		{
			break;
		}

	}
}




//绘制卡关失败的界面
//选择是否重新尝试卡关
//若分数小于0 直接失败
bool DrawLoseShell(int iScore)
{
	//若分数小于0 直接失败
	if (iScore < 0) return false;


	system("cls");

	printf("Oh! You Failed!\n");
	printf("Do you want to play this level again?\n");
	printf("Press Y for Yes or press N for Give Up\n");

	//监测玩家的选择
	while (1)
	{
		char cInput = _getch();
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