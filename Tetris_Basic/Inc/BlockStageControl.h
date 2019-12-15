
#ifndef __BLOCK_STAGE_CONTROL_H_
#define __BLOCK_STAGE_CONTROL_H_

void InitNewBlockPos(int x, int y);
void ChooseBlock(void);
int GetCurrentBlockIdx(void);
void ShowBlock(char blockInfo[][4]);
void DeleteBlock(char blockInfo[][4]);
int BlockDown(void);
void DrawBlock(uint16_t x, uint16_t y, uint32_t edge_color, uint32_t inner_coloer);
void ShiftLeft(void);
void ShiftRight(void);
void RotateBlock(void);
int DetectCollision(int posX, int posY, char blockModel[][4]);
void DrawGameBoard(void);
void AddCurrentBlockInfoToBoard(void);
int IsGameOver(void);
void SolidCurrentBlock(void);
void DrawSolidBlocks(void);
void RemoveFillUpLine(void);

#endif
