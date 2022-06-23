#include "dialoghelpdbread.h"
#include "ui_dialoghelpdbread.h"

DialogHelpDBRead::DialogHelpDBRead(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHelpDBRead)
{
    ui->setupUi(this);

    CreateQListFromUILabels();
}

DialogHelpDBRead::~DialogHelpDBRead()
{
    delete ui;
}

void DialogHelpDBRead::on_pushButtonCloseDialog_clicked()
{
    this->close();
}

void DialogHelpDBRead::CreateQListFromUILabels()
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

void DialogHelpDBRead::UpdateDispValues(bool boolFromPlc[],float floatFromPlc[])
{
   for(int i=0;i<8;i++)
   {
        this->boolFromPlc[i]=boolFromPlc[i];
   }

   for(int i=0;i<10;i++)
   {
       this->floatFromPlc[i]=floatFromPlc[i];
   }
    UpdateWindow();
}


void DialogHelpDBRead::UpdateWindow()
{
    if(!this->isVisible())
    {
    }
    else
    {
    for(int i=0;i<8;i++)
        {
            if(boolFromPlc[i])
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
        labelsFloat.at(i)->setText(QString::number(floatFromPlc[i]));
        labelsFloat.at(i)->setStyleSheet("QLabel {color: blue;font-weight: bold;}");

    }

    }
}
