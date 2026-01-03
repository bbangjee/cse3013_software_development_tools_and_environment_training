#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	
	isDFS = false;
	isOpen = 0;
	bShowInfo = true;
	bFullscreen = false;
	cells = nullptr;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetWidth())/2);

	// Load a font rather than the default
	myFont.load("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

} // end Setup

//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	// TO DO : DRAW MAZE;
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	if (isOpen) {
		int cellSize = 20;
		int offsetforX = 10;
		int offsetforY = 10;
		for (int row = 0; row < HEIGHT; row++) {
			for (int col = 0; col < WIDTH; col++) {
				int x = offsetforX + col * cellSize;
				int y = offsetforY + row * cellSize;
				
				if (cells[row][col].up_wall) {
					ofDrawLine(x, y, x + cellSize, y);
				}
				if (cells[row][col].down_wall) {
					ofDrawLine(x, y + cellSize, x + cellSize, y + cellSize);
				}
				if (cells[row][col].left_wall) {
					ofDrawLine(x, y, x, y + cellSize);
				}
				if (cells[row][col].right_wall) {
					ofDrawLine(x + cellSize, y, x + cellSize, y + cellSize);
				}
				
			}
		}
	}

	if (isDFS) {
		ofSetColor(200, 0, 0);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			myFont.drawString("You must open file first", 10, 20);
	}
	
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		ofSetColor(0);
		myFont.drawString("Comsil Project", 15, ofGetHeight() - 20);
		myFont.drawString("[O] Open File  [D] Toggle DFS  [F] Fullscreen", 15, ofGetHeight()-40);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	ofSetFullscreen(bFull);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if(key == 'o' || key == 'O') {
		readFile();
	}
	
	if(key == 'd' || key == 'D') {
		if(isOpen) {
			DFS();
			isDFS = !isDFS;
		}
		else {
			cout << "you must open file first\n";
		}
	}
	
	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
	}

	if(key == 'f' || key == 'F') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}
} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	if (!openFileResult.bSuccess) return false;

	ofLogVerbose("User selected a file");
	string filePath = openFileResult.getPath();
	string fileName = openFileResult.getName();
	
	// 확장자 체크
	if (!ofIsStringInString(fileName, ".maz")) {
		cout << "Needs a '.maz' extension\n";
		return false;
	}
	
	// 파일 로드
	ofFile file(filePath);
		
	if (!file.exists()) {
		cout << "Target file does not exists." << endl;
		return false;
	}
	
	ofBuffer buffer(file); // file을 버퍼로
	
	lines.clear();
	HEIGHT = 0;
	WIDTH = 0;
	bool firstLine = true;
	
	for (auto line : buffer.getLines()) {
		string str = line;
		lines.push_back(str);
		if (firstLine) {
			// 첫 줄에서만 '-' 개수 세기
			for (char ch : str) {
				if (ch == '-') WIDTH++;
			}
			firstLine = false;
		}
		if (str.empty() == false && str[0] == '|') HEIGHT++;
	}
	cout << "WIDTH: " << WIDTH << "  HEIGHT: " << HEIGHT << "\n";
	
	cells = (cell_info**)malloc(sizeof(cell_info*) * HEIGHT);
	for (int i = 0; i < HEIGHT; i++) {
		cells[i] = (cell_info*)malloc(sizeof(cell_info) * WIDTH);
		
		for (int j = 0; j < WIDTH; j++) {
			cells[i][j].up_wall = false;
			cells[i][j].down_wall = false;
			cells[i][j].left_wall = false;
			cells[i][j].right_wall = false;
		}
	}
	
	int i = 0;
	for (int row = 1; row < lines.size(); row += 2) {
		if (i >= HEIGHT) break;
		int j = 0;
		for (int col = 1; col < lines[row].size(); col += 2) {
			if (lines[row][col] == ' ') {
				if (lines[row - 1][col] == '-') cells[i][j].up_wall = true;
				if (lines[row + 1][col] == '-')	cells[i][j].down_wall = true;
				if (lines[row][col - 1] == '|')	cells[i][j].left_wall = true;
				if (lines[row][col + 1] == '|') cells[i][j].right_wall = true;
			}
			j++;
		}
		i++;
	}
	// TO DO
	// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

	cout << "We found the target file." << endl;
	
	buildGraph();
	isOpen = 1;
	return true;
}

void ofApp::freeMemory() {
	// TO DO
	// malloc한 memory를 free해주는 함수
	lines.clear();
	if (cells != nullptr) {
		for (int i = 0; i < HEIGHT; i++) {
			free(cells[i]);
		}
		free(cells);
		cells = nullptr;
	}
}

