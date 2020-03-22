#include "include/TestHelpers.h"

extern int register_dijkstra_tests();
extern int register_sp_tests();
extern int register_bf_tests();

int register_tests() {
  return (
      /* register_sp_tests() != 0 + */ 
	  /* register_dijkstra_tests() != 0 + */ 
	  register_bf_tests() != 0
	  ) *
         -1;
}

int main(void) { TestRunner(register_tests); }
