
#include "common.h"
#include "blockInfo.h"
#include "BlockStageControl.h"
#include "stm32f429i_discovery_lcd.h"

#define GBOARD_WIDTH		16	// 240/10 = 24, -8
#define GBOARD_HEIGHT		31	// 320/10 = 32, -1

#define LCD_WIDTH		24	// 240/10 = 24, -2
#define LCD_HEIGHT		32	// 320/10 = 32, -1

#define LEFT 				75
#define RIGHT 				77
#define UP 					72
#define DOWN 				80
#define SPACE 				32


//extern UART_HandleTypeDef huart1;
static int gameBoardInfo[LCD_HEIGHT][LCD_WIDTH] = {0,};

static int currentBlockModel = 0;
static int curPosX = 0, curPosY = 0;
static int rotateSte = 0;

extern RNG_HandleTypeDef hrng;
uint32_t u32randomNumber;


void InitNewBlockPos(int x, int y)
{

	if(x<0 || y<0) return;

	curPosX = x;
	curPosY = y;

}

void ChooseBlock(void)
{
	HAL_RNG_GenerateRandomNumber(&hrng, &u32randomNumber);
	currentBlockModel = (u32randomNumber % NUM_OF_BLOCK_MODEL) * 4;

}

int GetCurrentBlockIdx(void)
{
	return currentBlockModel + rotateSte;
}


void ShowBlock(char blockInfo[][4])
{
	int x, y;

	for(y=0; y<4; y++)
	{
		for(x=0; x<4; x++)
		{
			if(blockInfo[y][x]==1)
			{
				DrawBlock(curPosX+x, curPosY+y, LCD_COLOR_WHITE, LCD_COLOR_RED);
			}
		}
	}
}

void DeleteBlock(char blockInfo[][4])
{
	uint16_t x, y;

	for(y = 0; y<4; y++)
	{
		for(x=0; x<4; x++)
		{
			if(blockInfo[y][x]==1)
			{
				DrawBlock(curPosX+x, curPosY+y, LCD_COLOR_BLUE, LCD_COLOR_BLUE);
			}
		}
	}
}

int BlockDown(void)
{
	if(!DetectCollision(curPosX, curPosY+1, blockModel[GetCurrentBlockIdx()]))
	{
		AddCurrentBlockInfoToBoard();
		RemoveFillUpLine();
		return 0;
	}

	DeleteBlock(blockModel[GetCurrentBlockIdx()]);
	curPosY+=1;

	ShowBlock(blockModel[GetCurrentBlockIdx()]);

	return 1;
}

void DrawBlock(uint16_t x, uint16_t y, uint32_t edge_color, uint32_t inner_color)
{

	BSP_LCD_SetTextColor(edge_color);
	BSP_LCD_FillRect(x*10, y*10, 10, 10);
	BSP_LCD_SetTextColor(inner_color);
	BSP_LCD_FillRect((x*10)+1, (y*10)+1, 8, 8);

}

void DrawGameBoard(void)
{
	int x, y;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	for(y= 0; y<LCD_HEIGHT; y++)
	{
		for(x=GBOARD_WIDTH+1; x<LCD_WIDTH; x++)
		{
			gameBoardInfo[y][0] = 1;
			gameBoardInfo[y][x] = 1;

			BSP_LCD_FillRect(0, y*10, 10, 10);
			BSP_LCD_FillRect(x*10, y*10, 10, 10);
		}

	}

	for(x=0; x<LCD_WIDTH; x++)
	{
		gameBoardInfo[GBOARD_HEIGHT][x] = 1;
		BSP_LCD_FillRect(x*10, GBOARD_HEIGHT*10, 10, 10);
	}

}

void ShiftLeft(void)
{
	if(!DetectCollision(curPosX-1, curPosY, blockModel[GetCurrentBlockIdx()]))
	{
		return;
	}

	DeleteBlock(blockModel[GetCurrentBlockIdx()]);
	curPosX-=1;

	ShowBlock(blockModel[GetCurrentBlockIdx()]);
}

void ShiftRight(void)
{
	if(!DetectCollision(curPosX+1, curPosY, blockModel[GetCurrentBlockIdx()]))
	{
		return;
	}

	DeleteBlock(blockModel[GetCurrentBlockIdx()]);
	curPosX+=1;

	ShowBlock(blockModel[GetCurrentBlockIdx()]);
}

void RotateBlock(void)
{
	int nextRotSte;
	int beforeRotSte=rotateSte;

	DeleteBlock(blockModel[GetCurrentBlockIdx()]);

	nextRotSte = rotateSte + 1;
	nextRotSte %= 4;
	rotateSte = nextRotSte;

	if(!DetectCollision(curPosX, curPosY, blockModel[GetCurrentBlockIdx()]))
	{
		rotateSte=beforeRotSte;
		return;
	}

	ShowBlock(blockModel[GetCurrentBlockIdx()]);

}

int DetectCollision(int posX, int posY, char blockModel[][4])
{
	int x, y;

	int arrX = posX;
	int arrY = posY;

	for(x=0; x<4; x++)
	{
		for(y=0; y<4; y++)
		{
			if(blockModel[y][x] == 1 && gameBoardInfo[arrY+y][arrX+x] == 1)
			{
				return 0;
			}

		}
	}
	return 1;
}

void AddCurrentBlockInfoToBoard(void)
{
	int x, y;

	int arrCurX;
	int arrCurY;

	for(y=0; y<4; y++)
	{
		for(x=0; x<4; x++)
		{

			arrCurX = curPosX;
			arrCurY = curPosY;

			if(blockModel[GetCurrentBlockIdx()][y][x] == 1)
			{
				gameBoardInfo[arrCurY+y][arrCurX+x] = 1;
			}
		}
	}
}

int IsGameOver(void)
{
	if(!DetectCollision(curPosX, curPosY, blockModel[GetCurrentBlockIdx()]))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void DrawSolidBlocks(void)
{
	int x, y;

	for(y=0; y<GBOARD_HEIGHT; y++)
	{
		for(x=1; x<GBOARD_WIDTH+1; x++)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
			BSP_LCD_FillRect(x*10, y*10, 10, 10);

			if(gameBoardInfo[y][x] == 1)
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_FillRect(x*10, y*10, 10, 10);
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_FillRect(x*10+1, y*10+1, 8, 8);
			}
		}
	}
}

void RemoveFillUpLine(void)
{
	int x, y;
	int line;

	for(y=GBOARD_HEIGHT-1; y>0; y--)
	{
		for(x=1; x<GBOARD_WIDTH+1; x++)
		{
			if(gameBoardInfo[y][x]!=1)
				break;
		}
		if(x == (GBOARD_WIDTH+1))
		{
			for(line = 0; y-line > 0; line++)
			{
				for(x=1; x<GBOARD_WIDTH+1; x++)
				{
					gameBoardInfo[y-line][x] = gameBoardInfo[y-line-1][x];
					BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
					BSP_LCD_FillRect(x*10, y*10, 10, 10);
				}
			}
			y++;
		}
	}
	DrawSolidBlocks();
}


void SolidCurrentBlock(void)
{
	while(BlockDown());
}








