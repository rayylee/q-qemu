#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
    void _newMachineWizard();

private slots:
    void on_actionAbout_triggered();

    void on_actionNew1_triggered();

    void on_actionNew2_triggered();

private:
    Ui::MainWindow *ui;

    QString m_app_path;
};
#endif // MAINWINDOW_H
