#include "utility.h"
bool fileExists(const std::string& fileName)
{
	std::fstream file;
	file.open(fileName.c_str(), std::ios::in);
	if (file.is_open() == true)
	{
		file.close();
		return true;
	}
	file.close();
	return false;
}


vector<string> getFileNames(const char *path)
{
	vector<string> filenames;
	DIR *dir;
	struct dirent *ent;

	dir = opendir (path);
	if (dir != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			//tmp1 = strchr(ent->d_name,'b');
			//if(tmp1!=NULL)
			{		
				string postfix(ent->d_name);
				if(postfix.size()<4) continue;
				postfix = postfix.substr(postfix.size()-4,4);
				if(postfix!=".bmp"&&postfix!=".BMP")
					continue;
			}
			//else
				//continue;
//
			string tmpstr(path);
			tmpstr.append(ent->d_name);


			filenames.push_back(ent->d_name);			

		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return vector<string>();
	}
	return filenames;
}









vector<int> sortPairwithIndex(vector<pair<int,float>> &data)
{
	vector<int> index;
	int n = data.size();
	index.resize(n);
	for(int i=0;i<n;i++)
		index[i] = i;//.push_back(i);
	//return index;
	bool swapped = false;
	do{
		swapped = false;
		for(int i=0;i<n-1;i++)
		{
			if((data[i].second)>(data[i+1].second)){
				pair<int,float> tmp = data[i];
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