//DFS탐색을 하는 함수
bool ofApp::DFS() {
	// stack 초기화
	visited.assign(HEIGHT, vector<bool>(WIDTH, false));
	parent.assign(HEIGHT, vector<pair<int, int>>(WIDTH, {-1, -1}));
	
	pair<int, int> startpoint = {0, 0};
	pair<int, int> endpoint = {HEIGHT - 1, WIDTH - 1};
	
	stack <pair<int, int>> s;
	s.push(startpoint);
	visited[startpoint.first][startpoint.second] = true;
	visitOrder.push_back(startpoint);
	
	bool found = false;
	
	while(!s.empty()) {
		pair<int, int> curr = s.top();
		
		if (curr == endpoint) {
			found = true;
			break;
		}
		bool adjacent_neighbor_flag = false;
		for (auto &neighbor : graph[curr.first][curr.second]) {
			int ny = neighbor.first;
			int nx = neighbor.second;
			
			if (visited[ny][nx] == false) {
				visited[ny][nx] = true;
				parent[ny][nx] = curr;
				visitOrder.push_back({ny, nx});
				s.push({ny, nx});
				adjacent_neighbor_flag = true;
				break;
			}
			
		}
		if (adjacent_neighbor_flag == false) {
			s.pop();
		}
	}
	// 최종 경로 역추적
	if (found == true) {
		pair<int, int> temp = endpoint;
		while (temp != startpoint) {
			finalPath.push_back(temp);
			int temp_y = temp.first;
			int temp_x = temp.second;
			temp = parent[temp.first][temp.second];
		}
		finalPath.push_back(startpoint);
		cout << "DFS Success\n";
		return true;
	}
	cout << "DFS Fail\n";
	return false;
}

void ofApp::dfsdraw()
{
	int cellSize = 20;
	int offsetforX = 10;
	int offsetforY = 10;
	ofSetLineWidth(5);
	
	ofSetColor(200, 200, 200);
	for (int i = 1; i < visitOrder.size(); i++) {
		pair<int, int> temp = visitOrder[i];
		int curr_y = temp.first;
		int curr_x = temp.second;
		pair<int, int> temp_parent = parent[curr_y][curr_x];
		int parent_y = temp_parent.first;
		int parent_x = temp_parent.second;
		
		float y1 = offsetforY + parent_y * cellSize + cellSize / 2;
		float x1 = offsetforX + parent_x * cellSize + cellSize / 2;
		float y2 = offsetforY + curr_y * cellSize + cellSize / 2;
		float x2 = offsetforX + curr_x * cellSize + cellSize / 2;
		ofDrawLine(x1, y1, x2, y2);
	}
	
	ofSetColor(0, 0, 0);
	for (int i = 0; i < finalPath.size() - 1; i++) {
		pair<int, int> temp = finalPath[i];
		int curr_y = temp.first;
		int curr_x = temp.second;
		pair<int, int> temp_parent = finalPath[i + 1];
		int parent_y = temp_parent.first;
		int parent_x = temp_parent.second;
		
		float y1 = offsetforY + parent_y * cellSize + cellSize / 2;
		float x1 = offsetforX + parent_x * cellSize + cellSize / 2;
		float y2 = offsetforY + curr_y * cellSize + cellSize / 2;
		float x2 = offsetforX + curr_x * cellSize + cellSize / 2;
		ofDrawLine(x1, y1, x2, y2);
	}
	
	// 시작점 초록색
	ofSetColor(0, 255, 0);
	float drawY = offsetforY + 0 * cellSize + cellSize / 2;
	float drawX = offsetforX + 0 * cellSize + cellSize / 2;
	ofDrawCircle(drawX, drawY, 4);
	// 끝나는점 빨간색
	ofSetColor(255, 0, 0);
	drawY = offsetforY + (HEIGHT - 1) * cellSize + cellSize / 2;
	drawX = offsetforX + (WIDTH - 1) * cellSize + cellSize / 2;
	ofDrawCircle(drawX, drawY, 4);
}

void ofApp::buildGraph() {
	graph.clear();
	graph.resize(HEIGHT, vector<vector<pair<int,int>>>(WIDTH));
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			// 상
			if (cells[i][j].up_wall == 0 && i - 1 >= 0) {
				graph[i][j].push_back({i - 1, j});
			}
			// 하
			if (cells[i][j].down_wall == 0 && i + 1 < HEIGHT) {
				graph[i][j].push_back({i + 1, j});
				
			}
			// 좌
			if (cells[i][j].left_wall == 0 && j - 1 >= 0) {
				graph[i][j].push_back({i, j - 1});
			}
			// 우
			if (cells[i][j].right_wall == 0 && j + 1 < WIDTH) {
				graph[i][j].push_back({i, j + 1});
			}
			
		}
	}
	
}
