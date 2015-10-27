#pragma once

#include <nanoflann.hpp>

using namespace std;
using namespace nanoflann;

#include "Eigen\core"

template <typename T>
struct PointCloud{
	std::vector<Eigen::VectorXf>  pts;

	inline size_t kdtree_get_point_count() const { return pts.size(); }
};

typedef std::pair<size_t, float> KDResultPair;
typedef std::vector< KDResultPair  > KDResults;

template <int DIM>
class FeatureSearchingTree
{
public:
	PointCloud<float> cloud;

	typedef KDTreeSingleIndexAdaptor< L1_Adaptor<float, PointCloud<float> >, PointCloud<float>, DIM /* dim */ > SearchingTree;

	SearchingTree * tree;

	FeatureSearchingTree(){ tree = NULL; }
	~FeatureSearchingTree(){ delete tree; }

	void addPoint(const Eigen::VectorXf & p){
		cloud.pts.push_back(p);
	}

	void build()
	{
		if (tree) delete tree;

		// construct a kd-tree index:
		tree = new my_kd_tree(DIM /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
		tree->buildIndex();
	}

	size_t k_closest(Eigen::VectorXf p, size_t k, KDResults & ret_matches)
	{
		k = k < cloud.pts.size() ? k : cloud.pts.size();

		ret_matches.clear();
		ret_matches.resize(k);

		std::vector<size_t> ret_index(k);
		std::vector<float> out_dist(k);

		tree->knnSearch(&p[0], k, &ret_index[0], &out_dist[0]);

		for (size_t i = 0; i < k; i++)
			ret_matches[i] = std::make_pair(ret_index[i], out_dist[i]);

		return k;
	}

	/* Returns only number of points for a ball search query */
	size_t ball_search(Eigen::VectorXf p, double search_radius)
	{
		KDResults ret_matches;
		return ball_search(p, search_radius, ret_matches);
	}

	/* Return points inside sphere with center at 'p' and radius 'search_radius' */
	size_t ball_search(Eigen::VectorXf p, double search_radius, KDResults & ret_matches)
	{
		ret_matches.clear();

		nanoflann::SearchParams params;
		//params.sorted = false; // by default, the results are sorted from closest to furthest

		return tree->radiusSearch(&p[0], search_radius, ret_matches, params);
	}

	int closest(Eigen::VectorXf & p)
	{
		KDResults match;
		this->k_closest(p, 1, match);

		if (!match.size())
			return -1;
		else
			return (int)match[0].first;
	}

	inline bool has(Eigen::VectorXf & p, double eps_distance = 1e-7)
	{
		KDResults match;
		ball_search(p, eps_distance, match);
		return match.size();
	}
};

