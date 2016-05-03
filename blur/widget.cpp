#include "widget.h"
#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    ,mBlurLever(0)
{
    mSlider = new QSlider(Qt::Orientation::Vertical, this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);
    layout->addWidget(mSlider);
    this->setLayout(layout);
    this->setGeometry(100,100, 640,480);
    mSlider->setRange(0, 12);

    mMThread = new QThread(this);
    mMake = new MakeBlurImage;
    mMake->moveToThread(mMThread);
    connect(mMThread, &QThread::destroyed, mMake, &QObject::deleteLater);

    connect(mSlider, &QSlider::valueChanged, this, &Widget::toMake);
    connect(this, &Widget::mitMake, mMake, &MakeBlurImage::makeBlur);
    connect(mMake, &MakeBlurImage::makeOver, this, &Widget::Over);      //Widget线程为主线程 不需要Qt::DirectConnection标志。

    mMThread->start();
    toMake(0);
}

Widget::~Widget()
{
    mMThread->quit();
    mMThread->wait();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(QRectF(0,0,this->width(),this->height()),mImage);
}

void Widget::Over(const QImage &img)
{
    mImage = img;
    mSlider->setEnabled(true);
    update();
}

void Widget::toMake(int lever)
{
    mMake->setImage(":/test.png");
    mSlider->setDisabled(true);
    emit mitMake(lever);
}