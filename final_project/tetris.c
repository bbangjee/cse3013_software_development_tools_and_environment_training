#include "tetris.h"

static struct sigaction act, oact;  // signal handler

Node* head = NULL;        // 랭킹 리스트의 head 노드를 가리키는 포인터
RecNode* recRoot = NULL;  // 추천 트리의 루트 노드를 가리키는 포인터

int main() {
  int exit = 0;  // 프로그램 종료 flag

  initscr();  // 화면 초기화
  noecho();   // 입력한 문자를 화면에 표시하지 않음

  keypad(stdscr, TRUE);             // 키패드 입력을 가능하도록 설정
  srand((unsigned int)time(NULL));  // 난수 생성기 초기화 - 블록 랜덤 생성용
  createRankList();                 // rank.txt 파일로부터 랭킹 정보 읽어오기

  while (!exit) {
    clear();  // 화면 지우기
    switch (menu()) {
      case MENU_PLAY:
        play();  // 게임 수동 플레이
        break;
      case MENU_RANK:
        rank();  // 랭킹 확인
        break;
      case MENU_RECOMMENDED_PLAY:
        recommendedPlay();  // 게임 자동 플레이
        break;
      case MENU_EXIT:
        exit = 1;  // 종료 플래그 설정
        break;
      default:
        break;
    }
  }

  // 프로그램 종료 전 정리 작업
  writeRankFile();  // 랭킹 정보 저장

  // 랭킹 리스트 메모리 해제
  Node* curr = head;
  while (curr != NULL) {  // 모든 노드 순환
    Node* temp = curr;
    curr = curr->next;
    free(temp);  // 메모리 해제
  }
  head = NULL;
  score_number = 0;

  endwin();         // ncurses 모드 종료
  system("clear");  // 터미널 화면 정리
  return 0;
}

/*************************************************
 * InitTetris();
 * 설명: 모든 변수 및 상태 초기화
 * 1. 필드: 0으로 초기화
 * 2. nextBlock 3개: 랜덤 생성
 * 3. 현재 블록 위치 및 회전 초기화
 * 4. 점수 및 플래그 초기화
 * 5. 추천 트리 초기화 및 추천 위치 계산
 * 6. 화면 그리기
 *************************************************/
void InitTetris() {
  int i, j;  // 반복문용 변수

  // 게임 field 0으로 초기화
  for (j = 0; j < HEIGHT; j++)
    for (i = 0; i < WIDTH; i++) field[j][i] = 0;

  // 다음에 나올 block 3개 랜덤 생성
  for (int k = 0; k < BLOCK_NUM; k++) {
    nextBlock[k] = rand() % 7;
  }

  // 현재 블록 관련 초기화
  blockRotate = 0;  // 회전 횟수: 0
  blockY = -1;      // 초기 Y좌표: -1 (보이지 않는 영역)
  blockX = WIDTH / 2 - 2; // 초기 X좌표: 화면 중앙

  // 게임 상태 관련 초기화
  score = 0; // 점수는 0점
  gameOver = 0; // 게임오버 플래그 초기화: 0
  timed_out = 0; // 타이머 타임아웃 플래그 초기화: 0

  // 추천 위치 초기화
  recommendY = 0; // y좌표: 0
  recommendX = 0; // x좌표: 0
  recommendR = 0; // 추천 회전: 0

  // 추천 트리 초기화
  recRoot = (RecNode*)malloc(sizeof(RecNode)); // 루트 메모리 할당
  recRoot->level = 0; // 루트 레벨 초기화: 0
  recRoot->accumulatedScore = 0; // 누적 점수 초기화: 0
  recRoot->child = NULL; // 자식 노드 아직 없음: NULL
  // 추천 트리 필드 초기화: 0으로
  for (j = 0; j < HEIGHT; j++) {
    for (i = 0; i < WIDTH; i++) {
      recRoot->recField[j][i] = 0;
    }
  }

  modified_recommend(recRoot); // 추천 함수 호출
  DrawOutline(); // 화면 외곽선 그리기
  DrawField(); // 게임 필드 그리기
  DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate); // 현재 블록 및 그림자 그리기
  DrawNextBlock(nextBlock); // 다음 블록 그리기
  PrintScore(score); // 점수 표시
}

/*************************************************
 * DrawOutline();
 * 설명: 화면 외곽선 그리기
 * 1. 블록이 떨어지는 공간 테두리
 * 2. 다음 블록 공간 테두리
 * 3. 그 다음 블록 공간 테두리
 * 4. 점수 공간 테두리
 *************************************************/
void DrawOutline() {
  int i, j;
  /* 블럭이 떨어지는 공간의 태두리를 그린다.*/
  DrawBox(0, 0, HEIGHT, WIDTH);

  /* next block[0]을 보여주는 공간의 태두리를 그린다.*/
  move(2, WIDTH + 10);
  printw("NEXT BLOCK");
  DrawBox(3, WIDTH + 10, 4, 8);

  /* next block[1]을 보여주는 공간의 테두리를 그린다 */
  DrawBox(9, WIDTH + 10, 4, 8);

  /* score를 보여주는 공간의 태두리를 그린다.*/
  move(15, WIDTH + 10);
  printw("SCORE");
  DrawBox(16, WIDTH + 10, 1, 8);
}

/*************************************************
 * GetCommand();
 * 설명: 키보드 입력을 받아 명령어를 반환
 * 1. UP 키: 블록 회전
 * 2. DOWN 키: 블록 아래로 이동
 * 3. LEFT 키: 블록 왼쪽으로 이동
 * 4. RIGHT 키: 블록 오른쪽으로 이동
 * 5. SPACE 키: 블록 즉시 내리기
 * 6. Q 키: 게임 종료
 *************************************************/
