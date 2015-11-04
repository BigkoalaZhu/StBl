#pragma once

#include <QtCore>
#include <Eigen/core>

struct ShapeNetModelInfo{
	QString FileLocation;
	QString FileDescriptor;
	Eigen::Vector3d UpDirection;
	Eigen::Vector3d FrontDirection;
};

static inline Eigen::Vector3d QStringToVector3d(QString string)
{
	Eigen::Vector3d v;
	QStringList numbers = string.split("\\,");
	v[0] = numbers[0].toDouble();
	v[1] = numbers[1].toDouble();
	v[2] = numbers[2].toDouble();
	return v;
}

class ShapeNetFormate
{
public:
	static QVector<ShapeNetModelInfo> LoadFolder(QString folder)
	{
		QVector<ShapeNetModelInfo> ShapeList;
		ShapeNetModelInfo tmp;
		QString path = folder;
		path.append("/");

		QFile file(folder+".csv");
		if (!file.open(QFile::ReadOnly | QFile::Text)) return ShapeList;
		QTextStream in(&file);
		QString line = in.readLine();
		line = in.readLine();
		while (!line.isNull()) {
			QVector<QString> wholeline;
			wholeline.resize(6);
			int index = 0;
			int head = 0;
			bool status = true;
			for (int i = 0; i < line.length(); i++)
			{
				if (line.at(i) == ',' && status)
				{
					wholeline[index] = line.mid(head, i - head);
					head = i + 1;
					index++;
				}
				else if (line.at(i) == '\"')
					status = !status;
			}

			tmp.FileDescriptor = wholeline[2];
			tmp.FileDescriptor.remove("\"");
			tmp.FileLocation = wholeline[0];
			tmp.FileLocation.remove("3dw.");
			tmp.FileLocation.prepend(path);
			wholeline[3].remove("\"");
			wholeline[4].remove("\"");
			tmp.FrontDirection = QStringToVector3d(wholeline[4]);
			tmp.UpDirection = QStringToVector3d(wholeline[3]);
			ShapeList.push_back(tmp);
			line = in.readLine();
		}
		file.close();
		return ShapeList;
	}

};