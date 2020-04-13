#include "game/headers/frame-stats.hh"

#include <exception>
#include <limits>

template<std::size_t HISTORY_N>
void FrameStats<HISTORY_N>::addFrameTime(double frame_time) {
	frame_time_history_.add(frame_time);
}

template<std::size_t HISTORY_N>
double FrameStats<HISTORY_N>::getAVGFrameTime() const {
	const std::size_t records_n{frame_time_history_.getInsertedCount()};
	if (records_n < 1) {
		return std::numeric_limits<double>::max();
	}
	double sum{0.0};
	for (std::size_t i{0}; i < records_n; ++i) {
		sum += frame_time_history_.getLast(i);
	}
	return sum / records_n;
}

template<std::size_t HISTORY_N>
double FrameStats<HISTORY_N>::getAVGFPS() const {
	const double avg_frame_time{getAVGFrameTime()};
	return 1.0 / avg_frame_time;
}
