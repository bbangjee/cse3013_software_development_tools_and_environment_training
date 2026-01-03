#include "tetris.h"

static struct sigaction act, oact;

Node* head = NULL; // 랭킹 리스트의 head 노드를 가리키는 포인터

int main() {
  int exit = 0;

  initscr();
  noecho();
  
  keypad(stdscr, TRUE);
  srand((unsigned int)time(NULL));
  createRankList();

  while (!exit) {
    clear();
    switch (menu()) {
      case MENU_PLAY:
        play();
        break;
      case MENU_RANK:
        rank();
        break;
      case MENU_EXIT:
        exit = 1;
        break;
      default:
        break;
    }
  }

  writeRankFile();
  Node *curr = head;
  while (curr != NULL) {
    Node *temp = curr;
    curr = curr->next;
    free(temp);
  }
  head = NULL;
  score_number = 0;
  endwin();
  system("clear");
  return 0;
}

void InitTetris() {
  int i, j;

  for (j = 0; j < HEIGHT; j++)
    for (i = 0; i < WIDTH; i++) field[j][i] = 0;

  nextBlock[0] = rand() % 7;
  nextBlock[1] = rand() % 7;
  nextBlock[2] = rand() % 7;
  blockRotate = 0;
  blockY = -1;
  blockX = WIDTH / 2 - 2;
  score = 0;
  gameOver = 0;
  timed_out = 0;

  DrawOutline();
  DrawField();
  DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
  DrawNextBlock(nextBlock);
  PrintScore(score);
}

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
      /*fall block*/
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
      break;
    case KEY_DOWN:
      if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1,
                                  blockX)))
        blockY++;
      break;
    case KEY_RIGHT:
      if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY,
                                  blockX + 1)))
        blockX++;
      break;
    case KEY_LEFT:
      if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY,
                                  blockX - 1)))
        blockX--;
      break;
    default:
      break;
  }
  if (drawFlag)
    DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
  return ret;
}

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

void PrintScore(int score) {
  move(17, WIDTH + 11);
  printw("%8d", score);
}

void DrawNextBlock(int* nextBlock) {
  int i, j;
  for (i = 0; i < 4; i++) {
    move(4 + i, WIDTH + 13);
    for (j = 0; j < 4; j++) {
      if (block[nextBlock[1]][0][i][j] == 1) {
        attron(A_REVERSE);
        printw(" ");
        attroff(A_REVERSE);
      } else
        printw(" ");
    }
  }

  for (i = 0; i < 4; i++) {
    move(10 + i, WIDTH + 13);
    for (j = 0; j < 4; j++) {
      if (block[nextBlock[2]][0][i][j] == 1) {
        attron(A_REVERSE);
        printw(" ");
        attroff(A_REVERSE);
      } else
        printw(" ");
    }
  }
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile) {
  int i, j;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
        move(i + y + 1, j + x + 1);
        attron(A_REVERSE);
        printw("%c", tile);
        attroff(A_REVERSE);
      }
    }

  move(HEIGHT, WIDTH + 10);
}

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

void play() {
  int command;
  clear();
  act.sa_handler = BlockDown;
  sigaction(SIGALRM, &act, &oact);
  InitTetris();
  do {
    if (timed_out == 0) {
      alarm(1);
      timed_out = 1;
    }

    command = GetCommand();
    if (ProcessCommand(command) == QUIT) {
      alarm(0);
      DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
      move(HEIGHT / 2, WIDTH / 2 - 4);
      printw("Good-bye!!");
      refresh();
      getch();

      return;
    }
  } while (!gameOver);

  alarm(0);
  getch();
  DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
  move(HEIGHT / 2, WIDTH / 2 - 4);
  printw("GameOver!!");
  refresh();
  getch();
  newRank(score);
}

char menu() {
  printw("1. play\n");
  printw("2. rank\n");
  printw("3. recommended play\n");
  printw("4. exit\n");
  return wgetch(stdscr);
}

