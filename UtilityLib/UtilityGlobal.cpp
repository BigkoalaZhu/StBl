#include "UtilityGlobal.h"
#include <direct.h>


QString qStr( const Vector2 &v, char sep /*= ' '*/ )
{
	return QString("%1%2%3").arg(v.x()).arg(sep).arg(v.y());
}

QString qStr( const Vector3 &v, char sep)
{
	return QString("%1%2%3%4%5").arg(v.x()).arg(sep).arg(v.y()).arg(sep).arg(v.z());
}

QString qStr( const Vector4 &v, char sep )
{
	return QString("%1%2%3%4%5%6%7").arg(v[0]).arg(sep).arg(v[1]).arg(sep).arg(v[2]).arg(sep).arg(v[3]);
}

Vector3 toVector3( QString string )
{
	QStringList sl = string.split(' ');
	if (sl.size() != 3) 
		return Vector3();
	else
		return Vector3(sl[0].toDouble(), sl[1].toDouble(), sl[2].toDouble());
}

QString getcwd()
{
	char cCurrentPath[FILENAME_MAX];

	if (GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		QString cwd(cCurrentPath);
		cwd.replace("\\", "/");
		return cwd;
	}
	else
		return QString();
}

