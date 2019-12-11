#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include<conio.h>
#include<time.h>
#include<Windows.h>
#include<mmsystem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#pragma comment(lib, "WINMM.LIB")

#define _CRT_SECURE_NO_WARNINGS 1
//色彩库
#define Red SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_INTENSITY);
#define Blue SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|FOREGROUND_INTENSITY);
#define Green SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#define White SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#define Yellow SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#define Pink SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
#define Cyan SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
//使用后如要恢复原色请使用以下宏定义
#define YuanSe SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_INTENSITY);

//字母结构体
typedef struct Word
{
	char iPos;
	char cWord;
	char iPosY;
	Word* pNext;
}Word;
Word* pFirstWord; //= (Word*)malloc(sizeof(Word));
Word* pCurWord = pFirstWord;


//绘制游戏信息,卡关(实现),分数(全局);
void DrawGameInfo(int level);

//设置光标位置
void goto_xy(int x, int y);

//游戏运行
int StartGame(int iLevel);

//游戏胜负界面
void DrawWinShell(int iScore, int iNextLevel);
bool DrawLoseShell(int iScore);

//音乐播放函数
void play_music();

//字母色彩变化，利用视觉延迟效果形成彩色
void colour();

void DeleteWord(Word* pTmp)
{
	//删去字母
	goto_xy(pFirstWord->iPos, pFirstWord->iPosY);
	printf(" ");

	pFirstWord = pTmp->pNext;
}

char* GetStartText();
char* GetGoText();
char* GetGameOverText();
char* GetLevelText();



HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
CONSOLE_CURSOR_INFO cInfo;

//控制台宽,高
int CONSOLE_WIDTH;
int CONSOLE_HEIGHT;

//正确,错误的分数
int iRightScore = 10;
int iErrorScore = 5;

//胜利/失败 次数
int iWinTimes = 10;
int iLoseTimes = 10;

//int iSuccessScore = iRightScore* iWinTimes;
//int iLoseScore = iLoseTime * iErrorScore;

double dNotTypePunishRate = 0.5;

//初始分数
int iScore = 10000;
int iScoreOnNewLevel = 0;

//每个卡关实时胜利/失败分数
int iMission = 0;
int iDeadline = 0;

char iWordPos[40];

//初始卡关时间(毫秒)
int iInitialTime = 1000;
int iNewAlphaTime;
int iFallTime = 150;


//时间相关
time_t curtime;
clock_t start;
clock_t now;

int main()
{
	//初始化
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	
	//清屏
	system("cls");
	//设置控制台标题
	SetConsoleTitle(TEXT("TYPE GAME"));

	//设置控制台大小
	system("mode con cols=91 lines=29  ");
	//锁死控制台边框大小，防止因为拉伸控制台边框大小而导致的错误
	//（同时此处建议可以扩大控制台的大小
	HWND hWnd = GetConsoleWindow(); //获得cmd窗口句柄
	RECT rc;
	GetWindowRect(hWnd, &rc); //获得cmd窗口对应矩形
	//改变cmd窗口风格
	SetWindowLongPtr(hWnd,
		GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
	SetWindowPos(hWnd,
		NULL,
		rc.left,
		rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		NULL);



	//隐藏光标
	CONSOLE_CURSOR_INFO cInfo;
	cInfo.bVisible = 0;
	cInfo.dwSize = 1;

	SetConsoleCursorInfo(hOut, &cInfo);


	//获取控制台宽度
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	CONSOLE_WIDTH = bInfo.dwSize.X;
	//控制台高度,暂时无法获取
	CONSOLE_HEIGHT = 28;


	//GetCurrentTime

	time(&curtime);
	//Init Rand
	srand(curtime);

	//游戏欢迎界面

	goto_xy(0, 3);
	Cyan
	printf(GetStartText());
	goto_xy(34, 17);
	Yellow
	printf("欢迎来到打字游戏!\n\n\n");

	goto_xy(33, 22);
	Red
	printf("按下任何键来开始游戏\n");
	goto_xy(30, 23);
	Green
	printf("Type Any Key To Start Game\n\n\n");



	
	_getch();
	
	SetConsoleCursorInfo(hOut, &cInfo);

	//debug
	DrawWinShell(1, 1);

	_getch();
	//debug


	//游戏开始
	for (int iLevel = 1, iLastLevelScore = 0; iScore >= 0; )
	{
		//开始游戏
		iLastLevelScore = StartGame(iLevel);
		if(iLevel%4==0)
			mciSendString(L"close start", NULL, 0, NULL);
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
				Red
				printf("Game Over");
				break;
			}
		}
	};

	//结束语
	Blue
	printf("Thanks for playing!");
}

