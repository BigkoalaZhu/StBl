#include "HausdorffImageSimplify.h"
#include "SColorMap.h"
#include <fstream>
#include <sstream>
extern SColorMap g_colorMap;

HausdorffImageSimplify::HausdorffImageSimplify()
{
	m_nodes.clear();	
	m_minNumLines = 8;
	m_oversegThreshold = 30;
	m_maxNumLines = 30;
	
}

void HausdorffImageSimplify::run(string filename,int maxclust,bool bmerge,int minNumLines)
{
	if(minNumLines!=-1)
		m_minNumLines = minNumLines;
	m_rowset.clear();
	m_nodes.clear();
	m_distmap.clear();

	m_imagename = filename;
	cout<<"processing image "<<m_imagename<<endl;
	loadImage(filename);
	extractSets();
	computeRCDists();
	linkage(maxclust);
	if(bmerge)
		mergeNodes();
	//cout<<"done"<<endl;
}


void HausdorffImageSimplify::extractSets()
{

	for(int i=0;i<m_grayImage.rows;i++)
	{
		vector<int> tmp;
		for(int j=1;j<m_grayImage.cols-1;j++)
		{
			if(m_grayImage.at<uchar>(i,j)==0)
				tmp.push_back(j);
		} 
		//if(tmp!=0&&tmp!=511)
			m_rowset.push_back(tmp);
	}

}

void HausdorffImageSimplify::computeRCDists()
{

	for(unsigned int i=1;i<m_rowset.size();i++)
	{
	/*	if(i==310)	
			cout<<"debug\n";*/
		if(m_rowset[i].size()!=0&&m_rowset[i-1].size()!=0)
		{
			float dist = m_hausdorffDist.computeDist(m_rowset[i],m_rowset[i-1]);
			m_distmap.insert(pair<int,float>(i,dist));	
		}
	}

}