int GetCommand() {
  int command;
  command = wgetch(stdscr);
  switch (command) {
    case KEY_UP:
      break;
    case KEY_DOWN:
      break;
    case KEY_LEFT:
      break;
    case KEY_RIGHT:
      break;
    case ' ': /* space key*/
      /* fall block */
      break;
    case 'q':
    case 'Q':
      command = QUIT;
      break;
    default:
      command = NOTHING;
      break;
  }
  return command;
}

/******************************************************
 * ProcessCommand();
 * 설명: 명령어 처리
 * 1. UP 키: 블록 회전
 * 2. DOWN 키: 블록 아래로 이동
 * 3. LEFT 키: 블록 왼쪽으로 이동
 * 4. RIGHT 키: 블록 오른쪽으로 이동
 * 5. SPACE 키: 블록 즉시 내리기
 * 6. Q 키: 게임 종료
 * drawFlag 변수를 사용하여 해당 flag가 설정된 경우에만 화면을 갱신
 ******************************************************/
int ProcessCommand(int command) {
  int ret = 1;
  int drawFlag = 0;
  switch (command) {
    case QUIT:
      ret = QUIT;
      break;
    case KEY_UP:
      if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4,
                                  blockY, blockX)))
        blockRotate = (blockRotate + 1) % 4;
        // 블록 회전 후 화면 갱신
      break;
    case KEY_DOWN:
      if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1,
                                  blockX)))
        blockY++;
        // 블록 아래로 이동 후 화면 갱신
      break;
    case KEY_RIGHT:
      if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY,
                                  blockX + 1)))
        blockX++;
        // 블록 오른쪽으로 이동 후 화면 갱신
      break;
    case KEY_LEFT:
      if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY,
                                  blockX - 1)))
        blockX--;
        // 블록 왼쪽으로 이동 후 화면 갱신
      break;
    default:
      break;
  }
  if (drawFlag)
    DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
    // drawFlag가 설정된 경우에만 화면 갱신
  return ret;
}

/******************************************************
 * DrawField();
 * 설명: 게임 필드 그리기
 * 1. 필드의 각 셀을 순회
 * 2. if 셀 값 == 1: 역상으로 블록 그리기
 * 3. if 셀 값 == 0: 빈 칸으로 그리기
 ******************************************************/
void DrawField() {
  int i, j;
  for (j = 0; j < HEIGHT; j++) {
    move(j + 1, 1);
    for (i = 0; i < WIDTH; i++) {
      if (field[j][i] == 1) {
        attron(A_REVERSE);
        printw(" ");
        attroff(A_REVERSE);
      } else
        printw(".");
    }
  }
}

/******************************************************
 * PrintScore();
 * 설명: 점수 출력
 ******************************************************/
void PrintScore(int score) {
  move(17, WIDTH + 11);
  printw("%8d", score);
}

/******************************************************
 * DrawNextBlock();
 * 설명: 다음 블록 2개 그리기
 * 1. 첫 번째 다음 블록 모양 배열 순회
 * 2. 두 번째 다음 블록 모양 배열 순회
 ******************************************************/
void DrawNextBlock(int* nextBlock) {
  int i, j;
  // 첫 번째 다음 블록 그리기
  for (i = 0; i < 4; i++) {
    move(4 + i, WIDTH + 13);
    for (j = 0; j < 4; j++) {
      if (block[nextBlock[1]][0][i][j] == 1) {
        attron(A_REVERSE); // 역상 모드 설정
        printw(" ");
        attroff(A_REVERSE); // 역상 모드 해제
      } else
        printw(" "); // 빈 칸 출력
    }
  }
  // 두 번째 다음 블록 그리기
  for (i = 0; i < 4; i++) {
    move(10 + i, WIDTH + 13);
    for (j = 0; j < 4; j++) {
      if (block[nextBlock[2]][0][i][j] == 1) {
        attron(A_REVERSE); // 역상 모드 설정
        printw(" ");
        attroff(A_REVERSE); // 역상 모드 해제
      } else
        printw(" "); // 빈 칸 출력
    }
  }
}

/******************************************************
 * DrawBlock();
 * 설명: 블록 그리기
 * 1. 블록 모양 배열 순회
 * 2. if 블록 모양 배열 값 == 1: 해당 위치에 블록 그리기
 * 3. move 함수로 커서 위치 조정
 * 4. attron으로 역상 모드 설정
 * 5. printw 함수로 블록 문자 출력
 * 6. attroff으로 역상 모드 해제
 * 7. move 함수로 커서 위치 원래대로 이동
 ******************************************************/
void DrawBlock(int y, int x, int blockID, int blockRotate, char tile) {
  int i, j;
  // 블록 모양 배열 순회
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
        move(i + y + 1, j + x + 1);
        attron(A_REVERSE); // 역상 모드 설정
        printw("%c", tile);
        attroff(A_REVERSE); // 역상 모드 해제
      }
    }
  move(HEIGHT, WIDTH + 10); // 커서 원위치
}

/***********************************************************
 * DrawBox();	
 * 설명: 해당 좌표(y,x)에 원하는 크기(height,width)의 box를 그린다
 ***********************************************************/
void DrawBox(int y, int x, int height, int width) {
  int i, j;
  move(y, x);
  addch(ACS_ULCORNER);
  for (i = 0; i < width; i++) addch(ACS_HLINE);
  addch(ACS_URCORNER);
  for (j = 0; j < height; j++) {
    move(y + j + 1, x);
    addch(ACS_VLINE);
    move(y + j + 1, x + width + 1);
    addch(ACS_VLINE);
  }
  move(y + j + 1, x);
  addch(ACS_LLCORNER);
  for (i = 0; i < width; i++) addch(ACS_HLINE);
  addch(ACS_LRCORNER);
}

