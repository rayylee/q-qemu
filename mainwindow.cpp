#include <QDir>
#include <QStandardPaths>
#include <QLabel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpdialog.h"
#include "newmachinewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->layout()->setSpacing(20);

    ui->vmlistWidget->addItem("testvm1");
    ui->vmlistWidget->addItem("testvm2");
    ui->vmlistWidget->addItem("testvm3");

    QString home_path = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation);

    QDir dir_home(home_path);
    m_app_path = QDir::toNativeSeparators(dir_home.absoluteFilePath(".q-qemu"));
    QDir dir_app(m_app_path);
    if(!dir_app.exists()) {
        dir_app.mkpath(m_app_path);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_newMachineWizard()
{
    NewMachineWidget *w = new NewMachineWidget();

    w->setWindowModality(Qt::ApplicationModal);

    w->show();
}

void MainWindow::on_actionAbout_triggered()
{
    HelpDialog *dialog = new HelpDialog();

    dialog->setWindowModality(Qt::ApplicationModal);

    dialog->show();
}


void MainWindow::on_actionNew1_triggered()
{
    _newMachineWizard();
}


void MainWindow::on_actionNew2_triggered()
{
    _newMachineWizard();
}

