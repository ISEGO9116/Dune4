/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"
#include "stdbool.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 }; //�ڿ� ��ġ
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 1, 62 }; //����â ��ġ
const POSITION comand_pos = {20, 62}; //���â ��ġ
const POSITION systemMsg_pos = { 20, 0 }; //�ý��� �޽��� ��ġ


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

//����â �迭
//char status[50][10] = { {0} }; //���� 50, ���� 10
char status_var[50] = {0}; //���ڿ��� ���� �迭

//���â �迭
char command_var[50] = { 0 }; //���ڿ��� ���� �迭
//���â ���� (���� Ű �Է� �����)
int isReadyForKey = 0; //�⺻ False

//�ý��� �޽���
char systemMsg_var_cur[50] = { 0 }; //���ڿ��� ���� �迭(����)
char systemMsg_var_pre[50] = { 0 }; //���ڿ��� ���� �迭(����)

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
//����â
void display_object_info();
//���â
void display_commands();
void clear_command_display();
int State_IsReadyForKey();
void change_command_display(char string[50]);
void Change_IsReadyForKey(int to);
//�ý��� �޽���
void display_system_message();
//void change_systemMsg_display(char string[50]);
//void inspectBuildList_command_display();
//void write_command_display();

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


//�� ǥ�� �Լ�, �������� �ִٸ� printc()
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			//�� ĭ�� ���Ͽ� �������� �ִٸ� printc()
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = {i, j };
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
			}
			//���� ������ϰ� �ѱ��.
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

/// <summary>
/// �迭�� ���ڿ� ����
/// </summary>
void insertString(char status_var[], const char* input) {
	strncpy(status_var, input, 50);
	status_var[49] = '\0'; // null terminator �߰�
}

void change_display_info(int obj_id) {
	//1n : ���� (11 : �縷)
	//2n : ����
	switch (obj_id) {
	//���â Ŭ����
	case 0: insertString(status_var, "                ");
		break;
	case 11: insertString(status_var, "�縷����      ");
		break;
	case 12: insertString(status_var, "����            ");
		//���â�� ��ɾ� ǥ��
		change_command_display("�Ϻ�����(H)?");
		//Ű �Է� ��� ���·� ��ȯ
		Change_IsReadyForKey(1);
		//���� ��� ���¿��� ���� Ű �Է½� ���� ���� / ESC��, ���� ���
		break;
	case 13: insertString(status_var, "����            ");
		break;
	case 14: insertString(status_var, "����            ");
		break;
	case 15: insertString(status_var, "�����̽�������         ");
		break;
	case 21: insertString(status_var, "�����         ");
		break;
	case 22: insertString(status_var, "�Ϻ�����         ");
		break;
	}
}

//����â
void display_object_info() {
	gotoxy(status_pos); //(1, 62)
	printf("+=============����â=============+");
	//����â ���� ���
	POSITION pos_edited = { 2 , status_pos.column };
	gotoxy(pos_edited);
	printf("%s\n", status_var);
	POSITION pos_editedv2 = { 10 , status_pos.column };
	gotoxy(pos_editedv2);
	printf("-===============================-");
}

//���â
void display_commands() {
	gotoxy(comand_pos); //20, 62
	printf("+============���â============+"); //����
	//���â ���� ���
	POSITION pos_edited = { 21 , comand_pos.column };
	gotoxy(pos_edited);
	printf("%s\n", command_var);
	POSITION pos_editedv2 = { 25, comand_pos.column }; //����
	gotoxy(pos_editedv2); // 25, 62
	printf("-==============================-");
}

//isReadyForKey�� T/F���� Ȯ��
int State_IsReadyForKey() {
	if (isReadyForKey) {
		return true;
	}
	else {
		return false;
	}
}

//isReadyforKey�� ���¸� �����ϴ� �Լ�
void Change_IsReadyForKey(int to) {
	isReadyForKey = to;
}



// �ý��� �޽���
void display_system_message() {
	//����
	gotoxy(systemMsg_pos); //20, 62
	printf("+============�ý���============+");

	//���â ���� ���
	POSITION pos_pre = { systemMsg_pos.row+1 , map_pos.column };
	POSITION pos_cur = { systemMsg_pos.row+2 , map_pos.column };
	gotoxy(pos_pre);
	printf("%s\n", systemMsg_var_pre); //���� ���� ���
	gotoxy(pos_cur);
	printf("%s\n", systemMsg_var_cur); //���� ���� ���

	//����
	POSITION pos_editedv2 = { 25, systemMsg_pos.column };
	gotoxy(pos_editedv2); // 25, 62
	printf("-==============================-");
}

//�ý��� �޽��� ������Ʈ
void change_systemMsg_display(char new_string[50]) {
	//insertString(systemMsg_var_cur, new_string);
	// ���� �޽����� ���� �޽����� ����
	strncpy(systemMsg_var_pre, systemMsg_var_cur, 50);
	systemMsg_var_pre[49] = '\0'; // null terminator �߰�

	// ���ο� ���ڿ��� ���� �޽����� ����
	strncpy(systemMsg_var_cur, new_string, 50);
	systemMsg_var_cur[49] = '\0'; // null terminator �߰�
}



// ======== ���â ======== //
/// <summary>
/// ���â ������Ʈ
/// </summary>
void change_command_display(char new_string[50]) {
	insertString(command_var, new_string);
}

/// <summary>
/// ���â Ŭ����
/// </summary>
void clear_command_display() {
	insertString(command_var, "B: Build!       ");
}

/// <summary>
/// ���â�� �Ǽ� ������ �ǹ� ��� ����
/// </summary>
void inspectBuildList_command_display() {
	insertString(command_var, "P: Plate  ");
}