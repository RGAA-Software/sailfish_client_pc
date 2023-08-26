#pragma once

#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLWindow>
#include <QResizeEvent>
#include <mutex>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sdk/RawImage.h"
#include "messages.pb.h"
#include "VideoWidgetEvent.h"

namespace rgaa {

	class Sprite;
	class RawImage;
	class Director;
	class Context;
	class ShaderProgram;
	class Statistics;
    class SailfishSDK;

	class OpenGLVideoWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core, public VideoWidgetEvent {
	public:

		explicit OpenGLVideoWidget(std::shared_ptr<Context> ctx, const std::shared_ptr<SailfishSDK>& sdk, RawImageFormat format, QWidget* parent = nullptr);
		~OpenGLVideoWidget() override;

		void RefreshRGBImage(const std::shared_ptr<RawImage>& image);
		void RefreshRGBBuffer(const char* buf, int width, int height, int channel);
		void RefreshI420Image(const std::shared_ptr<RawImage>& image);
		void RefreshI420Buffer(const char* y, int y_size, const char* u, int u_size, const char* v, int v_size, int width, int height);

	protected:
		void resizeEvent(QResizeEvent* event) override;
		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int width, int height) override;

		void mouseMoveEvent(QMouseEvent*) override;
		void mousePressEvent(QMouseEvent*) override;
		void mouseReleaseEvent(QMouseEvent*) override;
		void mouseDoubleClickEvent(QMouseEvent*) override;
		void wheelEvent(QWheelEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;

	private:
		void InitRGBATexture();
		void InitI420Texture();

	private:

		std::shared_ptr<Context> context;

		glm::mat4 model;
		std::shared_ptr<ShaderProgram> shader_program = nullptr;
		GLuint			vao{ 0 };
		GLuint			vbo{ 0 };
		GLuint			ibo{ 0 };

		char* y_buffer = nullptr;
		char* uv_buffer = nullptr;
		GLuint			y_texture_id = 0;
		GLuint			uv_texture_id = 0;
		bool		    need_create_texture = true;
		int				tex_width = 0;
		int				tex_height = 0;
		int				tex_channel;
		char* rgb_buffer = nullptr;
		GLuint			rgb_texture_id = 0;

		// I420
		char* u_buffer = nullptr;
		char* v_buffer = nullptr;
		GLuint			u_texture_id = 0;
		GLuint			v_texture_id = 0;

		std::mutex		buf_mtx;

		RawImageFormat  raw_image_format;

		std::shared_ptr<Director> director = nullptr;
		std::shared_ptr<Sprite> cursor = nullptr;

		int render_fps = 0;
		uint64_t last_update_fps_time = 0;

		Statistics* statistics = nullptr;
	};

}