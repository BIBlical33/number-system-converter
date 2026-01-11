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
#include <limits>

namespace {

struct ConvertNumberResult final {
  QString text, color;
};

ConvertNumberResult GenerateResult(const QString &number,
                                   const QString &old_base_text,
                                   const QString &new_base_text) {
  // If the user hasn't initialized all arguments, don't display the result
  if (number.isEmpty() or old_base_text.isEmpty() or new_base_text.isEmpty())
    return {"", "#1967D2"};

  bool is_source_base_correct = false, is_new_base_correct = false;
  int old_base, new_base;
  if (old_base_text.size() < 3 && new_base_text.size() < 3) {
    old_base = old_base_text.toInt(&is_source_base_correct);
    new_base = new_base_text.toInt(&is_new_base_correct);
  }

  if (not is_source_base_correct or old_base < 2 or old_base > 36)
    return {"Error! Unsupported number system: " + old_base_text, "red"};
  else if (not is_new_base_correct or new_base < 2 or new_base > 36)
    return {"Error! Unsupported number system: " + new_base_text, "red"};

  static constexpr qint64 kMax = std::numeric_limits<qint64>::max();
  const qint64 kLimit = kMax / old_base;
  qint64 result = 0;
  for (QChar c : number.toUpper()) {
    static const QString kValidDigits("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    int digit = kValidDigits.indexOf(c);
    if (digit < 0 || digit >= old_base)
      return {"Error! Incorrect number", "red"};

    if (result > kLimit) return {"Error! Unsupported number", "red"};

    qint64 next_mult = result * old_base;
    if (next_mult > kMax - digit) return {"Error! Unsupported number", "red"};

    result = next_mult + digit;
  }

  return {QString::number(result, new_base), "#1967D2"};
}

}  // namespace

namespace number_system_converter {

Application::Application(QWidget *parent) : QMainWindow(parent) {
  CreateMenuBar();

  copy_input_button_ = new QPushButton();
  copy_result_button_ = new QPushButton();

  ApplyTheme(settings_.value("theme", "Dark").toString());

  QWidget *main_widget = new QWidget(this);
  setCentralWidget(main_widget);

  QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
  main_layout->setContentsMargins(15, 15, 15, 15);
  main_layout->setSpacing(15);
  main_layout->addStretch();

  CreateFromBaseSection(main_layout);

  input_number_edit_ = new QLineEdit();
  input_number_edit_->setPlaceholderText("Enter a number");
  input_number_edit_->setStyleSheet("font-size: 20px;");
  main_layout->addWidget(input_number_edit_);

  connect(input_number_edit_, &QLineEdit::textChanged, this,
          &Application::DisplayResult);

  separator_ = new QFrame();
  separator_->setFrameShape(QFrame::HLine);
  separator_->setStyleSheet("background-color: #1967D2;");
  main_layout->addWidget(separator_);

  CreateToBaseSection(main_layout);

  result_label_ = new QLabel("");

  main_layout->addWidget(result_label_);
  main_layout->addStretch();
}

void Application::ApplyTheme(const QString &theme_name) {
  const AppTheme &theme = theme_manager_.GetTheme(theme_name);

  settings_.setValue("theme", theme_name);

  setStyleSheet(theme.main_style_sheet);

  copy_input_button_->setIcon(theme.copy_icon);

  QString hoverStyle = theme.hover_style;
  copy_input_button_->setStyleSheet(hoverStyle);
  copy_result_button_->setStyleSheet(hoverStyle);
}

// Sets a application menu
void Application::CreateMenuBar() {
  QMenuBar *menu_bar = new QMenuBar;
  setMenuBar(menu_bar);

  SetUpToolsMenu();
  SetUpHelpMenu();
}

// Sets a tools options menu
void Application::SetUpToolsMenu() {
  QMenu *tools_menu = menuBar()->addMenu("Tools");
  tools_menu->setStyleSheet("font: 14px;");

  QMenu *theme_menu = tools_menu->addMenu("Color Theme");

  QActionGroup *theme_group = new QActionGroup(theme_menu);
  theme_group->setExclusive(true);

  for (QString theme_name : theme_manager_.AvailableThemes()) {
    QAction *action = new QAction(theme_name, theme_group);
    action->setCheckable(true);

    if (action->text() == settings_.value("theme", "Dark").toString())
      action->setChecked(true);

    theme_menu->addAction(action);

    connect(action, &QAction::triggered, this,
            [this, theme_name]() { ApplyTheme(theme_name); });
  }

  QMenu *letter_case_menu = tools_menu->addMenu("Result letter case");

  QActionGroup *letter_case_group = new QActionGroup(letter_case_menu);
  letter_case_group->setExclusive(true);

  QAction *lowercase_action = new QAction("Lowercase", letter_case_group);
  QAction *uppercase_action = new QAction("Uppercase", letter_case_group);

  lowercase_action->setCheckable(true);
  uppercase_action->setCheckable(true);

  settings_.value("lowercase", "1").toBool()
      ? lowercase_action->setChecked(true)
      : uppercase_action->setChecked(true);

  letter_case_menu->addAction(lowercase_action);
  letter_case_menu->addAction(uppercase_action);

  connect(lowercase_action, &QAction::triggered, this, [this]() {
    settings_.setValue("lowercase", "1");
    DisplayResult();
  });
  connect(uppercase_action, &QAction::triggered, this, [this]() {
    settings_.setValue("lowercase", "0");
    DisplayResult();
  });
}

// Sets a help options menu
void Application::SetUpHelpMenu() {
  QMenu *help_menu = menuBar()->addMenu("Help");
  QAction *about_action =
      new QAction("About Number System Converter", help_menu);
  help_menu->setStyleSheet("font: 14px;");
  help_menu->addAction(about_action);

  connect(about_action, &QAction::triggered, this, []() {
    QDesktopServices::openUrl(
        QUrl("https://github.com/BIBlical33/number-system-converter"));
  });
}

// Sets a fist layout
void Application::CreateFromBaseSection(QVBoxLayout *base_layout) {
  QHBoxLayout *from_base_layout = new QHBoxLayout;
  from_base_layout->setSpacing(0);

  from_base_button_ = new QPushButton("Old Base: ");

  from_base_edit_ = new QLineEdit();

  connect(from_base_edit_, &QLineEdit::textChanged, this,
          &Application::DisplayResult);
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
void Application::CreateToBaseSection(QVBoxLayout *base_layout) {
  QHBoxLayout *to_base_layout = new QHBoxLayout;
  to_base_layout->setSpacing(0);

  to_base_button_ = new QPushButton("New Base: ");
  to_base_button_->setStyleSheet("color: #1967D2;");

  to_base_edit_ = new QLineEdit();
  to_base_edit_->setStyleSheet("color: #1967D2;");

  connect(to_base_edit_, &QLineEdit::textChanged, this,
          &Application::DisplayResult);
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

// Calls a result and displays it
void Application::DisplayResult() {
  QString number = input_number_edit_->text(),
          source_base_text = from_base_edit_->text(),
          new_base_text = to_base_edit_->text();

  ConvertNumberResult result =
      GenerateResult(number, source_base_text, new_base_text);
  result_label_->setStyleSheet(
      QString("color: %1; font-size: 20px;").arg(result.color));

  if (settings_.value("lowercase", "1").toBool())
    result_label_->setText(result.text.toLower());
  else
    result_label_->setText(result.text.toUpper());
}

}  // namespace number_system_converter
