#pragma once

#include <QWidget>

#include <vector>
#include <deque>
#include <cstdint>

namespace rgaa
{
	class LineChart : public QWidget {
	public:

		LineChart(const QString& title, QWidget* parent = nullptr);
		~LineChart();

		void paintEvent(QPaintEvent* event) override;

		void UpdateData(const std::deque<uint32_t>& data);
		void UpdateData(const std::vector<uint32_t>& data);

	private:
		
		std::vector<uint32_t> chart_data;
        QString title_;

	};
}