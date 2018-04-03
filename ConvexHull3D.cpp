#include <limits>
#include <algorithm>
#include "ConvexHull3D.h"

const ld ConvexHull3D::INF_ = std::numeric_limits<ld>::max();

ConvexHull3D::ConvexHull3D(std::vector<std::valarray<ld>> points, bool need_removal) :
	points_(points) {
	CreateIndices();
	// true, если нужно работать с фигурами, содержащими точки с одинаковыми x 
	// или гранями, параллельными Oz (только для целых координат)
	if (need_removal) {
		RemoveCoincidences();
	}
}

std::vector<Facet> ConvexHull3D::Build() {
	std::vector<Facet> answer = BuildLower();
	// перевернем фигуру и снова найдем нижнюю оболочку
	for (auto& point : points_) {
		point[2] = -point[2];
	}
	std::list<Index*> events = Merge(0, indices_.size());
	for (auto ev : events) {
		answer.push_back(CreateFacet(ev, true));
		ev->Process();
	}
	return answer;
}

std::vector<Facet> ConvexHull3D::BuildLower() {
	// для построения верхней оболочки нужен начальный массив индексов
	const std::vector<Index> backup = indices_;
	std::list<Index*> events = Merge(0, indices_.size());
	std::vector<Facet> answer;
	for (auto ev : events) {
		answer.push_back(CreateFacet(ev, false));
		ev->Process();
	}
	indices_ = backup;
	return answer;
}

// начальная инициализация и сортировка массива индексов
void ConvexHull3D::CreateIndices() {
	indices_.clear();
	for (size_t i = 0; i < points_.size(); ++i) {
		indices_.emplace_back(i);
	}
	std::sort(indices_.begin(), indices_.end(), [this](const Index first, const Index second) {
		return (points_[first.point][0] < points_[second.point][0]);
	});
}

// сдвиг всех точек на малую величину, это не влияет на оболочку,
// но позволяет алгоритму Чана корректно их обрабатывать
void ConvexHull3D::RemoveCoincidences() {
	const ld epsilon = 0.01 / points_.size();
	for (size_t i = 1; i < indices_.size(); ++i) {
		points_[indices_[i].point][0] += epsilon * i;
		points_[indices_[i].point][1] += epsilon * 2 * i;
	}
}

// слияние двух оболочек
std::list<Index*> ConvexHull3D::Merge(const size_t left, const size_t right) {
	if (right - left == 1) {
		std::list<Index*> new_events;
		return new_events;
	}
	const size_t mid = (left + right) / 2;
	std::list<Index*> left_events = Merge(left, mid);
	std::list<Index*> right_events = Merge(mid, right);

	Index* l = &indices_[mid - 1];
	Index* r = &indices_[mid];

	// поиск моста
	while (true) {
		if (Turn(r, l, Next(r)) < 0) {
			r = Next(r);
		}
		else if (Turn(l, Prev(l), r) < 0) {
			l = Prev(l);
		}
		else {
			break;
		}
	}
	std::list<Index*> new_events = TrackBridges(l, r, left_events, right_events, mid);
	return new_events;
}

