#include <config.h>
#include <routing.h>


int main() {
    std::cout << "starting" << '\n';
    //old names
    std::string kapelanka3 = "stop_412_57603";
    std::string n_kleparz4 = "stop_50_7104";
    //new names WE CAN NOW CHANE IT TO INT
    std::string biprostal = "7420";
    std::string friedleina = "13906";
    auto t = std::chrono::system_clock::now();
    ///i wanted it to be in the zoned
    auto zoned = std::chrono::zoned_time{std::chrono::current_zone(), t}.get_sys_time();

    Routing rout(t);
    rout.output(biprostal, friedleina, zoned);
    std::cout << "the end" << '\n';
}