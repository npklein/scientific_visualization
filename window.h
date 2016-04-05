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
    explicit Window(QWidget *parent = 0);
    ~Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:


    void on_selectPoints_clicked(bool select_points);
    void on_drawingHedgehogsCheckbox_clicked(bool select_drawing_hedgehogs);
    void on_drawingMatterCheckbox_clicked(bool select_drawing_matter);
    void on_drawStreamLineCheckbox_clicked(bool drawStreamline);
    void on_selectPointsCheckbox_clicked(bool selectPoints);
    void on_selectPointsCheckbox2_clicked(bool selectPoints);
    void on_slicesCheckBox_clicked(bool set_slices);
    private:
    Ui::Window *ui;
};

#endif // WINDOW_H