void HausdorffImageSimplify::mergeTwoNodes(vector<HausdorffNode>  &nodes,int index)
{

	int up = index-1;
	while(up>=0&&nodes[up].merged)
		up--;
	if(up<0)
		up = 0;

	int upprev = up-1;
	while(upprev>=0&&nodes[upprev].merged)
		upprev--;
	if(upprev<0)
		upprev = 0;


	int below = index+1;
	while(below<nodes.size()&&nodes[below].merged)
		below++;
	int belownext = below+1;
	while(belownext<nodes.size()&&nodes[belownext].merged)
		belownext++;

	if(below>=nodes.size())
		below = nodes.size()-1;

	int newindex = index;

	if(nodes[index].uppersistence<nodes[index].downpersistence)
		newindex = up;
	else
		newindex = below;

	int start =  nodes[newindex].start<nodes[index].start?nodes[newindex].start:nodes[index].start;
	int end =    nodes[newindex].end>nodes[index].end? nodes[newindex].end:nodes[index].end;
	int newmid = (start+end)/2;	
	if(m_rowset[newmid].size()==0) 
	{ 
		int upmid = newmid--; 
		int bemid = newmid++; 			
		while(m_rowset[upmid].size()==0) 	
			upmid--; 			
		while(m_rowset[bemid].size()==0) 	
			bemid++; 		
		int updist = abs(upmid-newmid); 		
		int bedist = abs(bemid-newmid); 			
		if(updist>bedist) 			
			newmid = bemid; 		
		else 				
			newmid = upmid; 	
	}


	if(nodes[index].uppersistence<nodes[index].downpersistence)//和up node 合并
	{				

		int minindex =  newmid;	
		nodes[up].representiveIndex = minindex;

		//计算up的uppersistance和upprev的downpersistence
		if(up>0&&upprev>=0)
		{
			float lens = abs(newmid-nodes[upprev].representiveIndex+1);
			lens = sqrt(lens);
			nodes[up].uppersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[upprev].representiveIndex]);

			nodes[upprev].downpersistence = nodes[up].uppersistence;				
			nodes[upprev].minpersistence = nodes[upprev].uppersistence>nodes[upprev].downpersistence?nodes[upprev].downpersistence :nodes[upprev].uppersistence;

		}
		else
			nodes[up].uppersistence = FLT_MAX;
	
		//计算up的downpersistence和below的uppersistence
		if(below<nodes.size()-1)
		{
			float lens = abs(newmid-nodes[below].representiveIndex+1);
			lens = sqrt(lens);
			nodes[up].downpersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[below].representiveIndex]);
			nodes[below].uppersistence = nodes[up].downpersistence;
			nodes[below].minpersistence = nodes[below].uppersistence>nodes[below].downpersistence?nodes[below].downpersistence :nodes[below].uppersistence;			
		}
		else
			nodes[up].downpersistence = FLT_MAX;	

		nodes[up].minpersistence = nodes[up].uppersistence>nodes[up].downpersistence?nodes[up].downpersistence :nodes[up].uppersistence;
		nodes[up].end = nodes[index].end;		
		nodes[up].height = nodes[up].end-nodes[up].start+1;
		nodes[index].merged=true;
		//count++;
	}	
	else //和below nodes 合并
	{			
		nodes[index].merged=true;			
		nodes[below].representiveIndex = newmid;

		///计算up的downpersistence和below的uppersistence
		if(up>0)
		{
			float lens = abs(newmid-nodes[up].representiveIndex+1);
			lens = sqrt(lens);
			nodes[below].uppersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[up].representiveIndex]);	
			nodes[up].downpersistence = nodes[below].uppersistence;
			nodes[up].minpersistence = nodes[up].uppersistence>nodes[up].downpersistence?nodes[up].downpersistence :nodes[up].uppersistence;
		}
		else				
			nodes[below].uppersistence = FLT_MAX;				


		///计算below的downpersistence和belownext的uppersistence
		if(below<nodes.size()-1&&belownext<nodes.size())
		{
			float lens = abs(newmid-nodes[belownext].representiveIndex+1);
			lens = sqrt(lens);
			nodes[below].downpersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[belownext].representiveIndex]);
			nodes[belownext].uppersistence = nodes[below].downpersistence;
			nodes[belownext].minpersistence = nodes[belownext].uppersistence>nodes[belownext].downpersistence?nodes[belownext].downpersistence :nodes[belownext].uppersistence;
		}
		else
			nodes[below].downpersistence = FLT_MAX;
		nodes[below].minpersistence = nodes[below].uppersistence>nodes[below].downpersistence?nodes[below].downpersistence :nodes[below].uppersistence;
		nodes[below].start = nodes[index].start;
		nodes[below].height = nodes[below].end-nodes[below].start+1;
		//count++;
	}
}

