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

namespace rgaa {

    class CreateStreamDialog : public QDialog {
    public:

        explicit CreateStreamDialog(QWidget* parent = nullptr);
        ~CreateStreamDialog();

    private:
        void CreateLayout();

    };

}

#endif //SAILFISH_CLIENT_PC_CREATESTREAMDIALOG_H
