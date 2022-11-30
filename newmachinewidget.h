#ifndef NEW_MACHINE_WIDGET_H
#define NEW_MACHINE_WIDGET_H

#include <QWizard>
#include <utility>

namespace Ui {
class NewMachineWidget;
}

class NewMachineWidget : public QWizard
{
    Q_OBJECT

public:
    explicit NewMachineWidget(QWidget *parent = nullptr);
    ~NewMachineWidget() override;

public:
    void set_qemu_binary_path(QString in) { m_qemu_binary_path = std::move(in); }
    void set_app_dir(QString in) { m_app_dir = std::move(in); }
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
    int m_domain_id{};
};

#endif // NEW_MACHINE_WIDGET_H
