/*
* raw(?) I/O
*/
#include "io.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // ��, �� �ݴ�� ����
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
    if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
        return k_none;
    }

    int byte = _getch();    // �Էµ� Ű�� ���� �ޱ�
    switch (byte) {
    case 'q': return k_quit;  // 'q'�� ������ ����
    case 'b': return b_build; // 'b'�� ������ �Ǽ� ���
    case 'h': return k_harvester; //'h'�� ������ �Ϻ����� ���
    case ' ': return k_space; // ���� = �����̽� ����
    case 27: return k_esc;    // ESC Ű ����
    case 224:
        byte = _getch();  // MSB 224�� �Է� �Ǹ� 1����Ʈ �� ���� �ޱ�
        switch (byte) {
        case 72: return k_up;    // �� ����Ű
        case 75: return k_left;  // ���� ����Ű
        case 77: return k_right; // ������ ����Ű
        case 80: return k_down;  // �Ʒ� ����Ű
        default: return k_undef;
        }
    default: return k_undef;
    }
}
