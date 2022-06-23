#include "dialoghelpdbwrite.h"
#include "ui_dialoghelpdbwrite.h"

DialogHelpDBWrite::DialogHelpDBWrite(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHelpDBWrite)
{
    ui->setupUi(this);

    CreateQListFromUILabels();
}

DialogHelpDBWrite::~DialogHelpDBWrite()
{
    delete ui;
}

void DialogHelpDBWrite::CreateQListFromUILabels()
{
    labelsBit.append(ui->labelBit0);
    labelsBit.append(ui->labelBit1);
    labelsBit.append(ui->labelBit2);
    labelsBit.append(ui->labelBit3);
    labelsBit.append(ui->labelBit4);
    labelsBit.append(ui->labelBit5);
    labelsBit.append(ui->labelBit6);
    labelsBit.append(ui->labelBit7);

    labelsFloat.append(ui->labelFloat0);
    labelsFloat.append(ui->labelFloat1);
    labelsFloat.append(ui->labelFloat2);
    labelsFloat.append(ui->labelFloat3);
    labelsFloat.append(ui->labelFloat4);
    labelsFloat.append(ui->labelFloat5);
    labelsFloat.append(ui->labelFloat6);
    labelsFloat.append(ui->labelFloat7);
    labelsFloat.append(ui->labelFloat8);
    labelsFloat.append(ui->labelFloat9);
}

void DialogHelpDBWrite::on_pushButtonCloseDialog_clicked()
{
    this->close();
}

void DialogHelpDBWrite::UpdateDispValues(bool boolToPlc[],float floatToPlc[])
{
    for(int i=0;i<8;i++)
    {
        this->boolToPlc[i]=boolToPlc[i];
    }

    for(int i=0;i<10;i++)
    {
        this->floatToPlc[i]=floatToPlc[i];
    }
    UpdateWindow();
}

void DialogHelpDBWrite::UpdateWindow()
{
    if(!this->isVisible())
    {
    }
    else
    {
    for(int i=0;i<8;i++)
        {
            if(boolToPlc[i])
            {
            labelsBit.at(i)->setText("true");
            labelsBit.at(i)->setStyleSheet("QLabel {color: green;font-weight: bold;}");
            }
            else
            {
            labelsBit.at(i)->setText("false");
            labelsBit.at(i)->setStyleSheet("QLabel {color: red;font-weight: bold;}");
            }
        }

    for(int i=0; i<10;i++)
    {
        labelsFloat.at(i)->setText(QString::number(floatToPlc[i]));
        labelsFloat.at(i)->setStyleSheet("QLabel {color: blue;font-weight: bold;}");
    }

    }
}
