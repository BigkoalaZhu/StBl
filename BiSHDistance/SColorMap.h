#pragma once

#include <vector>
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/ts/ts.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

using namespace std;
// class
typedef cv::Vec3f Color;
class SColorMap
{
private:
	Color *m_classColors;
	int m_classColorNum;

	Color *m_boxColors;
	int m_boxColorNum;

public:
	vector<Color> m_lstClassColors;
	vector<Color> m_lstBoxColors;

public:
	SColorMap(void);
	~SColorMap(void){};

public:
	inline Color GetClassColor(const int i) { return m_classColors[i]; }
	inline Color GetBoxColor(const int i) { return m_boxColors[i]; }
	inline void SetClassColor(const int i, Color color)
	{
		m_classColors[i] = color;
	}
	inline void SetBoxColor(const int i, Color color)
	{
		m_boxColors[i] = color;
	}

	//void SetOpenGLClassColor(const float a, const int i,bool selected = false);

	//void SetOpenGLBoxColor(const float a, const int i,bool selected = false);

	//void SetOpenGLClassColor(const float a, const int i, const int num);
	//Color setRGB(float v);
};


