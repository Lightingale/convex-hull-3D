#include "Geometry.h"

ld VectorProduct(const std::valarray<ld>& first, const std::valarray<ld>& second) {
	return (first[0] * second[1] - second[0] * first[1]);
}

bool OrientPoint(const std::valarray<ld>& first, const std::valarray<ld>& second,
	const std::valarray<ld>& point) {
	return (VectorProduct(second - first, point - second) > 0);
}
