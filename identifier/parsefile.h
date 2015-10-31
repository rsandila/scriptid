/*
 * Copyright (C) 2003 Robert Sandilands (Pty) Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
 */
#ifndef __PARSEFILE_H__
#define __PARSEFILE_H__

#include <vector>
#include "seperator.h"

class parsefile
{
 public:
  parsefile( char *fname, seperator *sep );
  virtual ~parsefile();
  int isOk() { return( installed ); };
  int getNumUniqueWords();
  int getUniqueWord( unsigned intdex, std::string *word );
  int getNumWords();
  int getWord( unsigned index, std::string *word );
  int getNumSeperators();
  int getSeperator( unsigned index, std::string sep );
  unsigned getTotalSeperatorByteCount() { return( total_seperator_count ); };
  unsigned getCustomSeperatorByteCount() { return( custom_seperator_count ); };
  unsigned getTotalCharacterCount() { return( total_character_count ); };
  void dump( std::ostream &outp );
 protected:
  seperator *mysep;
  std::vector<std::string>uniquewords;
  std::vector<std::string>words;
  std::vector<std::string>seperators;
  unsigned total_seperator_count;
  unsigned custom_seperator_count;
  unsigned total_character_count;
 private:
  int installed;
};
#endif
