//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Randon.cpp
//
// Spew
//
// Copyright (C) 2004 Hewlett-Packard Corp.
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation version 2 of the License.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.

namespace std {} using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>

#include "common.h"
#include "Random.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local constants  //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const u32_t LCM_A = 42871;
const u32_t LCM_C = 0;
const u32_t LCM_M = 0xffffffff;
const u32_t SCHRAGES_Q = LCM_M / LCM_A;
const u32_t SCHRAGES_R = LCM_M % LCM_A;


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Member functions  /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//////////////////////////  Random::Random()  /////////////////////////////////
Random::Random()
{
   mPrevRandom = (u32_t)time((time_t *)NULL);
}


//////////////////////////  Random::Random(capacity_t seed)  //////////////////
Random::Random(u32_t seed)
{
    mPrevRandom = seed;
}


//////////////////////////  Random::Random(Random &rhs)  //////////////////////
Random::Random(Random &rhs)
{
   mPrevRandom = rhs.mPrevRandom;
}


//////////////////////////  Random::getSeed()  ////////////////////////////////
u32_t Random::getSeed()
{
   return mPrevRandom;
}


//////////////////////////  Random::setSeed()  ////////////////////////////////
void Random::setSeed(u32_t newSeed)
{
   mPrevRandom = newSeed;
}


//////////////////////////  Random::getRandom32() /////////////////////////////
u32_t Random::getRandom32()
{
   u32_t schrages_k = mPrevRandom / SCHRAGES_Q;
   u32_t newRandom = (LCM_A * (mPrevRandom - schrages_k * SCHRAGES_Q)) - (SCHRAGES_R * schrages_k);

   mPrevRandom = newRandom;
   return newRandom;
}


//////////////////////////  Random::getRandom32(u32_t max)  ///////////////////
u32_t Random::getRandom32(u32_t max)
{
   return this->getRandom32() % max;
}


//////////////////////////  Random::getRandom64() /////////////////////////////
u64_t Random::getRandom64()
{
   u64_t msb = (u64_t)this->getRandom32();
   u64_t lsb = (u64_t)this->getRandom32();

   return (msb << (sizeof(u32_t) * 8)) + lsb;
}


//////////////////////////  Random::getRandom64(u64_t max)  ///////////////////
u64_t Random::getRandom64(u64_t max)
{
   return this->getRandom64() % max;
}


//////////////////////////  Random::~Random()  ////////////////////////////////
Random::~Random()
{
}
