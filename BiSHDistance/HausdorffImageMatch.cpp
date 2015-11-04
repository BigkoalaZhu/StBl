#include "HausdorffImageMatch.h"
vector<pair<int,int>> HausdorffImageMatch::run(vector<HausdorffNode> srcNodes, vector<HausdorffNode> dstNodes,float &sumcost,vector<float>&regionDists)
{
	int num1 = srcNodes.size();
	int num2 = dstNodes.size();
	vector<vector<float>> distmatrix(num1);
	for(int jj=0;jj<num1;jj++)
		distmatrix[jj].resize(num2,0);

	for(int jj=0;jj<num1;jj++)
	{
        #pragma omp parallel for
		for(int kk=0;kk<num2;kk++)
		{			
		/*	if(kk==2&&jj==0)
				cout<<"debug\n";*/
			float dist = m_hausdorffDist.computeDist(srcNodes[jj].line,dstNodes[kk].line, 5);	
			float scalefactor = srcNodes[jj].height/(float)dstNodes[kk].height;
			//scalefactor=1/scalefactor;
			//if(scalefactor>=1)
			//	scalefactor = pow(scalefactor,2);
			//else
			//	scalefactor = pow(1.0/scalefactor,2);
	/*		if(scalefactor>=1)
				scalefactor = pow(scalefactor,0.5f);
			else
				scalefactor = pow(1.0f/scalefactor,0.5f);*/
			distmatrix[jj][kk] = dist*srcNodes[jj].height*scalefactor;//*(srcNodes[jj].end - srcNodes[jj].start+1);
		}
	
	}

	//float mcost = -1;
	vector<pair<int,int>>  pairs = findMincost(distmatrix, num1-1, num2-1, sumcost);
	for(int i=0;i<pairs.size();i++)
	{
		int m = pairs[i].first;
		int n = pairs[i].second;
		regionDists.push_back(distmatrix[m][n]);
	}
	//sumcost = mcost;
	return pairs;
	//cout<<"matching cost is "<<mcost<<endl;
	vector<pair<int,float>> costTestToLabel(srcNodes.size());

	for(unsigned int ii=0;ii<costTestToLabel.size();ii++)
	{
		costTestToLabel[ii].first = 0;
		costTestToLabel[ii].second = 0;
	}
	for(unsigned int ii=0;ii<pairs.size();ii++)
	{
		int src = pairs[ii].first;//test
		int dst = pairs[ii].second;//label
	
		costTestToLabel[src].second += distmatrix[src][dst]*dstNodes[dst].height;
		costTestToLabel[src].first += dstNodes[dst].height;			
	}
	
	sumcost  = 0;
	for(unsigned int ii=0;ii<costTestToLabel.size();ii++)
	{
		pair<int,float> ipair = costTestToLabel[ii];
		if(ipair.first!=0)
		{
			//cout<<sumcost<<endl;
			sumcost += (ipair.second/ipair.first)*srcNodes[ii].height;
		}
	}
	return pairs;
	
}



vector<pair<int,int>> HausdorffImageMatch::findMincost(vector<vector<float>> cost, int m, int n,float &mcost)
{

	int R = cost.size();
	int C = cost[0].size();
	// Instead of following line, we can use int tc[m+1][n+1] or 
	// dynamically allocate memoery to save space. The following line is
	// used to keep te program simple and make it working on all compilers.
	vector<vector<float>> tc(R); 
	vector<vector<pair<int,int>>> path(R);
	//vector<vector<int>> pathy(R);
	for(int i=0;i<R;i++)
	{
		path[i].resize(C);
		//pathy[i].resize(C);
		tc[i].resize(C);
	}

	tc[0][0] = cost[0][0];

	/* Initialize first column of total cost(tc) array */
	for (int i = 1; i <= m; i++)
	{
		tc[i][0] = tc[i-1][0] + cost[i][0];
		path[i][0] = pair<int,int>(i-1,0);
	}

	/* Initialize first row of tc array */
	for (int j = 1; j <= n; j++)
	{
		tc[0][j] = tc[0][j-1] + cost[0][j];
		path[0][j] = pair<int,int>(0,j-1);
	}

	/* Construct rest of the tc array */
	for (int i = 1; i <= m; i++)
		for (int j = 1; j <= n; j++)	
		{
			//float tmpmin =  min3(tc[i-1][j-1], tc[i-1][j],tc[i][j-1]);
			float tmpmin =  min(tc[i-1][j-1], tc[i-1][j]);

			if(tmpmin==tc[i-1][j])
			{
				if(path[i-1][j]==pair<int,int>(i-1,j-1))								
				{
					path[i][j]=pair<int,int>(i-1,j-1);									
					tmpmin = tc[i-1][j-1];
				}
				else
					path[i][j]=pair<int,int>(i-1,j);

			}
		/*	else if(tmpmin==tc[i][j-1])
			{
				if(path[i][j-1]==pair<int,int>(i-1,j-1))
				{
					path[i][j]=pair<int,int>(i-1,j-1);
					tmpmin = tc[i-1][j-1];				
				}
				else
					path[i][j]=pair<int,int>(i,j-1);			
			}*/
			else if(tmpmin==tc[i-1][j-1])			
				path[i][j]=pair<int,int>(i-1,j-1);		

			tc[i][j] = tmpmin + cost[i][j];

		}

		mcost = tc[m][n];

		vector<pair<int,int>> spath;
		
		pair<int,int> tmp(m,n);
		bool finish =false;
		while (!finish)
		{
			
			spath.push_back(tmp);
			m=tmp.first;
			n=tmp.second;
			tmp = path[m][n];

			if(tmp.first ==0 &&tmp.second==0)
			{
				finish=true;
				spath.push_back(pair<int,int>(0,0));
				
			}
		}

		return spath;
}


	


