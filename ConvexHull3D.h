#pragma once
#include <vector>
#include <list>
#include "Geometry.h"
#include "Facet.h"
#include "Index.h"

class ConvexHull3D {
public:
	ConvexHull3D(std::vector<std::valarray<ld>> points, bool need_removal);
	// построение выпуклой оболочки
	std::vector<Facet> Build();
	// построение нижней половины выпуклой оболочки
	std::vector<Facet> BuildLower();

private:
	static const ld INF_;
	std::vector<std::valarray<ld>> points_;
	std::vector<Index> indices_;

	void CreateIndices();
	void RemoveCoincidences();

	std::list<Index*> Merge(const size_t left, const size_t right);
	std::list<Index*> TrackBridges(Index*& l, Index*& r, std::list<Index*>& left_events,
		std::list<Index*>& right_events, const size_t mid);
	void ReverseTime(Index*& l, Index*& r, std::list<Index*>& events, const size_t mid);

	ld Time(const Index* left, const Index* mid, const Index* right);
	ld Turn(const Index* left, const Index* mid, const Index* right);
	Facet CreateFacet(const Index* point, const bool is_upper);
};