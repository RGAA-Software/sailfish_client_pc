#pragma once

#include <QWidget>
#include <QTimer>
#include <memory>
#include "sdk/RawImage.h"
#include "VideoWidget.h"
#include "VideoWidgetEvent.h"

#include <SDL2/SDL.h>
#include <QVBoxLayout>

namespace rgaa {
	
	class Thread;
	class Context; 
	class Statistics;

	class SDLVideoWidget : public VideoWidget, public VideoWidgetEvent {
	public:
		SDLVideoWidget(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, int dup_idx, RawImageFormat format, QWidget* parent = nullptr);
		~SDLVideoWidget();

		void Init(int frame_width, int frame_height);
        void Exit() override;

		void RefreshI420Image(const std::shared_ptr<RawImage>& image) override;
		void RefreshI420Buffer(const char* y_buf, int y_buf_size, const char* u_buf, int u_buf_size, const char* v_buf, int v_buf_size, int width, int height) override;

    private:

        void Update();

	protected:
		void mouseMoveEvent(QMouseEvent*) override;
		void mousePressEvent(QMouseEvent*) override;
		void mouseReleaseEvent(QMouseEvent*) override;
		void resizeEvent(QResizeEvent* event) override;
		void mouseDoubleClickEvent(QMouseEvent*) override;
		void wheelEvent(QWheelEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;

		void closeEvent(QCloseEvent* event) override;

//        QPaintEngine * paintEngine() const override;

	private:
		
		std::shared_ptr<Context> context = nullptr;
		int frame_width_ = 0;
		int frame_height_ = 0;
		RawImageFormat format;

		SDL_Window* screen = nullptr;
		SDL_Renderer* sdlRenderer = nullptr;
		SDL_Texture* sdlTexture = nullptr;
		SDL_Rect sdlRect;
		
		bool init = false;

		int render_fps = 0;
		uint64_t last_update_fps_time = 0;

//		Statistics* statistics = nullptr;

        std::shared_ptr<RawImage> last_refresh_image_ = nullptr;
        uint64_t last_refresh_image_time_ = 0;

        QTimer* timer_ = nullptr;

	};


    class SDLWidgetWrapper : public QWidget {
    public:

        SDLWidgetWrapper(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk, int dup_idx, RawImageFormat format, QWidget* parent) {
            this->context_ = ctx;
            QString title = "Sailfish client window [ " + QString::number(dup_idx+1) + " ]";
            setWindowTitle(title);

            auto layout = new QVBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            widget_ = new SDLVideoWidget(ctx, sdk, dup_idx, format, this);
            layout->addWidget(widget_);
            setLayout(layout);

            setAcceptDrops(true);
        }

    protected:

        void closeEvent(QCloseEvent *event) override;
        void dragEnterEvent(QDragEnterEvent *event) override;
        void dragLeaveEvent(QDragLeaveEvent *event) override;
        void dropEvent(QDropEvent *event) override;

    public:

        SDLVideoWidget* widget_ = nullptr;
        std::shared_ptr<Context> context_ = nullptr;

    };

}