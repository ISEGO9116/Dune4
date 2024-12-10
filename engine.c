//어디까지 했는지 : 
//1) 준비 (완료)
//2) 커서 & 상태창 (완료)
//3) 중립 유닛 (미완성)
//4) 유닛 1기 생산 (완료)
//5) 시스템 메시지 (완료)

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
void cursor_doublemove(DIRECTION dir);
void cursor_select();
int is_defined_object(int layer, int w, int h);
POSITION sample_obj_next_position(void);
void display_defined_object_info(int layer, int w, int h);
void status_display_clear();
void resource_Spice_Add(int amount);
void display_command_inspectBuildList();
void BuildMode_ChangeState(bool state);
bool BuildMode_GetBuildMode();
void BuildMode_Prepare();

// 유닛 생성 시도
void UnitGenerate(int cost, int population);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

// ==== 전역 변수 : 상태제어 ====
bool isBuildMode = false;

//테스트
RESOURCE resource = {
	.spice = 20,
	.spice_max = 20,
	.population = 0,
	.population_max = 20
};

DUNE_OBJECT obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300
};


/* ================= 유닛 =================== */
//하베스터
//DUNE_OBJECT havester = {
//	.pos = {MAP_HEIGHT - 4, 1},
//	.repr = 'H',
//	.speed = 0,
//	.next_move_time = 2000,
//};
//DUNE_OBJECT havester_AI = {
//	.pos = {3, MAP_WIDTH - 2},
//	.repr = 'H',
//	.speed = 0,
//	.next_move_time = 2000,
//};

DUNE_UNIT havester = {
	.pos = {MAP_HEIGHT - 4, 1},
	.type = 'H',
	.cost = 5,
	.population = 5,
};
DUNE_UNIT havester_AI = {
	.pos = {3, MAP_WIDTH - 2},
	.type = 'H',
	.cost = 5,
	.population = 5,
};

DUNE_UNIT sandwarm = {
	.pos = {5, 5},
	.type = 'W',
	.cost = 0,
	.population = 0,
};

DUNE_UNIT sandwarm2 = {
	.pos = {12, MAP_WIDTH - 7},
	.type = 'W',
	.cost = 0,
	.population = 0,
};



/* ================= 지형, 건물 =================== */
//기지
DUNE_STRUCTURE base = {
	.pos = {MAP_HEIGHT - 2, 1}, //두번째 인자가 가로, 첫 번째 인자가 높이
	.direction = 0,
	.radius = 2,
	.type = 'B'
};
DUNE_STRUCTURE base_AI = {
	.pos = {MAP_HEIGHT + 1, MAP_WIDTH - 2}, //두번째 인자가 가로, 첫 번째 인자가 높이
	.direction = 1,
	.radius = 2,
	.type = 'B'
};

//장판
DUNE_STRUCTURE Plate = {
	.pos = {MAP_HEIGHT - 2, 3},
	.direction = 0,
	.radius = 2,
	.type = 'P'
};
DUNE_STRUCTURE Plate_AI = {
	.pos = {MAP_HEIGHT + 1, MAP_WIDTH - 4},
	.direction = 1,
	.radius = 2,
	.type = 'P'
};

//스파이스 매장지
DUNE_SPICEMINE spicemine = {
	.pos = {MAP_HEIGHT - 6, 1},
	.mineable_amount = 5,
};
DUNE_SPICEMINE spicemine_AI = {
	.pos = {5, MAP_WIDTH - 2},
	.mineable_amount = 5,
};

//바위
DUNE_STRUCTURE rock_small = {
	.pos = {MAP_HEIGHT - 7, 12},
	.direction = 2,
	.radius = 1,
	.type = 'R'
};
DUNE_STRUCTURE rock_small_AI = {
	.pos = {6, MAP_WIDTH - 13},
	.direction = 2,
	.radius = 1,
	.type = 'R'
};
DUNE_STRUCTURE rock_large = {
	.pos = {MAP_HEIGHT - 13, 16},
	.direction = 0,
	.radius = 2,
	.type = 'R'
};
DUNE_STRUCTURE rock_large_AI = {
	.pos = {13, MAP_WIDTH - 27},
	.direction = 1,
	.radius = 2,
	.type = 'R'
};


/*     ===== (전역변수) =====     */
int arrow_time = 0; //방향키 연속입력 타이머





