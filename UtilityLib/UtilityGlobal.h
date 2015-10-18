#pragma once

#include <QtXml/QDomDocument>

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QMap>
#include <QSet>
#include <QFile>
#include <QDir>

#include <stack>
#include <set>

#include <qglviewer/qglviewer.h>
#include "RenderObjectExt.h"
#include "writeOBJ.h"

#include "SurfaceMeshModel.h"
#include "SurfaceMeshHelper.h"
using namespace SurfaceMesh;

struct ShapeNetModelInfo{
	QString FileLocation;
	QString FileDescriptor;
	Eigen::Vector3d UpDirection;
	Eigen::Vector3d FrontDirection;
};

namespace SurfaceMesh{
typedef Eigen::Vector2d Vector2;
typedef Eigen::Vector4d Vector4;
}

typedef QSharedPointer<SurfaceMeshModel> MeshPtr;

QString qStr(const Vector2 &v, char sep = ' ');
QString qStr(const Vector3 &v, char sep = ' ');
QString qStr(const Vector4 &v, char sep = ' ');

Vector3 toVector3(QString string);

typedef QVector< QVector<QString> > StrArray2D;

typedef QMap< QString, QVariant > PropertyMap;

#ifdef Q_OS_WIN
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
QString getcwd();

// Simplify runtime debugging
#include <QMessageBox>
template<typename DataType>
static inline void debugBox( DataType message ){
	QMessageBox msgBox;
	msgBox.setTextFormat(Qt::RichText);
	msgBox.setText( QString("%1").arg(message) );
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}
static inline void debugBoxList( QStringList messages ){
	debugBox( messages.join("<br>") );
}
template<typename Container>
static inline void debugBoxVec( Container data ){
	QStringList l;
	for(auto d : data) l << QString("%1").arg(d);
	debugBoxList(l);
}
template<typename Container2D>
static inline void debugBoxVec2( Container2D data, int limit = -1 ){
	QStringList l;
	for(auto row : data){
		QStringList line;
		for(auto d : row) line << QString("%1").arg( d );
		l << QString("%1").arg( line.join(", ") );
		if(limit > 0 && l.size() == limit) break;
	}
	if(limit > 0 && data.size() - l.size() > 0) l << QString("... (%1) more").arg(data.size() - l.size());
	debugBoxList(l);
}

static inline void matrixToFile(const Eigen::MatrixXd & M, QString filename)
{
	QFile file( filename );
	if(!file.open(QFile::WriteOnly | QFile::Text)) return;
	QTextStream out(&file);
	for(unsigned int i = 0; i < M.rows(); i++)
	{
		QStringList row;
		for(unsigned int j = 0; j < M.cols(); j++)
			row << QString::number(M(i,j));
		out << (row.join(",") + "\n");
	}
}


static inline std::vector< SurfaceMesh::SurfaceMeshModel* > connectedPieces(SurfaceMesh::SurfaceMeshModel * mesh) {
	std::vector< SurfaceMesh::SurfaceMeshModel* > pieces;
	std::vector< std::vector< SurfaceMesh::Face > > pieceFaces;

	Vector3VertexProperty points = mesh->vertex_coordinates();
	BoolFaceProperty fvisisted = mesh->face_property<bool>("f:visisted", false);
	ScalarFaceProperty fsegment = mesh->face_property<Scalar>("f:segment", -1);
	for (auto f : mesh->faces()){
		fvisisted[f] = false;
		fsegment[f] = -1;
	}

	std::vector<int> sizes;

	for (Face f : mesh->faces())
	{
		if (fvisisted[f]) continue;

		pieceFaces.push_back(std::vector< SurfaceMesh::Face >());

		std::stack<Face> unprocessed;
		unprocessed.push(f);

		while (!unprocessed.empty())
		{
			Face curFace = unprocessed.top();
			unprocessed.pop();
			if (fvisisted[curFace]) continue;

			fvisisted[curFace] = true;

			for (auto v : mesh->vertices(curFace))
			{
				for (auto fj : mesh->faces(v))
				{
					if (!mesh->is_valid(fj) || fvisisted[fj]) continue;
					unprocessed.push(fj);
				}
			}

			fsegment[curFace] = (pieceFaces.size() - 1);

			pieceFaces.back().push_back(curFace);
		}

		SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
		piece->reserve(pieceFaces.back().size() * 3, pieceFaces.back().size() * 6, pieceFaces.back().size());

		for (auto v : mesh->vertices())
		{
			piece->add_vertex(points[v]);
		}

		for (auto f : pieceFaces.back()){
			std::vector<Vertex> face;
			for (auto v : mesh->vertices(f)) face.push_back(v);
			piece->add_face(face);
		}

		for (auto v : piece->vertices()) if (piece->is_isolated(v)) piece->remove_vertex(v);
		piece->garbage_collection();

		sizes.push_back(piece->n_vertices());

		pieces.push_back(piece);
	}

	// Assign colors
	{
		std::set<int> segments;
		for (auto f : mesh->faces()) segments.insert(fsegment[f]);
		segments.erase(-1);
		std::vector< std::vector<double> > colors = starlab::randomColors(segments.size());
		QVariant colorsVar;
		colorsVar.setValue(colors);
		mesh->setProperty("colors", colorsVar);
	}

	return pieces;
}

static inline Eigen::Vector3d QStringToVector3d(QString string)
{
	Eigen::Vector3d v;
	QStringList numbers = string.split("\\,");
	v[0] = numbers[0].toDouble();
	v[1] = numbers[1].toDouble();
	v[2] = numbers[2].toDouble();
	return v;
}

static inline void SplitListShapes(QString ListName)
{
	////////////////////////////
	QVector<ShapeNetModelInfo> ShapeList;
	ShapeNetModelInfo tmp;
	QString path = ListName;
	path.chop(4);
	path.append("/");
	////////////////////////////
	QFile file(ListName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return;
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
	//////////////////////////////////////
	for (int i = 0; i < ShapeList.size(); i++)
	{
		QString location = ShapeList[i].FileLocation + "/model.obj";
		SurfaceMeshModel * model = new SurfaceMeshModel(location);
		model->read(location.toStdString());
		model->update_face_normals();
		model->update_vertex_normals();
		std::vector< SurfaceMesh::SurfaceMeshModel* > parts = connectedPieces(model);

		QDir dir(ShapeList[i].FileLocation+"/parts");
		if (!dir.exists())
			dir.mkdir(dir.absolutePath());

		for (int j = 0; j < parts.size(); j++)
		{
			QString index = dir.absolutePath() + "/part_" + QString::number(j) + ".obj";
			//////////////
			//////////////
			parts[j]->update_face_normals();
			parts[j]->update_vertex_normals();
			writeOBJ::wirte(parts[j], index);
		}
	}
}

#include "BasicTable.h"