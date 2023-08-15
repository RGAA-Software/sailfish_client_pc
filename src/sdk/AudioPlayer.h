#pragma once

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QIODevice>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QAudioSink>

namespace rgaa
{

	class Data;

	class AudioPlayer : public QObject {
	public:
		
		explicit AudioPlayer(QObject* parent = nullptr);
		~AudioPlayer() override;

		void Init(int samples, int channels);
		void Write(const char* data, int size);
		void Write(std::shared_ptr<Data> data);
		bool IsInit();

	private:

		bool init = false;
		QIODevice* io = nullptr;

        QMediaDevices* devices_ = nullptr;
        QAudioSink* sink_ = nullptr;
	};
}