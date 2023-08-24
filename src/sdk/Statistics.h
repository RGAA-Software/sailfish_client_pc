#pragma once

#include <cstdint>
#include <string>
#include <deque>

namespace rgaa
{

	class Statistics {
	public:

		void AppendVideoFrame(uint32_t bytes, uint32_t diff_time);
		uint32_t FormatVideoFrameMB();
		void AppendVideoDecode(uint32_t time);
		std::string FormatStreamingTime();

	public:

		uint32_t render_fps = 0;
		uint32_t video_recv_fps = 0;
		uint32_t video_recv_frames = 0;
		uint32_t video_recv_bytes = 0;
		uint64_t frame_index = 0;
		std::deque<uint32_t> video_recv_diff_times;
		std::deque<uint32_t> video_decode_times;

		std::string video_encode_format;
		uint32_t video_width = 0;
		uint32_t video_height = 0;

		uint64_t streaming_time = 0;

	};

}