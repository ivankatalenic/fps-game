#include "game/headers/utility/circular-queue.hh"

#include <exception>

template<typename T, std::size_t N>
std::size_t CircularQueue<T, N>::getSize() const {
	return size;
}

template<typename T, std::size_t N>
std::size_t CircularQueue<T, N>::getInsertedCount() const {
	return inserted_count > size ? size : inserted_count;
}

template<typename T, std::size_t N>
void CircularQueue<T, N>::add(T elem) {
	arr[inserted_count % size] = elem;
	++inserted_count;
}

template<typename T, std::size_t N>
T CircularQueue<T, N>::getLast(int64_t index) const {
	if (index < 0) {
		throw std::domain_error("negative index");
	}
	if (inserted_count < 1) {
		throw std::out_of_range("no more elements");
	}
	if (index + 1 > static_cast<int64_t>(inserted_count)) {
		throw std::out_of_range("no more elements");
	}
	if (index + 1 > static_cast<int64_t>(size)) {
		throw std::length_error("index exceeds the queue's size");
	}

	return arr[(inserted_count - 1 - index) % size];
}
