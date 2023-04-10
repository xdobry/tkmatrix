#include <stdio.h>
#include <gmp.h>
int main () {
  mpq_t r;
  mpf_t f;
  mpq_init(r);
  mpf_init(f);
  mpq_set_si(r,-2,3);
  mpf_set_q(f,r);
  mpf_out_str(stdout,10,0,f);
  printf(" number printed\n");
  return 0;
}
