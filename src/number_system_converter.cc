// Copyright 2025, github.com/BIBlical33
//
// Sets app's main window
//
// License: The MIT License
// See: https://opensource.org/license/mit

#include "include/number_system_converter.h"

#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QHBoxLayout>
#include <QIcon>
#include <QIntValidator>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QStyle>
#include <QVBoxLayout>

namespace {

struct ConvertNumberResult {
  QString result;
  bool is_number_correct;
};

// Converts the number to a new base
ConvertNumberResult ConvertNumber(const QString &number, int old_base,
                                  int new_base) {
  assert(old_base >= 2 and new_base >= 2 and old_base <= 36 and new_base <= 36);

  bool is_number_correct;
  int64_t dec = number.toLongLong(&is_number_correct, old_base);
  if (not is_number_correct) return {"", false};

  return {QString::number(dec, new_base).toUpper(), true};
}

}  // namespace

namespace number_system_converter {

NumberSystemConverter::NumberSystemConverter(QWidget *parent)
    : QMainWindow(parent) {
  CreateMenuBar();

  ApplyDarkTheme();

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
          &NumberSystemConverter::GenerateResult);

  separator_ = new QFrame();
  separator_->setFrameShape(QFrame::HLine);
  separator_->setStyleSheet("background-color: #1967D2;");
  main_layout->addWidget(separator_);

  SetUpToBaseLayout(main_layout);

  result_label_ = new QLabel("");

  main_layout->addWidget(result_label_);
  main_layout->addStretch();
}

// Sets a app's dark theme
void NumberSystemConverter::ApplyDarkTheme() {
  setStyleSheet(
      "background-color: #1b1b1b; color: #F5F5F5; font: 16px 'Roboto'; border: "
      "none;");
  if (not copy_input_button_) copy_input_button_ = new QPushButton(this);
  if (not copy_result_button_) copy_result_button_ = new QPushButton(this);

  copy_input_button_->setIcon(kCopyIconWhite);

  QString hoverStyle =
      "QPushButton:hover {"
      "   background-color: rgba(255, 255, 255, 0.2);"
      "   border-radius: 5px;"
      "}";

  copy_input_button_->setStyleSheet(hoverStyle);
  copy_result_button_->setStyleSheet(hoverStyle);
}

// Sets a app's light theme
void NumberSystemConverter::ApplyLightTheme() {
  setStyleSheet(
      "background-color: #F5F5F5; color: #1b1b1b; font: 16px 'Roboto'; border: "
      "none;");
  if (not copy_input_button_) copy_input_button_ = new QPushButton(this);
  if (not copy_result_button_) copy_result_button_ = new QPushButton(this);

  copy_input_button_->setIcon(kCopyIconBlack);

  QString hoverStyle =
      "QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0.2);"
      "   border-radius: 5px;"
      "}";

  copy_input_button_->setStyleSheet(hoverStyle);
  copy_result_button_->setStyleSheet(hoverStyle);
}

// Sets a application menu
void NumberSystemConverter::CreateMenuBar() {
  QMenuBar *menu_bar = new QMenuBar(this);
  setMenuBar(menu_bar);

  SetUpToolsMenu();
  SetUpHelpMenu();
}

// Sets a tools options menu
void NumberSystemConverter::SetUpToolsMenu() {
  QMenu *tools_menu = menuBar()->addMenu("Tools");
  tools_menu->setStyleSheet("font: 14px;");

  QMenu *theme_menu = tools_menu->addMenu("Color Theme");

  QActionGroup *theme_group = new QActionGroup(this);
  theme_group->setExclusive(true);

  QAction *light_action = new QAction("Light", this);
  QAction *dark_action = new QAction("Dark", this);

  light_action->setCheckable(true);
  dark_action->setCheckable(true);
  dark_action->setChecked(true);

  theme_group->addAction(light_action);
  theme_group->addAction(dark_action);

  theme_menu->addAction(light_action);
  theme_menu->addAction(dark_action);

  connect(light_action, &QAction::triggered, this,
          &NumberSystemConverter::ApplyLightTheme);
  connect(dark_action, &QAction::triggered, this,
          &NumberSystemConverter::ApplyDarkTheme);

  QMenu *letter_case_menu = tools_menu->addMenu("Result letter case");

  QActionGroup *letter_case_group = new QActionGroup(this);
  letter_case_group->setExclusive(true);

  QAction *lowercase_action = new QAction("Lowercase", this);
  QAction *uppercase_action = new QAction("Uppercase", this);

  lowercase_action->setCheckable(true);
  uppercase_action->setCheckable(true);
  uppercase_action->setChecked(true);

  letter_case_group->addAction(lowercase_action);
  letter_case_group->addAction(uppercase_action);

  letter_case_menu->addAction(lowercase_action);
  letter_case_menu->addAction(uppercase_action);

  connect(lowercase_action, &QAction::triggered, this, [this]() {
    is_results_in_lowercase_ = true;
    GenerateResult();
  });
  connect(uppercase_action, &QAction::triggered, this, [this]() {
    is_results_in_lowercase_ = false;
    GenerateResult();
  });
}

// Sets a help options menu
void NumberSystemConverter::SetUpHelpMenu() {
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
void NumberSystemConverter::SetUpFromBaseLayout(QVBoxLayout *base_layout) {
  QHBoxLayout *from_base_layout = new QHBoxLayout();
  from_base_layout->setSpacing(0);

  from_base_button_ = new QPushButton("Old Base: ");

  from_base_edit_ = new QLineEdit();

  connect(from_base_edit_, &QLineEdit::textChanged, this,
          &NumberSystemConverter::GenerateResult);
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
void NumberSystemConverter::SetUpToBaseLayout(QVBoxLayout *base_layout) {
  QHBoxLayout *to_base_layout = new QHBoxLayout();
  to_base_layout->setSpacing(0);

  to_base_button_ = new QPushButton("New Base: ");
  to_base_button_->setStyleSheet("color: #1967D2;");

  to_base_edit_ = new QLineEdit();
  to_base_edit_->setStyleSheet("color: #1967D2;");

  connect(to_base_edit_, &QLineEdit::textChanged, this,
          &NumberSystemConverter::GenerateResult);
  connect(to_base_button_, &QPushButton::clicked, this,
          [this]() { to_base_edit_->setFocus(); });

  copy_result_button_->setIcon(kCopyIconBlue);

  connect(copy_result_button_, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(result_label_->text());
  });

  to_base_layout->addWidget(to_base_button_);
  to_base_layout->addWidget(to_base_edit_);
  to_base_layout->addWidget(copy_result_button_);

  base_layout->addLayout(to_base_layout);
}

// Generates a result when all fields are filled in
void NumberSystemConverter::GenerateResult() {
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
    result_label_->setStyleSheet("color: red; font-size: 20px;");
    result_label_->setText("Error! Unsupported number system: " +
                           source_base_text);
    return;
  } else if (not is_new_base_correct or new_base < 2 or new_base > 36) {
    result_label_->setStyleSheet("color: red; font-size: 20px;");
    result_label_->setText("Error! Unsupported number system: " +
                           new_base_text);
    return;
  }

  result_label_->setStyleSheet("color: #1967D2; font-size: 20px;");
  ConvertNumberResult result = ConvertNumber(number, source_base, new_base);

  if (not result.is_number_correct) {
    result_label_->setStyleSheet("color: red; font-size: 20px;");
    result_label_->setText(
        "Error! Number mismatch with the entered base or not supported");
    return;
  }

  if (is_results_in_lowercase_)
    result_label_->setText(result.result.toLower());
  else
    result_label_->setText(result.result);
}

}  // namespace number_system_converter
