// Copyright 2025, github.com/BIBlical33
//
// Sets app's main window
//
// License: The MIT License
// See: https://opensource.org/license/mit

#include "include/application.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QVBoxLayout>
#include <QVariant>

namespace {

struct ConvertNumberResult {
  QString result;
  bool is_number_correct;
};

// Converts the number to a new base
ConvertNumberResult ConvertNumber(const QString &number, int old_base,
                                  int new_base) {
  if (old_base < 2 or old_base > 36 or new_base < 2 or new_base > 36)
    return {"", false};

  bool is_number_correct;
  int64_t dec = number.toLongLong(&is_number_correct, old_base);
  if (not is_number_correct) return {"", false};

  return {QString::number(dec, new_base).toUpper(), true};
}

}  // namespace

namespace number_system_converter {

Application::Application(QWidget *parent)
    : QMainWindow(parent) {
  config_service_.LoadConfig(config_);

  CreateMenuBar();

  copy_input_button_ = new QPushButton(this);
  copy_result_button_ = new QPushButton(this);

  assert(theme_manager_.IsValidTheme(config_.current_theme_name));
  ApplyTheme(config_.current_theme_name);

  QWidget *main_widget = new QWidget(this);
  setCentralWidget(main_widget);

  QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
  main_layout->setContentsMargins(15, 15, 15, 15);
  main_layout->setSpacing(15);
  main_layout->addStretch();

  SetUpFromBaseLayout(main_layout);

  input_number_edit_ = new QLineEdit();
  input_number_edit_->setPlaceholderText("Enter a number");
  input_number_edit_->setStyleSheet("font-size: 20px;");
  main_layout->addWidget(input_number_edit_);

  connect(input_number_edit_, &QLineEdit::textChanged, this,
          &Application::GenerateResult);

  separator_ = new QFrame();
  separator_->setFrameShape(QFrame::HLine);
  separator_->setStyleSheet("background-color: #1967D2;");
  main_layout->addWidget(separator_);

  SetUpToBaseLayout(main_layout);

  result_label_ = new QLabel("");

  main_layout->addWidget(result_label_);
  main_layout->addStretch();
}

// Sets a app's theme
void Application::ApplyTheme(const QString &theme_name) {
  if (not theme_manager_.IsValidTheme(theme_name)) return;

  config_.current_theme_name = theme_name;

  const AppTheme &theme = theme_manager_.GetTheme(theme_name);

  setStyleSheet(theme.main_style_sheet);

  copy_input_button_->setIcon(theme.copy_icon);

  QString hoverStyle = theme.hover_style;

  copy_input_button_->setStyleSheet(hoverStyle);
  copy_result_button_->setStyleSheet(hoverStyle);
}

// Sets a application menu
void Application::CreateMenuBar() {
  QMenuBar *menu_bar = new QMenuBar(this);
  setMenuBar(menu_bar);

  SetUpToolsMenu();
  SetUpHelpMenu();
}

// Sets a tools options menu
void Application::SetUpToolsMenu() {
  QMenu *tools_menu = menuBar()->addMenu("Tools");
  tools_menu->setStyleSheet("font: 14px;");

  QMenu *theme_menu = tools_menu->addMenu("Color Theme");

  QActionGroup *theme_group = new QActionGroup(this);
  theme_group->setExclusive(true);

  for (QString theme_name : theme_manager_.AvailableThemes()) {
    QAction *action = new QAction(theme_name, this);
    action->setCheckable(true);

    theme_group->addAction(action);

    if (action->text() == config_.current_theme_name) action->setChecked(true);

    theme_menu->addAction(action);

    connect(action, &QAction::triggered, this,
            [this, theme_name]() { ApplyTheme(theme_name); });
  }

  QMenu *letter_case_menu = tools_menu->addMenu("Result letter case");

  QActionGroup *letter_case_group = new QActionGroup(this);
  letter_case_group->setExclusive(true);

  QAction *lowercase_action = new QAction("Lowercase", this);
  QAction *uppercase_action = new QAction("Uppercase", this);

  lowercase_action->setCheckable(true);
  uppercase_action->setCheckable(true);

  config_.is_results_in_lowercase ? lowercase_action->setChecked(true)
                           : uppercase_action->setChecked(true);

  letter_case_group->addAction(lowercase_action);
  letter_case_group->addAction(uppercase_action);

  letter_case_menu->addAction(lowercase_action);
  letter_case_menu->addAction(uppercase_action);

  connect(lowercase_action, &QAction::triggered, this, [this]() {
    config_.is_results_in_lowercase = true;
    GenerateResult();
  });
  connect(uppercase_action, &QAction::triggered, this, [this]() {
    config_.is_results_in_lowercase = false;
    GenerateResult();
  });
}

// Sets a help options menu
void Application::SetUpHelpMenu() {
  QMenu *help_menu = menuBar()->addMenu("Help");
  QAction *about_action = new QAction("About Number System Converter", this);
  help_menu->setStyleSheet("font: 14px;");
  help_menu->addAction(about_action);

  connect(about_action, &QAction::triggered, this, []() {
    QDesktopServices::openUrl(
        QUrl("https://github.com/BIBlical33/number-system-converter"));
  });
}

// Sets a fist layout
void Application::SetUpFromBaseLayout(QVBoxLayout *base_layout) {
  QHBoxLayout *from_base_layout = new QHBoxLayout();
  from_base_layout->setSpacing(0);

  from_base_button_ = new QPushButton("Old Base: ");

  from_base_edit_ = new QLineEdit();

  connect(from_base_edit_, &QLineEdit::textChanged, this,
          &Application::GenerateResult);
  connect(from_base_button_, &QPushButton::clicked, this,
          [this]() { from_base_edit_->setFocus(); });

  connect(copy_input_button_, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(input_number_edit_->text());
  });