/******************************************************
 * play();
 * 설명: 실제 게임 진행
 * 1. InitTetris()로 게임 초기화
 * 2. 블록이 떨어지는 동안 명령어 입력 대기
 * 3. 명령어에 따라 블록 이동 및 회전 처리
 * 4. QUIT 명령어 입력 시 게임 종료
 * 5. 게임 오버 시 메시지 출력 및 랭킹 등록
 ******************************************************/
void play() {
  int command;
  clear(); // 화면 지우기
  act.sa_handler = BlockDown; // 시그널 핸들러에 대해 BlockDown 함수 설정
  sigaction(SIGALRM, &act, &oact); // SIGALRM 시그널에 대해 act 설정
  InitTetris(); // 초기화
  do {
    if (timed_out == 0) {
      alarm(1); // 1초 후에 SIGALRM 시그널 발생
      timed_out = 1; // 타임아웃 플래그 설정
    }

    command = GetCommand(); // 명령 입력 대기 
    // quit 명령어 처리
    if (ProcessCommand(command) == QUIT) {
      alarm(0);
      DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
      move(HEIGHT / 2, WIDTH / 2 - 4);
      printw("Good-bye!!");
      refresh();
      getch();
      // 추천 트리 메모리 해제
      if (recRoot != NULL) freeRecNodes(recRoot);
      return;
    }
  } while (!gameOver);
  // 게임 오버 처리
  alarm(0);
  getch();
  DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
  move(HEIGHT / 2, WIDTH / 2 - 4);
  printw("GameOver!!");
  refresh();
  getch();

  if (recRoot != NULL) freeRecNodes(recRoot); // 추천 트리 메모리 해제

  newRank(score); // 랭킹 등록
}

/******************************************************
 * menu();
 * 설명: 메인 메뉴 출력 및 사용자 입력 처리
 * 1. 메뉴 옵션 출력
 * 2. 사용자 입력 대기
 * 3. 입력된 값 반환
 ******************************************************/
char menu() {
  printw("1. play\n");
  printw("2. rank\n");
  printw("3. recommended play\n");
  printw("4. exit\n");
  return wgetch(stdscr);
}

/*****************************************************
 * checkToMove();
 * 설명: 블록이 해당 위치로 이동 가능한지 검사
 * 1. 블록 모양 배열 순회
 * 2. if 블록 모양 배열 값 == 1:
 *    a. 필드 경계 검사
 *    b. 다른 블록과 겹치는지 검사
 * 3. 이동 가능 여부 반환
 *****************************************************/
int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate,
                int blockY, int blockX) {
  // 현재 블록을 움직일 수 없는 경우 2가지
  // 1. block의 좌표와 필드의 경계를 비교하여 바깥으로 나갈 경우
  // 2. block의 좌표와 필드의 다른 블록과 겹칠 경우
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      // block의 모양이 있는 부분에 대해서만 검사
      if (block[currentBlock][blockRotate][i][j] == 1) {
        if (i + blockY < 0) continue;
        // 필드의 경계를 벗어나는지 검사 & 필드의 다른 블록과 겹치는지 검사
        if (i + blockY >= HEIGHT || j + blockX < 0 || j + blockX >= WIDTH ||
            f[i + blockY][j + blockX] == 1) {
          return 0;  // 이동 불가
        }
      }
    }
  }
  return 1;  // 이동 가능
}

/******************************************************
 * DrawChange();
 * 설명: 블록의 이전 위치를 지우고 새로운 위치에 그리기
 * 1. 이전 블록 위치 조회
 * 2. 이전 블록 위치 지우기
 * 3. 새로운 블록 위치 그리기
 ******************************************************/
void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock,
                int blockRotate, int blockY, int blockX) {
  // user code
  // 1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
  // 2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
  // 3. 새로운 블록 정보를 그린다.
  int prevBlockRotate = blockRotate; // 이전 블록 회전
  int prevBlockY = blockY; // 이전 블록 Y좌표
  int prevBlockX = blockX; // 이전 블록 X좌표
  switch (command) {
    case KEY_UP:
      prevBlockRotate = (blockRotate + 3) % 4; // 이전 회전 상태 계산
      break;
    case KEY_DOWN:
      prevBlockY = blockY - 1; // 이전 Y좌표 계산
      break;
    case KEY_RIGHT:
      prevBlockX = blockX - 1; // 이전 X좌표 계산
      break;
    case KEY_LEFT:
      prevBlockX = blockX + 1; // 이전 X좌표 계산
      break;
    default:
      break;
  }

  // 이전 블록 위치 지우기
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (block[currentBlock][prevBlockRotate][i][j] == 1 &&
          i + prevBlockY >= 0) {
        move(i + prevBlockY + 1, j + prevBlockX + 1);
        printw(".");
      }
    }
  }

  // DrawShadow 구현
  int shadowY = prevBlockY + 1;
  int shadowX = prevBlockX;

  // 그림자 위치 계산
  while (CheckToMove(f, currentBlock, prevBlockRotate, shadowY, shadowX)) {
    shadowY++; // 한 칸 아래로 이동
  }
  shadowY--; // 이동 불가한 위치에서 한 칸 위로 조정
  // 이전 블록 그림자 지우기
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (block[currentBlock][prevBlockRotate][i][j] == 1 && i + shadowY >= 0) {
        move(i + shadowY + 1, j + shadowX + 1);
        printw(".");
      }
    }
  }

  // DrawBlock(prevBlockY, prevBlockX, currentBlock, prevBlockRotate, '.');
  DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