/* 1주차 실습 구현 */

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
        if (i + blockY >= HEIGHT || j + blockX < 0 || j + blockX >= WIDTH || f[i + blockY][j + blockX] == 1) {
          return 0;  // 이동 불가
        }
      }
    }
  }
  return 1;  // 이동 가능
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock,
                int blockRotate, int blockY, int blockX) {
  // user code
  // 1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
  // 2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
  // 3. 새로운 블록 정보를 그린다.
  int prevBlockRotate = blockRotate;
  int prevBlockY = blockY;
  int prevBlockX = blockX;
  switch (command) {
    case KEY_UP:
      prevBlockRotate = (blockRotate + 3) % 4;
      break;
    case KEY_DOWN:
      prevBlockY = blockY - 1;
      break;
    case KEY_RIGHT:
      prevBlockX = blockX - 1;
      break;
    case KEY_LEFT:
      prevBlockX = blockX + 1;
      break;
    default:
      break;
  }
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if(block[currentBlock][prevBlockRotate][i][j] == 1 && i + prevBlockY >= 0){
        move(i + prevBlockY + 1, j + prevBlockX + 1);
        printw(".");
      }
    }
  }

  int shadowY = prevBlockY + 1;
  int shadowX = prevBlockX;

  while (CheckToMove(f, currentBlock, prevBlockRotate, shadowY, shadowX)) {
    shadowY++;
  }
  shadowY--;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if(block[currentBlock][prevBlockRotate][i][j] == 1 && i + shadowY >= 0){
        move(i + shadowY + 1, j + shadowX + 1);
        printw(".");
      }
    }
  }

  // DrawBlock(prevBlockY, prevBlockX, currentBlock, prevBlockRotate, '.');
  DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

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
    if (blockY == -1) gameOver = 1;
    score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
    score += DeleteLine(field);

    blockY = -1;
    blockX = WIDTH / 2 - 2;
    blockRotate = 0;

    nextBlock[0] = nextBlock[1];
    nextBlock[1] = nextBlock[2];
    nextBlock[2] = rand() % 7;

    DrawNextBlock(nextBlock);
    PrintScore(score);
    DrawField();
  }
  return;
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate,
                     int blockY, int blockX) {
  // user code
  // Block이 추가된 영역의 필드값을 바꾼다.
  int touched_bottom = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (block[currentBlock][blockRotate][i][j] == 1) {
        if (i + blockY == HEIGHT - 1) {
          // 블록이 필드의 맨 아래에 닿은 경우
          touched_bottom ++;
        }
        else if (f[i + blockY + 1][j + blockX] == 1) {
          // 블록이 다른 블록에 닿은 경우
          touched_bottom ++;
        }
        f[i + blockY][j + blockX] = 1;
      }
    }
  }
  return touched_bottom * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
  // user code
  // 1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
  // 2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을
  // 한칸씩 내린다.
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
        }
        else {
          for (int l = 0; l < WIDTH; l++) {
            f[k][l] = f[k - 1][l];
          }
        }
      }
    }
    else {
      i--;
    }
  }
  return deleted_lines_cnt * deleted_lines_cnt * 100;
}

/* 1주차 과제 구현 */

void DrawShadow(int y, int x, int blockID, int blockRotate) {
  // user code
  y += 1;
  while (CheckToMove(field, blockID, blockRotate, y, x)) {
    y++;
  }
  y--;
  DrawBlock(y, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
  // user code
  DrawBlock(y, x, blockID, blockRotate, ' ');
  DrawShadow(y, x, blockID, blockRotate);
}

void createRankList() {
  // 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	//1. 파일 열기
	FILE *fp = fopen("rank.txt", "r");
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

void rank() {
  // user code
  //목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X = 1, Y = score_number, ch;
	clear();

	//2. printw()로 3개의 메뉴출력
  printw("1. list ranks from X to Y\n");
  printw("2. list ranks by a specific name\n");
  printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
  ch = wgetch(stdscr);
	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
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

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
    echo();
    printw("input the name: ");
    scanw("%16s", str);
    noecho();
    
    printw("        name        |    score\n");
    printw("---------------------------------------\n");

    Node* ptr = head;
    while (ptr != NULL) {
      if (strcmp(ptr->name, str) == 0) {
        printw("%-19s | %d\n", ptr->name, ptr->score);
        check = 1;
      }
      ptr = ptr->next;
    }
    if (check == 0) {
      printw("search failure: no name in the list.\n");
      getch();
      return;
    }
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
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
    }
    else {
      prev->next = ptr->next;
    }
    free(ptr);
    
    printw("result: the rank deleted.\n");
	}
	getch();
}

void writeRankFile() {
  // user code
  // 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");
  if (fp == NULL) {
    // 파일 열기 실패
    printw("Rank.txt open failure while writeRankfile\n");
    return;
  }
	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
  fprintf(fp, "%d\n", score_number);
  Node* ptr = head;
  while (ptr != NULL) {
    fprintf(fp, "%s %d\n", ptr->name, ptr->score);
    ptr = ptr->next;;
  }
  fclose(fp);
}

void newRank(int score) {
  // user code
  // 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
  printw("Your name: ");
  echo();
  scanw("%16s", str);
  noecho();

  Node *newNode = (Node*)malloc(sizeof(Node));
  if (newNode == NULL) {
    printw("Memory allocation failed\n");
    return;
  }

  strcpy(newNode->name, str);
  newNode->score = score;
  newNode->next = NULL;
	insertNode(&head, newNode);
  score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID, int blockRotate) {
  // user code
}

int recommend(RecNode* root) {
  int max =
      0;  // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

  // user code

  return max;
}

void recommendedPlay() {
  // user code
}

void insertNode(Node** ref_head, Node* newNode) {
  // user code
  if (*ref_head == NULL || newNode->score > (*ref_head)->score) {
    newNode->next = *ref_head;
    *ref_head = newNode;
    return;
  }
  Node* current = *ref_head;
  while (current->next != NULL && current->next->score >= newNode->score) {
    current = current->next; 
  }

  newNode->next = current->next;
  current->next = newNode;
}