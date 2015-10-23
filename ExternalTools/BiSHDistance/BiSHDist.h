#pragma once
#include "utility.h"
#include "HausdorffNode.h"
#include "HausdorffImageProcessor.h"
#include "HausdorffImageSimplify.h"
#include "HausdorffImageMatch.h"

class BiSHDist
{
public:
	static void ImageSimlify(string input_image, string output_file, int num, bool merge = true, int minNumLines = -1)
	{
		HausdorffImageSimplify Simplify;
		Simplify.run(input_image, num, merge, minNumLines);
		Simplify.saveNodeInfoSpecific(output_file);
	}

	static vector<pair<int, int>> ImageMatch(string img_A, string img_B, float cost)
	{
		HausdorffImageSimplify Simplify_A, Simplify_B;
		Simplify_A.loadNodeInfo(img_A);
		Simplify_B.loadNodeInfo(img_B);

		HausdorffImageMatch Match;
		return Match.run(Simplify_A.getNodes(), Simplify_B.getNodes(), cost);
	}
};

