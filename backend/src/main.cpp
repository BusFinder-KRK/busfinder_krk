#include <config.h>
#include <routing.h>
#include <api.h>

int main() {

  auto start = std::chrono::system_clock::now();
  std::cout << "starting\n";
  std::string biprostal = "7420";
  std::string friedleina = "13906";
  std::string zielinskiego = "8631";
  std::string sloneczna = "17380";
  std::string filharmonia = "7813";
    std::string kleparz04 = "7430";

  auto t = std::chrono::zoned_time{
      std::chrono::current_zone(),
      std::chrono::time_point_cast<std::chrono::seconds>(
          std::chrono::system_clock::now())};
  Routing rout(t);
  //  api API;
  //  API.run();
    rout.output(sloneczna, kleparz04, t);
  std::cout << "the end." << '\n';
  auto end = std::chrono::system_clock::now();
  auto durr = end - start;
  std::cout << "time elapsed: "
            << std::chrono::duration_cast<std::chrono::seconds>(durr) << " "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   durr - std::chrono::floor<std::chrono::seconds>(durr));
}
