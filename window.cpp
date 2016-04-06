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


void Window::on_drawStreamLineCheckbox_clicked(bool draw_streamline){
    if(draw_streamline){
        ui->drawingMatterCheckbox->setChecked(false);
        ui->drawingHedgehogsCheckbox->setChecked(false);
        ui->slicesCheckBox->setChecked(false);
    }

}

void Window::on_selectPointsCheckbox_clicked(bool select_points){
    if(select_points){
        ui->showPointsCheckbox->setChecked(true);
    }
}

void Window::on_selectPointsCheckbox2_clicked(bool selectPoints){
    if(selectPoints){
        ui->showPointsCheckbox->setChecked(true);
    }
    if(!selectPoints){
        ui->drawStreamLineCheckbox->setChecked(true);
    }
}

void Window::on_slicesCheckBox_clicked(bool set_slices){
    if(set_slices){
     ui->drawStreamLineCheckbox->setChecked(false);
    }
}


void Window::on_radioButtonDrawSelectedPointsStreamline_clicked(bool selectPoints)
{
    ui->drawStreamLineCheckbox->setChecked(true);
}

