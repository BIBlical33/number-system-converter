// Copyright 2025, github.com/BIBlical33
//
// Sets app's main window
//
// License: The MIT License
// See: https://opensource.org/license/mit

#ifndef INCLUDE_NUMBER_SYSTEM_CONVERTER_H
#define INCLUDE_NUMBER_SYSTEM_CONVERTER_H

#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QString>

#include "include/config_service.h"
#include "include/theme_manager.h"

namespace number_system_converter {

class Application : public QMainWindow {
  Q_OBJECT

 public:
  explicit Application(QWidget *parent = nullptr);

  ~Application() override = default;

  // Sets the app's dark theme
  void ApplyTheme(const QString &theme_name);

 private:
  // Sets a application menu
  void CreateMenuBar();

  // Sets a tools options menu
  void SetUpToolsMenu();

  // Sets a help options menu
  void SetUpHelpMenu();

  // Sets a fist layout
  void SetUpFromBaseLayout(QVBoxLayout *base_layout);

  // Sets a second layout
  void SetUpToBaseLayout(QVBoxLayout *base_layout);

  // Generates a result when all fields are filled in
  void GenerateResult();

  // Activates when user closes the app
  void closeEvent(QCloseEvent *event) override;

  ConfigService config_service_{"BIBlical", "Number System Converter"};

  Config config_;

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

#endif  // INCLUDE_NUMBER_SYSTEM_CONVERTER_H
