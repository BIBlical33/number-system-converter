// Copyright 2025, github.com/BIBlical33
//
// Sets app's settings
//
// License: The MIT License
// See: https://opensource.org/license/mit

#ifndef INCLUDE_APP_config_H
#define INCLUDE_APP_config_H

#include <QSettings>
#include <QString>

namespace number_system_converter {

struct Config {
  bool is_results_in_lowercase = false;

  QString current_theme_name = "Dark";
};

class ConfigService {
 public:
  explicit inline ConfigService(QString author, QString project_name)
      : config_{author, project_name} {}

  inline void LoadConfig(Config& config) {
    config.current_theme_name = config_.value("theme", "Dark").toString();
    config.is_results_in_lowercase =
        config_.value("lowercase", false).toBool();
  }

  inline void SaveConfig(const Config& config) {
    config_.setValue("theme", config.current_theme_name);
    config_.setValue("lowercase", config.is_results_in_lowercase);
  }

 private:
  QSettings config_;
};

}  // namespace number_system_converter

#endif  // INCLUDE_APP_config_H