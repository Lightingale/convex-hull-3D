#include <iostream>
#include "ConvexHull3D.h"
#include <algorithm>

int main() {
	size_t m = 0;
	std::cin >> m;
	std::vector<std::vector<Facet>> answer;
	for (size_t i = 0; i < m; ++i) {
		size_t n = 0;
		std::cin >> n;
		std::vector<std::valarray<ld>> points(n);
		for (size_t j = 0; j < n; ++j) {
			points[j].resize(3);
			std::cin >> points[j][0] >> points[j][1] >> points[j][2];
		}
		ConvexHull3D hull(points, true);
		std::vector<Facet> facets = hull.Build();
		std::sort(facets.begin(), facets.end());
		answer.push_back(facets);
	}
	for (auto facets : answer) {
		std::cout << facets.size() << '\n';
		for (auto point : facets) {
			std::cout << 3 << ' ' << point.left << ' ' << point.mid << ' ' << point.right << '\n';
		}
	}

	system("pause");
	return 0;
}