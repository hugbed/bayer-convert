#include "ui/mainwindow.h"
#include "ui_mainwindow.h"

#include <QAbstractButton>
#include <QFileDialog>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    connectButtons();
}

void MainWindow::connectButtons() {
    auto handleOpen = [this] (QLineEdit* lineEdit) {
        QFileDialog fileDialog;
        QString dir = fileDialog.getExistingDirectory(
            this,
            tr("Open Directory"),
            "/home",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
        ui->inputDirectoryLineEdit->setText(dir);
    };

    auto handleOpenInput = [this, handleOpen] {
        handleOpen(ui->inputDirectoryLineEdit);
    };

    auto handleOpenOutput = [this, handleOpen] {
        handleOpen(ui->outputDirectoryLineEdit);
    };

    connect(ui->inputDirectoryButton, &QAbstractButton::clicked, handleOpenInput);
    connect(ui->outputDirectoryButton, &QAbstractButton::clicked, handleOpenOutput);
}

MainWindow::~MainWindow()
{
    delete ui;
}
