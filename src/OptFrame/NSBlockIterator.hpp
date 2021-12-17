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

#ifndef OPTFRAME_NSBLOCKITERATOR_HPP_
#define OPTFRAME_NSBLOCKITERATOR_HPP_

#include "Component.hpp"
#include "NSIterator.hpp"

using namespace std;

namespace optframe {

// NSBlockIterator: iterates from blocks (parts) of the neighborhood structure
// these parts may share any characteristic that may help predicting the behavior of local optima

template<XSolution S, XEvaluation XEv = Evaluation<>>
class NSBlockIterator : public Component
{
public:
   virtual ~NSBlockIterator()
   {
   }

   virtual void first() = 0;
   virtual void next() = 0;
   virtual bool isDone() = 0;
   virtual NSIterator<S, XEv>& current() = 0;

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":NSBlockIterator";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>>
class DefaultNSBlockIterator : public NSBlockIterator<S, XEv>
{
protected:
   NSIterator<S, XEv>* it;

public:
   DefaultNSBlockIterator(NSIterator<S, XEv>& _it)
     : it(&_it)
   {
   }

   virtual ~DefaultNSBlockIterator()
   {
   }

   void first()
   {
   }

   void next()
   {
      it = nullptr;
   }

   bool isDone()
   {
      return it == nullptr;
   }

   virtual NSIterator<S, XEv>& current()
   {
      return *it;
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":NSBlockIterator";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual std::string toString() const override
   {
      return id();
   }
};

}

#endif //OPTFRAME_NSBLOCKITERATOR_HPP_
