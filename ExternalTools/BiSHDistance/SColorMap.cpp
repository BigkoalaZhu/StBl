#include "SColorMap.h"

// global variables
unsigned char g_classColorMap[45] = {
	255,81,63,
	232,242,28,
	153,227,15,
	99,104,183,
	166, 206, 227,
	31, 120, 180,
	178, 223, 138,
	51, 160,  44,
	251, 154, 153,
	227,  26,  28,
	253, 191, 111,
	255, 127,   0,
	202, 178, 214,
	106,  61, 154,
	255, 255, 153
};

unsigned char g_normalColorMap[75] = {
	0,   0, 127,
	0,   0, 255,
	0, 127,   0,
	0, 127, 127,
	0, 127, 255,
	0, 255,   0,
	0, 255, 127,
	0, 255, 255,
	127,   0,   0,
	127,   0, 127,
	127,   0, 255,
	127, 127,   0,
	127, 127, 127,
	127, 127, 255,
	127, 255,   0,
	127, 255, 127,
	127, 255, 255,
	255,   0,   0,
	255,   0, 127,
	255,   0, 255,
	255, 127, 127,
	255, 127, 255,
	255, 255,   0,
	255, 255, 127,
	255, 255, 255
};

unsigned char g_boxColorMap[24] = {
	228,  26,  28,
	55, 126, 184,
	77, 175,  74,
	152,  78, 163,
	255, 127,   0,
	255, 255,  51,
	166,  86,  40,
	247, 129, 191
};


SColorMap g_colorMap;


// implementations
SColorMap::SColorMap(void)
{
	m_classColorNum = 92;
	m_classColors = new Color[m_classColorNum];

	for (int i=0; i<11; i++) {
		float r = static_cast<float>(g_classColorMap[i*3+0]) / 255.0f;
		float g = static_cast<float>(g_classColorMap[i*3+1]) / 255.0f;
		float b = static_cast<float>(g_classColorMap[i*3+2]) / 255.0f;
		m_classColors[i][0] = r; m_classColors[i][1]= g; m_classColors[i][2] = b;
	}


	for (int i=11; i<21; i++) {
		int ii = i ;
		int ij = i ;
		float r = m_classColors[ii][0] + m_classColors[ij][0];
		float g = m_classColors[ii][1] + m_classColors[ij][1];
		float b = m_classColors[ii][2] + m_classColors[ij][2];
		r = r / 2.0f;
		g = g / 2.0f;
		b = b / 2.0f;
		m_classColors[i][0] = r; m_classColors[i][1] = g; m_classColors[i][2] = b;
	}
	for (int i=21; i<46; i++) {
		int index = i - 21;
		float r = static_cast<float>(g_normalColorMap[index*3+0]) / 255.0f;
		float g = static_cast<float>(g_normalColorMap[index*3+1]) / 255.0f;
		float b = static_cast<float>(g_normalColorMap[index*3+2]) / 255.0f;
		m_classColors[i][0] = r; m_classColors[i][1]= g; m_classColors[i][2]= b;
	}
	for (int i=46; i<92; i++) {
		m_classColors[i] = m_classColors[i-46];
	}

	m_boxColorNum = 8;
	m_boxColors = new Color[m_boxColorNum];
	int s1 = 204;
	int s2 = 51;
	m_boxColors[0][0]= s1/255.0f;m_boxColors[0][1]= s1/255.0f;m_boxColors[0][2]= s1/255.0f;
	m_boxColors[1][0]= s2/255.0f;m_boxColors[1][1]= s2/255.0f;m_boxColors[1][2]= s2/255.0f;
	for (int i=2; i<m_boxColorNum; i++) {
		float r = static_cast<float>(g_boxColorMap[i*3+0]) / 255.0f;
		float g = static_cast<float>(g_boxColorMap[i*3+1]) / 255.0f;
		float b = static_cast<float>(g_boxColorMap[i*3+2]) / 255.0f;
		m_boxColors[i][0] = r; m_boxColors[i][1] = g; m_boxColors[i][2] = b;
	}

	for (int i=0; i<m_classColorNum; i++) {
		int r = static_cast<int>(m_classColors[i][0] * 255.0f);
		int g = static_cast<int>(m_classColors[i][1] * 255.0f);
		int b = static_cast<int>(m_classColors[i][2] * 255.0f);
		m_lstClassColors.push_back(Color(r,g,b));
	}

	for (int i=0; i<m_boxColorNum; i++) {
		int r = g_boxColorMap[i*3+0];
		int g = g_boxColorMap[i*3+1];
		int b = g_boxColorMap[i*3+2];

		m_lstBoxColors.push_back(Color(r,g,b));
	}
}

