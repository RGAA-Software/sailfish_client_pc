#pragma once

#include <cstdint>
#include <string>
#include <deque>
#include <map>
#include <sstream>

namespace rgaa
{

	class Statistics {
	public:

		void AppendVideoFrame(uint32_t bytes, uint32_t diff_time);
		uint32_t FormatVideoFrameMB();
		void AppendVideoDecode(uint32_t time);
		std::string FormatStreamingTime();
        void AppendEncodeTime(int dup_idx, uint32_t time);
        void AppendNetworkTime(uint32_t time);
        void AppendAudioBytes(uint32_t bytes);
        std::string FormatAudioFrameMB();

	public:

		uint32_t render_fps = 0;
		uint32_t video_recv_fps = 0;
		uint32_t video_recv_frames = 0;
		uint32_t video_recv_bytes = 0;
		uint64_t frame_index = 0;
		std::deque<uint32_t> video_recv_diff_times;
		std::deque<uint32_t> video_decode_times;
        std::map<int, std::deque<uint32_t>> encode_times_;
        std::deque<uint32_t> network_times_;

		std::string video_encode_format;
		uint32_t video_width = 0;
		uint32_t video_height = 0;

		uint64_t streaming_time = 0;

        std::string audio_format_{"OPUS"};
        uint32_t audio_bytes_ = 0;
        uint32_t audio_channel_;
        uint32_t audio_samples_;
        uint32_t audio_bits_;

    private:
        uint32_t video_recv_fps_count_ = 0;
        uint64_t last_recv_video_fps_time = 0;
	};

}