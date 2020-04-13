#ifndef CIRCULAR_QUEUE_HH
#define CIRCULAR_QUEUE_HH

#include <cstdint>
#include <array>

template <typename T, std::size_t N>
class CircularQueue {
public:
	std::size_t getSize() const;
	std::size_t getInsertedCount() const;
	void add(T elem);
	T getLast(int64_t index) const;

private:
	std::array<T, N> arr;
	
	std::size_t size{N};
	std::size_t inserted_count{0};
};

#include "game/sources/utility/circular-queue.inl"

#endif // CIRCULAR_QUEUE_HH
