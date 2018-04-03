#include "Facet.h"

void Facet::Sort() {
	while (left >= mid || left >= right) {
		const size_t temp = left;
		left = mid;
		mid = right;
		right = temp;
	}
}

bool Facet::operator< (const Facet& other) const {
	if (left < other.left) {
		return true;
	}
	if (left == other.left) {
		if (mid < other.mid) {
			return true;
		}
		if (mid == other.mid && right < other.right) {
			return true;
		}
	}
	return false;
}