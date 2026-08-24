#ifndef BUSFINDER_BACKEND_TRANSLATOR_H
#define BUSFINDER_BACKEND_TRANSLATOR_H
#include <string>
#include <pqxx/pqxx>
#include <unordered_map>

inline std::string test_query1 = R"(
                SELECT stop_id, stop_name, stop_desc FROM stops
                )";
inline std::string test_query2 = R"(
                SELECT trip_id, route_id FROM trips
                )";

class Translator {
public:
    std::string get_human_line(std::string input);
    std::string get_human_stop_name(std::string input);
    Translator();
private:
    std::optional<pqxx::connection> connection_;
    std::unordered_map<std::string, std::string> line_;
    std::unordered_map<std::string, std::string> stop_name_;
    void generate_names();
    void generate_lines();
};
#endif //BUSFINDER_BACKEND_TRANSLATOR_H