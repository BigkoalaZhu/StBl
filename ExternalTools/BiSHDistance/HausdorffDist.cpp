#include "HausdorffDist.h"
#include "HausdorffNode.h"
#include <time.h>

vector<int> HausdorffDist::detecHoles(vector<int> postion,vector<pair<bool,int>> &endInShape, vector<pair<bool,int>> &endInHole)
{
	if(postion.size()==0)return vector<int>();

	if(postion.size()==1)
		endInShape.push_back(pair<bool,int>(false,postion[0]));

	vector<int> holes;

	endInHole.push_back(pair<bool,int>(false,postion[0]-1));
	holes.push_back(postion[0]-1);


	int segmentEnd = postion[0];




	bool hasstart = false;
	for(unsigned int i=1;i<postion.size();i++)
	{	

		if(postion[i]==postion[i-1]+1)	
		{
			segmentEnd = postion[i];
			if(!hasstart)
			{
				endInShape.push_back(pair<bool,int>(true,postion[i-1]));
				hasstart =	true;
			}
		}
		else
		{
			endInShape.push_back(pair<bool,int>(hasstart,segmentEnd));

			hasstart = false;
			if(segmentEnd+1==postion[i]-1)
				endInHole.push_back(pair<bool,int>(false,postion[i]-1));
			else
			{
				endInHole.push_back(pair<bool,int>(true,segmentEnd+1));
				endInHole.push_back(pair<bool,int>(true,postion[i]-1));
			}

			for(int j=segmentEnd+1;j<postion[i];j++)
				holes.push_back(j);	
			segmentEnd = postion[i];

		}		
	}
	if(postion.size()>1)	
		endInShape.push_back(pair<bool,int>(hasstart,postion[postion.size()-1]));


	endInHole.push_back(pair<bool,int>(false,postion[postion.size()-1]+1));
	holes.push_back(postion[postion.size()-1]+1);


	return holes;

}

void HausdorffDist::detectEnds(vector<int> postion,vector<pair<bool,int>> &endInShape, vector<pair<bool,int>> &endInHole)
{

	if(postion.size()==1)
	{
		endInShape.push_back(pair<bool,int>(false,postion[0]));
		endInHole.push_back(pair<bool,int>(false,postion[postion.size()-1]+1));
		return;
	}

	
	endInHole.push_back(pair<bool,int>(false,postion[0]-1));
	

	int segmentEnd = postion[0];


	bool hasstart = false;
	for(unsigned int i=1;i<postion.size();i++)
	{	

		if(postion[i]==postion[i-1]+1)	
		{
			segmentEnd = postion[i];
			if(!hasstart)
			{
				endInShape.push_back(pair<bool,int>(true,postion[i-1]));
				hasstart =	true;
			}
		}
		else
		{
			endInShape.push_back(pair<bool,int>(hasstart,segmentEnd));

			hasstart = false;
			if(segmentEnd+1==postion[i]-1)
				endInHole.push_back(pair<bool,int>(false,postion[i]-1));
			else
			{
				endInHole.push_back(pair<bool,int>(true,segmentEnd+1));
				endInHole.push_back(pair<bool,int>(true,postion[i]-1));
			}			
			segmentEnd = postion[i];

		}		
	}
	//if(postion.size()>1)	
	endInShape.push_back(pair<bool,int>(hasstart,postion[postion.size()-1]));

	endInHole.push_back(pair<bool,int>(false,postion[postion.size()-1]+1));

	return ;

}

void HausdorffDist::detectShapeEnds(vector<int> postion,vector<pair<bool,int>> &endInShape)
{

	if(postion.size()==1)
	{
		endInShape.push_back(pair<bool,int>(false,postion[0]));
		
		return;
	}

	int segmentEnd = postion[0];


	bool hasstart = false;
	for(unsigned int i=1;i<postion.size();i++)
	{	

		if(postion[i]==postion[i-1]+1)	
		{
			segmentEnd = postion[i];
			if(!hasstart)
			{
				endInShape.push_back(pair<bool,int>(true,postion[i-1]));
				hasstart =	true;
			}
		}
		else
		{
			endInShape.push_back(pair<bool,int>(hasstart,segmentEnd));

			hasstart = false;
				
			segmentEnd = postion[i];

		}		
	}
	
	endInShape.push_back(pair<bool,int>(hasstart,postion[postion.size()-1]));	

	return ;

}

