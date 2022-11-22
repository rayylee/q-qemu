#ifndef NEWMACHINEWIDGET_H
#define NEWMACHINEWIDGET_H

#include <QWizard>

namespace Ui {
class NewMachineWidget;
}

class NewMachineWidget : public QWizard
{
    Q_OBJECT

public:
    explicit NewMachineWidget(QWidget *parent = nullptr);
    ~NewMachineWidget();    

private slots:
    void _on_finished(int result);

    void on_choseDisk2Button_clicked();

    void on_choseDisk1Button_clicked();

    void on_choseCdromButton_clicked();

private:
    Ui::NewMachineWidget *ui;
};

#endif // NEWMACHINEWIDGET_H
