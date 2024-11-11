/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 1, 62 }; //����â ��ġ


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

//����â �迭
char status[50][10] = { 0 }; //���� 50, ���� 10
//[n][0]�� [n][9]�� #�� ä���, �������� ���ϴ� ����

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
//����â
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

void display_object_info() {
	//��ġ ����
	//printf("**********");
	
	//ù ��, ������ ���� ����,
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 50; j++) {
			if (i == 0 || i == 9) {
				status[j][i] = '#';
			}
		}
	}
	//�߰� ������ �޾ƿͼ� ���Ѵ�.
	//���� ������ �̾ �����Ѵ�.
	//���⼭�� ��¸� �Ѵ�.

	//gotoxy(status_pos); //��ġ �̵�(printc ���� ������� ��ü)

	//���� ��� * 10
	for (int i = 0; i < 10; i++) {
		//���� ��� 
		for (int j = 0; j < 50; j++) {
			char ch = status[j][0]; //50, 5

			POSITION pos_edited = { 1 + i , 
				status_pos.column + j };
			//���� ��ġ, ���� ��ġ

			printc(pos_edited, ch, COLOR_DEFAULT); //��ġ, ���ڿ�, ����
		}
	}

	//ù��, ������ �������� ���, 
	//�߰��� Ư�� �迭variable_status[]��
	//������ ���




	//�ܼ� ���䵵
	//char ch1 = status[0][0];
	//char ch2 = status[0][1];

	//int ch1_pos = status_pos.column + 1;
	//int ch2_pos = status_pos.column + 2;

	//POSITION pos1 = { 1, ch1_pos };
	//POSITION pos2 = { 1, ch2_pos };

	//printc(pos1, ch1, COLOR_DEFAULT); //��ġ, ���ڿ�, ����
	//printc(pos2, ch2, COLOR_DEFAULT); //��ġ, ���ڿ�, ����
}