void HausdorffImageSimplify::linkage(int maxclust)
{
	bool overseg = false;
	if(maxclust>=m_oversegThreshold)
		overseg = true;

	vector<HausdorffNode>  nodes = constructInitialNodes();
	int numdata = nodes.size();
	vector<bool> merged(numdata-1,false);;
	int count = 0;

	map<int,float>::iterator it = m_distmap.begin();
	int firstrow = (*it).first;

	map<int,float>::reverse_iterator rit = m_distmap.rbegin();
	int lastrow = (*rit).first;

	while(count<numdata-maxclust)
	{

		
		int index = findMinNode(nodes,overseg);
		if(index!=-1)
		{
			mergeTwoNodes(nodes,index);
			count++;
		}
	}
		/*int up = index-1;
		while(up>=0&&nodes[up].merged)
			up--;
		if(up<0)
			up = 0;

		int upprev = up-1;
		while(upprev>=0&&nodes[upprev].merged)
			upprev--;
		if(upprev<0)
			upprev = 0;


		int below = index+1;
		while(below<nodes.size()&&nodes[below].merged)
			below++;
		int belownext = below+1;
		while(belownext<nodes.size()&&nodes[belownext].merged)
			belownext++;

		if(below>=nodes.size())
			below = nodes.size()-1;

		int newindex = index;

		if(nodes[index].uppersistence<nodes[index].downpersistence)
			newindex = up;
		else
			newindex = below;
	
		int start =  nodes[newindex].start<nodes[index].start?nodes[newindex].start:nodes[index].start;
		int end =    nodes[newindex].end>nodes[index].end? nodes[newindex].end:nodes[index].end;
		int newmid = (start+end)/2;	*/
		/*if(nodes[index].uppersistence<nodes[index].downpersistence)
		{				
			int minindex =  newmid;	
			nodes[up].representiveIndex = minindex;
			if(nodes[up].start-1<firstrow)
				nodes[up].uppersistence = FLT_MAX;
			else
			{
					
				nodes[up].uppersistence = m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[up].start-1]);	
			}
			if(nodes[index].end+1>lastrow)
				nodes[up].downpersistence = FLT_MAX;
			else
				nodes[up].downpersistence = m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[index].end+1]);
			nodes[up].minpersistence = nodes[up].uppersistence>nodes[up].downpersistence?nodes[up].downpersistence :nodes[up].uppersistence;
			nodes[up].end = nodes[index].end;		
			nodes[up].height = nodes[up].end-nodes[up].start+1;
			nodes[index].merged=true;
			count++;
		}	
		else 
		{			
			nodes[index].merged=true;			
			nodes[below].representiveIndex = newmid;
			if(nodes[index].start-1<firstrow)
				nodes[below].uppersistence = FLT_MAX;
			else
				nodes[below].uppersistence = m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[index].start-1]);		
			if(nodes[below].end+1>lastrow)
				nodes[below].downpersistence = FLT_MAX;
			else
				nodes[below].downpersistence = m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[below].end+1]);
			nodes[below].minpersistence = nodes[below].uppersistence>nodes[below].downpersistence?nodes[below].downpersistence :nodes[below].uppersistence;
			nodes[below].start = nodes[index].start;
			nodes[below].height = nodes[below].end-nodes[below].start+1;
			count++;
		}	*/

	/*	if(up==0||upprev==0)
			cout<<"debugging\n";*/
		/*if(nodes[index].uppersistence<nodes[index].downpersistence)
		{				

			int minindex =  newmid;	
			nodes[up].representiveIndex = minindex;

			if(up>0&&upprev>=0)
			{
				float lens = abs(newmid-nodes[upprev].representiveIndex+1);
				lens = sqrt(lens);
				nodes[up].uppersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[upprev].representiveIndex]);

				nodes[upprev].downpersistence = nodes[up].uppersistence;				
				nodes[upprev].minpersistence = nodes[upprev].uppersistence>nodes[upprev].downpersistence?nodes[upprev].downpersistence :nodes[upprev].uppersistence;

			}
			else
				nodes[up].uppersistence = FLT_MAX;

			if(below<nodes.size()-1)
			{
				float lens = abs(newmid-nodes[below].representiveIndex+1);
				lens = sqrt(lens);
				nodes[up].downpersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[below].representiveIndex]);
				nodes[below].uppersistence = nodes[up].downpersistence;
				nodes[below].minpersistence = nodes[below].uppersistence>nodes[below].downpersistence?nodes[below].downpersistence :nodes[below].uppersistence;			
			}
			else
				nodes[up].downpersistence = FLT_MAX;	

			nodes[up].minpersistence = nodes[up].uppersistence>nodes[up].downpersistence?nodes[up].downpersistence :nodes[up].uppersistence;
			nodes[up].end = nodes[index].end;		
			nodes[up].height = nodes[up].end-nodes[up].start+1;
			nodes[index].merged=true;
			count++;
		}	
		else 
		{			
			nodes[index].merged=true;			
			nodes[below].representiveIndex = newmid;

			if(up>0)
			{
				float lens = abs(newmid-nodes[up].representiveIndex+1);
				lens = sqrt(lens);
				nodes[below].uppersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[up].representiveIndex]);	
				nodes[up].downpersistence = nodes[below].uppersistence;
				nodes[up].minpersistence = nodes[up].uppersistence>nodes[up].downpersistence?nodes[up].downpersistence :nodes[up].uppersistence;
			}
			else				
				nodes[below].uppersistence = FLT_MAX;				


			if(below<nodes.size()-1&&belownext<nodes.size())
			{
				float lens = abs(newmid-nodes[belownext].representiveIndex+1);
				lens = sqrt(lens);
				nodes[below].downpersistence = lens*m_hausdorffDist.computeDist(m_rowset[newmid],m_rowset[nodes[belownext].representiveIndex]);
				nodes[belownext].uppersistence = nodes[below].downpersistence;
				nodes[belownext].minpersistence = nodes[belownext].uppersistence>nodes[belownext].downpersistence?nodes[belownext].downpersistence :nodes[belownext].uppersistence;
			}
			else
				nodes[below].downpersistence = FLT_MAX;
			nodes[below].minpersistence = nodes[below].uppersistence>nodes[below].downpersistence?nodes[below].downpersistence :nodes[below].uppersistence;
			nodes[below].start = nodes[index].start;
			nodes[below].height = nodes[below].end-nodes[below].start+1;
			count++;
		}*/
	//}
	m_nodes.clear();
	for(unsigned int i=0;i<nodes.size();i++)
	{
		if(!nodes[i].merged)
		{
			nodes[i].line = m_rowset[nodes[i].representiveIndex];
			nodes[i].height = nodes[i].end-nodes[i].start+1;
			m_nodes.push_back(nodes[i]);
		}
	}

}