int StartGame(int iLevel)
{
	system("cls");

	//设置产生字母的间隔时间
	iNewAlphaTime = (int)iInitialTime * (pow(0.85, iLevel));
	iWinTimes = (int)iWinTimes * (pow(1.1, iLevel));
	iLoseTimes = (int)iLoseTimes * (pow(1.2, iLevel));
	if (iNewAlphaTime < iFallTime)
	{
		iFallTime = iNewAlphaTime * 0.5;
	}



	//背景音乐产生(运用mci播放背景音乐)
	play_music();
	

	//读取控制台宽度
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	CONSOLE_WIDTH = bInfo.dwSize.X;

	Pink
	printf("%60s", "GAME START\n");

	//开始计时
	start = clock();
	int iTimeInDrop = 0;
	int iTimeInNewWord = 0;


	//初始化字母链表
	pFirstWord = (Word*)malloc(sizeof(Word));
	pFirstWord->pNext = 0;
	pCurWord = pFirstWord;
	//计算 胜利/失败分数
	//iScoreOnNewLevel这个变量似乎没用,可用iScore代替?
	iScoreOnNewLevel = iScore;
	iMission = iRightScore * iWinTimes + iScoreOnNewLevel;
	iDeadline = iScoreOnNewLevel - iErrorScore * iLoseTimes;

	//若失败分数小于0,则设为0(防止出现负分数)
	if (iDeadline < 0) iDeadline = 0;



	//上次的字母是否已经敲入
	bool bHasTyped = false;

	//---------------------------------------
	//绘制游戏界面
	DrawGameInfo(iLevel);
	//画底部
	goto_xy(0, CONSOLE_HEIGHT);
	// ╦ ╩ ╝ ╔ ╗ ╚ ═ ║ 
	/*
			╔═╦═╗
			║ ║ ║
			╠═╬═╣
			║ ║ ║
			╚═╩═╝
	*/
	YuanSe
	printf("╚");
	for (size_t i = 0; i < CONSOLE_WIDTH - 3; i++)
	{
		printf("═");
	}
	printf("╝");
	//输出左右边框
	for (size_t i = 0; i < CONSOLE_HEIGHT; i++)
	{
		goto_xy(0, i);
		printf("║");
		goto_xy(CONSOLE_WIDTH-2, i);
		printf("║");
	}


	//开始生成
	while (1) {
		//暂时不Draw了
		//DrawGameInfo(iLevel);

		//获取运行时间
		now = clock();

		//检测是否敲入字母
		if (_kbhit() && pFirstWord->cWord)
		{
			//cTypedAplha 上次敲入的字母
			char cTypedAplha = _getch();

			//若当前的字母没有敲入
			//if (!bHasTyped)
			//若当前屏幕有字母
			if (pFirstWord->cWord > 0)
			{

				//正确输入
				if (cTypedAplha == pFirstWord->cWord || cTypedAplha == pFirstWord->cWord + ('a' - 'A')) {
					iScore += iRightScore;
					DeleteWord(pFirstWord);
					PlaySound(TEXT("../res/Right.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
					//此处可增加反馈:OnInputCorrectly()
					//1.正确输入的字母变绿
					//2.正确输入的音效!
				}
				else
				{
					//错误输入
					iScore -= iErrorScore;
					PlaySound(TEXT("../res/Wrong.wav"), NULL, SND_ASYNC | SND_NODEFAULT);
					//此处可增加反馈:OnInputIncorrectly()
					//1.错误输入的字母变红
					//2.错误输入的音效!
				}

				bHasTyped = 1;
				//更新游戏信息
				DrawGameInfo(iLevel);

			}
		}

		//整体下落

		int iGameInfoLength = 5;
		int iGameBufferHeight = CONSOLE_HEIGHT - iGameInfoLength - 1;

		if ((now - start) / (CLOCKS_PER_SEC / 1000) - iTimeInDrop >= iFallTime) {

			//更新计时器
			iTimeInDrop = (now - start) / (CLOCKS_PER_SEC / 1000);


			//使用新的掉落方式
			for (Word* pTmp = pFirstWord; pTmp->pNext; pTmp = pTmp->pNext)
			{
				if (pTmp->iPosY > (iGameInfoLength + iGameBufferHeight - 1))
				{
					//若上个字母没有输入,则扣分
					//扣分规则:错误扣分 * 未输入的惩罚倍率
					if (!bHasTyped && pFirstWord->cWord)
					{
						iScore -= (int)(iErrorScore * dNotTypePunishRate);
						//此处增加未输入的反馈
						//1.未输入的字母变灰
						//2.未输入的音效?(生成新字母的音效 和这个重合?)

					}

					DeleteWord(pTmp);
					pTmp = pTmp->pNext;
					////删去此字母
					//goto_xy(pFirstWord->iPos, pFirstWord->iPosY);
					//printf(" ");
					//pFirstWord = pTmp->pNext;
					//pTmp = pTmp->pNext;

				}

				//将字母下移一行
				goto_xy((int)pTmp->iPos, (int)pTmp->iPosY);
				printf(" ");
				pTmp->iPosY++;
				goto_xy(pTmp->iPos, pTmp->iPosY);
				colour();
				printf("%c", pTmp->cWord);

			}


			//bHasTyped = 0;
			DrawGameInfo(iLevel);
		}


		//生成新的字母
		if ((now - start) / (CLOCKS_PER_SEC / 1000) - iTimeInNewWord >= iNewAlphaTime) {

			//更新计时器
			iTimeInNewWord = (now - start) / (CLOCKS_PER_SEC / 1000);



			//产生新字母
			char cRandChar = rand() % 26 + 'A';
			pCurWord->cWord = cRandChar;

			//iPos = 随机字母的位置
			int iPos = rand() % (CONSOLE_WIDTH-6) + 3;
			goto_xy(iPos, iGameInfoLength + 1);
			colour();
			printf("%c", cRandChar);

			pCurWord->iPos = iPos;
			pCurWord->iPosY = iGameInfoLength + 1;
			pCurWord->cWord = cRandChar;
			pCurWord->pNext = (Word*)malloc(sizeof(Word));
			pCurWord = pCurWord->pNext;
			pCurWord->pNext = 0;


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
	goto_xy(0, 0);
	//goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 2 : 0);

	//开始绘制       
	//制表符: ╦ ╩ ╝ ╔ ╗ ╚ ═ ║ ╠   ╬ ╠   ╦ ╬ ╩   ╣ 
/*
			╔═╦═╗
			║ ║ ║
			╠═╬═╣
			║ ║ ║
			╚═╩═╝
*/
//不要用putchar 会不显示
	YuanSe
	printf("╔");
	for (size_t i = 1; i < CONSOLE_WIDTH - 2; i++)
	{
		printf("═");
	}
	printf("╗");


	//开始绘制游戏信息
	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 3 : 2);
	Cyan
	//输出卡关,分数,速度,胜利/失败分数,时间
	goto_xy(10, 2);
	printf("LEVEL:%3d SPEED:%4dms SCORE:%5d Mission:%4d Deadline:%4d",
		level, iNewAlphaTime, iScore,iMission, iDeadline);



	goto_xy(0, bInfo.dwCursorPosition.Y > CONSOLE_HEIGHT ? bInfo.dwCursorPosition.Y - CONSOLE_HEIGHT + 5 : 4);

	YuanSe
	printf("╠");
	for (size_t i = 1; i < CONSOLE_WIDTH - 2; i++)
	{
		printf("═");
	}
	printf("╣");

	//恢复光标信息
	goto_xy(bInfo.dwCursorPosition.X, bInfo.dwCursorPosition.Y - 1);

}


//将光标移动到(x,y)
void goto_xy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);
}

//绘制卡关胜利的界面
void DrawWinShell(int iScore, int iNextLevel)
{
	system("cls");

	goto_xy(0, 2);
	char* a = GetLevelText();
	Green
	printf(a);
	int line = 10;
	int col = 33;
	goto_xy(col, line++);
	White
	printf("╔══════════════════╗\n"); goto_xy(col, line++);
	printf("║                  ║\n"); goto_xy(col, line++);
	printf("║    Your Score    ║\n"); goto_xy(col, line++);
	printf("║      %6d      ║\n",iScore); goto_xy(col, line++);
	printf("║                  ║\n"); goto_xy(col, line++);
	printf("╚══════════════════╝\n\n", iScore); goto_xy(col-3, line+5);
	Red
	printf("Are you ready for LEVEL - %d?\n\n", iNextLevel);
	Pink
	puts(GetGoText());

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
	Red
	printf(GetGameOverText());
	Yellow
	printf("\n\n\nDo you want to play this level again?\n");
	White
	printf("Press Y for Yes or press N for Give Up\n");

	//监测玩家的选择
	while (1)
	{
		char cInput = _getch();
		if (cInput == 'y' || cInput == 'Y')
		{
			return true;
		}
		else if (cInput == 'n' || cInput == 'N')
		{
			return false;
		}
	}
}
void play_music()
{
	//背景音乐部分
	char str[50];
	int r = 1;
	r = rand() % 5 + 1;
	switch (r)
	{
	case 1:
		mciSendString(L"open ../res/bgm1.mp3 alias start", NULL, 0, NULL);
		mciSendString(L"play start repeat", NULL, 0, NULL);
		break;
	case 2:
		mciSendString(L"open ../res/bgm2.mp3 alias start", NULL, 0, NULL);
		mciSendString(L"play start repeat", NULL, 0, NULL);
		break;
	case 3:
		mciSendString(L"open ../res/bgm3.mp3 alias start", NULL, 0, NULL);
		mciSendString(L"play start repeat", NULL, 0, NULL);
		break;
	case 4:
		mciSendString(L"open ../res/bgm4.mp3 alias start", NULL, 0, NULL);
		mciSendString(L"play start repeat", NULL, 0, NULL);
		break;
	case 5:
		mciSendString(L"open ../res/bgm5.mp3 alias start", NULL, 0, NULL);
		mciSendString(L"play start repeat", NULL, 0, NULL);
		break;
	case 6:
		mciSendString(L"open ../res/bgm6.mp3 alias start", NULL, 0, NULL);
		mciSendString(L"play start repeat", NULL, 0, NULL);
		break;
	}
	
}
char* GetLevelText()
{
	char* str = (char*)calloc(1000,1);
	FILE* file;
	fopen_s(&file, "resource/LEVEL.txt", "r");
	fread(str, 666, 1, file);
	fclose(file);
	str[666] = 0;
	return str;
}
char* GetStartText()
{
	char* str = (char*)calloc(1000,1);
	FILE* file;
	fopen_s(&file, "resource/TypeGame.txt", "r");
	fread(str, 555, 1, file);
	fclose(file);
	str[555] = 0;
	return str;
}
char* GetGoText()
{
	char* str = (char*)calloc(1000,1);
	FILE* file;
	fopen_s(&file, "resource/Go_Next.txt", "r");
	fread(str, 555, 1, file);
	fclose(file);
	str[555] = 0;
	return str;
}
char* GetGameOverText()
{
	char* str = (char*)calloc(1000,1);
	FILE* file;
	fopen_s(&file, "resource/Gameover.txt", "r");
	fread(str, 555, 1, file);
	fclose(file);
	str[555] = 0;
	return str;
}
//色彩变化
void colour()
{
	int r = 1;
	r = rand() % 5 + 1;
	switch (r)
	{
	case 1:
		Red
		break;
	case 2:
		Blue
		break;
	case 3:
		Green;
		break;
	case 4:
		Pink;
		break;
	case 5:
		Yellow
		break;
	case 6:
		Cyan
		break;
	}
}
