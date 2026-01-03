#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N, M;  // 미로의 너비, 미로의 높이

// 핵심 규칙 2가지:
// 같은 집합끼리는 벽을 세워야 함 (안 그러면 순환 경로 생김)
// 각 집합은 최소 1개는 아래로 연결되어야 함 (안 그러면 고립됨)
// 각 방의 모서리 : ‘+’, 가로 벽 : ‘-’, 세로 벽 : ‘|’, 방 : ' '

int main() {
  int temp = scanf("%d %d", &N, &M);
  // printf("N = %d, M = %d\n", N, M);
  srand((unsigned int)time(NULL));

  // 1차원 배열
  int* sets = (int*)malloc(N * sizeof(int));
  // 2차원 배열
  int** right_walls = (int**)malloc(M * sizeof(int*));
  for (int i = 0; i < M; i++) {
    right_walls[i] = (int*)malloc(N * sizeof(int));
    for (int j = 0; j < N; j++) {
      right_walls[i][j] = 0;
    }
  }

  int** up_down_walls = (int**)malloc(M * sizeof(int*));
  for (int i = 0; i < M; i++) {
    up_down_walls[i] = (int*)malloc(N * sizeof(int));
    for (int j = 0; j < N; j++) {
      up_down_walls[i][j] = 0;
    }
  }

  int set_num = 1;

  for (int row = 0; row < M; row++) {
    // 첫 줄의 경우 집합 초기화
    if (row == 0) {
      for (int i = 0; i < N; i++) sets[i] = set_num++;
    }

    for (int col = 0; col < N - 1; col++) {
      // 집합 번호가 다르다면
      if (sets[col] != sets[col + 1]) {
        // 벽을 제거할지 말지 임의로 선택
        right_walls[row][col] = rand() % 2;
        if (right_walls[row][col] == 0) {
          // 어 벽이 제거되었네? 그럼 두 집합을 합쳐야지
          int old_set = sets[col + 1];
          int new_set = sets[col];
          for (int k = 0; k < N; k++) {
            if (sets[k] == old_set) {
              sets[k] = new_set;
            }
          }
        }
      }
      // 집합 번호가 같다면
      else {
        right_walls[row][col] = 1;  // 같은 집합이면 벽 세우기
      }
    }
    right_walls[row][N - 1] = 1;  // 마지막 열은 무조건 벽 세우기

    // 각 집합이 최소 1개는 아래로 연결되도록 하기
    if (row < M - 1) {
      int* set_connected = (int*)malloc(set_num * sizeof(int));
      for (int i = 0; i < set_num; i++) set_connected[i] = 0;
      for (int col = 0; col < N; col++) {
        // 아래로 연결할지 말지 임의로 선택
        int which_set = sets[col];
        if (set_connected[which_set] == 0) {
          // 아직 이 집합은 아래로 연결된 적이 없음
          up_down_walls[row][col] = 0;  // 무조건 아래로 연결
          set_connected[which_set] = 1; // 이 집합은 아래로 연결됨
        }
        else {
          up_down_walls[row][col] = rand() % 2;
        }
      }

      for (int col = 0; col < N; col++) {
        if (up_down_walls[row][col] == 1) {
          // 아래로 연결하지 않는다면 새로운 집합 번호 부여
          sets[col] = set_num++;
        }
      }
      free(set_connected);
    }
    
    else {
      for (int col = 0; col < N - 1; col++) {
        if (sets[col] != sets[col + 1]) {
          right_walls[row][col] = 0;
          int old_set = sets[col + 1];
          int new_set = sets[col];
          for (int k = 0; k < N; k++) {
            if (sets[k] == old_set) {
              sets[k] = new_set;
            }
          }
        }
      }
      right_walls[row][N - 1] = 1;
      for (int col = 0; col < N; col++) {
        up_down_walls[row][col] = 1;
      }
    }
  }

  // 미로 출력
  FILE *fp = fopen("maze.maz", "w");
  // 맨 위 벽 출력
  for (int i = 0; i < N; i++) {
    fprintf(fp, "+-");
  }
  fprintf(fp, "+\n");

  for (int row = 0; row < M; row++) {
    // 방 및 세로 벽 출력
    fprintf(fp, "|");
    for (int col = 0; col < N; col++) {
      fprintf(fp, " ");
      if (right_walls[row][col] == 1) {
        fprintf(fp, "|");
      }
      else {
        fprintf(fp, " ");
      }
    }
    fprintf(fp, "\n");
    // 가로 벽 출력
    for (int col = 0; col < N; col++) {
      fprintf(fp, "+");
      if (up_down_walls[row][col] == 1) fprintf(fp, "-");
      else fprintf(fp, " ");
    }
    fprintf(fp, "+\n");
  }

  fclose(fp);
  free(sets);
  for (int i = 0; i < M; i++) {
    free(right_walls[i]);
  }
  free(right_walls);

  for (int i = 0; i < M; i++) {
    free(up_down_walls[i]);
  }
  free(up_down_walls);
  
  return 0;
}