#include <config.h>
#include <transporttable.h>
#include <iostream>
#include <pqxx/pqxx>

using namespace std;

int main() {
  transporttable t;
  t.generate_table();
  // t.printtable();
  pqxx::connection conn(Config::connection_string);
  pqxx::work transaction(conn);
  cout << t.size();
}