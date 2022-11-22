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


void MainWindow::on_actionNew_1_triggered()
{
    _newMachineWizard();
}


void MainWindow::on_actionNew_2_triggered()
{
    _newMachineWizard();
}

