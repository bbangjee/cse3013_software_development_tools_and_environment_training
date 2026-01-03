#pragma once

#include "ofMain.h"
#include <stack>
#include <vector>

typedef struct cell_info {
	bool left_wall;
	bool right_wall;
	bool up_wall;
	bool down_wall;
}cell_info;

class ofApp : public ofBaseApp {
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		bool readFile();
		void freeMemory();
		bool DFS();
		void dfsdraw();
		void buildGraph();
		void doFullScreen(bool bFull);

		int HEIGHT;       // 미로의 높이
		int WIDTH;        // 미로의 너비
		char** input;     // 텍스트 파일의 모든 정보를 담는 이차원 배열이다.
		int maze_col;     // 미로칸의 열의 인덱스를 가리킨다.
		int maze_row;     // 미로칸의 행의 인덱스를 가리킨다.

		int isOpen;       // 파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
		bool isDFS;       // DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		bool isBFS;       // BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		bool bShowInfo;   // 화면 정보 표시
		bool bFullscreen; // 전체 화면
		
		vector<string> lines; // 미로 그대로 저장
		cell_info** cells;
		vector<vector<vector<pair<int,int>>>> graph;
		vector<vector<bool>> visited;
		vector<vector<pair<int, int>>> parent; // 경로 추적용 - 부모 저장
		vector<pair<int, int>> visitOrder; // 탐색 순서 저장
		vector<pair<int, int>> finalPath; // 최종 경로
		
		// font
		ofTrueTypeFont myFont;
};

