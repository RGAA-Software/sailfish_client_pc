#include "Statistics.h"

#include <chrono>
#include <iomanip>
#include <iostream>

constexpr auto kMaxDiffTimes = 180;
constexpr auto kMaxDecodeTimes = 180;

namespace rgaa
{

	void Statistics::AppendVideoFrame(uint32_t bytes, uint32_t diff_time) {
		video_recv_frames += 1;
		video_recv_bytes += bytes;
		video_recv_diff_times.push_back(diff_time);
		if (video_recv_diff_times.size() > kMaxDiffTimes) {
			video_recv_diff_times.pop_front();
		}
	}

	uint32_t Statistics::FormatVideoFrameMB() {
		return video_recv_bytes / 1024 / 1024;
	}

	void Statistics::AppendVideoDecode(uint32_t time) {
		video_decode_times.push_back(time);
		if (video_decode_times.size() > kMaxDecodeTimes) {
			video_decode_times.pop_front();
		}
	}

	std::string Statistics::FormatStreamingTime() {
		auto hours = std::chrono::hours(streaming_time / 3600);
		auto minutes = std::chrono::minutes((streaming_time % 3600) / 60);
		auto secs = std::chrono::seconds(streaming_time % 60);

		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(2) << hours.count() << ":"
			<< std::setfill('0') << std::setw(2) << minutes.count() << ":"
			<< std::setfill('0') << std::setw(2) << secs.count();

		return oss.str();
	}

}