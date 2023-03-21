#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected slots:
    void onAddClick();
    void onRemoveClick();
    void onFolderClick();
    void onStartClick();

private:
    Ui::MainWindow *ui;
};
