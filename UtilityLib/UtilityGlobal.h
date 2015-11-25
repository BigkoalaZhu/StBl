#pragma once

#include <fstream>
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
#include "ShapeNetFormat.h"

#include "SurfaceMeshModel.h"
#include "SurfaceMeshHelper.h"
using namespace SurfaceMesh;



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

namespace Eigen{
	template<class Matrix>
	void write_binary(const char* filename, const Matrix& matrix){
		std::ofstream out(filename, std::ios::out | std::ios::binary | std::ios::trunc);
		typename Matrix::Index rows = matrix.rows(), cols = matrix.cols();
		out.write((char*)(&rows), sizeof(typename Matrix::Index));
		out.write((char*)(&cols), sizeof(typename Matrix::Index));
		out.write((char*)matrix.data(), rows*cols*sizeof(typename Matrix::Scalar));
		out.close();
	}

	template<class Matrix>
	void write_txt(const char* filename, const Matrix& matrix){
		std::ofstream out(filename, std::ios::out | std::ios::trunc);
		typename Matrix::Index rows = matrix.rows(), cols = matrix.cols();
		out.write((char*)(&rows), sizeof(typename Matrix::Index));
		out.write((char*)(&cols), sizeof(typename Matrix::Index));
		out.write((char*)matrix.data(), rows*cols*sizeof(typename Matrix::Scalar));
		out.close();
	}

	template<class Matrix>
	void read_binary(const char* filename, Matrix& matrix){
		std::ifstream in(filename, std::ios::in | std::ios::binary);
		typename Matrix::Index rows = 0, cols = 0;
		in.read((char*)(&rows), sizeof(typename Matrix::Index));
		in.read((char*)(&cols), sizeof(typename Matrix::Index));
		matrix.resize(rows, cols);
		in.read((char *)matrix.data(), rows*cols*sizeof(typename Matrix::Scalar));
		in.close();
	}
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
	file.close();
}

static inline void fileToMatrix(Eigen::MatrixXd & M, QString filename, int width)
{
	QFile file(filename);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return;
	QTextStream in(&file);

	M = Eigen::MatrixXd::Zero(width, width);

	for (unsigned int i = 0; i < width; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			in >> M(i, j);
		}
	}
	file.close();
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
	file.close();
	//////////////////////////////////////
	for (int i = 0; i < ShapeList.size(); i++)
	{
		//QDir dir(ShapeList[i].FileLocation + "/parts");
		//if (!dir.exists())
		//	dir.mkdir(dir.absolutePath());
		//else if (dir.count() != 2)
		//	continue;

		QDir dir(ShapeList[i].FileLocation + "/seg.obj");
		if (!dir.exists())
			dir.mkdir(dir.absolutePath());
		else
			continue;

		QString location = ShapeList[i].FileLocation + "/model.obj";
		SurfaceMeshModel * model = new SurfaceMeshModel(location);
		if (!model->read(location.toStdString()))
			continue;
		model->update_face_normals();
		model->update_vertex_normals();
		std::vector< SurfaceMesh::SurfaceMeshModel* > parts = connectedPieces(model);

		for (int j = 0; j < parts.size(); j++)
		{
//			QString index = dir.absolutePath() + "/part_" + QString::number(j) + ".obj";
			//////////////
			//////////////
			parts[j]->update_face_normals();
			parts[j]->update_vertex_normals();
//			writeOBJ::wirte(parts[j], index);
		}
		writeOBJ::wirte(parts, dir.absolutePath()+"seg.obj");
	}
	///////////////////////////Generate load List
	path.chop(1);
	path.append(".list");
	QFile outputList(ListName);
	if (!outputList.open(QIODevice::WriteOnly | QFile::Text)) return;
	QTextStream out(&outputList);
	for (int i = 0; i < ShapeList.size(); i++)
	{
		out << ShapeList[i].FileLocation << ";;";
		out << ShapeList[i].FileDescriptor << ";;";
		out << ShapeList[i].FrontDirection[0] << "\\," << ShapeList[i].FrontDirection[1] << "\\," << ShapeList[i].FrontDirection[2] << ";;";;
		out << ShapeList[i].UpDirection[0] << "\\," << ShapeList[i].UpDirection[1] << "\\," << ShapeList[i].UpDirection[2] << ";;";;
		out << endl;
	}
	outputList.close();
}

static inline bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
	toDir.replace("\\", "/");
	if (sourceDir == toDir){
		return true;
	}
	if (!QFile::exists(sourceDir)){
		return false;
	}
	QDir *createfile = new QDir;
	bool exist = createfile->exists(toDir);
	if (exist){
		if (coverFileIfExist){
			createfile->remove(toDir);
		}
	}//end if  

	if (!QFile::copy(sourceDir, toDir))
	{
		return false;
	}
	return true;
}
  
static inline bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
	QDir sourceDir(fromDir);
	QDir targetDir(toDir);
	if (!targetDir.exists()){    
		if (!targetDir.mkdir(targetDir.absolutePath()))
			return false;
	}

	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()){    
			if (!copyDirectoryFiles(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()),
				coverFileIfExist))
				return false;
		}
		else{            
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())){
				targetDir.remove(fileInfo.fileName());
			}

			
			if (!QFile::copy(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()))){
				return false;
			}
		}
	}
	return true;
}

#include "BasicTable.h"