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

public:
    void set_qemu_binary_path(QString in) { m_qemu_binary_path = in; }
    void set_app_dir(QString in) { m_app_dir = in; }
    void set_domain_id(int in) { m_domain_id = in; }

signals:
    void send_new_machine(QString);

private slots:    
    void _on_finished(int result);

    void on_choseDisk2Button_clicked();

    void on_choseDisk1Button_clicked();

    void on_choseCdromButton_clicked();

private:
    Ui::NewMachineWidget *ui;

    QString m_qemu_binary_path;
    QString m_app_dir;
    int m_domain_id;
};

#endif // NEWMACHINEWIDGET_H
