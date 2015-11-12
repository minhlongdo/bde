// bdlf_bind_test13.t.cpp                                             -*-C++-*-

#include <bdlf_bind_test13.h>

// Count
#define BBT_n 13

// S with parameter count appended
#define BBT_C(S) S##13

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S)                                                              \
    S##1,S##2,S##3,S##4,S##5,S##6,S##7,S##8,S##9,S##10,S##11,S##12,S##13

// Repeat comma-separated S once per number of parameters
#define BBT_R(S)                                                              \
    S,   S,   S,   S,   S,   S,   S,   S,   S,   S,    S,    S,    S

#include <bdlf_bind_testn.t.cpp>
