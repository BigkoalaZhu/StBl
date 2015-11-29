#pragma once
#include <QAbstractTableModel>
#include <QTableView>

#include "Colormaps.h"

class BasicTable : public QAbstractTableModel {
public:
	BasicTable(const std::vector< std::vector<float> > & data, bool colored = false, QObject *parent = 0) 
		: QAbstractTableModel(parent), colored(colored){
		Columns = data;
		cmap = makeColorMap();
	}

	QVariant data(const QModelIndex& index, int role) const{
		if(role == Qt::DisplayRole)
		{
			return QString::number( Columns[index.row()][index.column()], 'g', 3 );
		}
		if(role == Qt::BackgroundRole && colored)
		{
			return getColorFromMap( Columns[index.row()][index.column()], cmap );			
		}
		return QVariant::Invalid;
	}

	int rowCount(const QModelIndex& ) const{
		return (int)Columns.size();
	}

	int columnCount(const QModelIndex& ) const{
		return (int)Columns[0].size();
	}

	std::vector< std::vector<float> > Columns;

	bool colored;
	QVector< QColor > cmap;
};

inline QTableView * showTable( std::vector< std::vector<float> > data, size_t count )
{
	static QTableView * tableview = new QTableView;

	data.resize(count);
	auto table = new BasicTable(data);

	tableview->setModel(table);
	tableview->setMinimumSize(400,900);
	tableview->show();

	return tableview;
}

inline QTableView * showTable( const std::vector<float> & _data )
{
	std::vector< std::vector<float> > data;
	for(size_t i = 0; i < _data.size(); i++){
		std::vector<float> row;
		row.push_back(_data[i]);
		data.push_back(row);
	}

	static QTableView * tableview = new QTableView;
	auto table = new BasicTable(data);
	tableview->setModel(table);
	tableview->setMinimumSize(400,900);
	tableview->show();

	return tableview;
}

template<typename T>
inline QTableView * showTableColorMap( const std::vector< std::vector<T> > & data, bool isRowNormalize = false )
{
	static QTableView * tableview = new QTableView;

	// Make into float
	std::vector< std::vector<float> > dataf;
	float min_val = FLT_MAX, max_val = -FLT_MAX;
	for(auto row : data){
		std::vector<float> r;
		for(auto d : row){
			r.push_back(d);
			min_val = std::min(min_val, (float)d);
			max_val = std::max(max_val, (float)d);
		}
		dataf.push_back(r);
	}
	float range = (max_val-min_val);
	if(range == 0) range = 1.0;

	// Normalize
	for(auto & row : dataf){
		if( isRowNormalize )
		{
			auto minmax = std::minmax_element(row.begin(), row.end());
			range = *minmax.second - *minmax.first;
			if(range == 0) range = 1.0;
			for(auto & d : row)
				d = (d-*minmax.first) / range;
		}
		else
		{
			for(auto & d : row)
				d = (d-min_val) / range;
		}
	}

	auto table = new BasicTable(dataf, true);
	tableview->setModel(table);
	tableview->setMinimumSize(600,600);
	tableview->show();

	return tableview;
}
