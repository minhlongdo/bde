// bdlma_guardingallocator.h                                          -*-C++-*-
#ifndef INCLUDED_BDLMA_GUARDINGALLOCATOR
#define INCLUDED_BDLMA_GUARDINGALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a memory allocator that guards against buffer overruns.
//
//@CLASSES:
//  bdlma::GuardingAllocator: memory allocator that detects buffer overruns
//
//@SEE_ALSO: bslma_allocator, bslma_testallocator
//
//@DESCRIPTION: This component provides a concrete allocation mechanism,
// 'bdlma::GuardingAllocator', that implements the 'bslma::Allocator' protocol
// and adjoins a read/write protected guard page to each block of memory
// returned by the 'allocate' method.  The guard page is located immediately
// following or immediately preceding the block returned from 'allocate'
// according to an optionally-supplied constructor argument:
//..
//   ,------------------------.
//  ( bdlma::GuardingAllocator )
//   `------------------------'
//               |         ctor/dtor
//               V
//      ,----------------.
//     ( bslma::Allocator )
//      `----------------'
//                         allocate
//                         deallocate
//..
// *WARNING*: Note that this allocator should *not* be used for production use;
// it is intended for debugging purposes only.  In particular, clients should
// be aware that a multiple of the page size is allocated for *each* 'allocate'
// invocation (unless the size of the request is 0).
//
// Also note that, unlike many other BDE allocators, a 'bslma::Allocator *'
// cannot be (optionally) supplied upon construction of a 'GuardingAllocator';
// instead, a system facility is used that allocates blocks of memory in
// multiples of the system page size.
//
///Guard Pages
///-----------
// A 'GuardingAllocator' may be used to debug buffer overflow (or underflow) by
// protecting a memory page after (or before) each block of memory returned
// from 'allocate'.  Consequently, certain memory access outside the block
// returned to the client will trigger a memory protection fault.
//
// A constructor argument of type 'GuardingAllocator::GuardPageLocation', an
// enumeration, determines whether guard pages are located following
// ('e_AFTER_USER_BLOCK') or preceding ('e_BEFORE_USER_BLOCK') the user block.
// If no value is supplied at construction, 'e_AFTER_USER_BLOCK' is assumed.
//
// To illustrate, the following diagram shows the memory layout resulting from
// an 'N'-byte allocation request from a guarding allocator, where 'N' is
// assumed to be less than or equal to the size of a memory page.  Note that
// two pages of memory are consumed for each such allocation request:
//..
//  N` - N rounded up to the least multiple of the maximum alignment
//  A  - address of (2-page) block of memory returned by system allocator
//  U  - address returned from 'allocate' to user
//  G  - address of the guard page
//  PS - page size (in bytes)
//
//                          e_AFTER_USER_BLOCK
//                          ------------------
//
//      [ - - - one memory page  - - - | - - - one memory page  - - - ]
//      ---------------------------------------------------------------
//      |                 |  N` bytes  | ******* R/W protected ****** |
//      ---------------------------------------------------------------
//      ^                 ^            ^
//      A                 U == G - N`  G == A + PS
//
//
//                          e_BEFORE_USER_BLOCK
//                          -------------------
//
//      ---------------------------------------------------------------
//      | ******* R/W protected ****** |  N` bytes  |                 |
//      ---------------------------------------------------------------
//      ^                              ^
//      A == G                         U == A + PS
//..
//
///Thread Safety
///-------------
// The 'bdlma::GuardingAllocator' class is fully thread-safe (see
// 'bsldoc_glossary').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Diagnosing Buffer Overflow
///- - - - - - - - - - - - - - - - - - -
// Use of a 'bdlma::GuardingAllocator' is indicated, for example, if some code
// under development is suspected of having a buffer overrun (or underrun) bug,
// and more sophisticated tools that detect such conditions are either not
// available, or are inconvenient to apply to the situation at hand.
//
// This usage example illustrates a guarding allocator being brought to bear on
// a buffer overrun bug.  The bug in question arises in the context of an
// artificial data handling class, 'my_DataHandler'.  This class makes use of a
// (similarly artificial) data translation utility that translates chunks of
// data among various data styles.  In our idealized example, we assume that
// the length of the output resulting from some data translation is precisely
// determinable from the length of the input data and the respective styles of
// the input and the (desired) output.  For simplicity, we also assume that
// input data comes from a trusted source.
//
// First, we define an enumeration of data styles:
//..
//  enum my_DataStyle {
//      e_STYLE_NONE = 0
//    , e_STYLE_A    = 1  // default style
//    , e_STYLE_AA   = 2  // style exactly twice as verbose as 'e_STYLE_A'
//    // etc.
//  };
//..
// Next, we define the (elided) interface of our data translation utility:
//..
//  struct my_DataTranslationUtil {
//      // This 'struct' provides a namespace for data translation utilities.
//
//      // CLASS METHODS
//      static int outputSize(my_DataStyle outputStyle,
//                            my_DataStyle inputStyle,
//                            int          inputLength);
//          // Return the buffer size (in bytes) required to store the result
//          // of converting input data of the specified 'inputLength' (in
//          // bytes), in the specified 'inputStyle', into the specified
//          // 'outputStyle'.  The behavior is undefined unless
//          // '0 <= inputLength'.
//
//      static int translate(char         *output,
//                           my_DataStyle  outputStyle,
//                           const char   *input,
//                           my_DataStyle  inputStyle);
//          // Load into the specified 'output' buffer the result of converting
//          // the specified 'input' data, in the specified 'inputStyle', into
//          // the specified 'outputStyle'.  Return 0 on success, and a
//          // non-zero value otherwise.  The behavior is undefined unless
//          // 'output' has sufficient capacity to hold the translated result.
//          // Note that this method assumes that 'input' originated from a
//          // trusted source.
//  };
//..
// Next, we define 'my_DataHandler', a simple class that makes use of
// 'my_DataTranslationUtil':
//..
//  class my_DataHandler {
//      // This 'class' provides a basic data handler.
//
//      // DATA
//      my_DataStyle      d_inStyle;     // style of 'd_inBuffer' contents
//      char             *d_inBuffer;    // input supplied at construction
//      int               d_inCapacity;  // capacity (in bytes) of 'd_inBuffer'
//      my_DataStyle      d_altStyle;    // alternative style (if requested)
//      char             *d_altBuffer;   // buffer for alternative style
//      bslma::Allocator *d_allocator_p; // memory allocator (held, not owned)
//
//    private:
//      // Not implemented:
//      my_DataHandler(const my_DataHandler&);
//
//    public:
//      // CREATORS
//      my_DataHandler(const char       *input,
//                     int               inputLength,
//                     my_DataStyle      inputStyle,
//                     bslma::Allocator *basicAllocator = 0);
//          // Create a data handler for the specified 'input' data, in the
//          // specified 'inputStyle', having the specified 'inputLength' (in
//          // bytes).  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently installed
//          // default allocator is used.  The behavior is undefined unless
//          // '0 <= inputLength'.
//
//      ~my_DataHandler();
//          // Destroy this data handler.
//
//      // ...
//
//      // MANIPULATORS
//      int generateAlternate(my_DataStyle alternateStyle);
//          // Generate data for this data handler in the specified
//          // 'alternateStyle'.  Return 0 on success, and a non-zero value
//          // otherwise.  If 'alternateStyle' is the same as the style of data
//          // supplied at construction, this method returns 0 with no effect.
//
//      // ...
//  };
//..
// Next, we show the definition of the 'my_DataHandler' constructor:
//..
//  my_DataHandler::my_DataHandler(const char       *input,
//                                 int               inputLength,
//                                 my_DataStyle      inputStyle,
//                                 bslma::Allocator *basicAllocator)
//  : d_inStyle(inputStyle)
//  , d_inBuffer(0)
//  , d_inCapacity(inputLength)
//  , d_altStyle(e_STYLE_NONE)
//  , d_altBuffer(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      BSLS_ASSERT(0 <= inputLength);
//
//      void *tmp = d_allocator_p->allocate(inputLength);
//      bsl::memcpy(tmp, input, inputLength);
//      d_inBuffer = static_cast<char *>(tmp);
//  }
//..
// Next, we show the definition of the 'generateAlternate' manipulator.  Note
// that we have deliberately introduced a bug in 'generateAlternate' to cause
// buffer overrun:
//..
//  int my_DataHandler::generateAlternate(my_DataStyle alternateStyle)
//  {
//      if (alternateStyle == d_inStyle) {
//          return 0;                                                 // RETURN
//      }
//
//      int altLength = my_DataTranslationUtil::outputSize(alternateStyle,
//                                                         d_inStyle,
//                                                         d_inCapacity);
//      (void)altLength;
//
//      // Oops!  Should have used 'altLength'.
//      char *tmpAltBuffer = (char *)d_allocator_p->allocate(d_inCapacity);
//      int rc = my_DataTranslationUtil::translate(tmpAltBuffer,
//                                                 alternateStyle,
//                                                 d_inBuffer,
//                                                 d_inStyle);
//
//      if (rc) {
//          d_allocator_p->deallocate(tmpAltBuffer);
//          return rc;                                                // RETURN
//      }
//
//      d_altStyle  = alternateStyle;
//      d_altBuffer = tmpAltBuffer;
//
//      return 0;
//  }
//..
// Next, we define some data (in 'e_STYLE_A'):
//..
//  const char *input = "AAAAAAAAAAAAAAA@";  // data always terminated with '@'
//..
// Then, we define a 'my_DataHandler' object, 'handler', to process that data:
//..
//  my_DataHandler handler(input, 16, e_STYLE_A);
//..
// Note that our 'handler' object uses the default allocator.
//
// Next, we request that an alternate data style, 'e_STYLE_AA', be generated by
// 'handler'.  Unfortunately, data in style 'e_STYLE_AA' is twice as large as
// that in style 'e_STYLE_A' making it a virtual certainty that the program
// will crash due to the insufficiently sized buffer that is allocated in the
// 'generateAlternate' method to accommodate the 'e_STYLE_AA' data:
//..
//  int rc = handler.generateAlternate(e_STYLE_AA);
//  if (!rc) {
//      // use data in alternate style
//  }
//..
// Suppose that after performing a brief post-mortem on the resulting core
// file, we strongly suspect that a buffer overrun is the root cause, but the
// program crashed in a context far removed from that of the source of the
// problem (which is often the case with buffer overrun issues).
//
// Consequently, we modify the code to supply a guarding allocator to the
// 'handler' object, then rebuild and rerun the program.  We have configured
// the guarding allocator (below) to place guard pages *after* user blocks.
// Note that 'e_AFTER_USER_BLOCK' is the default, so it need not be specified
// at construction as we have (pedantically) done here:
//..
//  typedef bdlma::GuardingAllocator GA;
//  GA guard(GA::e_AFTER_USER_BLOCK);
//
//  my_DataHandler handler(input, 16, e_STYLE_A, &guard);
//..
// With a guarding allocator now in place, a memory fault is triggered when a
// guard page is overwritten as a result of the buffer overrun bug.  Hence, the
// program will dump core in a context that is more proximate to the buggy
// code, resulting in a core file that will be more amenable to revealing the
// issue when analyzed in a debugger.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {
namespace bdlma {

