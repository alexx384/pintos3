#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int dx[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
int dy[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
int desk[8][8] = { NULL };
int min;
int mx, my;
int counter = 1;

void search_move(int x, int y);
void move(int k, int x, int y);
void desk_init();
void show_desk();


void search_move (int x, int y){
  if (x > 7 || y > 7 || x < 0 || y < 0 || desk[y][x] > 0)
    return;
  int local_min = 0;
  for (int i = 0; i < 8; i++) {
    if (!(x + dx[i] > 7 || y + dy[i] > 7 || x + dx[i] < 0 || y + dy[i] < 0 || desk[y + dy[i]][x + dx[i]] > 0))
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
  //system("cls");
  //show_desk();
  //system("pause");
  move(k - 1, mx, my);
}

void desk_init (){
  int i, j;
  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
      desk[i][j] = 0;
  return;
}

void show_desk() {
  int i, j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      if (desk[i][j] < 10)
        printf("0%d ", desk[i][j]);
      else
        printf("%d ", desk[i][j]);
    }
    printf("\n");
  }
}

void main(int argc, char *argv[]) {
  desk_init();
  mx=atoi(argv[1]);
  my=atoi(argv[2]);
//  printf("Print y: ");

  int buffx = mx;
  int buffy = my;
  int n = 0;
  desk[my][mx] = counter;
  counter++;
  //system("cls");
  show_desk();
  while (n != 1000) {
    move(63, mx, my);
    mx = buffx;
    my = buffy;
    desk_init;
    counter = 1;
    desk[my][mx] = counter;
    counter++;
    n++;
  }
  printf("\n");
  show_desk();
  printf("\n");

  return 0;
}