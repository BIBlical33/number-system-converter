// Copyright 2025, github.com/BIBlical33
//
// Sets app's main window
//
// License: The MIT License
// See: https://opensource.org/license/mit

#ifndef INCLUDE_APPLICATION_H_
#define INCLUDE_APPLICATION_H_

#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QSettings>
#include <QPushButton>
#include <QString>

#include "include/theme_manager.h"

namespace number_system_converter {

class Application : public QMainWindow {
  Q_OBJECT

 public:
  explicit Application(QWidget *parent = nullptr);

  ~Application() override = default;

  void ApplyTheme(const QString &theme_name);

 private:
  // Sets a application menu
  void CreateMenuBar();

  // Sets a tools options menu
  void SetUpToolsMenu();

  // Sets a help options menu
  void SetUpHelpMenu();

  // Sets a fist layout
  void CreateFromBaseSection(QVBoxLayout *base_layout);

  // Sets a second layout
  void CreateToBaseSection(QVBoxLayout *base_layout);

// Calls a result and displays it
  void DisplayResult();

  QSettings settings_{"BIBlical", "Number System Converter"};

  ThemeManager theme_manager_;

  QPushButton *from_base_button_{nullptr};

  QLineEdit *from_base_edit_{nullptr};

  QLineEdit *input_number_edit_{nullptr};

  QPushButton *copy_input_button_{nullptr};

  QFrame *separator_{nullptr};

  QPushButton *to_base_button_{nullptr};

  QLineEdit *to_base_edit_{nullptr};

  QLabel *result_label_{nullptr};

  QPushButton *copy_result_button_{nullptr};
};

}  // namespace number_system_converter

#endif  // INCLUDE_APPLICATION_H_
