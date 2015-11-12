// bdlf_bind_test8.t.cpp                                              -*-C++-*-

#include <bdlf_bind_test8.h>

// Count
#define BBT_n 8

// S with parameter count appended
#define BBT_C(S) S##8

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S) S##1,S##2,S##3,S##4,S##5,S##6,S##7,S##8

// Repeat comma-separated S once per number of parameters
#define BBT_R(S) S,   S,   S,   S,   S,   S,   S,   S

#include <bdlf_bind_testn.t.cpp>
