#include <stdio.h>
#include <stdlib.h>
//#include "../threads/malloc.h"

int max_x, max_y;

int dx[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
int dy[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
int desk[10][10]={NULL};
int min;
int mx, my;
int counter = 1;

void search_move(int x, int y);
void move(int k, int x, int y);
//void desk_init();
void show_desk();


void search_move (int x, int y){
  if (x > max_x || y > max_y || x < 0 || y < 0 || desk[y][x] > 0)
    return;
  int local_min = 0;
  for (int i = 0; i < 8; i++) {
    if (!(x + dx[i] > max_x || y + dy[i] > max_y || x + dx[i] < 0 || y + dy[i] < 0 || desk[y + dy[i]][x + dx[i]] > 0))
      local_min++;
  }
  if (local_min < min) {
    min = local_min;
    my = y;
    mx = x;
  }

}

void move(int k, int x, int y) {
  min = 16;
  if (k == 0) 
    return;
  for (int i = 0; i < 8; i++) {
    search_move(x + dx[i], y + dy[i]);
  }
  desk[my][mx] = counter;
  counter++;
  
  move(k - 1, mx, my);
}

void show_desk() {
  int i, j;
  for (i = 0; i < max_y+1; i++) {
    for (j = 0; j < max_x+1; j++) {
      if (desk[i][j] < 10)
        printf("0%d ", desk[i][j]);
      else
        printf("%d ", desk[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[])
{
//  desk_init();
  mx=atoi(argv[1]);
  my=atoi(argv[2]);
  max_x=atoi(argv[3])-1;
  max_y=atoi(argv[4])-1;
//  printf("Print y: ");

  int buffx = mx;
  int buffy = my;
  int n = 0, i;

  //desk=calloc(max_y+1, 4);
  //for (i = 0; i < max_y+1; ++i)
  //{
  //  desk[i]=calloc(max_x+1,4);
  //}


  desk[my][mx] = counter;
  counter++;
  //system("cls");
  show_desk();
  while (n != 1000) {
    move(63, mx, my);
    mx = buffx;
    my = buffy;
    
    counter = 1;
    desk[my][mx] = counter;
    counter++;
    n++;
  }
  printf("\n");
  show_desk();
  printf("\n");

  //for (i = 0; i < max_y+1; ++i)
  //{
  //  free(desk[i]);
 // }
  //free(desk);

  return 0;
}