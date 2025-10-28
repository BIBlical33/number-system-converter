// Copyright 2025, github.com/BIBlical33
//
// Sets app's themes
//
// License: The MIT License
// See: https://opensource.org/license/mit

#ifndef INCLUDE_THEME_MANAGER_H_
#define INCLUDE_THEME_MANAGER_H_

#include <QHash>
#include <QIcon>
#include <QString>
#include <QStringList>
#include <stdexcept>

namespace number_system_converter {

struct AppTheme {
  QString main_style_sheet;
  QString hover_style;
  QIcon copy_icon;
};

class ThemeManager {
 public:
  ThemeManager();

  inline AppTheme GetTheme(const QString& theme_name) const {
    if (IsValidTheme(theme_name))
      return themes_[theme_name];
    else
      throw std::invalid_argument("Invalid theme" + theme_name.toStdString());
  };

  inline QStringList AvailableThemes() const { return themes_.keys(); };

  inline bool IsValidTheme(const QString& theme_name) const {
    return themes_.contains(theme_name) ? true : false;
  }

 private:
  QHash<QString, AppTheme> themes_;
};

}  // namespace number_system_converter

#endif  // INCLUDE_THEME_MANAGER_H_
