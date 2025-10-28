// Copyright 2025, github.com/BIBlical33
//
// Application entry point
//
// License: The MIT License
// See: https://opensource.org/license/mit

#include <QApplication>
#include <QString>

#include "include/number_system_converter.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/resources/app-icon.svg"));

  number_system_converter::NumberSystemConverter widget;
  widget.setWindowTitle("Number System Converter");
  widget.resize(310, 220);
  widget.show();

  return app.exec();
}
