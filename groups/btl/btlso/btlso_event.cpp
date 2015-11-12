// btlso_event.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_event.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_event_cpp,"$Id$ $CSID$")

#include <bsl_functional.h>

namespace BloombergLP {

namespace btlso {

                           // ---------------
                           // class EventHash
                           // ---------------

// ACCESSORS
bsl::size_t EventHash::operator()(const Event& value) const
{
    bsl::hash<int> hasher;
    return hasher((value.type() << 24) ^ (int) value.handle());
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
