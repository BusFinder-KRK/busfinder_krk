#include <config.h>
#include <routing.h>


int main() {
    std::cout << "starting" << '\n';
    std::string kapelanka3 = "stop_412_57603";
    std::string n_kleparz4 = "stop_50_7104";
    std::chrono::time_point<std::chrono::system_clock> t =
                     std::chrono::system_clock::now();
    Routing rout;
    rout.output(kapelanka3, n_kleparz4, t);
    std::cout << "the end" << '\n';
}