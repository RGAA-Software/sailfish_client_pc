#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "messages.pb.h"
#include <memory>

namespace rgaa
{

	class RawImage;
    class SailfishSDK;

	class VideoWidget : public QWidget {
	public:
		
		VideoWidget(const std::shared_ptr<SailfishSDK>& sdk, int dup_idx, QWidget* parent = nullptr);
		virtual ~VideoWidget();

        virtual void Exit() {}
        int GetDupIdx();

	protected:

		virtual void RefreshNV12Image(const std::shared_ptr<RawImage>& image);
		virtual void RefreshNV12Buffer(const char* y_buf, int y_buf_size, const char* uv_buf, int uv_buf_size, int width, int height);
		virtual void RefreshRGBImage(const std::shared_ptr<RawImage>& image);
		virtual void RefreshRGBBuffer(const char* buf, int width, int height, int channel);
		virtual void RefreshI420Image(const std::shared_ptr<RawImage>& image);
		virtual void RefreshI420Buffer(const char* y_buf, int y_buf_size, const char* u_buf, int u_buf_size, const char* v_buf, int v_buf_size, int width, int height);

    protected:

        int dup_idx_ = -1;

        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

	};

}