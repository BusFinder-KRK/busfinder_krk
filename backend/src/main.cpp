#include <config.h>
#include <routing.h>
#include <api.h>

int main() {
  // api API;
  // API.run();

  auto start = std::chrono::system_clock::now();
  std::cout << "starting\n";
  std::string biprostal = "7420";
  std::string friedleina = "13906";
  std::string zielinskiego = "8631";
  std::string sloneczna = "17380";
  std::string filharmonia = "7813";

  auto t = std::chrono::zoned_time{
      std::chrono::current_zone(),
      std::chrono::time_point_cast<std::chrono::seconds>(
          std::chrono::system_clock::now())};
  Routing rout(t);
  rout.output(sloneczna, biprostal, t);
  std::cout << "the end." << '\n';
  auto end = std::chrono::system_clock::now();
  auto durr = end - start;
  std::cout << "time elapsed: "
            << std::chrono::duration_cast<std::chrono::seconds>(durr) << " "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   durr - std::chrono::floor<std::chrono::seconds>(durr));
}
