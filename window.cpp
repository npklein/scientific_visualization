#include "window.h"
//#include "fluids.cpp"
#include <QApplication>
 #include <QPushButton>
Window::Window(QWidget *parent) :
 QWidget(parent)
 {
  // Set size of the window
  setFixedSize(400, 200);

  // Create and position the button
  m_button = new QPushButton("Hello World", this);
  m_button->setGeometry(10, 10, 80, 30);
  // NEW : Do the connection
  connect(m_button, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));
 }
