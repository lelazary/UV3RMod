#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



char uiDisplay[12];
char symbol[2];
char uiTopNum[2];

enum UI_Loc {UI_TL, UI_TR, UI_BL, UI_BR, UI_TOP_NUM};
enum Ui_Types {UI_NONE, UI_NUM_SMALL, UI_NUM_LARGE, UI_STR};
void* uiPtr[5];
char uiType[5] = {UI_NONE, UI_NONE, UI_NONE, UI_NONE, UI_NONE};
char uiEditLoc[5] = {-1, -1, -1, -1, -1};
unsigned char flashTime = 0;

void showStr(unsigned char idx, void* ptr)
{
  char* str = (char*)ptr;
  unsigned char p = idx*3;
  while(*str != 0)
    uiDisplay[p++] = *str++;

  if (uiEditLoc[idx] != -1 && 
      ((flashTime++)&0x04) )
  {
    uiDisplay[idx*3 + uiEditLoc[idx]] = '-';
  }
}

void showNumSmall(unsigned char idx, void* ptr)
{

  unsigned char base = 10;
  unsigned short num = *(unsigned short*)ptr;

  unsigned char uiIdx = idx*3;
  uiDisplay[uiIdx] =   ' ';
  uiDisplay[uiIdx+1] = ' ';
  uiDisplay[uiIdx+2] = ' ';

  if (num == 0)
  {
    uiDisplay[uiIdx+2] = '0';
  } else {
    uiIdx += 2;
    while(num > 0)
    {
      unsigned char digi = num % base;
      digi = digi < 10 ? '0' + digi : 'A' + digi - 10;
      uiDisplay[uiIdx--] = digi;
      num /= base;
    }
  }

  if (uiEditLoc[idx] != -1 && 
      ((flashTime++)&0x04) )
  {
    uiDisplay[idx*3 + (2-uiEditLoc[idx])] = '-';
  }


}

void showNumTop(unsigned char idx, void* ptr)
{

  unsigned short num = *(unsigned short*)ptr;

  uiTopNum[0] =   ' ';
  uiTopNum[1] = ' ';

  unsigned char tens = num/10;
  unsigned char ones = num%10;

  uiTopNum[0] = '0' + tens;
  uiTopNum[1] = '0' + ones;

  if (uiEditLoc[idx] != -1 && 
      ((flashTime++)&0x04) )
  {
    uiTopNum[1-uiEditLoc[idx]] = '-';
  }


}

void showUI()
{

  unsigned char i;
  for(i=0; i<5; i++)
  {
    switch(uiType[i])
    {
      case UI_NUM_SMALL:
        if (i==UI_TOP_NUM)
          showNumTop(i, uiPtr[i]);
        else
          showNumSmall(i, uiPtr[i]);
        break;
      case UI_STR:
        showStr(i, uiPtr[i]);
        break;
    }
  }

  printf("%c[1;1f", 27); //Move to 1x1
  printf("%c%c\n", uiTopNum[0], uiTopNum[1]);
  printf("%c[2;1f", 27); //Move to 1x1
  printf("%c%c%c%c%c%c%c\n",
      uiDisplay[0], uiDisplay[1], uiDisplay[2],
      symbol[0],
      uiDisplay[3], uiDisplay[4], uiDisplay[5]);
  printf("%c[3;1f", 27); //Move to 1x1
  printf("%c%c%c%c%c%c%c\n",
      uiDisplay[6], uiDisplay[7], uiDisplay[8],
      symbol[1],
      uiDisplay[9], uiDisplay[10], uiDisplay[11]);
  printf("\n");
}

void initUI()
{
  unsigned char i=0;

  for(i=0; i<12; i++)
    uiDisplay[i] = '_';
  symbol[0] = ' ';
  symbol[1] = ' ';
  uiTopNum[0] = '_';
  uiTopNum[1] = '_';
}


void uiAddNum(unsigned char pos, void* ptr)
{
  uiPtr[pos] = ptr;
  uiType[pos] = UI_NUM_SMALL;
}

void uiAddStr(unsigned char pos, void* ptr)
{
  uiPtr[pos] = ptr;
  uiType[pos] = UI_STR;
}

void uiIncDecStr(unsigned char pos, char dir)
{
  if (uiType[pos] != UI_STR)
    return;

  char* str = (char*)uiPtr[pos];
  
  str[uiEditLoc[pos]] += dir;
  if (str[uiEditLoc[pos]] > 'z')
    str[uiEditLoc[pos]] = 0;
}

void uiIncDecNum(unsigned char pos, char dir)
{
  if (uiType[pos] != UI_NUM_SMALL)
    return;

  unsigned short* num = (unsigned short*)uiPtr[pos];

  switch(uiEditLoc[pos])
  {
    case 2:
      if (pos != UI_TOP_NUM)
      {
        if (dir > 0)
          *num += 100;
        else
          *num -= 100;
        break;
      }
    case 1:
      if (dir > 0)
        *num += 10;
      else
        *num -= 10;
      break;
    case 0:
      *num += dir;
      break;
  }

  if (pos == UI_TOP_NUM)
  {
    if (*num > 99) 
      *num = 99;

  } else {
    if (*num > 999) 
      *num = 999;
  }
}

void uiSetEditLoc(unsigned char pos, unsigned char loc)
{
  uiEditLoc[pos] = loc;
}

int main()
{
  initUI();
  unsigned short num = 10;
  char testStr[5] = "test";

  uiAddNum(UI_TOP_NUM, &num);
  uiAddStr(UI_BL, testStr);

  uiSetEditLoc(UI_TOP_NUM, 1);

  int c;
  system ("/bin/stty raw");
  while((c=getchar())!= 'q') {
    switch (c)
    {
      case '+':
        uiIncDecNum(UI_TOP_NUM, 1);
        //uiIncDecStr(UI_BL, 1);
        break;
      case '-':
        uiIncDecNum(UI_TOP_NUM, -1);
        //uiIncDecStr(UI_BL, -1);
        break;
    }
    printf("%c[2J", 27); //Clear screen
    showUI();
    /* type a period to break out of the loop, since CTRL-D won't work raw */
    //putchar(c);
  }
  /* use system call to set terminal behaviour to more normal behaviour */
  system ("/bin/stty cooked");

  return 0;
}
