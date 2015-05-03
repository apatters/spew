//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Random.h
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

//
// Use Park & Miller params for linear congruential method random (LCM) with
// Schrage's method to prevent overflow.
//

#ifndef RANDOM_H
#define RANDOM_H

#include <string>

#include "common.h"


class Random
{

public:

public:
   Random();
   Random(u32_t seed);
   Random(Random &rhs);

   u32_t getSeed();
   void setSeed(u32_t newSeed);
   
   u32_t getRandom32();
   u32_t getRandom32(u32_t max);
   u64_t getRandom64();
   u64_t getRandom64(u64_t max);

   ~Random();

private:
   unsigned long mPrevRandom;
};

#endif  // RANDOM_H
