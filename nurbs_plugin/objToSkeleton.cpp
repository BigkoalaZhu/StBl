#include <QApplication>
#include <QCommandLineParser>
#include <iostream>

#include "objToSkeleton.h"

enum CommandLineParseResult{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	
	QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "The file to open."));
    parser.addOptions({
        { { "c", "curve" }, QString("Extract a curve.") },
        { { "s", "sheet" }, QString("Extract a sheet.") },
        { { "b", "box" }, QString("Uses bounding box.") },
		{ { "n", "nonmanifold" }, QString("Object is not clean.") },
		{ { "h", "concavehull" }, QString("Reconstruct using concavehull.") },
		{ { "v", "voxelize" }, QString("Reconstruct using voxels.") },
		{ { "r", "remesh" }, QString("Remesh to increase density.") },
		{ { "m", "medial" }, QString("Follow medial axis during skeletonization.") },
		{ { "f", "features" }, QString("Respect small features.") },
		{ { "V", "voxelparam" }, QString("Voxels size."), QString("voxel_param") },
		{ { "R", "remeshparam" }, QString("Remesh size."), QString("remesh_param") }
    });

	if (!parser.parse(QCoreApplication::arguments())) {
        QString errorText = parser.errorText();
        std::cout << qPrintable(errorText);
        return CommandLineError;
    }
	else
		parser.process(a);

    if(QCoreApplication::arguments().size() == 1)
    {
        std::cout << qPrintable(parser.helpText());
        return CommandLineHelpRequested;
    }

    // Perform operations
    {
		double VoxelParam = 0.05, RemeshParam = 0.02;

		if (parser.isSet("voxelparam")) VoxelParam = parser.value("voxelparam").toDouble();
		if (parser.isSet("remeshparam")) RemeshParam = parser.value("remeshparam").toDouble();

        QString filename = parser.positionalArguments().at(0);
        nurbs_plugin plugin(filename, !parser.isSet("nonmanifold"), 
			parser.isSet("b"), parser.isSet("h"), parser.isSet("v"), 
			parser.isSet("r"), parser.isSet("m"), parser.isSet("f"),
			VoxelParam, RemeshParam);

		if (parser.isSet("curve"))
		{
			auto c = plugin.toCurve();

			// Save control points
			QFile file("curve_output.points");
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return CommandLineError;
			QTextStream out(&file);
			out << QString("%1\n").arg(c.GetNumCtrlPoints());
			for (auto p : c.mCtrlPoint) out << QString("%1 %2 %3\n").arg(p.x()).arg(p.y()).arg(p.z());
		}

		if (parser.isSet("sheet"))
		{
			auto s = plugin.toSheet();

			// Save control points
			QFile file("sheet_output.points");
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return CommandLineError;
			QTextStream out(&file);
			out << QString("%1\n").arg(s.mNumUCtrlPoints);
			out << QString("%1\n").arg(s.mNumVCtrlPoints);
			for (auto row : s.mCtrlPoint) 
				for (auto p : row)
					out << QString("%1 %2 %3\n").arg(p.x()).arg(p.y()).arg(p.z());
		}
    }

    return CommandLineOk;
}