                         // -----------------------
                         // class GuardingAllocator
                         // -----------------------

class GuardingAllocator : public bslma::Allocator {
    // This class defines a concrete thread-safe "guarding" allocator mechanism
    // that implements the 'bslma::Allocator' protocol, and adjoins a
    // read/write protected guard page to each block of memory returned by the
    // 'allocate' method.  The guard page is placed immediately before or
    // immediately following the block returned from 'allocate' according to
    // the 'GuardPageLocation' enumerator value (optionally) supplied at
    // construction.  Note that, unlike many other allocators, an allocator
    // cannot be (optionally) supplied at construction; instead, a system
    // facility is used that allocates blocks of memory in multiples of the
    // system page size.  Also note that this allocator is intended for
    // debugging purposes *only*.

  public:
    // TYPES
    enum GuardPageLocation {
        // Enumerate the configuration options for 'GuardingAllocator' that may
        // be (optionally) supplied at construction.

        e_AFTER_USER_BLOCK,  // locate the guard page after the user block
        e_BEFORE_USER_BLOCK  // locate the guard page before the user block
    };

  private:
    // DATA
    GuardPageLocation d_guardPageLocation;  // if 'e_AFTER_USER_BLOCK', place
                                            // the read/write protected guard
                                            // page after the user block;
                                            // otherwise, place it before the
                                            // block ('e_BEFORE_USER_BLOCK')

