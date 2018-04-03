#pragma once

struct Facet {
	Facet(const size_t _left, const size_t _mid, const size_t _right) :
		left(_left),
		mid(_mid),
		right(_right) {}
	size_t left;
	size_t mid;
	size_t right;
	// упорядочивание точек по номерам внутри грани 
	void Sort();
	// оператор лексикографического сравнения двух граней
	bool operator< (const Facet& other) const;
};
