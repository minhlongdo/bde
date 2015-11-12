// bdlscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLSCM_VERSION
#define INCLUDED_BDLSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
// bdlscm::Version: namespace for RCS and SCCS versioning information for 'bdl'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bdl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bdl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bdlscm::Version' 'struct' can be used to query version
// information for the 'bdl' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bdl' version information
// embedded in an executable.  It is only necessary to use one or more 'bdl'
// components (and, hence, link in the 'bdl' library).
//
///Usage
///-----
// A program can display the version of BDL that was used to build it by
// printing the version string returned by 'bdlscm::Version::version()' to
// 'stdout' as follows:
//..
//  bsl::cout << "BDL version: " <<  bdlscm::Version::version() << bsl::endl;
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BDLSCM_VERSIONTAG
#include <bdlscm_versiontag.h>     // 'BDL_VERSION_MAJOR', 'BDL_VERSION_MINOR'
#endif

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


namespace BloombergLP {
namespace bdlscm {

struct Version {
    static const char *s_ident;
    static const char *s_what;

#define BDLSCM_CONCAT2(a,b,c,d,e) a ## b ## c ## d ## e
#define BDLSCM_CONCAT(a,b,c,d,e)  BDLSCM_CONCAT2(a,b,c,d,e)

// 'BDLSCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.
#define BDLSCM_D_VERSION BDLSCM_CONCAT(                \
                       d_version_BDL_,                 \
                       BDL_VERSION_MAJOR,              \
                       _,                              \
                       BDL_VERSION_MINOR,              \
                       _compiled_this_object)

    static const char *BDLSCM_S_VERSION;

    static const char *s_dependencies;
    static const char *s_buildInfo;
    static const char *s_timestamp;
    static const char *s_sourceControlInfo;

    static const char *version();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

inline
const char *Version::version()
{
    return BDLSCM_S_VERSION;
}

}  // close package namespace

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                          bdlscm_version_assertion,
                                          bdlscm::Version::BDLSCM_S_VERSION);

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2012 Bloomberg Finance L.P.
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