  private:
    // NOT IMPLEMENTED
    GuardingAllocator(const GuardingAllocator&);
    GuardingAllocator& operator=(const GuardingAllocator&);

  public:
    // CREATORS
    explicit
    GuardingAllocator(GuardPageLocation guardLocation = e_AFTER_USER_BLOCK);
        // Create a guarding allocator.  Optionally specify a 'guardLocation'
        // indicating where read/write protected guard pages should be placed
        // with respect to the memory blocks returned by the 'allocate' method.
        // If 'guardLocation' is not specified, guard pages are placed
        // immediately following the memory blocks returned by 'allocate'.

    virtual ~GuardingAllocator();
        // Destroy this allocator object.  Note that destroying this allocator
        // has no effect on any outstanding allocated memory.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly-allocated maximally-aligned block of memory of the
        // specified 'size' (in bytes) that has a read/write protected guard
        // page located immediately before or after it according to the
        // 'GuardPageLocation' indicated at construction.  If 'size' is 0, no
        // memory is allocated and 0 is returned.  Note that a multiple of the
        // platform's memory page size is allocated for *every* call to this
        // method.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this method has no effect.
        // Otherwise, the guard page associated with 'address' is unprotected
        // and also deallocated.  The behavior is undefined unless 'address'
        // was returned by 'allocate' and has not already been deallocated.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                         // -----------------------
                         // class GuardingAllocator
                         // -----------------------

// CREATORS
inline
GuardingAllocator::GuardingAllocator(GuardPageLocation guardLocation)
: d_guardPageLocation(guardLocation)
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
