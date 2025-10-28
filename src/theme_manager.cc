// Copyright 2025, github.com/BIBlical33
//
// Sets app's settings
//
// License: The MIT License
// See: https://opensource.org/license/mit

#include "include/theme_manager.h"

#include <QString>

namespace number_system_converter {

ThemeManager::ThemeManager() {
  AppTheme dark_theme{
      "background-color: #1b1b1b; color: #F5F5F5; font: 16px 'Roboto'; border: "
      "none;",
      "QPushButton:hover {"
      "   background-color: rgba(255, 255, 255, 0.2);"
      "   border-radius: 5px;"
      "}",
      QIcon{":/resources/copy-icon-white.svg"}};

  AppTheme light_theme{
      "background-color: #F5F5F5; color: #1b1b1b; font: 16px 'Roboto'; "
      "border: "
      "none;",
      "QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0.2);"
      "   border-radius: 5px;"
      "}",
      QIcon{":/resources/copy-icon-black.svg"}};

  themes_["Dark"] = dark_theme;
  themes_["Light"] = light_theme;
}

}  // namespace number_system_converter