/******************************************************
 * BlockDown();
 * 설명: 블록을 한 칸 아래로 이동시키거나 고정
 * 1. 블록을 한 칸 아래로 이동 가능하면 이동
 * 2. 더 이상 이동 불가하면 블록을 필드에 추가하고 점수 계산
 * 3. 다음 블록으로 교체 및 초기 위치로 재설정
 * 4. 추천 트리 메모리 해제 및 재생성
 * 5. 추천 알고리즘 수행 및 화면 갱신
 ******************************************************/
void BlockDown(int sig) {
  // user code
  // 블록의 y좌표가 -1일 경우 gameOver 변수를 1로 setting한다.
  // 1. 한 칸 아래로 이동 가능함
  timed_out = 0;
  if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
    // 가능하면 블록을 내림
    blockY += 1;
    DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
  }
  // 2. 더는 못 내려가 이러다 우리 다 죽어!
  else {
    if (blockY == -1) gameOver = 1; // 더 이상 이동 불가하면 게임 오버
    // 필드에 블록 추가 및 점수 획득
    score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
    // 삭제된 라인 점수 추가
    score += DeleteLine(field);

    blockY = -1; // 블록 초기 위치로 재설정
    blockX = WIDTH / 2 - 2; // 블록 초기 X좌표로 재설정
    blockRotate = 0; // 블록 회전 초기화

    // 다음 블록으로 교체
    for (int k = 0; k < BLOCK_NUM - 1; k++) {
      nextBlock[k] = nextBlock[k + 1];
    }
    nextBlock[BLOCK_NUM - 1] = rand() % 7; // 새로운 블록 랜덤 생성

    if (recRoot != NULL) {
      freeRecNodes(recRoot); // 기존 추천 트리 메모리 해제
      recRoot = NULL; // 포인터 초기화
    }

    recRoot = (RecNode*)malloc(sizeof(RecNode)); // 새로운 추천 트리 루트 메모리 할당
    recRoot->level = 0; // 레벨 0으로 초기화
    recRoot->accumulatedScore = 0; // 누적 점수 0으로 초기화
    recommendR = 0; // 추천 회전 초기화
    recommendY = 0; // 추천 Y좌표 초기화
    recommendX = 0; // 추천 X좌표 초기화
    recRoot->child = NULL; // 자식 노드 초기화

    // 추천 알고리즘 수행
    memcpy(recRoot->recField, field, sizeof(char) * HEIGHT * WIDTH);
    modified_recommend(recRoot); // 추천 위치 계산
    DrawNextBlock(nextBlock); // 다음 블록 그리기
    PrintScore(score); // 점수 출력
    DrawField(); // 필드 다시 그리기
  }
  return;
}

/******************************************************
 * AddBlockToField();
 * 설명: 블록을 필드에 추가하고 바닥에 닿았는지 확인
 * 1. 블록 모양 배열 순회
 * 2. 필드에 블록 추가
 * 3. 블록이 바닥에 닿았는지 확인 - 닿은 경우 고려 점수 반환
 ******************************************************/
int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate,
                    int blockY, int blockX) {
  // Block이 추가된 영역의 필드값을 바꾼다.
  int touched_bottom = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (block[currentBlock][blockRotate][i][j] == 1) {
        if (i + blockY == HEIGHT - 1) {
          // 블록이 필드의 맨 아래에 닿은 경우
          touched_bottom++;
        } else if (f[i + blockY + 1][j + blockX] == 1) {
          // 블록이 다른 블록에 닿은 경우
          touched_bottom++;
        }
        f[i + blockY][j + blockX] = 1; // 필드에 블록 추가
      }
    }
  }
  return touched_bottom * 10; // 닿은 면적에 따라 점수 반환
}

/******************************************************
 * DeleteLine();
 * 설명: 꽉 찬 라인을 찾아 삭제하고 점수 계산
 * 1. 필드 순회하여 꽉 찬 라인 탐색
 * 2. 꽉 찬 라인 삭제 및 위의 라인 내리기
 * 3. 삭제된 라인 수에 따른 점수 반환
 ******************************************************/
int DeleteLine(char f[HEIGHT][WIDTH]) {
  // user code
  // 1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
  // 2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
  int deleted_lines_cnt = 0;

  for (int i = HEIGHT - 1; i >= 0;) {
    int j = 0;
    for (; j < WIDTH; j++) {
      if (f[i][j] == 0) break;
    }
    if (j == WIDTH) {
      // 해당 라인이 꽉 찼음
      deleted_lines_cnt++;
      // 위의 라인들 지우기
      for (int k = i; k >= 0; k--) {
        if (k == 0) {
          // 최상단 라인인 경우 0으로 초기화
          for (int l = 0; l < WIDTH; l++) {
            f[k][l] = 0; 
          }
        } else {
          for (int l = 0; l < WIDTH; l++) {
            f[k][l] = f[k - 1][l]; // 한 칸씩 내리기
          }
        }
      }
    } else {
      i--; // 다음 라인으로 이동
    }
  }
  return deleted_lines_cnt * deleted_lines_cnt * 20000; // 점수 계산 및 반환
}

/******************************************************
 * DrawShadow();
 * 설명: 블록의 그림자 그리기
 * 1. 그림자가 바닥이나 블록에 닿을 때까지 한 칸씩 아래로 이동
 * 2. 최종 위치에 그림자 블록 그리기
 ******************************************************/
void DrawShadow(int y, int x, int blockID, int blockRotate) {
  // user code
  y += 1;
  while (CheckToMove(field, blockID, blockRotate, y, x)) {
    y++;
  }
  y--;
  DrawBlock(y, x, blockID, blockRotate, '/'); // 그림자 블록 그리기
}

