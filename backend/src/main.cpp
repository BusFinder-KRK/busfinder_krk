#include <config.h>
#include <transporttable.h>

int main() {
  transporttable t;
  t.generate_table();
  t.printtable();
}