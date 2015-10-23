#include "HausdorffImageProcessor.h"

bool HausdorffImageProcessor::loadImage(string filename)
{
	m_filename = filename;
	m_originalImage = cv::imread( filename.c_str(), 1 );
	if(m_originalImage.data==NULL)
		return false;
	m_grayImage = convertToGray(m_originalImage);
	return true;
}


//shape is black and background is white
cv::Mat HausdorffImageProcessor::convertToGray(cv::Mat image,bool isShapeBlack)
{
	cv::Size sz = image.size();
	int width = sz.width;
	int height = sz.height;

	cv::Mat gimage(sz,CV_8U);
	for(int iy=0;iy<height;iy++)
		for(int ix=0;ix<width;ix++)		
		{
			cv::Vec3b intensity = image.at<cv::Vec3b>(iy,ix);
			uchar blue = intensity.val[0];
			uchar green = intensity.val[1];
			uchar red = intensity.val[2];

			int index = iy*width+ix;
			uchar gvalue1 = 0;
			uchar gvalue2 = 255;
			if((red>=250)&&(green>=250)&&(blue>=250))			
			{
				gvalue1 = 255;		
				gvalue2 = 0;
			}
			if(isShapeBlack)
				gimage.at<uchar>(iy,ix) = gvalue1;	
			else
				gimage.at<uchar>(iy,ix) = gvalue2;	

		}
	return gimage;

}