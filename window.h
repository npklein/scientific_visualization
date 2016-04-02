// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    void setDrawingHedgehogsCheckbox(bool setDrawingHedgehogs);
    void setDrawingMatterCheckbox(bool setDrawingMatter);
    explicit Window(QWidget *parent = 0);
    ~Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:


    void on_selectPoints_clicked(bool select_points);

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
