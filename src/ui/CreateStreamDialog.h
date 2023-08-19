//
// Created by RGAA on 2023-08-18.
//

#ifndef SAILFISH_CLIENT_PC_CREATESTREAMDIALOG_H
#define SAILFISH_CLIENT_PC_CREATESTREAMDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QComboBox>

namespace rgaa {

    class Context;

    class CreateStreamDialog : public QDialog {
    public:

        explicit CreateStreamDialog(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~CreateStreamDialog();

    private:
        void CreateLayout();
        bool GenStream();

    private:

        std::shared_ptr<Context> context_ = nullptr;

        QLineEdit* ed_name_ = nullptr;
        QLineEdit* ed_host_ = nullptr;
        QLineEdit* ed_port_ = nullptr;
        QLineEdit* ed_bitrate_ = nullptr;
        QComboBox* cb_fps_ = nullptr;

    };

}

#endif //SAILFISH_CLIENT_PC_CREATESTREAMDIALOG_H
