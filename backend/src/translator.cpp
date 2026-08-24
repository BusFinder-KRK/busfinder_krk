#include "translator.h"
#include "config.h"



Translator::Translator() {
    Config::load();
    connection_.emplace(Config::connection_string);
    generate_names();
    generate_lines();
}

void Translator::generate_names() {
    pqxx::work transaction{*connection_};
    for (const auto &[stopid, name, num] :
       transaction.query<std::string, std::string, std::string>(test_query1)) {
        stop_name_[stopid] = name+num;
       }
    transaction.commit();
}

void Translator::generate_lines() {
    pqxx::work transaction{*connection_};
    for (const auto &[service, route_id] :
       transaction.query<std::string, std::string>(test_query2)) {
        line_[service] = route_id;
       }
    line_["walk"] = "walk";
    line_["start"] = "start";
    transaction.commit();
}

std::string Translator::get_human_line(std::string input) {
    return line_[input];
}
std::string Translator::get_human_stop_name(std::string input) {
    return stop_name_[input];
}
