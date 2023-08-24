#include "LineChart.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>

#include <iostream>

namespace rgaa
{

	LineChart::LineChart(const QString& title, QWidget* parent) : QWidget(parent) {
        title_ = title;
	}

	LineChart::~LineChart() {

	}

	void LineChart::paintEvent(QPaintEvent* event) {
		QPainter painter(this);
		//painter.setRenderHint(QPainter::Antialiasing, true);
		painter.drawLine(0, 0, 0, height());
		painter.drawLine(0, height(), width(), height());

        {
            QPen pen(QColor(0xffffff));
            QFont font;
            font.setPointSize(12);
            QFontMetrics fm(font);
            auto rect = fm.boundingRect(title_);
            //int pixelsWide = fm.width("What's the width of this text?");
            //int pixelsHigh = fm.height();

            painter.setPen(pen);
            painter.setFont(font);
            painter.setRenderHint(QPainter::TextAntialiasing, true);
            painter.drawText((this->width() - rect.width())/2, rect.height()*2/3, title_);
            painter.setRenderHint(QPainter::TextAntialiasing, false);
        }

        painter.drawLine(1, 0, 1, this->height());
        painter.drawLine(0, this->height()-1, this->width(), this->height()-1);

		int segments = std::min(5, height() / 20);
		for (int i = 1; i < segments; i++) {
			int target_height = height() - i * 20;
            QPen pen_line(QColor(0xff, 0xff, 0xff, 0x55));
            pen_line.setWidth(1);
            painter.setPen(pen_line);
			painter.drawLine(0, target_height, width(), target_height);

            QPen pen_font(QColor(0xff, 0xff, 0xff, 0xff));
            pen_font.setWidth(1);
            painter.setPen(pen_font);
            QFont font;
            font.setPointSize(10);
            painter.setFont(font);
            painter.drawText(0, target_height, QString::number(i*20) + "ms");
		}

		int data_size = chart_data.size();
		if (data_size <= 0) {
			return;
		}

        {
            QPen pen(QColor(0x00, 0xff, 0x00, 0xff));
            pen.setWidth(1);
            painter.setPen(pen);
        }

		int gap = width() * 1.0f / data_size + 0.5f;

		QPainterPath path;
		path.moveTo(0, height());
		for (int i = 0; i < data_size; i++) {
			uint32_t value = height() - chart_data[i];
			auto x = (i + 1) * gap;
			auto y = value;
			path.lineTo(x, y); 
		}
		painter.drawPath(path);
	}

	void LineChart::UpdateData(const std::deque<uint32_t>& data) {
		if (chart_data.size() != data.size()) {
			chart_data.resize(data.size());
		}
		for (int i = 0; i < data.size(); i++) {
			chart_data[i] = data[i];
		}
		repaint();
	}

	void LineChart::UpdateData(const std::vector<uint32_t>& data) {
		if (chart_data.size() != data.size()) {
			chart_data.resize(data.size());
		}
		memcpy(chart_data.data(), data.data(), data.size());
		repaint();
	}

}