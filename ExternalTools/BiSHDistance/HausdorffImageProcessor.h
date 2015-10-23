#pragma  once
#include "utility.h"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/ts/ts.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "HausdorffDist.h"
#include "HausdorffNode.h"
#include <vector>
using namespace std;
class HausdorffImageProcessor
{
public:
	cv::Mat m_originalImage;
	cv::Mat m_grayImage;
	string m_filename;
public:
	HausdorffDist m_hausdorffDist;
public:
	HausdorffImageProcessor(){};
	~HausdorffImageProcessor(){};
public:	
	bool loadImage(string);
	cv::Mat getInputImage(){return m_originalImage;}
private:
	cv::Mat convertToGray(cv::Mat,bool isShapeBlack=true);
};