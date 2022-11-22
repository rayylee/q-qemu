#ifndef MAINSETTING_H
#define MAINSETTING_H

#include <QDialog>

namespace Ui {
class MainSetting;
}

class MainSetting : public QDialog
{
    Q_OBJECT

public:
    explicit MainSetting(QWidget *parent = nullptr);
    ~MainSetting();

    void set_qemu_dir(QString);
    QString qemu_dir();

signals:
    void send_qemu_dir(QString);

private slots:
    void on_confirmButtonBox_accepted();

    void on_choseQemuDirButton_clicked();

private:
    Ui::MainSetting *ui;    
};

#endif // MAINSETTING_H