/******************************************************
 * DrawRecommend();
 * 설명: 추천 위치에 블록과 그림자 그리기
 ******************************************************/
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
  // user code
  DrawBlock(y, x, blockID, blockRotate, ' '); // 실제 블록 그리기
  DrawShadow(y, x, blockID, blockRotate); // 그림자 그리기
  if (recRoot == NULL) return; // 추천 트리가 없는 경우 종료
  DrawRecommend(recommendY, recommendX, blockID, recommendR); // 추천 위치 그리기
}

/******************************************************
 * createRankList();
 * 설명: "rank.txt" 파일에서 랭킹 정보를 읽어와 링크드 리스트로 저장
 * 1. 파일 열기
 * 2. 랭킹 정보 읽어오기
 * 3. 링크드 리스트에 저장
 * 4. 파일 닫기
 ******************************************************/
void createRankList() {
  // 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹
  // 목록 생성
  // 1. 파일 열기
  FILE* fp = fopen("rank.txt", "r");
  if (fp == NULL) {
    // 파일 열기 실패
    head = NULL;
    score_number = 0;
    printw("There is no rank.txt in current directory\n");
    return;
  }
  // 2. 정보읽어오기
  int cnt = 0;
  if (fscanf(fp, "%d", &cnt) != 1) {
    fclose(fp);
    head = NULL;
    return;
  }
  score_number = cnt;
  // 3. LinkedList로 저장
  for (int i = 0; i < cnt; i++) {
    char temp_name[NAMELEN];
    int temp_score;
    if (fscanf(fp, "%s %d", temp_name, &temp_score) != 2) {
      // 메모리 해제
      Node* ptr = head;
      while (ptr != NULL) {
        Node* toFree = ptr;
        ptr = ptr->next;
        free(toFree);
      }
      head = NULL;
      printw("Error reading while rank data\n");
      fclose(fp);
      return;
    }
    Node* tempNode = (Node*)malloc(sizeof(Node));
    if (tempNode == NULL) {
      // 메모리 해제
      Node* ptr = head;
      while (ptr != NULL) {
        Node* toFree = ptr;
        ptr = ptr->next;
        free(toFree);
      }
      head = NULL;
      printw("Memory allocation failed\n");
      fclose(fp);
      return;
    }
    tempNode->next = NULL;
    strcpy(tempNode->name, temp_name);
    tempNode->score = temp_score;
    // LinkedList에 추가
    insertNode(&head, tempNode);
  }
  // 4. 파일닫기
  fclose(fp);
}

/*****************************************************
 * rank();
 * 설명: 랭킹 메뉴 출력 및 사용자 입력 처리
 * 1. 랭킹 메뉴 출력
 * 2. 사용자 명령 대기
 * 3. 입력된 값에 따라 랭킹 리스트 출력, 검색, 삭제 처리
 *****************************************************/
void rank() {
  // 목적: rank 메뉴를 출력하고 점수 순으로 X부터 Y까지 출력함
  // 1. 문자열 초기화
  int X = 1, Y = score_number, ch;
  clear();

  // 2. printw()로 3개의 메뉴출력
  printw("1. list ranks from X to Y\n");
  printw("2. list ranks by a specific name\n");
  printw("3. delete a specific rank\n");

  // 3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
  ch = wgetch(stdscr);
  // 4. 각 메뉴에 따라 입력받을 값을 변수에 저장
  // 4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의
  // rank 출력
  if (ch == '1') {
    echo();
    printw("X: ");
    scanw("%d", &X);
    printw("Y: ");
    scanw("%d", &Y);
    noecho();

    printw("        name        |    score\n");
    printw("---------------------------------------\n");

    if (X > Y || X < 1 || Y > score_number) {
      printw("search failure: no rank in the list.\n");
      getch();
      return;
    }

    // X와 Y 모두 입력: X부터 Y까지 출력
    Node* ptr = head;
    for (int i = 1; i < X && ptr != NULL; i++) {
      ptr = ptr->next;
    }

    for (int j = X; j <= Y && ptr != NULL; j++) {
      printw("%-19s | %d\n", ptr->name, ptr->score);
      ptr = ptr->next;
    }
  }

  // 4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를
  // 출력
  else if (ch == '2') {
    char str[NAMELEN + 1];
    int check = 0;
    echo();
    printw("input the name: ");
    scanw("%16s", str);
    noecho();

    printw("        name        |    score\n");
    printw("---------------------------------------\n");

    // 이름과 일치하는 노드 출력
    Node* ptr = head;
    while (ptr != NULL) {
      if (strcmp(ptr->name, str) == 0) {
        printw("%-19s | %d\n", ptr->name, ptr->score);
        check = 1;
      }
      ptr = ptr->next;
    }
    // 일치하는 이름 없는 경우
    if (check == 0) {
      printw("search failure: no name in the list.\n");
      getch();
      return;
    }
  }

  // 4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
  else if (ch == '3') {
    int num;
    echo();
    printw("input the rank number: ");
    scanw("%d", &num);
    noecho();

    if (num < 1 || num > score_number) {
      printw("search failure: the rank not in the list.\n");
      getch();
      return;
    }

    Node* ptr = head;
    Node* prev = NULL;
    score_number--;

    for (int i = 1; i < num; i++) {
      prev = ptr;
      ptr = ptr->next;
    }

    if (prev == NULL) {
      // 첫 번째 노드를 삭제하는 경우
      head = ptr->next;
    } else {
      prev->next = ptr->next;
    }
    free(ptr);

    printw("result: the rank deleted.\n");
  }
  getch();
}

/*****************************************************
 * writeRankFile();
 * 설명: 랭킹 정보를 "rank.txt"에 기록
 *****************************************************/