//cv::Mat HausdorffLabelTransfer::run(string srcImgname,string dstImgname,
//									vector<HausdorffNode> srcRegions,vector<HausdorffNode> dstRegions,
//									vector<pair<int,int>> pairs)
//{
//
//	int debug = 1;
//	m_srcSimplifier->loadImage(srcImgname);
//	m_srcSimplifier->extractSets();
//
//	m_dstSimplifier->loadImage(dstImgname);
//	m_dstSimplifier->extractSets();
//
//	vector<vector<int>> srcRowset = m_srcSimplifier->getRowset();
//	vector<vector<int>> dstRowset = m_dstSimplifier->getRowset();
//
//
//
//
//
//	if(debug==1)
//	{
//
//		//cout<<"cost is"<<cost<<endl;
//		printf("cos is %6f\n",cost);
//		for(int ii=pairs.size()-1;ii>=0;ii--)
//		{
//			printf("%d --> %d\n",pairs[ii].first,pairs[ii].second);
//			//matchLines(testImgRegons[pairs[ii].first],labeledImgRegions[pairs[ii].second]);
//		}
//	}
//
//
//	vector<vector<int>> dstToSrc(dstRegions.size());
//	
//	for(unsigned int i=0;i<pairs.size();i++)
//	{
//		int src = pairs[i].first;
//		int dst = pairs[i].second;		
//		dstToSrc[dst].push_back(src);
//	
//	}
//
//	vector<bool> pairProcessed(pairs.size(),false);
//
//	for(unsigned int i=0;i<pairs.size();i++)
//	{
//		if(pairProcessed[i])continue;
//		int src = pairs[i].first;
//		int dst = pairs[i].second;				
//
//		float sumHeight = 0;
//		int startRegion = INT_MAX;
//		for(unsigned int j=0;j<dstToSrc[dst].size();j++)
//		{
//			int jsrc = dstToSrc[dst][j];
//			sumHeight += srcRegions[jsrc].height;
//			int index = searchInPairs(pairs,pair<int,int>(jsrc,dst));
//			if(index!=-1)
//				pairProcessed[index] = true;	
//			if(jsrc<startRegion)
//				startRegion = jsrc;
//		}
//		float ratio = dstRegions[dst].height/sumHeight;
//		for(unsigned int j=0;j<sumHeight;j++)
//		{
//			int srcRow = srcRegions[startRegion].start + j;
//			int dstRow = (int)(ratio*j) + dstRegions[dst].start;
//			map<int,int>  matchedPoints;
//
//			map<int,float>  sf = m_hausdorffDist.computeShift2(srcRowset[srcRow],dstRowset[dstRow],matchedPoints);
//			for(map<int,int>::iterator it=matchedPoints.begin();it!=matchedPoints.end();it++)
//			{
//				int xSrc = srcRow;
//				int ySrc = (*it).first;//srcRowset[srcRow][k];
//				int xDst = dstRow;
//				int yDst = (*it).second;//matchedPoints[k];
//				cv::Vec3b intensity = dstImage.at<cv::Vec3b>(xDst,yDst);
//				srcImage.at<cv::Vec3b>(xSrc,ySrc) = dstImage.at<cv::Vec3b>(xDst,yDst);
//			}			
//		}	
//
//	}
//	return srcImage;
//
//}