  from_base_layout->addWidget(from_base_button_);
  from_base_layout->addWidget(from_base_edit_);
  from_base_layout->addWidget(copy_input_button_);

  base_layout->addLayout(from_base_layout);
}

// Sets a second layout
void Application::SetUpToBaseLayout(QVBoxLayout *base_layout) {
  QHBoxLayout *to_base_layout = new QHBoxLayout();
  to_base_layout->setSpacing(0);

  to_base_button_ = new QPushButton("New Base: ");
  to_base_button_->setStyleSheet("color: #1967D2;");

  to_base_edit_ = new QLineEdit();
  to_base_edit_->setStyleSheet("color: #1967D2;");

  connect(to_base_edit_, &QLineEdit::textChanged, this,
          &Application::GenerateResult);
  connect(to_base_button_, &QPushButton::clicked, this,
          [this]() { to_base_edit_->setFocus(); });

  copy_result_button_->setIcon(QIcon(":/resources/copy-icon-blue.svg"));

  connect(copy_result_button_, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(result_label_->text());
  });

  to_base_layout->addWidget(to_base_button_);
  to_base_layout->addWidget(to_base_edit_);
  to_base_layout->addWidget(copy_result_button_);

  base_layout->addLayout(to_base_layout);
}

// Generates a result when all fields are filled in
void Application::GenerateResult() {
  auto ShowError = [&](const QString &message) {
    result_label_->setStyleSheet("color: red; font-size: 20px;");
    result_label_->setText(message);
  };

  QString number = input_number_edit_->text(),
          source_base_text = from_base_edit_->text(),
          new_base_text = to_base_edit_->text();

  // If the user hasn't initialized all arguments, don't display the result
  if (number.isEmpty() or source_base_text.isEmpty() or
      new_base_text.isEmpty()) {
    result_label_->setText("");
    return;
  }

  bool is_source_base_correct, is_new_base_correct;
  int source_base = from_base_edit_->text().toLongLong(&is_source_base_correct),
      new_base = to_base_edit_->text().toLongLong(&is_new_base_correct);

  if (not is_source_base_correct or source_base < 2 or source_base > 36) {
    ShowError("Error! Unsupported number system: " + source_base_text);
    return;
  } else if (not is_new_base_correct or new_base < 2 or new_base > 36) {
    ShowError("Error! Unsupported number system: " + new_base_text);
    return;
  }

  result_label_->setStyleSheet("color: #1967D2; font-size: 20px;");
  ConvertNumberResult result = ConvertNumber(number, source_base, new_base);

  if (not result.is_number_correct) {
    ShowError("Error! Number mismatch with the entered base or not supported");
    return;
  }

  if (config_.is_results_in_lowercase)
    result_label_->setText(result.result.toLower());
  else
    result_label_->setText(result.result);
}

// Activates when user closes the app
void Application::closeEvent(QCloseEvent *event) {
  config_service_.SaveConfig(config_);

  QMainWindow::closeEvent(event);
}

}  // namespace number_system_converter
