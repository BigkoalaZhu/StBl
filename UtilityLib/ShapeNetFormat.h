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

	static void SaveFolder(QString folder, QVector<ShapeNetModelInfo> List)
	{
		QFile file(folder);
		if (!file.open(QFile::WriteOnly | QFile::Text)) return;
		QTextStream out(&file);
		out << "fullId,wnsynset,wnlemmas,up,front,name,tags\n";

		for (int i = 0; i < List.size(); i++)
		{
			int index = List[i].FileLocation.lastIndexOf('/');
			QString name = List[i].FileLocation.right(List[i].FileLocation.length() - index - 1);
			out << "3dw." << name << ",\" \",";
			out << "\"" << List[i].FileDescriptor << "\",";
			out << "\"" << QString::number(List[i].UpDirection[0]) << "\\," << QString::number(List[i].UpDirection[1]) << "\\," << QString::number(List[i].UpDirection[2]) << "\",";
			out << "\"" << QString::number(List[i].FrontDirection[0]) << "\\," << QString::number(List[i].FrontDirection[1]) << "\\," << QString::number(List[i].FrontDirection[2]) << "\",";
			out << "\" \"\n";
		}
		file.close();
	}

	static QVector<ShapeNetModelInfo> LoadFolderSem(QString folder, QString subFolder)
	{
		QVector<ShapeNetModelInfo> ShapeList;
		QVector<ShapeNetModelInfo> ShapeListFull;
		ShapeNetModelInfo tmp;
		QString path = folder;
		path.append("/");

		ShapeListFull = LoadFolder(path + subFolder);

		QFile file(path + "sem.csv");
		if (!file.open(QFile::ReadOnly | QFile::Text)) return ShapeList;
		QTextStream in(&file);
		QString line = in.readLine();
		line = in.readLine();
		while (!line.isNull()) {
			QVector<QString> wholeline;
			wholeline.resize(16);
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

			if (!(wholeline[1].contains("Chair") || wholeline[1].contains("Stool")))
			{
				line = in.readLine();
				continue;
			}
			QString location = wholeline[0];
			location.remove("wss.");
			location.prepend(path + subFolder + "/");
			for (int i = 0; i < ShapeListFull.size(); i++)
			{
				if (ShapeListFull[i].FileLocation == location)
				{
					ShapeList.push_back(ShapeListFull[i]);
					break;
				}
			}
			line = in.readLine();
		}
		file.close();

		SaveFolder(path + "sub.csv", ShapeList);

		return ShapeList;
	}
};