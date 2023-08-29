#include "Statistics.h"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "rgaa_common/RTime.h"
#include "rgaa_common/RLog.h"

constexpr auto kMaxDiffTimes = 120;
constexpr auto kMaxDecodeTimes = 120;
constexpr auto kMaxEncodeTimes = 120;
constexpr auto kMaxNetworkTimes = 120;

namespace rgaa
{

	void Statistics::AppendVideoFrame(uint32_t bytes, uint32_t diff_time) {
		video_recv_frames += 1;
		video_recv_bytes += bytes;
		video_recv_diff_times.push_back(diff_time);
		if (video_recv_diff_times.size() > kMaxDiffTimes) {
			video_recv_diff_times.pop_front();
		}

        auto current_time = GetCurrentTimestamp();
        video_recv_fps_count_++;
        if (last_recv_video_fps_time == 0) {
            last_recv_video_fps_time = current_time;
        }
        auto fps_diff = current_time - last_recv_video_fps_time;
        if (fps_diff >= 1000) {
            this->video_recv_fps = video_recv_fps_count_;
            last_recv_video_fps_time = current_time;
            video_recv_fps_count_ = 0;
            //LOGI("//recv fps : {}", video_recv_fps);
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

    void Statistics::AppendEncodeTime(int dup_idx, uint32_t time) {
        if (encode_times_.find(dup_idx) == encode_times_.end()) {
            std::deque<uint32_t> times;
            times.push_back(time);
            encode_times_.insert({dup_idx, times});
        }
        else {
            auto& times = encode_times_[dup_idx];
            times.push_back(time);
            if (times.size() > kMaxEncodeTimes) {
                times.pop_front();
            }
        }
    }

    void Statistics::AppendNetworkTime(uint32_t time) {
        network_times_.push_back(time);
        if (network_times_.size() > kMaxNetworkTimes) {
            network_times_.pop_front();
        }
    }

}