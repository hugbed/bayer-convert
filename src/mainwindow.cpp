#include "ui/mainwindow.h"
#include "ui_mainwindow.h"

#include "bayer_converter.h"

#include <QAbstractButton>
#include <QEvent>
#include <QFileDialog>
#include <QLineEdit>

#include <QDebug>

#include <memory>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->progressBar->setVisible(false);
  ui->completeLabel->setVisible(false);
  connectButtons();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::connectButtons() {
  auto handleOpenInput = [this] { openFileDialog(ui->inputDirectoryLineEdit); };
  auto handleOpenOutput = [this] { openFileDialog(ui->outputDirectoryLineEdit); };

  connect(ui->inputDirectoryButton, &QAbstractButton::clicked, handleOpenInput);
  connect(ui->outputDirectoryButton, &QAbstractButton::clicked, handleOpenOutput);
  connect(ui->startConversionButton, &QAbstractButton::clicked, this, &MainWindow::startConversion);
}

void MainWindow::startConversion() {
  // todo: make sure that path is valid
  ui->completeLabel->setVisible(false);
  ui->progressBar->setVisible(true);
  ui->startConversionButton->setVisible(false);
  QList<QString> inputFiles = filesInFolder(ui->inputDirectoryLineEdit->text());
  ui->progressBar->setMaximum(inputFiles.size());

  int width = ui->widthSpinBox->value();
  int height = ui->heightSpinBox->value();

  bayerConverter_ = std::make_unique<BayerConverter>(
    inputFiles,
    ui->outputDirectoryLineEdit->text(),
    RawImageInfo{width, height, width*height*sizeof(uint16_t)}
  );

  connect(bayerConverter_.get(), &BayerConverter::conversionProgressed,
          this, &MainWindow::updateConversionProgress,
          Qt::QueuedConnection);

  bayerConverter_->start();
}

// static
QList<QString> MainWindow::filesInFolder(const QString& directoryPath) {
  QList<QString> fileNames;
  for (QString entry : QDir(directoryPath).entryList(QDir::Files)) {
      fileNames.push_back(directoryPath + "/" + entry);
  }
  return fileNames;
}

void MainWindow::openFileDialog(QLineEdit* destinationLineEdit) {
    QFileDialog fileDialog;
    QString dir = fileDialog.getExistingDirectory(
      this,
      tr("Open Directory"),
      lastPath_ + "/..",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    lastPath_ = dir;
    destinationLineEdit->setText(dir);
}

void MainWindow::updateConversionProgress(int currentNb, int totalNb) {
    ui->progressBar->setValue(currentNb);
    if (currentNb == totalNb) {
        ui->progressBar->setVisible(false);
        ui->completeLabel->setVisible(true);
        ui->startConversionButton->setVisible(true);
    }
}

bool MainWindow::event(QEvent *ev) {
    if(ev->type() == QEvent::LayoutRequest) {
        setFixedSize(sizeHint());
    }
    return QMainWindow::event(ev);
}
