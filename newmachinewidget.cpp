#include <QDebug>
#include <QFile>
#include <QDomDocument>

#include "newmachinewidget.h"
#include "ui_newmachinewidget.h"


NewMachineWidget::NewMachineWidget(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::NewMachineWidget)
{
    ui->setupUi(this);

    connect(this, &NewMachineWidget::finished, this,
            &NewMachineWidget::_on_finished);
}

void NewMachineWidget::_on_finished(int result)
{
    qDebug() << "VM Name: "<< ui->vmNameLineEdit->text();
    qDebug() << "Select OS Type: "<< ui->osTypeComboBox->currentText();

    QString xmlString;
    QDomDocument doc;

    QDomElement rootElement = doc.createElement("domain");
    rootElement.setAttribute("type", "kvm");
    doc.appendChild(rootElement);

    QDomElement nameElement = doc.createElement("name");
    nameElement.appendChild(doc.createTextNode("testvm"));
    rootElement.appendChild(nameElement);

    QDomElement osElement = doc.createElement("os");
    QDomElement osSystemElement = doc.createElement("system");
    osSystemElement.appendChild(doc.createTextNode("linux"));
    osElement.appendChild(osSystemElement);
    rootElement.appendChild(osElement);

    QTextStream outStream(&xmlString);
    doc.save(outStream, 4);

    qDebug().noquote() << xmlString;
}

NewMachineWidget::~NewMachineWidget()
{
    delete ui;
}
