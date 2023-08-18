//
// Created by RGAA on 2023-08-18.
//

#include "CreateStreamDialog.h"

namespace rgaa {

    CreateStreamDialog::CreateStreamDialog(QWidget* parent) : QDialog(parent) {
        setFixedSize(500, 400);
        CreateLayout();
    }

    CreateStreamDialog::~CreateStreamDialog() {

    }

    void CreateStreamDialog::CreateLayout() {
        auto root_layout = new QVBoxLayout();
        root_layout->setSpacing(0);
        root_layout->setContentsMargins(60,0,60, 0);

        auto label_size = QSize(100, 35);
        auto edit_size = QSize(200, 35);

        // 1. host
        {
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            layout->addStretch();

            auto label = new QLabel(this);
            label->setFixedSize(label_size);
            label->setText(tr("Host"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto edit = new QLineEdit(this);
            edit->setFixedSize(edit_size);
            layout->addWidget(edit);
            layout->addStretch();

            root_layout->addSpacing(40);
            root_layout->addLayout(layout);
        }

        // 2. port
        {
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            layout->addStretch();

            auto label = new QLabel(this);
            label->setFixedSize(label_size);
            label->setText(tr("Port"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto edit = new QLineEdit(this);
            edit->setFixedSize(edit_size);
            layout->addWidget(edit);
            layout->addStretch();

            root_layout->addSpacing(20);
            root_layout->addLayout(layout);
        }

        // 3. local or relay
        {
//            auto layout = new QHBoxLayout();
//            layout->setSpacing(0);
//            layout->setContentsMargins(0,0,0,0);
//            auto group = new QGroupBox(this);
//
//            auto radio_local = new QRadioButton(tr("Local"));
//
//            group->setLayout(layout);
//            root_layout->addSpacing(20);
//            root_layout->addWidget(group);
        }
        // 4. sure or cancel
        {
            auto btn_size = QSize(100, 35);
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            auto btn_cancel = new QPushButton(tr("Cancel"));
            btn_cancel->setFixedSize(btn_size);
            layout->addStretch();
            layout->addWidget(btn_cancel);
            layout->addStretch();
            auto btn_sure = new QPushButton(tr("Sure"));
            layout->addWidget(btn_sure);
            btn_sure->setFixedSize(btn_size);
            layout->addStretch();

            root_layout->addSpacing(170);
            root_layout->addLayout(layout);
        }
        root_layout->addStretch();

        setLayout(root_layout);
    }

}