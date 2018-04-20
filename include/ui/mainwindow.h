#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLineEdit>

#include "bayer_converter.h"

class QEvent;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

private:
    void connectButtons();

    static QList<QString> filesInFolder(const QString& directoryPath);

    void openFileDialog(QLineEdit* destinationLineEdit);

    void updateConversionProgress(int currentNb, int totalNb);

    bool event(QEvent* ev) override;

    void startConversion();

private:
    QString lastPath_{"/Users/hugbed/Pictures/elections_1/left/"};
    std::unique_ptr<BayerConverter> bayerConverter_;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
