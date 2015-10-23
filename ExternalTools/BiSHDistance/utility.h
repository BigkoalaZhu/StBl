#pragma once
#include<string>
#include <vector>
#include "dirent.h"
#include <algorithm>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <functional>
#include <map>
using namespace std;

vector<string> getFileNames(const char *path);

bool fileExists(const std::string& fileName);

template<typename elemType>
vector<int> sortWithIndex(vector<elemType> data);

vector<int> sortPairwithIndex(vector<pair<int,float>> &data);



template<typename elemType>
vector<int> sortWithIndex(vector<elemType> data)
{
	vector<int> index;
	int n = data.size();
	index.resize(n);
	for(int i=0;i<n;i++)
		index[i] = i;//.push_back(i);
	bool swapped = false;
	do{
		swapped = false;
		for(int i=0;i<n-1;i++)
		{
			if((data[i])>(data[i+1])){
				elemType tmp = data[i];
				data[i] = data[i+1];
				data[i+1] =  tmp;
				int tmpi = index[i];
				index[i] =  index[i+1];
				index[i+1] = tmpi;
				swapped = true;
			}
		}
		n=n-1;
	}while(swapped);


	return index;
}




template<typename elemType>
vector<elemType>  findTopK(vector<elemType> inputArray, int nOutputLength )
{
	//construct the minimum heap the size is K
	vector<elemType> vec(inputArray.begin(),inputArray.begin()+nOutputLength);
	make_heap (vec.begin(),vec.end(), greater<elemType>());

	for(int i=nOutputLength; i<inputArray.size(); i++)
	{
		if(inputArray[i] >= vec[0])
		{
			vec[0] = inputArray[i];
			//此处其实只需要保持堆的性质即可，并不需要重建堆
			make_heap (vec.begin(),vec.end(), greater<elemType>());
		}
	}
	return vec;


}



template<typename elemType>
elemType min3(elemType x, elemType y, elemType z)
{
	if (x < y)
		return (x < z)? x : z;
	else
		return (y < z)? y : z;
}

template<typename elemType>
int searchInPairs(vector<pair<elemType,elemType>> pairs,pair<elemType,elemType> target)
{
	int index = -1;
	for(unsigned int i=0;i<pairs.size();i++)
	{
		pair<int,int> ipari = pairs[i];
		if(ipari.first == target.first&&ipari.second==target.second)
			return i;	
	}
	return index;
}