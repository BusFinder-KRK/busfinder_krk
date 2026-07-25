#ifndef BUSFINDER_BACKEND_DOTENV_PARSE_H
#define BUSFINDER_BACKEND_DOTENV_PARSE_H

#include <format>
#include <fstream>
#include <string>

inline void dotenv_parse() {
  std::ifstream file(".env");
  if (!file.is_open()) throw std::ios_base::failure("error: can't open .env");
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    auto delimiter_position = line.find('=');
    if (delimiter_position == std::string::npos) continue;
    std::string key = line.substr(0, delimiter_position);
    std::string value = line.substr(delimiter_position + 1);
    setenv(key.c_str(), value.c_str(), 1);
  }
}

inline std::string load_var(const char* key) {
  const char* val = std::getenv(key);
  if (!val || std::string(val).empty())
    throw std::runtime_error(std::string("Missing environment variable: ") + key);
  return val;
}

struct Config {
  inline static std::string db_host;
  inline static int db_port;
  inline static std::string db_name;
  inline static std::string db_user;
  inline static std::string db_password;
  inline static std::string connection_string;
  static void load() {
    dotenv_parse();
    db_host = load_var("host");
    db_port = std::stoi(load_var("port"));
    db_name = load_var("dbname");
    db_user = load_var("user");
    db_password = load_var("password");
    connection_string = std::format("host={} port={} dbname={} user={} password={}", db_host, db_port, db_name, db_user, db_password);
  }
};
#endif // BUSFINDER_BACKEND_DOTENV_PARSE_H
