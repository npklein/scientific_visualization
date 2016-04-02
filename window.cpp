// window.cpp

#include <QtWidgets>
#include "window.h"
#include "ui_window.h"
#include "myglwidget.h"

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
}

Window::~Window()
{
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void Window::setDrawingHedgehogsCheckbox(bool setDrawingHedgehogs){
    ui->drawingHedgehogsCheckbox->setChecked(setDrawingHedgehogs);
}

void Window::setDrawingMatterCheckbox(bool setDrawingMatter){
    ui->drawingMatterCheckbox->setChecked(setDrawingMatter);
}


void Window::on_selectPoints_clicked(bool select_points)
{
    ui->drawingMatterCheckbox->setChecked(select_points);
}
