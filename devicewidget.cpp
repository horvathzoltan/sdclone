#include "devicewidget.h"

DeviceWidget::DeviceWidget() {}

void DeviceWidget::setStatus(bool status)
{
    //QString msg = status?"OK":"FAILED";
    QColor color = status?Qt::green:Qt::red;

    QPalette pal = QPalette();
    pal.setColor(_statusLabel->backgroundRole(), color);
    //_statusLabel->setText(msg);
    _statusLabel->setPalette(pal);
    _statusLabel->setAutoFillBackground(true);
    _statusLabel->update();

    //pal = QPalette();
    pal.setColor(_labelLabel->backgroundRole(), color);
    _labelLabel->setPalette(pal);
    _labelLabel->setAutoFillBackground(true);
    _labelLabel->update();
}

void DeviceWidget::resetStatus()
{
    //QString msg = status?"OK":"FAILED";
    //QColor color = status?Qt::green:Qt::red;

    QPalette pal = QPalette();
    pal.color(_statusLabel->backgroundRole());
    pal.setColor(_statusLabel->backgroundRole(), _defaultBackground);
    //_statusLabel->setText(msg);
    _statusLabel->setPalette(pal);
    _statusLabel->setAutoFillBackground(true);
    _statusLabel->update();

    //pal = QPalette();
    pal.setColor(_labelLabel->backgroundRole(), _defaultBackground);
    _labelLabel->setPalette(pal);
    _labelLabel->setAutoFillBackground(true);
    _labelLabel->update();
}

int DeviceWidget::GetPt(const QString& txt){
    QStringList as = txt.split('\n');
    int l = 0;
    for(auto&a:as){
        if(a.length()>l)l=a.length();
    }
    if(l>25) return 6;
    if(l>12) return 8;
    return 12;
}

void DeviceWidget::UpdateL2(const QString &txt)
{
    int u = txt.count('\n');
    bool oneLine = u<1;

    QFont f2 = _l2->font();
    if(oneLine){

        _l2->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        int pt = GetPt(txt);
        f2.setPointSize(pt);
        _l2->setFont(f2);
    } else{
        _l2->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        int pt = GetPt(txt);
        f2.setPointSize(pt);
        _l2->setFont(f2);
    }
    _l2->setText(txt);
}
