#ifndef FRAME_STATS_HH
#define FRAME_STATS_HH

#include "game/headers/utility/circular-queue.hh"

template<std::size_t HISTORY_N>
class FrameStats {
public:
	void addFrameTime(double frame_time);

	double getAVGFrameTime() const;
	double getAVGFPS() const;
private:
	CircularQueue<double, HISTORY_N> frame_time_history_;
};

#include "game/sources/frame-stats.inl"

#endif // FRAME_STATS_HH
