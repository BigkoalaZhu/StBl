#pragma  once
#include "HausdorffImageProcessor.h"
#include "HausdorffNode.h"
class HausdorffImageSimplify:public HausdorffImageProcessor
{
private:		
	vector<HausdorffNode> m_nodes;
	vector<Eigen::VectorXd> m_rowset;
	map<int,float> m_distmap;
	int m_minNumLines;
	int m_maxNumLines;
	int m_oversegThreshold;
	string m_imagename;	
	
public:
	HausdorffImageSimplify();
	~HausdorffImageSimplify(void){};
public:
	void run(string filename,int,bool merge=true,int minNumLines=-1);
public:
	vector<Eigen::VectorXd>  getRowset(){ return m_rowset; };
	vector<HausdorffNode> getNodes(){return m_nodes;}
	void saveSegmentImg(string name=string());
	void saveNodeInfo();
	void saveNodeInfoSpecific(string filename);

	vector<HausdorffNode> loadNodeInfo(string nodefile);
private:
	void linkage(int maxclus);
	void extractSets();
	vector<HausdorffNode> constructInitialNodes();
	void computeRCDists();
private:
	int findMinNode(vector<HausdorffNode> nodes,bool overseg);
	void mergeNodes();
	vector<int> sortNodes(vector<HausdorffNode>  nodes);
	void mergeTwoNodes(vector<HausdorffNode>  &nodes,int index);
};