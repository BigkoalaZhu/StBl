#pragma once

typedef struct HausdorffNode
{
public:
	int start;
	int end;
	float uppersistence;
	float downpersistence;
	float minpersistence;
	bool merged;
	int representiveIndex;
	vector<int> line;
	int height;
public:
	HausdorffNode(void)
	{
		height = 0;
		merged = false;
		representiveIndex = -1;
	}
	HausdorffNode(int s,int e,int h,vector<int>data)
	{
		start = s;
		end = e;
		height = h;
		line = data;
	}

}HausdorffNode;


typedef struct Scanline{
	vector<pair<bool, int>> shapes;
	vector<pair<bool, int>> holes;
}Scanline;

typedef struct Segment{
	int start;
	int end;
	bool continous;
	float scalefactor;
	Segment(int s,int e, bool c)
	{
		start = s;
		end = e;
		continous = c;
	}
}Segment;