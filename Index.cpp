#include "Index.h"

Index::Index(size_t _point) :
	point(_point),
	prev(nullptr),
	next(nullptr) {}

Index* Next(Index*& index) {
	if (index != nullptr) {
		return index->next;
	}
	else {
		return nullptr;
	}
}

Index* Prev(Index*& index) {
	if (index != nullptr) {
		return index->prev;
	}
	else {
		return nullptr;
	}
}

void Index::Process() {
	if (this == nullptr) {
		return;
	}
	if (Next(prev) != this) {  // вставка
		if (prev != nullptr) {
			prev->next = this;
		}
		if (next != nullptr) {
			next->prev = this;
		}
		return;
	}
	// удаление
	if (prev != nullptr) {
		prev->next = next;
	}
	if (next != nullptr) {
		next->prev = prev;
	}
}