#pragma once
#include <valarray>

typedef long double ld;

// векторное произведение векторов на плоскости (z-координата)
ld VectorProduct(const std::valarray<ld>& first, const std::valarray<ld>& second);

// проверка, с какой стороны от прямой находится точка
bool OrientPoint(const std::valarray<ld>& first, const std::valarray<ld>& second,
	const std::valarray<ld>& point);