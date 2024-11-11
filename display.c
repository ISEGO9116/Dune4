/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 1, 62 }; //상태창 위치


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

//상태창 배열
char status[50][10] = { 0 }; //가로 50, 세로 10
//[n][0]과 [n][9]는 #로 채우고, 나머지가 변하는 내용

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
//상태창
void display_object_info();


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// display_system_message()
	display_object_info();
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}


//맵 표시 함수, 변경점이 있다면 printc()
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			//각 칸에 대하여 변경점이 있다면 printc()
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = {i, j };
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
			}
			//이후 덮어쓰기하고 넘긴다.
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_object_info() {
	//위치 지정
	//printf("**********");
	
	//첫 줄, 막줄은 별로 덮음,
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 50; j++) {
			if (i == 0 || i == 9) {
				status[j][i] = '#';
			}
		}
	}
	//중간 내용은 받아와서 변한다.
	//쓰는 곳에서 이어서 구현한다.
	//여기서는 출력만 한다.

	//gotoxy(status_pos); //위치 이동(printc 내부 기능으로 대체)

	//한줄 출력 * 10
	for (int i = 0; i < 10; i++) {
		//가로 출력 
		for (int j = 0; j < 50; j++) {
			char ch = status[j][0]; //50, 5

			POSITION pos_edited = { 1 + i , 
				status_pos.column + j };
			//세로 위치, 가로 위치

			printc(pos_edited, ch, COLOR_DEFAULT); //위치, 문자열, 색상
		}
	}

	//첫줄, 막줄은 고정으로 출력, 
	//중간만 특수 배열variable_status[]의
	//내용을 출력




	//단순 개념도
	//char ch1 = status[0][0];
	//char ch2 = status[0][1];

	//int ch1_pos = status_pos.column + 1;
	//int ch2_pos = status_pos.column + 2;

	//POSITION pos1 = { 1, ch1_pos };
	//POSITION pos2 = { 1, ch2_pos };

	//printc(pos1, ch1, COLOR_DEFAULT); //위치, 문자열, 색상
	//printc(pos2, ch2, COLOR_DEFAULT); //위치, 문자열, 색상
}