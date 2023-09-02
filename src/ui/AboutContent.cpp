//
// Created by RGAA on 2023/8/16.
//

#include "AboutContent.h"
#include "WidgetHelper.h"

#include <QLabel>

namespace rgaa {

    AboutContent::AboutContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(root_layout);
        root_layout->addSpacing(100);
        {
            auto title_layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(title_layout);
            title_layout->addStretch();

            auto title = new QLabel(this);
            title_layout->addWidget(title);
            title->setText(tr("TOGETHER"));
            title->setStyleSheet("font-size: 32pt; font-family: 'Archaic1897'; color:#2e317c;");
            title_layout->addStretch();
            root_layout->addLayout(title_layout);
        }

        root_layout->addSpacing(50);

        {
            auto poem_layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(poem_layout);

            std::stringstream ss;
            ss << "Birds build nest and sleep in" << std::endl;
            ss << "Shining diamonds fall off on my bed" << std::endl;
            ss << "Gave them back to the sprites" << std::endl;
            ss << "I am gifted melliferous toffee today" << std::endl;
            ss << "We will sit at the beach in the future" << std::endl;
            ss << "Wait for free wind to full in our sleeves" << std::endl;

            auto label = new QLabel(this);
            label->setText(ss.str().c_str());
            label->setStyleSheet("font-size: 27pt; font-family: 'Archaic1897'; color:#2e317c;");
            poem_layout->addStretch();
            poem_layout->addWidget(label);
            poem_layout->addStretch();
            root_layout->addLayout(poem_layout);
        }

        root_layout->addStretch();

        {
            auto title_layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(title_layout);
            title_layout->addStretch();

            auto title = new QLabel(this);
            title_layout->addWidget(title);
            title->setText(tr("BEST"));
            title->setStyleSheet("font-size: 23pt; font-family: 'Archaic1897'; color:#2e317c;");
            title_layout->addSpacing(30);
            root_layout->addLayout(title_layout);
            root_layout->addSpacing(30);
        }

        setLayout(root_layout);
    }

    AboutContent::~AboutContent() {

    }

    void AboutContent::OnContentShow() {
        AppContent::OnContentShow();
    }

    void AboutContent::OnContentHide() {
        AppContent::OnContentHide();
    }

}