int HausdorffDist::computeDist(vector<int> A, vector<int> B)
{
	//if(A.size()==0 ||B.size()==0)	
	//	return INT_MAX;

	vector<pair<bool,int>>  endInA,endInB;
	vector<pair<bool,int>> endInAHole, endInBHole;
	vector<int> holesInA = detecHoles(A,endInA,endInAHole);
	vector<int> holesInB = detecHoles(B,endInB,endInBHole);
	
	//int dist1 = compute_dist(A,B);
	//int dist2 = compute_dist(B,A);
	//
	//int disthole1,disthole2;
	////if(holesInA.size()>2||holesInB.size()>2)
	///*{	
	//	disthole1 = compute_dist(holesInA,holesInB);
	//	disthole2 = compute_dist(holesInB,holesInA);
	//	dist1 = dist1>disthole1?dist1:disthole1;
	//	dist2 = dist2>disthole2?dist2:disthole2;
	//}	*/
	//
	//int finaldist = dist1>dist2?dist1:dist2;
	//return finaldist;
	//
	//int fdist1 = compute_dist_fast(A,endInB);
	//int fdist2 = compute_dist_fast(B,endInA);
	//int fdisthole1 = compute_hole_dist_fast(holesInA,endInBHole);
	//int fdisthole2 = compute_hole_dist_fast(holesInB,endInAHole);
	//int ffdist1 = fdist1>fdisthole1?fdist1:fdisthole1;
	//int ffdist2 = fdist2>fdisthole2?fdist2:fdisthole2;
	//
	//int ffinaldist = ffdist1>ffdist2?ffdist1:ffdist2;
	//return ffinaldist;

	


	//int fmdist1 = compute_dist_fast_more(endInA,endInB);
	//int fmdist2 = compute_dist_fast_more(endInB,endInA);
	//int fmdisthole1 = compute_dist_fast_more(endInAHole,endInBHole);
	//int fmdisthole2 = compute_dist_fast_more(endInBHole,endInAHole);

	//int ffmdist1 = fmdist1>fmdisthole1?fmdist1:fmdisthole1;
	//int ffmdist2 = fmdist2>fmdisthole2?fmdist2:fmdisthole2;

	//int maxdist = ffmdist1>ffmdist2?ffmdist1:ffmdist2;

	//if(maxdist!=ffinaldist)
	//	cout<<"bug\n";
	//return maxdist;
	/*detectEnds(A,endInA,endInAHole);
	detectEnds(B,endInB,endInBHole);*/


	//compute_dist_fast_and_match(endInA,endInB);
	int fmmdist1 = compute_dist_fast_more_more(endInA,endInB);
	int fmmdist2 = compute_dist_fast_more_more(endInB,endInA);
	int fmmdisthole1 = compute_dist_fast_more_more(endInAHole,endInBHole);
	int fmmdisthole2 = compute_dist_fast_more_more(endInBHole,endInAHole);

	int ffmmdist1 = fmmdist1>fmmdisthole1?fmmdist1:fmmdisthole1;
	int ffmmdist2 = fmmdist2>fmmdisthole2?fmmdist2:fmmdisthole2;

	int mmaxdist = ffmmdist1>ffmmdist2?ffmmdist1:ffmmdist2;

	//if(ffinaldist!=mmaxdist)
	//	cout<<"bug"<<endl;
	return mmaxdist;




	//return maxdist;
}

//float HausdorffDist::computeDist(vector<int> A, vector<int> B,vector<int> &matchedPoints)
//{
//
//
//	vector<pair<bool,int>>  endInA,endInB;
//	vector<pair<bool,int>> endInAHole, endInBHole;
//	//detecEnds(A,endInA,endInAHole);
//	detecEnds(B,endInB,endInBHole);
//	//
//
//	float dist1 = compute_dist(A,endInB,matchedPoints);
//	//float dist2 = compute_dist_fast_more_more(endInB,endInA);	
//	//float finaldist = dist1>dist2?dist1:dist2;
//	return dist1;
//}

