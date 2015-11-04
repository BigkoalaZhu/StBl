#pragma once
#include <vector>
#include <time.h>
#include "utility.h"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/ts/ts.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

using namespace  std;
#include "HausdorffNode.h"

#include <Eigen\core>

class HausdorffDist
{
public:
	HausdorffDist(void){};
	~HausdorffDist(void){};
public:
	int computeDist(vector<int> A, vector<int> B);
	void computeShift(vector<int> A, vector<int> B,vector<int> &matchedPoints);
	
	//void computeShift2(vector<int> A, vector<int> B,vector<int> &matchedPoints);
	map<int,float> computeShift2(vector<int> A, vector<int> B,map<int,int> &matchedPoints);

	double computeDist(Eigen::VectorXd A, Eigen::VectorXd B, double lameda = 0.05);
private:
	int compute_dist(vector<int> A, vector<int> B);
	int compute_dist_fast(vector<int> A, vector<pair<bool,int>> BEnd);
	int compute_hole_dist_fast(vector<int> A, vector<pair<bool,int>>   Bend);
	int compute_dist_fast_more(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd);
	int compute_dist_fast_more_more(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd);
	
	int compute_dist(vector<int> A, vector<int> B,vector<int> &matchedPoints);
	vector<int> detecHoles(vector<int> postion,vector<pair<bool,int>> &endInShape, vector<pair<bool,int>>  &endInHole);
	void detectEnds(vector<int> postion,vector<pair<bool,int>> &endInShape, vector<pair<bool,int>> &endInHole);
	void detectShapeEnds(vector<int> postion,vector<pair<bool,int>> &endInShape);
	int pointInSegment(vector<pair<bool,int>> AEnd,int mid);
	int compute_dist_fast_and_match(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd);
	vector<Segment> updateEnd(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd);
	vector<pair<Segment,Segment>> matchPair(vector<Segment>  A, vector<Segment>  B);
	vector<Segment> unionPair(vector<Segment>  A, vector<Segment>  B);
	bool checkOverlap(Segment a, Segment b);	
	int computeShift(vector<int> A, vector<pair<bool,int>> BEnd);
};



