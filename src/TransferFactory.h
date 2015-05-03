//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferFactory.h
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

#ifndef TRANSFERFACTORY_H
#define TRANSFERFACTORY_H

#include <string>

#include "common.h"
#include "Transfer.h"


class TransferFactory
{
public:

public:
   static Transfer *createInstance(Job::pattern_t pattern,
											  unsigned char userPattern,
                                   int fd, 
                                   unsigned char *buffer, 
                                   capacity_t maxBufferSize,
                                   capacity_t id,
                                   u64_t seed);
             
private:
   // Hide default methods.
   TransferFactory();          
   TransferFactory(const Transfer&); 
   TransferFactory& operator=(const TransferFactory &transfer);
};

#endif  // TRANSFERFACTORY_H