vector<HausdorffNode> HausdorffImageSimplify::constructInitialNodes()
{
	vector<HausdorffNode> nodes;	
	int count = 0;
	HausdorffNode tmp;
	for(map<int,float>::iterator it=m_distmap.begin();it!=m_distmap.end();it++)
	{		
		
		tmp.start = (*it).first;
		tmp.end=(*it).first;
		nodes.push_back(tmp);		
	}
	if(nodes.size()>0)	
		nodes[0].start = nodes[0].end-1;		
		

	if(nodes.size()>0)
	{	
		nodes[0].uppersistence = FLT_MAX;
		nodes[0].downpersistence = m_distmap[nodes[0].end+1];
	}
	if(nodes.size()>1)
	{
		
		for(unsigned int i=1;i<nodes.size()-1;i++)
		{			
			nodes[i].uppersistence =  m_distmap[nodes[i].start];;
			nodes[i].downpersistence = m_distmap[nodes[i].end+1];
		}		
		nodes[nodes.size()-1].uppersistence = m_distmap[nodes[nodes.size()-1].start];
		nodes[nodes.size()-1].downpersistence = FLT_MAX;	
	}

	for(unsigned int i=0;i<nodes.size();i++)
	{
		nodes[i].minpersistence = nodes[i].uppersistence>nodes[i].downpersistence?nodes[i].downpersistence :nodes[i].uppersistence;
		nodes[i].representiveIndex = nodes[i].start;
		nodes[i].height =  1;
	}

	return nodes;	
}


int HausdorffImageSimplify::findMinNode(vector<HausdorffNode> nodes,bool overseg)
{
	int index = -1;
	float minP =FLT_MAX;
	if(overseg)
	{	
		for(unsigned int i=0;i<nodes.size();i++)
		{
			if(nodes[i].minpersistence<minP&&!nodes[i].merged&&nodes[i].height<=m_maxNumLines)		
			{

				minP = nodes[i].minpersistence;
				index = i;
			}	
		}	
	}
	else
	{
		for(unsigned int i=0;i<nodes.size();i++)
		{
			if(nodes[i].minpersistence<minP&&!nodes[i].merged)		
			{

				minP = nodes[i].minpersistence;
				index = i;
			}	
		}	
	}
	return index;	
}


void HausdorffImageSimplify::mergeNodes()
{
	if(m_nodes.size()==0) return;

	vector<HausdorffNode> nodes = m_nodes;
	vector<int> sortIndex = sortNodes(nodes);
	for(unsigned int i=0;i<nodes.size();i++)
		nodes[i].merged =false;
	
	
	map<int,float>::iterator it = m_distmap.begin();
	int firstrow = (*it).first;

	map<int,float>::reverse_iterator rit = m_distmap.rbegin();
	int lastrow = (*rit).first;
	for(unsigned int i=0;i<nodes.size();i++)	
	{

		int ii= sortIndex[i];

		int len = nodes[ii].end - nodes[ii].start+1;
		if(len>m_minNumLines|| nodes[ii].merged ) continue;
		mergeTwoNodes(nodes,ii);

		
	}
	vector<HausdorffNode> fnodes;
	for(unsigned int i=0;i<nodes.size();i++)
	{		
		if(!nodes[i].merged)
		{
			nodes[i].line = m_rowset[nodes[i].representiveIndex];
			nodes[i].height = nodes[i].end-nodes[i].start+1;
			fnodes.push_back(nodes[i]);
		}
	}

	m_nodes= fnodes;
}