void writeRankFile() {
  // user code
  // 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
  int sn, i;
  // 1. "rank.txt" 연다
  FILE* fp = fopen("rank.txt", "w");
  if (fp == NULL) {
    // 파일 열기 실패
    printw("Rank.txt open failure while writeRankfile\n");
    return;
  }
  // 2. 랭킹 정보들의 수를 "rank.txt"에 기록
  fprintf(fp, "%d\n", score_number);
  Node* ptr = head;
  while (ptr != NULL) {
    fprintf(fp, "%s %d\n", ptr->name, ptr->score);
    ptr = ptr->next;
    ;
  }
  fclose(fp);
}

/*****************************************************
 * newRank();
 * 설명: 게임 오버 시 새로운 랭킹 등록
 *****************************************************/
void newRank(int score) {
  // user code
  // 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의
  // 적절한 위치에 저장
  char str[NAMELEN + 1];
  int i, j;
  clear();
  // 1. 사용자 이름을 입력받음
  printw("Your name: ");
  echo(); // 입력된 문자열이 화면에 보이도록 설정
  scanw("%16s", str); // 최대 16자까지 입력
  noecho(); // 입력된 문자열이 화면에 보이지 않도록 설정 해제

  // 새 노드 할당
  Node* newNode = (Node*)malloc(sizeof(Node));
  if (newNode == NULL) {
    printw("Memory allocation failed\n");
    return;
  }

  // 적절한 위치에 삽입
  strcpy(newNode->name, str);
  newNode->score = score;
  newNode->next = NULL;
  insertNode(&head, newNode);
  score_number++;
  writeRankFile(); // 랭킹 파일에 기록
}

/************************
 * DrawRecommend()
 * 설명 : 추천 블록 그리기 함수
 ************************/
void DrawRecommend(int y, int x, int blockID, int blockRotate) {
  DrawBlock(y, x, blockID, blockRotate, 'R');
}

/***********************************************************
 * evaluateField()
 * 설명 : 필드 상태 평가 함수
 * 1. 구멍 수 계산
 * 2. 전체 높이 계산
 * 3. 울퉁불퉁 계산
 * 4. 우물 계산
 * 5. 높이 차이 계산
 * 6. 완성 가능 라인 보너스 계산
 * 7. 패널티 점수 계산 및 반환
 ***********************************************************/
int evaluateField(char f[HEIGHT][WIDTH]) {
  int holes = 0; // 구멍 수
  int heightColumns[WIDTH] = {0}; // 각 열 높이 저장
  int totalHeight = 0; // 총 높이
  int bumpiness = 0; // 울퉁불퉁 정도
  int wells = 0; // 골짜기 점수
  int maxHeight = 0; // 최대 높이
  int minHeight = HEIGHT; // 최소 높이: 초기값은 최대 높이로 설정
  // 각 열마다 구멍 및 높이 계산
  for (int x = 0; x < WIDTH; x++) {
    int blockFoundFlag = 0; // 블록 발견 플래그
    for (int y = 0; y < HEIGHT; y++) {
      if (f[y][x] != 0) { // 블록이 있음
        blockFoundFlag = 1; // 블록 flag 설정
        if (heightColumns[x] == 0) { // 아직 높이 계산 안됨
          heightColumns[x] = HEIGHT - y;  // 해당 열의 높이 계산
        }
      } else if (blockFoundFlag == 1) { // 블록이 이미 발견된 상태에서 빈 칸
        holes++;  // 구멍 발견
      }
    }
    totalHeight += heightColumns[x]; // 현재 열의 높이를 전체 높이 누적
    if (heightColumns[x] > maxHeight) { // 최대 높이 갱신
      maxHeight = heightColumns[x];
    }
    if (heightColumns[x] < minHeight) { // 최소 높이 갱신
      minHeight = heightColumns[x];
    }
  }

  // 울퉁불퉁 계산
  for (int i = 0; i < WIDTH - 1; i++) {
    bumpiness += abs(heightColumns[i] - heightColumns[i + 1]);
  }

  // 골짜기 계산
  for (int x = 0; x < WIDTH; x++) {
    int heightleft = HEIGHT;
    int l = x - 1;
    while (l >= 0) { // 왼쪽으로 탐색
      if (heightColumns[l] > heightColumns[x]) {
        heightleft = heightColumns[l]; // 더 높은 열 발견
        break;
      } else if (heightColumns[l] < heightColumns[x]) {
        heightleft = 0; // 더 낮은 열 발견 - 골짜기 아님
        break;
      }
      l--; // 계속 왼쪽으로 이동
    }
    // 오른쪽으로 탐색
    int heightright = HEIGHT;
    int r = x + 1;
    while (r < WIDTH) {
      if (heightColumns[r] > heightColumns[x]) {
        heightright = heightColumns[r]; // 더 높은 열 발견
        break;
      } else if (heightColumns[r] < heightColumns[x]) {
        heightright = 0; // 더 낮은 열 발견 - 골짜기 아님
        break;
      }
      r++; // 계속 오른쪽으로 이동
    }
    // 둘 중 더 낮은 높이 선택
    int min = 0;
    if (heightleft < heightright)
      min = heightleft;
    else
      min = heightright;
    int depth = 0;
    if (heightColumns[x] < min) {
      depth = min - heightColumns[x];
      if (x == 0 || x == WIDTH - 1) {
        wells += (depth * depth * 10);
      }
      else {
        wells += (depth * depth);
      }
    }
  }
  // 높이 차이 계산
  int heightDiff = maxHeight - minHeight;
  int heightPenalty = 0;

  // 최대 높이에 따른 패널티 계산식 다름
  if (maxHeight > 18) {
    heightPenalty = 100000;
  } else if (maxHeight > 15) {
    heightPenalty = (maxHeight - 15) * 5000;
  } else if (maxHeight > 12) {
    heightPenalty = (maxHeight - 12) * 1000;
  }
  // 완성 가능 라인 보너스
  int almostFullBonus = 0;
  for (int y = 0; y < HEIGHT; y++) {
    int filled = 0;
    for (int x = 0; x < WIDTH; x++) {
      if (f[y][x] != 0) filled++;
    }
    // 8칸 이상 차있으면 보너스
    if (filled >= WIDTH - 1) {
      almostFullBonus += 3000;
    }
    // 9칸 차있으면 더 큰 보너스
    if (filled >= WIDTH - 1) {
      almostFullBonus += 5000;
    }
  }
  // 최종 페널티 점수
  int penalty = 0;
  penalty = holes * 5000 + bumpiness * 100 + totalHeight * 50 + wells * 300 +
            heightPenalty + heightDiff * 800 - almostFullBonus;
  return penalty;
}

