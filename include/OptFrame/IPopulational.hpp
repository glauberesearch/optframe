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

#ifndef OPTFRAME_IPOPULATIONAL_HPP_
#define OPTFRAME_IPOPULATIONAL_HPP_

#include "EPopulation.hpp"
#include "StopCriteria.hpp"

namespace optframe {

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
template<XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES>
class IPopulational // do not inherit here!
{
   using XEv = typename XES::second_type;
   using XSH2 = optframe::EPopulation<XES2>; // this uses EPopulation, not "legacy Population"

public:
   // global search method (maybe, someday, create some abstract IGlobalSearch.. not now)
   //virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;
   //
   //virtual SearchOutput<XES, BestType> search(const StopCriteria<XEv>& stopCriteria) = 0;

   //
   // virtual method with search signature for populational methods
   //
   virtual SearchOutput<XES, BestType> searchPopulational(
     std::optional<XSH>& _best,
     XSH2& _inc,
     const StopCriteria<XEv>& stopCriteria) = 0;

   /*
   virtual SearchStatus searchBy(
     std::optional<XSH>& _best,
     std::optional<XSH2>& _inc,
     const StopCriteria<XEv>& stopCriteria) = 0;
     */
};

} // namespace optframe

#endif /* OPTFRAME_IPOPULATIONAL_HPP_ */
