//
//  water_line.h
//  WaterFall
//
//  Created by 방지혁 on 10/27/25.
//

#ifndef water_line_h
#define water_line_h

#include "ofMain.h"

#define EPSILON 0.01

struct LineSegment{
	int x1, x2, y1, y2;
	double slope, x_coef, y_coef, constant;
};

struct Dot{
	int x1, y1;
};

class WaterLine{
	
public:
	WaterLine(int num_of_line);
	~WaterLine();
	
	void reset();
	void update();
	void draw();
	void calculate_path(LineSegment *lineseg, int num_of_line);
	
	Dot* path = NULL;
	
	float uniqueColor_r, uniqueColor_g, uniqueColor_b;
	float scale;
	int hexcolor;
	
	int path_idx;
	
	/* 그리기가 완료되었는지 체크하는 flag */
	int draw_complete;
	/* 경로계산이 완료되었는지 체크하는 flag, draw() 함수에서 이게 1이여야 시작 */
	int calc_complete;
	
	ofPoint start_dot;
	float dot_radius;
	
};

#endif /* water_line_h */

