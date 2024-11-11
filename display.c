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
const POSITION comand_pos = {21, 62}; //명령창 위치
const POSITION systemMsg_pos = { 20, 0 }; //명령창 위치


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

//상태창 배열
//char status[50][10] = { {0} }; //가로 50, 세로 10
char status_var[50] = {0}; //문자열이 담기는 배열

//명령창 배열
char command_var[50] = { 0 }; //문자열이 담기는 배열
//명령창 변수 (유닛 키 입력 대기중)
int isReadyForKey = 0; //기본 False

//시스템 메시지
char systemMsg_var_cur[50] = { 0 }; //문자열이 담기는 배열(현재)
char systemMsg_var_pre[50] = { 0 }; //문자열이 담기는 배열(이전)

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
//상태창
void display_object_info();
//명령창
void display_commands();
void clear_command_display();
int State_IsReadyForKey();
void change_command_display(char string[50]);
void Change_IsReadyForKey(int to);
//시스템 메시지
void display_system_message();
void change_systemMsg_display(char string[50]);

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message();
	display_object_info();
	display_commands();
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

//배열에 문자열 삽입
void insertString(char status_var[], const char* input) {
	strncpy(status_var, input, 50);
	status_var[49] = '\0'; // null terminator 추가
}

void change_display_info(int obj_id) {
	//1n : 지형 (11 : 사막)
	//2n : 유닛
	switch (obj_id) {
	case 0: insertString(status_var, "                ");
		break;
	case 11: insertString(status_var, "사막지형      ");
		break;
	case 12: insertString(status_var, "기지            ");
		//명령창에 명령어 표시
		change_command_display("하베스터(H)");
		//키 입력 대기 상태로 전환
		Change_IsReadyForKey(1);
		//만약 대기 상태에서 유닛 키 입력시 생산 시작 / ESC시, 선택 취소
		break;
	case 13: insertString(status_var, "장판            ");
		break;
	case 14: insertString(status_var, "바위            ");
		break;
	case 15: insertString(status_var, "스파이스매장지         ");
		break;
	case 21: insertString(status_var, "샌드웜         ");
		break;
	case 22: insertString(status_var, "하베스터         ");
		break;
	}
}

void display_object_info() {
	gotoxy(status_pos);
	printf("=================================");
	//상태창 내용 출력
	POSITION pos_edited = { 2 , status_pos.column };
	gotoxy(pos_edited);
	printf("%s\n", status_var);
	//for (int i = 0; i < 20; i++) {
	//	POSITION pos_edited = { 2 , status_pos.column+i };
	//	printc(pos_edited, status_var, COLOR_DEFAULT);
	//}
	POSITION pos_editedv2 = { 10 , status_pos.column };
	gotoxy(pos_editedv2);
	printf("=================================");
}

//명령창
void display_commands() {
	//윗줄
	gotoxy(comand_pos); //20, 62
	printf("=================================");
	
	//명령창 내용 출력
	POSITION pos_edited = { 11 , status_pos.column };
	gotoxy(pos_edited);
	//clear_command_display(); //명령창 클리어
	printf("%s\n", command_var);

	//밑줄
	POSITION pos_editedv2 = { 25, comand_pos.column };
	gotoxy(pos_editedv2); // 25, 62
	printf("=================================");
}

//명령창 클리어
void clear_command_display() {
	insertString(command_var, "                              ");
}

//명령창 업데이트
void change_command_display(char string[50]) {
	insertString(command_var, string);
	//텍스트 변경
}

//isReadyForKey가 T/F인지 확인
int State_IsReadyForKey() {
	if (isReadyForKey) {
		return true;
	}
	else {
		return false;
	}
}

//isReadyforKey의 상태를 변경하는 함수
void Change_IsReadyForKey(int to) {
	isReadyForKey = to;
}



// 시스템 메시지
void display_system_message() {
	//윗줄
	gotoxy(systemMsg_pos); //20, 62
	printf("=================================");

	//명령창 내용 출력
	POSITION pos_pre = { systemMsg_pos.row+1 , map_pos.column };
	POSITION pos_cur = { systemMsg_pos.row+2 , map_pos.column };
	gotoxy(pos_pre);
	printf("%s\n", systemMsg_var_pre); //이전 내용 출력
	gotoxy(pos_cur);
	printf("%s\n", systemMsg_var_cur); //현재 내용 출력

	//밑줄
	POSITION pos_editedv2 = { 25, systemMsg_pos.column };
	gotoxy(pos_editedv2); // 25, 62
	printf("=================================");
}

//시스템 메시지 업데이트
void change_systemMsg_display(char new_string[50]) {
	//insertString(systemMsg_var_cur, new_string);
	// 현재 메시지를 이전 메시지에 복사
	strncpy(systemMsg_var_pre, systemMsg_var_cur, 50);
	systemMsg_var_pre[49] = '\0'; // null terminator 추가

	// 새로운 문자열을 현재 메시지에 복사
	strncpy(systemMsg_var_cur, new_string, 50);
	systemMsg_var_cur[49] = '\0'; // null terminator 추가
}