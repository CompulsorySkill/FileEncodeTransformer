#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTextStream>
#include <QStringConverter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("文件编码转换"));
    using encoding = QStringConverter::Encoding;
    for (int i = encoding::Utf8; i <= encoding::LastEncoding; i++) {
        ui->inputDecoding->addItem(QStringConverter::nameForEncoding(encoding(i)), i);
        ui->inputEncoding->addItem(QStringConverter::nameForEncoding(encoding(i)), i);
    }
    connect(ui->buttonAdd, &QPushButton::clicked, this, &MainWindow::onAddClick);
    connect(ui->buttonRemove, &QPushButton::clicked, this, &MainWindow::onRemoveClick);
    connect(ui->buttonFolder, &QToolButton::clicked, this, &MainWindow::onFolderClick);
    connect(ui->buttonStart, &QCommandLinkButton::clicked, this, &MainWindow::onStartClick);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddClick()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("打开文件"));
    foreach (QString s, files) {
        if (auto res = ui->listWidget->findItems(s, Qt::MatchExactly); res.isEmpty()) {
            ui->listWidget->addItem(s);
        }
    }
}

void MainWindow::onRemoveClick()
{
    auto row = ui->listWidget->currentRow();
    ui->listWidget->takeItem(row);
}

void MainWindow::onFolderClick()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("打开文件夹"));
    if (!dir.isEmpty()) {
        ui->inputFolder->setText(dir);
    }
}

void MainWindow::onStartClick()
{
    using encoding = QStringConverter::Encoding;
    encoding decoder = ui->inputDecoding->currentData().value<encoding>();
    encoding encoder = ui->inputEncoding->currentData().value<encoding>();
    QString dir = ui->inputFolder->text();

    if (ui->listWidget->count() <= 0) {
        QMessageBox::warning(this, tr("转换错误"), tr("请添加要转换的文件"));
        return;
    }
    if (decoder == encoder) {
        QMessageBox::warning(this, tr("转换错误"), tr("请选择与源编码不同的目标编码"));
        return;
    }
    if (dir.isEmpty()) {
        QMessageBox::warning(this, tr("转换错误"), tr("请选择存放转换结果的目标文件夹"));
        return;
    }

    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *item = ui->listWidget->item(i);
        QFile inFile(item->text(), this);
        QFileInfo info(inFile);
        QFile outFile(QString("%1/%2").arg(ui->inputFolder->text(), info.fileName()));
        if (inFile.open(QIODevice::ReadOnly) && outFile.open(QIODevice::WriteOnly)) {
            QTextStream inStream(&inFile);
            inStream.setEncoding(decoder);
            QTextStream outStream(&outFile);
            outStream.setEncoding(encoder);
            outStream << inStream.readAll();
        } else if (!inFile.isOpen()) {
            QMessageBox::critical(this, tr("转换错误"), tr("无法打开文件 %1").arg(inFile.fileName()));
        } else if (!outFile.isOpen()) {
            QMessageBox::critical(this, tr("转换错误"), tr("无法打开文件 %1").arg(outFile.fileName()));
        }
        inFile.close();
        outFile.close();
    }
    ui->listWidget->clear();
    QMessageBox::information(this, tr("转换完成"), tr("已完成所选文件的编码转换"));
}