// отслеживание изменений моста с течением времени
std::list<Index*> ConvexHull3D::TrackBridges(Index*& l, Index*& r, std::list<Index*>& left_events,
	std::list<Index*>& right_events, const size_t mid) {

	ld old_time = -INF_;
	std::list<Index*> new_events;

	while (true) {
		std::vector<ld> time(6, INF_);
		// события в сливаемых оболочках 
		if (!left_events.empty()) {
			time[0] = Time(left_events.front(), Prev(left_events.front()), Next(left_events.front()));
		}
		if (!right_events.empty()) {
			time[1] = Time(right_events.front(), Prev(right_events.front()), Next(right_events.front()));
		}
		// времена, когда тройка вершин меняет ориентацию
		time[2] = Time(Next(l), l, r);
		time[3] = Time(l, Prev(l), r);
		time[4] = Time(Prev(r), l, r);
		time[5] = Time(r, l, Next(r));
		ld new_time = INF_;
		int case_num = -1;
		for (size_t i = 0; i < time.size(); ++i) {
			if (time[i] > old_time && time[i] < new_time) {
				case_num = static_cast<size_t>(i);
				new_time = time[i];
			}
		}
		if (new_time >= INF_) {
			break;
		}
		switch (case_num) {
			// если точка события лежит в результирующей оболочке, добавляем событие туда
		case 0: {
			if (points_[left_events.front()->point][0] < points_[l->point][0]) {
				new_events.push_back(left_events.front());
			}
			left_events.front()->Process();
			left_events.pop_front();
			break;
		}
		case 1: {
			if (points_[right_events.front()->point][0] > points_[r->point][0]) {
				new_events.push_back(right_events.front());
			}
			right_events.front()->Process();
			right_events.pop_front();
			break;
		}
				// вставка или удаление вершины из результирующей грани при смене ориентации
		case 2: {
			l = Next(l);
			new_events.push_back(l);
			break;
		}
		case 3: {
			new_events.push_back(l);
			l = Prev(l);
			break;
		}
		case 4: {
			r = Prev(r);
			new_events.push_back(r);
			break;
		}
		case 5: {
			new_events.push_back(r);
			r = Next(r);
			break;
		}
		default:
			break;
		}
		old_time = new_time;
	}
	l->next = r; // значения непусты по построению алгоритма
	r->prev = l;
	ReverseTime(l, r, new_events, mid);

	return new_events;
}

// обработка событий в обратном порядке, чтобы восстановить указатели
void ConvexHull3D::ReverseTime(Index*& l, Index*& r, std::list<Index*>& events, const size_t mid) {
	auto it = events.end();
	while (it != events.begin()) {
		--it;
		if ((*it) == nullptr) {
			continue;
		}
		// если точка лежит на оболочке, обрабатываем событие и обновляем мост
		if (points_[(*it)->point][0] <= points_[l->point][0] ||
			points_[(*it)->point][0] >= points_[r->point][0]) {
			(*it)->Process();
			if ((*it) == l) {
				l = Prev(l);
			}
			if ((*it) == r) {
				r = Next(r);
			}
			continue;
		}
		// если нет - вставляем ее в оболочку и обновляем мост
		l->next = r->prev = (*it);
		(*it)->prev = l;
		(*it)->next = r;
		if (points_[(*it)->point][0] <= points_[indices_[mid - 1].point][0]) {
			l = (*it);
		}
		else {
			r = (*it);
		}
	}
}

// проверка ориентации точек
ld ConvexHull3D::Turn(const Index* left, const Index* mid, const Index* right) {
	if (left == nullptr || mid == nullptr || right == nullptr) {
		return INF_;
	}
	return VectorProduct(points_[left->point] - points_[mid->point],
		points_[right->point] - points_[mid->point]);
}

// расчет времени, когда ориентация изменится (решение уравнения отн-но t)
ld ConvexHull3D::Time(const Index* left, const Index* mid, const Index* right) {
	if (left == nullptr || mid == nullptr || right == nullptr) {
		return INF_;
	}
	ld turn = Turn(left, mid, right);
	ld numerator = (points_[left->point][0] - points_[mid->point][0]) *
		(points_[right->point][2] - points_[mid->point][2]) -
		(points_[right->point][0] - points_[mid->point][0]) *
		(points_[left->point][2] - points_[mid->point][2]);

	if (turn == 0) {
		return (numerator >= 0 ? INF_ : -INF_);
	}
	else {
		return numerator / turn;
	}
}

// создание ориентированной грани
Facet ConvexHull3D::CreateFacet(const Index* mid, const bool is_upper) {
	ld turn = Turn(mid->prev, mid, mid->next);
	if (is_upper) {
		turn = -turn;
	}
	Facet facet(mid->prev->point, mid->point, mid->next->point);
	if (turn < 0) {
		std::swap(facet.mid, facet.right);
	}
	facet.Sort();
	return facet;
}