/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();
		// 키 입력 처리
		if (is_arrow_key(key)) {
			//만약 방향키 입력이면
			if (arrow_time == 0) {
				//연속입력X
				arrow_time = 80; //딜레이 부여
				cursor_move(ktod(key));
				//printf("1회 입력");
			}
			else {
				//연속입력O
				arrow_time = 0; //연속 입력 가능
				//cursor_move(ktod(key));
				cursor_doublemove(ktod(key));
				//printf("2회 입력");
			}
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case b_build:
				BuildMode_Prepare();
				//건설 가능한 건물 목록 명령창에 표시
			case k_esc: status_display_clear(0); 
				Change_IsReadyForKey(0); //선택 취소
				clear_command_display(); //명령창 클리어
				continue;
			case k_harvester:
				if (State_IsReadyForKey()) { //T/F
					//만약 선택한 상태에서 하베스터 입력시,
					Change_IsReadyForKey(0); //작동했으니 비활성화하고
					//하베스터 생산시도
					UnitGenerate(5, 5);
				}
				else {
					printf("현재false입니다.");
				}
			case k_space: cursor_select();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		//sample_obj_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10; //10ms 경과
		if (arrow_time != 0) {
			arrow_time -= 10; //10ms경과
		}
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("TV 애니메이션 : 걸즈 밴드 크라이 2기 기원\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("토에이님 제발 2기 주세요\n");
	exit(0);
}

/* ======== 유닛 생성 ========= */
void spawn_unit(DUNE_UNIT d_obj) {
	//int speed = d_obj.speed;
	POSITION pos = d_obj.pos; //위치
	char Type = d_obj.type;   //타입
	int population;           //인구수
	int cost;                 //생성 코스트
	map[1][pos.row][pos.column] = Type;
}

/* ======== 건물 생성 ========= */
//중심 위치, 크기(1x1, 2x2, ...), 방향(0:플레이어, 1:적), 표시 문자
void spawn_struct(DUNE_STRUCTURE d_strcture) {
	//dir이 0이면 좌표에서 우상단으로, 
	//1이면 좌하단을 바라보게 배치
	POSITION pos = d_strcture.pos;
	int size = d_strcture.radius;
	int dir = d_strcture.direction;
	char Type = d_strcture.type;
	//map[0][pos.row][pos.column] = 'C'; //중심 좌표 확인 용도
	map[0][pos.row][pos.column] = Type; //중심 좌표 확인 용도
	if (size == 2) {
		if (dir == 0) {
			//우상단을 향해 배치
			map[0][pos.row - 1][pos.column] = Type;
			map[0][pos.row][pos.column + 1] = Type;
			map[0][pos.row - 1][pos.column + 1] = Type;
		}
		else if (dir == 1) {
			//좌하단을 향해 배치
			map[0][pos.row + 1][pos.column] = Type;
			map[0][pos.row][pos.column - 1] = Type;
			map[0][pos.row + 1][pos.column - 1] = Type;
		}
	}
}

/* ======== 스파이스 매장지 생성 ========= */
void spawn_spicemine(DUNE_SPICEMINE d_spicemine) {
	int spice_amount = d_spicemine.mineable_amount;
	POSITION pos = d_spicemine.pos;

	//map[0][pos.row][pos.column] = (char)spice_amount;
	//임시) S로 표기
	map[0][pos.row][pos.column] = 'S';

}


void init(void) {
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	//건물 생성(사실 생성보다는 도장찍기에 가까움)
	//(실제 이동은 이동 함수에서 이루어지고 곧장 화면이 새로고침되면서 사라지기 때문)
	spawn_struct(base);
	spawn_struct(base_AI);
	spawn_struct(Plate);
	spawn_struct(Plate_AI);
	spawn_struct(rock_small);
	spawn_struct(rock_small_AI);
	spawn_struct(rock_large);
	spawn_struct(rock_large_AI);
	spawn_spicemine(spicemine);
	spawn_spicemine(spicemine_AI);

	//오브젝트 생성
	spawn_unit(havester);
	spawn_unit(havester_AI);
	spawn_unit(sandwarm);
	spawn_unit(sandwarm2);
	//map[1][havester.pos.row][havester.pos.column] = havester.repr;

	//// object sample
	//map[1][obj.pos.row][obj.pos.column] = 'o';

	//UI : 상태창 그리기
	//display_object_info();
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

void cursor_doublemove(DIRECTION dir) {
	POSITION curr = cursor.current;
	//POSITION new_pos = pmove(curr, dir);
	POSITION new_pos = curr; // 현재 위치를 새로운 위치로 초기화
	switch (dir) {
	case d_up:
		new_pos.row-=2; // 위로 2칸 이동
		break;
	case d_down:
		new_pos.row+=2; // 아래로 2칸 이동
		break;
	case d_left:
		new_pos.column-=2; // 왼쪽으로 2칸 이동
		break;
	case d_right:
		new_pos.column+=2; // 오른쪽으로 2칸 이동
		break;
	default:
		break;
	}
	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

//스페이스바 감지시 작동
//현재 커서 위치를 기반으로 map의 layer를 검사한다.
void cursor_select() {
	POSITION curr = cursor.current; //커서 위치 받아옴
	if (is_defined_object(1, curr.row, curr.column)) {
		//해당 위치의 문자열이 유효하다면
		//상태창에 표시한다.
		display_defined_object_info(1, curr.row, curr.column);
	}
	else {
		display_defined_object_info(0, curr.row, curr.column);
	}
}

int is_defined_object(int layer, int w, int h) {
	//해당 값이 사전에 정의된 객체, 오브젝트면 t, 아니면 f
	char pos_char = map[layer][w][h];
	if (layer == 1){ //유닛 모드
		switch (pos_char) {
		case 'W': 
			return true;
		case 'H': 
			return true;
		default: //판별 불가 (=기본)
			return false;
		}
	}
	else { //지형 모드
		switch (pos_char) {
		case 'B': 
			return true;
		case 'P': 
			return true;
		case 'R': 
			return true;
		case 'S': 
			return true;
		default:
			return false;
		}
	}
}

// 오브젝트 정보(레이어, 위치)를 넣으면 해당 오브젝트를 확인하고 
//그에 맞는 정보를 출력하게 한다. 
void display_defined_object_info(int layer, int w, int h) {
	char pos_char = map[layer][w][h];
	if (layer == 1) { //유닛 모드
		switch (pos_char) {
		case 'W': change_display_info(21); break;
		case 'H': change_display_info(22); break;
		default: //판별 불가 (=기본)
			change_display_info(11); break;
		}
	}
	else { //지형 모드
		switch (pos_char) {
		case 'B': change_display_info(12);
			//기지 선택
			break;
		case 'P': change_display_info(13); break;
		case 'R': change_display_info(14); break;
		case 'S': change_display_info(15); break;
		default:
			change_display_info(11); break;
		}
	}
}

//
void status_display_clear() {
	change_display_info(0);
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}

//유닛 생성 시도
void UnitGenerate(int cost, int population) {
	// 자원이 충분한지 확인
	//자원 : 생산비용5, 인구수5, (일단 하드코딩)
	if (resource.population + population <= resource.population_max) {
		//인구수가 충분한지(현재인구수+생산할유닛인구수<=최대인구수)
		if ((resource.spice - cost) >= 0) {
			//자원이 충분한지(현재스파이스-유닛코스트<= 0)

			//자원이 충분하다면 즉시 본진 옆에 하베스터 1기 생산
			DUNE_UNIT harvester3 = {
				.cost = 5,
				.population = 5,
				.pos = {16,3},
				.type = 'H',
			};
			spawn_unit(harvester3);
			resource_Spice_Add(-harvester3.cost);
			//시스템 메시지 출력
			change_systemMsg_display("하베스터가 생산되었습니다.          ");
		}
		else {
			//자원이 부족할 경우
			//시스템 메시지 출력
			change_systemMsg_display("스파이스가 부족합니다.          ");
		}
		
	}
	else {
		//인구수가 부족할 경우
		//시스템 메시지 출력
		change_systemMsg_display("인구수가 부족합니다.          ");
	}
}

//스파이스 보유량 조절 함수
void resource_Spice_Add(int amount) {
	resource.spice += amount;
}



// ===== 건설 =====
// 흐름-1) 'BuildMode_Prepare'
// 트리거 : B키 다운
// 1. 빌드 모드 활성화
// 2. 명령창에 건물 목록 표시
void BuildMode_Prepare() {
	BuildMode_ChangeState(true); //빌드 모드 활성화
	display_command_inspectBuildList(); // 건물 목록 표시
}

//건설 모드 on / off
void BuildMode_ChangeState(bool state) {
	isBuildMode = state;
	change_systemMsg_display("빌드모드 변경.          ");
}

void display_command_inspectBuildList() {
	clear_command_display();
	//change_command_display("Terset");	
	inspectBuildList_command_display();
	change_systemMsg_display("건물 목록 표시.      ");
	
}

//빌드 모드 가져오는 함수
bool BuildMode_GetBuildMode() {
	return isBuildMode;
}