/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "db/modest/StateMutatorChain.h"

using namespace db::modest;

StateMutatorChain::StateMutatorChain(
   StateMutator* g1, StateMutator* g2)
{
   mMutator1 = g1;
   mMutator2 = g2;
}

StateMutatorChain::StateMutatorChain(
   StateMutatorRef& g1, StateMutator* g2) :
   mMutatorReference1(g1)
{
   mMutator1 = &(*g1);
   mMutator2 = g2;
}

StateMutatorChain::StateMutatorChain(
   StateMutator* g1, StateMutatorRef& g2) :
   mMutatorReference2(g2)
{
   mMutator1 = g1;
   mMutator2 = &(*g2);
}

StateMutatorChain::StateMutatorChain(
   StateMutatorRef& g1, StateMutatorRef& g2) :
   mMutatorReference1(g1), mMutatorReference2(g2)
{
   mMutator1 = &(*g1);
   mMutator2 = &(*g2);
}

StateMutatorChain::~StateMutatorChain()
{
}

void StateMutatorChain::mutatePreExecutionState(State* s, Operation& op)
{
   mMutator1->mutatePreExecutionState(s, op);
   if(mMutator2 != NULL)
   {
      mMutator2->mutatePreExecutionState(s, op);
   }
}

void StateMutatorChain::mutatePostExecutionState(State* s, Operation& op)
{
   mMutator1->mutatePostExecutionState(s, op);
   if(mMutator2 != NULL)
   {
      mMutator2->mutatePostExecutionState(s, op);
   }
}
