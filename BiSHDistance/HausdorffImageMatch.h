#pragma  once
#include "HausdorffImageProcessor.h"

class HausdorffImageMatch:public HausdorffImageProcessor
{
private:	
public:
	HausdorffImageMatch(void){};
	~HausdorffImageMatch(void){};
public:
	vector<pair<int,int>> run(vector<HausdorffNode> srcNodes, vector<HausdorffNode> dstNodes,float &sumcost,vector<float>&regionDists=vector<float>());
private:
	vector<pair<int,int>> findMincost(vector<vector<float>> cost, int m, int n,float &mcost);
};