/***********************************************************
 * 함수명: modified_recommend()
 * 설명: Beam Search를 이용한 추천 알고리즘 구현
 * 1. 점수가 높은 상위 BEAM WIDTH개의 자식 노드만큼만 선택
 * 2. 재귀적으로 다음 블록 평가
 * 3. 가장 좋은 경로 추천 위치
 ***********************************************************/
int modified_recommend(RecNode* root) {
  int currentLevel = root->level; // 현재 깊이
  int maxScore = -1000000000; // 최대 점수 매우 작은 값으로 초기화
  int currentBlockID = nextBlock[currentLevel];

  // 종료 조건: 깊이가 3일 경우
  if (currentLevel == BLOCK_NUM) {
    return root->accumulatedScore; // 누적 점수 반환
  }

  // 각 블록 type별 최대 회전 수
  // currentBlockID == 1, 2, 3 일 때는 다 고려
  // currentBlockID == 0, 5, 6 일 때는 2번 회전까지 고려
  // currentBlockID == 4 일 때는 1번 회전까지 고려
  int maxRotations[7] = {2, 4, 4, 4, 1, 2, 2};
  
  RecNode* candidates[CHILDREN_MAX]; // 후보군 배열
  int candidateCnt = 0; // 실제 생성한 후보군 수

  // 모든 가능한 회전에 대해 시도
  for (int rotate = 0; rotate < maxRotations[currentBlockID]; rotate++) {
    // 블록을 필드의 모든 x좌표에 대해 시도
    for (int x = -3; x < WIDTH; x++) {
      int y = 0; // 맨 위에서부터 시작
      // 블록이 해당 위치에 놓일 수 있는지 확인
      if (!CheckToMove(root->recField, currentBlockID, rotate, y, x)) {
        continue;  // 해당 위치에 놓을 수 없으면 다음 x좌표로
      }
      while (CheckToMove(root->recField, currentBlockID, rotate, y + 1, x)) {
        y++; // 블록이 더 내려갈 수 있을 때까지 내림
      }

      // 현재 필드 상태를 복사하기 위한 임시 필드 생성
      char tempField[HEIGHT][WIDTH];
      // 블록을 추가하기 전에 필드 상태 복사
      memcpy(tempField, root->recField, sizeof(char) * HEIGHT * WIDTH);

      // 블록을 필드에 추가 및 점수 계산
      int scoreToAdd = AddBlockToField(tempField, currentBlockID, rotate, y, x);
      // 라인 삭제 및 점수 계산
      scoreToAdd += DeleteLine(tempField);
      // 필드 평가 penalty 차감
      scoreToAdd -= evaluateField(tempField);
      // 최종 누적 점수 계산
      int accumulatedScore = root->accumulatedScore + scoreToAdd;
      // 새로운 후보 노드 생성
      RecNode* childNode = (RecNode*)malloc(sizeof(RecNode));
      childNode->level = currentLevel + 1; // 깊이는 현재 깊이 + 1
      childNode->accumulatedScore = accumulatedScore; // 누적 점수 설정
      childNode->child = NULL; // 자식 노드 초기화
      childNode->blockX = x; // 블록 x좌표 설정
      childNode->blockY = y; // 블록 y좌표 설정
      childNode->blockRotate = rotate; // 블록 회전 설정

      // 필드 상태 복사
      memcpy(childNode->recField, tempField, sizeof(char) * HEIGHT * WIDTH);

      // 자리 있으면 후보 배열에 추가
      if (candidateCnt < CHILDREN_MAX) {
        candidates[candidateCnt++] = childNode;
      }

      else {
        free(childNode);  // 후보군이 너무 많으면 메모리 해제
      }
    }
  }
  // 후보군이 없으면(모든 위치 불가능) 현재 누적 점수 반환
  if (candidateCnt == 0) {
    return root->accumulatedScore;
  }
  // 후보군 정렬: beam search로 점수 기준 내림차순 정렬
  qsort(candidates, candidateCnt, sizeof(RecNode*), compareRecNodes);

  // 상위 BEAM WIDTH개만 자식 노드로 추가
  int beamWidth;
  if (BEAM_WIDTH < candidateCnt) {
    beamWidth = BEAM_WIDTH;
  } else {
    beamWidth = candidateCnt;
  }

  // 자식노드 배열 할당
  root->child = (RecNode**)malloc(sizeof(RecNode*) * CHILDREN_MAX);
  for (int i = 0; i < CHILDREN_MAX; i++) {
    root->child[i] = NULL; // 전부 NULL로 초기화
  }

  // 자식 노드들에 대해 재귀적으로 평가
  for (int i = 0; i < beamWidth; i++) {
    if (candidates[i] != NULL)
      root->child[i] = candidates[i]; // 자식 노드 연결
    else
      root->child[i] = NULL;
    // 재귀호출
    int childMaxScore = modified_recommend(root->child[i]);
    // 더 크면 최대 점수 갱신
    if (childMaxScore > maxScore) {
      maxScore = childMaxScore;
      // 루트 레벨이면 추천 위치 갱신
      if (currentLevel == 0) {
        recommendX = root->child[i]->blockX; // x좌표
        recommendY = root->child[i]->blockY; // y좌표
        recommendR = root->child[i]->blockRotate; // 회전
      }
    }
  }
  for (int i = beamWidth; i < candidateCnt; i++) {
    if (candidates[i] != NULL) {
      free(candidates[i]);  // 사용되지 않은 후보군 메모리 해제
    }
  }
  return maxScore; // 최대 점수 반환
}

