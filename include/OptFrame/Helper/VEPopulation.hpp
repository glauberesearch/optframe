// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_VEPOPULATION_HPP_
#define OPTFRAME_VEPOPULATION_HPP_

#include <OptFrame/BaseConcepts.hpp>
#include <vector>
//

// ================================================================
// Remember that VEPopulation is NOT a optframe::Component!
//
// This was created just to simplify development, using std::vector
// as the container for a std::pair of solution and evaluation.
// It implements concept X2ESolution.
//
// If you need optframe::Component, try EPopulation or "legacy" Population
// Also, you can try MultiESolution, as a "neutral" alternative (semantically free)
//

namespace optframe {

template <XSolution S>
using VPopulation = std::vector<S>;

template <XESolution XES>
using VEPopulation = std::vector<XES>;

}  // namespace optframe

#include "VEPopulation.test.hpp"

#endif /* OPTFRAME_VEPOPULATION_HPP_ */