int HausdorffDist::compute_dist(vector<int> A, vector<int> B)
{
	
	int m = A.size(); 
	int n = B.size(); 
	vector<int> diffs;
	for(int i=0;i<m;i++)
	{
		int diff = abs((A[i]-B[0]));
		for(int j=1;j<n;j++)
		{
			int tmp = abs((A[i]-B[j]));
			if(tmp<diff)
				diff=tmp;
			if(diff==0)
				break;
		}
		diffs.push_back(diff);
	}
	if(diffs.size()==0) return 0;
	int maxdiff = diffs[0];
	for(int i=1;i<m;i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}


int HausdorffDist::compute_dist_fast(vector<int> A, vector<pair<bool,int>> Bend)
{
	int m = A.size(); 
	int n = Bend.size(); 
	vector<int> diffs;
	
	
	for(int i=0;i<m;i++)
	{
		int diff = INT_MAX;
		//float diff = fabs((float)(A[i]-B[0]));
		int j=0;
		while (j<n)
		{
			int tmp = INT_MAX;
			if(Bend[j].first)
			{
				int start = Bend[j].second;
				int end = Bend[j+1].second;
				int diffs = (A[i] - start);
				int diffe = (A[i] - end);					
				if(diffs>=0&&diffe<=0)
					tmp = 0;
				else
				{
					int absdiffs = abs(diffs);
					int absdiffe = abs(diffe);
					tmp = absdiffs<absdiffe?absdiffs:absdiffe;
				}					
				j = j + 2;
			}
			else				
				tmp=abs(A[i]-Bend[j++].second);

			if(tmp<diff)
				diff=tmp;				
			if(diff==0)
				break;
		}
		/*for(int j=0;j<n;j+=2)
		{
			int start = Bend[j];
			int end = Bend[j+1];
			int diffs = (A[i] - start);
			int diffe = (A[i] - end);
			int tmp;
			if(diffs>=0&&diffe<=0)
				tmp = 0;
			else
			{
				int absdiffs = abs(diffs);
				int absdiffe = abs(diffe);
				tmp = absdiffs<absdiffe?absdiffs:absdiffe;
			}
		
			if(tmp<diff)
				diff=tmp;
			if(diff==0)
				break;
		}*/
		diffs.push_back(diff);
	}
	


	if(diffs.size()==0) return 0;
	int maxdiff = diffs[0];
	for(int i=1;i<m;i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}

int HausdorffDist::pointInSegment(vector<pair<bool,int>> AEnd,int mid)
{
	int n = AEnd.size();
	int j=0;
	while (j<n)
	{
		if(!AEnd[j].first)
		{
			if(mid==AEnd[j].second)
				return j;
			else
				j++;
		}
		else
		{
			int start = AEnd[j].second;
			int end = AEnd[j+1].second;
			if(mid>=start&&mid<=end)
				return j;
			else
				j+=2;
		}
	}
	return -1;
}
int HausdorffDist::compute_dist_fast_more(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd)
{
	int m = AEnd.size(); 
	int n = BEnd.size(); 
	vector<int> diffs;


	for(int i=0;i<m;i++)
	{
		int diff = INT_MAX;
		//float diff = fabs((float)(A[i]-B[0]));
		int j=0;
		while (j<n)
		{
			int tmp = INT_MAX;
			if(BEnd[j].first&&j+1<n)
			{
				int start = BEnd[j].second;
				int end = BEnd[j+1].second;
				int diffs = (AEnd[i].second - start);
				int diffe = (AEnd[i].second - end);					
				if(diffs>=0&&diffe<=0)
					tmp = 0;
				else
				{
					int absdiffs = abs(diffs);
					int absdiffe = abs(diffe);
					tmp = absdiffs<absdiffe?absdiffs:absdiffe;
				}
			
				j = j + 2;
			}
			else				
				tmp=abs(AEnd[i].second-BEnd[j++].second);

			if(tmp<diff)
				diff=tmp;				
			if(diff==0)
				break;
		}	
		diffs.push_back(diff);
	}

	int j=0;
	
	
	while (j<n-1)
	{
	
		if(BEnd[j].first)
		{
			int end = BEnd[j+1].second;
			if(j+2<n)
			{			
				int nextend = BEnd[j+2].second;
				int mid = (nextend+end)/2;
				int tmp = (nextend-end)/2;
				int midInA = pointInSegment(AEnd,mid);
				if(midInA!=-1)
					diffs.push_back(tmp);									
			}
			j+=2;			
		}
		else
		{
			int end = BEnd[j].second;
			int nextend = BEnd[j+1].second;
			int mid = (nextend+end)/2;
			int tmp = (nextend-end)/2;
			int midInA = pointInSegment(AEnd,mid);
			if(midInA!=-1)
				diffs.push_back(tmp);	
			j++;
		}
	}

	
	int maxdiff = diffs[0];
	for(int i=1;i<diffs.size();i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}


int HausdorffDist::compute_dist_fast_more_more(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd)
{
	int m = AEnd.size(); 
	int n = BEnd.size(); 
	vector<int> diffs;


	for(int i=0;i<m;i++)
	{
		int diff = INT_MAX;
	
		int j=0;
		
		while (j<n)
		{
			int tmp = INT_MAX;
			
			if(BEnd[j].first&&j+1<n)
			{

				int start = BEnd[j++].second;
				
				int diffTos = (AEnd[i].second - start);		

				if(diffTos<=0)		
					tmp= diffTos;			
				else
				{	
					int end = BEnd[j++].second;
					int diffToe = (AEnd[i].second - end);					
					if(diffToe<=0)
						tmp = 0;			
					else				
						tmp = diffToe;				
					
				}
			}
			else				
				tmp= AEnd[i].second-BEnd[j++].second;

			int abstmp = abs(tmp);
			if(abstmp<diff)
				diff=abstmp;				
			if(tmp<=0)
				break;
		}	
		diffs.push_back(diff);
	}

	int j=0;


	while (j<n-1)
	{

		if(BEnd[j].first)
		{
			int end = BEnd[j+1].second;
			if(j+2<n)
			{			
				int nextend = BEnd[j+2].second;
				int mid = (nextend+end)/2;
				
				int midInA = pointInSegment(AEnd,mid);
				if(midInA!=-1)
					diffs.push_back(mid-end);									
			}
			j+=2;			
		}
		else
		{
			int end = BEnd[j].second;
			int nextend = BEnd[j+1].second;
			int mid = (nextend+end)/2;			
			int midInA = pointInSegment(AEnd,mid);
			if(midInA!=-1)
				diffs.push_back(mid-end);	
			j++;
		}
	}


	int maxdiff = diffs[0];
	for(int i=1;i<diffs.size();i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}



int HausdorffDist::compute_hole_dist_fast(vector<int> A, vector<pair<bool,int>>  Bend)
{
	int m = A.size(); 
	
	vector<int> diffs;

	

	int n = Bend.size(); 
	
	if(n>1)
	{
		for(int i=0;i<m;i++)
		{
			/*if(i==35)
				cout<<"debug"<<endl;*/

			int diff = abs((A[i]-Bend[0].second));			
			int j=1;
			while(j<n)
			{
				int tmp = diff;
				if(!Bend[j].first)
					tmp = abs(A[i] - Bend[j++].second);				
				else
				{
					int start = Bend[j].second;
					int end = Bend[j+1].second;
					int diffs = (A[i] - start);
					int diffe = (A[i] - end);
					
					if(diffs>=0&&diffe<=0)
						tmp = 0;
					else
					{
						int absdiffs = abs(diffs);
						int absdiffe = abs(diffe);
						tmp = absdiffs<absdiffe?absdiffs:absdiffe;
					}
					j+=2;
				}
				if(tmp<diff)
					diff=tmp;

				if(diff==0)
					break;
			}
			
			diffs.push_back(diff);
		}
	}
	

	if(diffs.size()==0) return 0;
	int maxdiff = diffs[0];
	for(int i=1;i<m;i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}


int HausdorffDist::compute_dist(vector<int> A, vector<int> B,vector<int> &matchedPoints)
{

	int m = A.size(); 
	int n = B.size(); 
	vector<int> diffs;
	for(int i=0;i<m;i++)
	{
		int diff = abs((A[i]-B[0]));
		int minj=B[0];
		for(int j=1;j<n;j++)
		{
			int tmp = abs((A[i]-B[j]));
			if(tmp<diff)
			{
				diff=tmp;
				minj = B[j];
			}	
			if(diff==0)
				break;
		}		
		
		diffs.push_back(diff);
		matchedPoints.push_back(minj);
	}
	if(diffs.size()==0) return 0;
	int maxdiff = diffs[0];
	for(int i=1;i<m;i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}


int HausdorffDist::computeShift(vector<int> A, vector<pair<bool,int>> Bend)
{

	int m = A.size(); 
	int n = Bend.size(); 
	vector<int> diffs;


	for(int i=0;i<m;i++)
	{
		int diff = INT_MAX;		
		int minj=diff;
		int j=0;
		while (j<n)
		{
			int tmp = INT_MAX;
			int shift = tmp;
			if(Bend[j].first)
			{
				int start = Bend[j].second;
				int end = Bend[j+1].second;
				int diffs = (A[i] - start);
				int diffe = (A[i] - end);					
				if(diffs>=0&&diffe<=0)
				{
					shift = A[i];
					tmp = 0;
				}
				else
				{
					int absdiffs = abs(diffs);
					int absdiffe = abs(diffe);
					tmp = absdiffs<absdiffe?absdiffs:absdiffe;
					if(tmp==absdiffs)
						shift = start;
					else
						shift = end;
				}					
				j = j + 2;
			}
			else
			{
				tmp=abs(A[i]-Bend[j].second);
				shift = Bend[j].second;
				j++;
			}

			if(tmp<diff)
			{
				diff=tmp;				
				minj=shift;
			}
			if(diff==0)
				break;
		}
		
		diffs.push_back(diff);
	}



	if(diffs.size()==0) return 0;
	int maxdiff = diffs[0];
	for(int i=1;i<m;i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}



void HausdorffDist::computeShift(vector<int> A,vector<int> B,vector<int> &matchedPoints)
{
	vector<pair<bool,int>> Bend;
	vector<pair<bool,int>> Aend;
	detectShapeEnds(A,Aend);
	detectShapeEnds(B,Bend);
	int m = A.size(); 
	int n = Bend.size(); 
	vector<int> diffs;


	for(int i=0;i<m;i++)
	{
		int diff = INT_MAX;
		//float diff = fabs((float)(A[i]-B[0]));
		int minj=diff;
		int j=0;
		while (j<n)
		{
			int tmp = INT_MAX;
			int shift = tmp;
			if(Bend[j].first)
			{
				int start = Bend[j].second;
				int end = Bend[j+1].second;
				int diffs = (A[i] - start);
				int diffe = (A[i] - end);					
				if(diffs>=0&&diffe<=0)
				{
					shift = A[i];
					tmp = 0;
				}
				else
				{
					int absdiffs = abs(diffs);
					int absdiffe = abs(diffe);
					tmp = absdiffs<absdiffe?absdiffs:absdiffe;
					if(tmp==absdiffs)
						shift = start;
					else
						shift = end;
				}					
				j = j + 2;
			}
			else
			{
				tmp=abs(A[i]-Bend[j].second);
				shift = Bend[j].second;
				j++;
			}

			if(tmp<diff)
			{
				diff=tmp;				
				minj=shift;
			}
			if(diff==0)
				break;
		}
		matchedPoints.push_back(minj);
		diffs.push_back(diff);
	}

	return;


	//vector<int> omatchedPoints;
	//int om = A.size(); 
	//int on = B.size(); 
	//vector<int> odiffs;
	//for(int i=0;i<om;i++)
	//{
	//	int diff = abs((A[i]-B[0]));
	//	int minj=B[0];
	//	for(int j=1;j<on;j++)
	//	{
	//		int tmp = abs((A[i]-B[j]));
	//		if(tmp<diff)
	//		{
	//			diff=tmp;
	//			minj = B[j];
	//		}	
	//		if(diff==0)
	//			break;
	//	}		

	//	odiffs.push_back(diff);
	//	omatchedPoints.push_back(minj);
	//}
	//
	//for(int i=0;i<matchedPoints.size();i++)
	//{
	//	if(matchedPoints[i]!=omatchedPoints[i])
	//		cout<<"bug\n";
	//}

	/*if(diffs.size()==0) return 0;
	int maxdiff = diffs[0];
	for(int i=1;i<m;i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;*/
}


vector<Segment> HausdorffDist::updateEnd(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd)
{
	
	int n = BEnd.size(); 
	

	
	int j=0;


	
	while (j<n-1)
	{

		if(BEnd[j].first)
		{
			int end = BEnd[j+1].second;
			if(j+2<n)
			{			
				int nextend = BEnd[j+2].second;
				int mid = (nextend+end)/2;
				int tmp = (nextend-end)/2;
				int midInA = pointInSegment(AEnd,mid);
				if(midInA!=-1&&AEnd[midInA].first)
				{
					if(AEnd[midInA+1].second!=mid)
					{
						AEnd.insert(AEnd.begin()+midInA+1,pair<bool,int>(true,mid));
						AEnd.insert(AEnd.begin()+midInA+2,pair<bool,int>(true,mid+1));												
					}
				}
			}
			j+=2;			
		}
		else
		{
			int end = BEnd[j].second;
			int nextend = BEnd[j+1].second;
			int mid = (nextend+end)/2;
			int tmp = (nextend-end)/2;
			int midInA = pointInSegment(AEnd,mid);
			if(midInA!=-1 && midInA!=AEnd[midInA].second)
			{
				//AEnd[midInA] = pair<bool,int>(false,mid);
				if(AEnd[midInA].first)
				{				
					AEnd.insert(AEnd.begin()+midInA+1,pair<bool,int>(true,mid-1));
					AEnd.insert(AEnd.begin()+midInA+2,pair<bool,int>(false,mid));
					AEnd.insert(AEnd.begin()+midInA+3,pair<bool,int>(true,mid+1));				
				}
				else
				{
					//cout<<"debug\n";
				}
			}
			else if(midInA!=-1&&midInA==AEnd[midInA].second)
			{
				//cout<<"debug\n";
			}
			j++;
		}
	}

	vector<Segment> segments;
	int i=0;
	while(i<AEnd.size())
	{
		int start = -1;
		int end = -1;
		bool continous = false;
		if(AEnd[i].first)
		{
			start = AEnd[i++].second;
			end = AEnd[i++].second;
			continous = true;
		}
		else
		{
			start = AEnd[i++].second;
			end=start;
			continous = false;
		}
		segments.push_back(Segment(start,end,continous));

	}
	return segments;

}


vector<pair<Segment,Segment>> HausdorffDist::matchPair(vector<Segment>  A, vector<Segment>  B)
{
	vector<pair<Segment,Segment>>  pairs;
	map<int,int> pairmap;
	int m = A.size();
	int n = B.size();
	int len = m<n?m:n;
	
	for(int i=0;i<m;i++)
	{
		
		int minstart = INT_MAX;// abs(A[i].start +A[i].end - B[0].start - B[0].end);;
		int minindex = 0;
		bool overlap = false;
		for(int j=0;j<n;j++)
		{
			if(A[i].continous!=B[j].continous) continue;

			//int as = A[i].start - B[j].start;
			 overlap = checkOverlap(A[i],B[j]);
			//else
			//{				
			//	int tmp = abs(A[i].start - B[j].start);// +A[i].end - B[j].end  );
				if(overlap)
				{
					//minstart = tmp;
					minindex = j;
					break;
				}
			//}

			
				
		}

		if(overlap)		
			pairmap.insert(pair<int,int>(minindex,i));
			
		
		//else
		//{		
		//	map<int,int>::iterator it = pairmap.find(minindex);
		//	if(it!=pairmap.end())
		//	{
		//		int existAIndex = pairmap[minindex];
		//		
		//		int exisitStart = abs(A[existAIndex].start-B[minindex].start);//+A[existAIndex].end-B[minindex].end);
		//		if(exisitStart>minstart)
		//			pairmap[minindex] = i;
		//	}
		//	else
		//		pairmap.insert(pair<int,int>(minindex,i));
		//}
	
	}
	//if(m>0&&pairmap.size()==0)
	//	cout<<"debug\n";
	for(map<int,int>::iterator it=pairmap.begin();it!=pairmap.end();it++)
		pairs.push_back(pair<Segment,Segment>(A[(*it).second],B[(*it).first]));
	return pairs;
}


vector<Segment> HausdorffDist::unionPair(vector<Segment>  A, vector<Segment>  B)
{
	vector<Segment>  segments;
/*	
	int m = A.size();
	int n = B.size();
	
	for(int i=0;i<m;i++)
	{
		Segment seg;
		for(int j=0;j<n;j++)
		{
			bool hasnew = checkOverlap(A[i],B[j],seg);
			if(hasnew)
				segments
		}
	}*/
	return segments;
}

bool HausdorffDist::checkOverlap(Segment a, Segment b)
{
	
	int astart = a.start;
	int aend = a.end;
	int bstart = b.start;
	int bend = b.end;

	//int astartTobstart = astart - bstart;
	int astartTobend = astart - bend;

	//int bstartToaend = bstart- aend;
	int bstartToaend = bstart - aend;

	if(bstartToaend>=0||astartTobend>=0)
		return false;
	//else
	//{
	//	if(bstartToaend<0)
	//	{
	//		newseg.start = astart;
	//		newseg.end = bend;
	//	}
	//	else if(astartTobend<0)

	//	{
	//		newseg.start = bstart;
	//		newseg.end = aend;
	//	}
	//}
	return true;

}

int HausdorffDist::compute_dist_fast_and_match(vector<pair<bool,int>> AEnd, vector<pair<bool,int>> BEnd)
{
	int m = AEnd.size(); 
	int n = BEnd.size(); 
	vector<int> diffs;

	vector<pair<Segment,Segment>> pairs;
	int j=0;

	vector<pair<bool,int>> newA=AEnd;
	while (j<n-1)
	{

		if(BEnd[j].first)
		{
			int end = BEnd[j+1].second;
			if(j+2<n)
			{			
				int nextend = BEnd[j+2].second;
				int mid = (nextend+end)/2;
				int tmp = (nextend-end)/2;
				int midInA = pointInSegment(AEnd,mid);
				if(midInA!=-1)
				{
					newA.insert(AEnd.begin()+midInA,pair<bool,int>(true,mid));
					newA.insert(AEnd.begin()+midInA,pair<bool,int>(true,mid));				
					diffs.push_back(tmp);									
				}
			}
			j+=2;			
		}
		else
		{
			int end = BEnd[j].second;
			int nextend = BEnd[j+1].second;
			int mid = (nextend+end)/2;
			int tmp = (nextend-end)/2;
			int midInA = pointInSegment(AEnd,mid);
			if(midInA!=-1)
			{
				newA.insert(AEnd.begin()+midInA,pair<bool,int>(true,mid));
				newA.insert(AEnd.begin()+midInA,pair<bool,int>(true,mid));	
				diffs.push_back(tmp);	
			}
			j++;
		}
	}


	int maxdiff = diffs[0];
	for(int i=1;i<diffs.size();i++)
	{
		if(diffs[i]>maxdiff)
			maxdiff = diffs[i];
	}

	return maxdiff;
}



map<int,float> HausdorffDist::computeShift2(vector<int> A, vector<int> B,map<int,int> &matchedPoints)
{
	vector<pair<bool,int>>  endInA,endInB;
	map<int,float> scaleFactor;
	
	detectShapeEnds(A,endInA);
	detectShapeEnds(B,endInB);

	vector<Segment> segmentInA = updateEnd(endInA,endInB);
	vector<Segment> segmentInB = updateEnd(endInB,endInA);
	//int shift = computeShift(A,endInB);
	
	vector<pair<Segment,Segment>> segmentPairs = matchPair(segmentInA,segmentInB);
	for(int i=0;i<segmentPairs.size();i++)
	{
		Segment segmentInA = segmentPairs[i].first;
		Segment segmentInB = segmentPairs[i].second;

		int startInA = segmentInA.start;
		int endInA = segmentInA.end;
		int startInB = segmentInB.start;
		int endInB = segmentInB.end;

		float slope = (float)(endInB-startInB)/(endInA-startInA);
		float intercept = startInB - slope*startInA; 
		
		float tmpSlope = slope;
		if(tmpSlope<1)
			tmpSlope=1/tmpSlope;
		//int shiftA = startInB - startInA;
		//int shiftB = endInB - endInA;
		//int absShiftA = abs(shiftA);
		//int absShiftB = abs(shiftB);
		//int maxshift;
		//if(shiftA<=0&&shiftB>=0)
		//	maxshift = 0;
		//else
		//	maxshift  = absShiftA>absShiftB?absShiftA:absShiftB;
		for(int j=startInA;j<=endInA;j++)
		{
			float fshift = j*slope + intercept;
			int shift = fshift;
			matchedPoints.insert(pair<int,int>(j,shift));					
			float cost = abs(j-shift);//*tmpSlope;
			scaleFactor.insert(pair<int,float>(j,cost));
		
		}
	}
		

	return scaleFactor;
	
}