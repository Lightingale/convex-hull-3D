#pragma once

struct Index {
	Index(size_t _point);
	size_t point;
	Index* prev;
	Index* next;
	// обработка события точки
	void Process();
};

// безопасное получение потомка и предка (работает с nullptr)
Index* Next(Index*& index);
Index* Prev(Index*& index);