vector<int> HausdorffImageSimplify::sortNodes(vector<HausdorffNode>  nodes)
{

	vector<float> persistences;
	for(int i=0;i<nodes.size();i++)		
		persistences.push_back(nodes[i].minpersistence);
		
	return  sortWithIndex(persistences);
	
}

void HausdorffImageSimplify::saveSegmentImg(string name)
{

	cv::Mat rimg(m_grayImage.size(),CV_8UC3);	
	for(int j=0;j<rimg.cols;j++)
		for(int k=0;k<rimg.rows;k++)
			rimg.at<cv::Vec3b>(j,k) =cv::Vec3b(255,255,255);

	for(int i=0;i<m_nodes.size();i++)
	{
		int start = m_nodes[i].start;
		int end=m_nodes[i].end;
		for(int j=start;j<=end;j++)
			for(int k=0;k<rimg.rows;k++)
			{
				if(m_grayImage.at<UCHAR>(j,k)==0)
				{
					Color rgb = g_colorMap.GetClassColor(i%4);//GetBoxColor(i%2);//g_colorMap.GetClassColor(i%4);//GetBoxColor(i%2);//GetClassColor(i%4);//GetBoxColor(i%2)
					UCHAR r = rgb[2]*255;
					UCHAR g = rgb[1]*255;
					UCHAR b = rgb[0]*255;
					rimg.at<cv::Vec3b>(j,k) =cv::Vec3b(r,g,b);


				}	
			}
	}
	if(name.size()<2)
	{

	

	int pos = m_imagename.find_last_of("/");
	string dir = m_imagename.substr(0,pos);
	string name = m_imagename.substr(pos+1,m_imagename.size()-pos);
	char buf[255];
	itoa(m_nodes.size(),buf,10);
	string tmpname = dir;
	tmpname.append("/light/");
	if(!fileExists(tmpname))
		_mkdir(tmpname.c_str());
	tmpname.append(name);
	tmpname.append("-");
	tmpname.append(buf);
	tmpname.append("-l.bmp");
	cv::imwrite(tmpname.c_str(),rimg);
	}
	else 
		cv::imwrite(name.c_str(),rimg);

}


void HausdorffImageSimplify::saveNodeInfo()
{
	int pos = m_imagename.find_last_of("/");
	string dir = m_imagename.substr(0,pos);
	string name = m_imagename.substr(pos+1,m_imagename.size()-pos);
	string tmpname = dir;
	tmpname.append("/nodes/");
	if(!fileExists(tmpname))
		_mkdir(tmpname.c_str());
	tmpname.append(name.substr(0,name.size()-4));
	tmpname.append(".txt");

	ofstream nodefile(tmpname.c_str());		
	for(int i=0;i<m_nodes.size();i++)
	{
		nodefile<<m_nodes[i].start<<'\t'<<m_nodes[i].end<<'\t'<<m_nodes[i].height<<'\t'<<m_nodes[i].line.size()<<endl;
		for(int j=0;j<m_nodes[i].line.size();j++)
			nodefile<<m_nodes[i].line[j]<<' ';
		nodefile<<endl;		
	}
	nodefile.close();
}


vector<HausdorffNode> HausdorffImageSimplify::loadNodeInfo(string nodefile)
{
	ifstream nodein(nodefile.c_str());
	
	vector<HausdorffNode>  nodes;
	string nodeinfo;

	
	while(!nodein.eof()){

		//read data from file
		int start,end,height,numPtInline;
		std::getline(nodein,nodeinfo);
		if(nodeinfo.size()<1)
			break;
		std::istringstream(nodeinfo,ios_base::in) >> start >> end >> height>>numPtInline;
		std::getline(nodein,nodeinfo);
		//sscanf (nodeinfo," %d %d %d %d ", &start, &end, &height,&numPtInline);
		
		vector<int> pts(numPtInline,-1);
		stringstream ss(nodeinfo);
		size_t j=0; // the current word index
		while (j<numPtInline) {
			ss >> pts[j]; // here i is as above: the current line index
			++j;
		}

		
		nodes.push_back(HausdorffNode(start,end,height,pts));

	}
	
	
	nodein.close();
	return nodes;	
}