/******************************************************
 * recommendedPlay();
 * 설명: 추천 위치에 자동으로 블록 배치 및 게임 플레이
 * 1. InitTetris() 호출하여 테트리스 초기화
 * 2. 추천 알고리즘을 이용하여 위치 받음
 * 3. 블록을 해당 위치에 배치
 * 4. 다음 블록으로 반복
 * 5. 게임 오버 시 랭킹 등록
 ******************************************************/
void recommendedPlay() {
  clear();
  
  InitTetris(); // 게임 초기화

  alarm(0); // 타이머 중지
  nodelay(stdscr, TRUE); // 키 입력 기다리지 않도록

  while (!gameOver) {
    // q키로 종료
    if (GetCommand() == QUIT) {
      break;
    }
    // 추천 알고리즘이 추천한 위치 및 회전
    int targetRotate = recommendR; // 추천 회전
    int targetX = recommendX; // 추천 X 위치

    // 새 블록 초기화
    blockRotate = targetRotate; // 회전 적용
    blockX = targetX; // x 위치 가져오기
    blockY = -1;  // 맨 위에서 시작!

    // 맨 위에서도 놓을 수 없으면 게임오버
    if (!CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX)) {
      gameOver = 1;
      break;
    }

    // 블록을 떨어뜨림
    while (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
      blockY++;
    }

    DrawField(); // 필드 그리기
    DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate); // 블록 및 그림자 그리기
    DrawNextBlock(nextBlock); // 다음 블록 그리기
    PrintScore(score); // 점수 출력
    refresh();

    usleep(300000);  // 0.3초 대기 - 시각적으로 보여줘야 해서

    BlockDown(0); // 블록 내리기 및 추가

    usleep(200000);  // 0.2초 대기
  }
  // 게임 종료시
  nodelay(stdscr, FALSE); // 키 입력 모드 복원
  DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
  move(HEIGHT / 2, WIDTH / 2 - 4);
  printw("GameOver!!");
  refresh();
  getch();

  if (recRoot != NULL) freeRecNodes(recRoot); // 추천 트리 해제

  newRank(score); // 랭킹 등록
}

/******************************************************
 * insertNode();
 * 설명: 점수에 따라 노드를 정렬하여 링크드리스트에 삽입
 * 해당 링크드리스트는 점수가 내림차순으로 정렬됨
 * 1. ref_head가 NULL 또는 삽입 노드의 점수가 더 크면: 맨 앞에 삽입
 * 2. 그렇지 않으면 순회하며 적절한 위치를 찾아 삽입
 ******************************************************/
void insertNode(Node** ref_head, Node* newNode) {
  // 맨 앞에 삽입하는 경우
  if (*ref_head == NULL || newNode->score > (*ref_head)->score) {
    newNode->next = *ref_head; // 삽입할 노드의 다음을 현재 헤드로 설정
    *ref_head = newNode; // 헤드를 삽입할 노드로 업데이트
    return;
  }
  Node* current = *ref_head; // 순회용 포인터
  while (current->next != NULL && current->next->score >= newNode->score) {
    current = current->next; // 다음 노드로 이동
  }

  newNode->next = current->next; // 삽입할 노드의 다음을 현재 노드의 다음으로 설정
  current->next = newNode; // 현재 노드의 다음을 삽입할 노드로 설정
}

/******************************************************
 * freeRecNodes();
 * 설명: RecNode 트리를 메모리에서 해제
 * 1. 추천트리의 모든 노드를 재귀적으로 해제
 * 2. 현재 노드(최정족으로 root)를 해제
 ******************************************************/
void freeRecNodes(RecNode* root) {
  // NULL이면 종료
  if (root == NULL) return;
  // 재귀적으로 자식 노드 해제
  if (root->child != NULL) {
    for (int i = 0; i < CHILDREN_MAX; i++) {
      if (root->child[i] != NULL) {
        freeRecNodes(root->child[i]); // 재귀 호출로 자식 노드 해제
      }
    }
    free(root->child); // 자식 배열 해제
    root->child = NULL;
  }
  free(root); // 현재 노드 해제
}

/******************************************************
 * compareRecNodes();
 * 설명: RecNode의 점수를 비교하는 함수로 qsort에서 사용
 * 1. 두 노드의 accumulatedScore 값을 두 변수에 저장
 * 2. scoreA가 scoreB보다 크면 -1 반환 (내림차순 정렬)
 * 3. scoreA가 scoreB보다 작으면 1 반환
 * 4. 같으면 0 반환
 ******************************************************/
int compareRecNodes(const void* a, const void* b) {
  RecNode* nodeA = *(RecNode**)a;
  RecNode* nodeB = *(RecNode**)b;
  int scoreA = nodeA->accumulatedScore;
  int scoreB = nodeB->accumulatedScore;
  // 내림차순 정렬
  if (scoreA > scoreB)
    return -1; // scoreA가 더 크면 앞에 오도록
  else if (scoreA < scoreB)
    return 1; // scoreB가 더 크면 뒤에 오도록
  else
    return 0; // 같으면 순서 유지
}
