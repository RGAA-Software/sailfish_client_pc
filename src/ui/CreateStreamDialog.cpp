//
// Created by RGAA on 2023-08-18.
//

#include "CreateStreamDialog.h"

#include "AppMessage.h"
#include "Context.h"

#include <QValidator>

#include "MessageDialog.h"

namespace rgaa {

    CreateStreamDialog::CreateStreamDialog(const std::shared_ptr<Context>& ctx, QWidget* parent) : QDialog(parent) {
        context_ = ctx;
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

        // 0. name
        {
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            layout->addStretch();

            auto label = new QLabel(this);
            label->setFixedSize(label_size);
            label->setText(tr("Name"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto edit = new QLineEdit(this);
            ed_name_ = edit;
            edit->setFixedSize(edit_size);
            layout->addWidget(edit);
            layout->addStretch();

            root_layout->addSpacing(40);
            root_layout->addLayout(layout);

        }

        // 1. host
        {
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            layout->addStretch();

            auto label = new QLabel(this);
            label->setFixedSize(label_size);
            label->setText(tr("Host *"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto edit = new QLineEdit(this);
            ed_host_ = edit;
            edit->setFixedSize(edit_size);
            layout->addWidget(edit);
            layout->addStretch();

            root_layout->addSpacing(10);
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
            label->setText(tr("Port *"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto edit = new QLineEdit(this);
            auto validator = new QIntValidator(this);
            edit->setValidator(validator);
            ed_port_ = edit;
            edit->setFixedSize(edit_size);
            layout->addWidget(edit);
            layout->addStretch();

            root_layout->addSpacing(10);
            root_layout->addLayout(layout);
        }

        // 3. bitrate
        {
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            layout->addStretch();

            auto label = new QLabel(this);
            label->setFixedSize(label_size);
            label->setText(tr("Bitrate(M)"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto edit = new QLineEdit(this);
            auto validator = new QIntValidator(this);
            edit->setValidator(validator);
            ed_bitrate_ = edit;
            ed_bitrate_->setText("5");
            edit->setFixedSize(edit_size);
            layout->addWidget(edit);
            layout->addStretch();

            root_layout->addSpacing(10);
            root_layout->addLayout(layout);
        }

        {
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            layout->addStretch();

            auto label = new QLabel(this);
            label->setFixedSize(label_size);
            label->setText(tr("FPS"));
            label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            layout->addWidget(label);

            auto cb = new QComboBox(this);
            cb_fps_ = cb;
            cb->addItem("15");
            cb->addItem("30");
            cb->addItem("60");
            cb->setFixedSize(edit_size);
            layout->addWidget(cb);
            layout->addStretch();

            root_layout->addSpacing(10);
            root_layout->addLayout(layout);
        }

        // sure or cancel
        {
            auto btn_size = QSize(100, 35);
            auto layout = new QHBoxLayout();
            layout->setSpacing(0);
            layout->setContentsMargins(0,0,0,0);
            auto btn_cancel = new QPushButton(tr("Cancel"));
            connect(btn_cancel, &QPushButton::clicked, this, [=, this]() {
                done(0);
            });
            btn_cancel->setFixedSize(btn_size);
            layout->addStretch();
            layout->addWidget(btn_cancel);
            layout->addStretch();

            auto btn_sure = new QPushButton(tr("Sure"));
            connect(btn_sure, &QPushButton::clicked, this, [=, this] () {
                if (!GenStream()) {
                    return;
                }
                done(0);
            });

            layout->addWidget(btn_sure);
            btn_sure->setFixedSize(btn_size);
            layout->addStretch();

            root_layout->addStretch();
            root_layout->addLayout(layout);
        }
        root_layout->addSpacing(30);

        setLayout(root_layout);
    }

    bool CreateStreamDialog::GenStream() {
        auto host = ed_host_->text().toStdString();
        auto port = std::atoi(ed_host_->text().toStdString().c_str());
        auto name = ed_name_->text().toStdString();
        auto bitrate = std::atoi(ed_bitrate_->text().toStdString().c_str());

        if (host.empty() || port == 0) {
            auto dialog = MessageDialog::Make(context_, tr("Please input necessary information !"));
            dialog->exec();
            return false;
        }

        StreamItem item;
        item.stream_name = name;
        item.stream_host = host;
        item.stream_port = port;
        item.encode_bps = bitrate;

        auto msg = StreamItemAdded::Make(item);
        context_->SendAppMessage(msg);
        return true;
    }

}