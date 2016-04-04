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


void Window::on_selectPoints_clicked(bool select_points)
{
   ui->drawingMatterCheckbox->setChecked(!select_points);
   ui->drawingHedgehogsCheckbox->setChecked(!select_points);
}




void Window::on_drawingHedgehogsCheckbox_clicked(bool select_drawing_hedgehogs)
{
   if(!select_drawing_hedgehogs){
       ui->drawingMatterCheckbox->setChecked(!select_drawing_hedgehogs);
   }
}

void Window::on_drawingMatterCheckbox_clicked(bool select_drawing_matter)
{
   if(!select_drawing_matter){
       ui->drawingHedgehogsCheckbox->setChecked(!select_drawing_matter);
   }
}

