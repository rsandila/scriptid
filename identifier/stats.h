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
#ifndef __STATS_H__
#define __STATS_H__

#include <iostream>
#include <iosfwd>
#include <fstream>
#include <string>
#include <map>
#include "parsefile.h"
#include "reservedword.h"

class stats
{
 public:
  stats( std::string basedir, std::string language, std::string file );
  virtual ~stats();
  int isOk() { return( installed ); };
  void dump();
  double getTotalNumberofSeperators() { return( tsbc ); };
  double getTotalNumberofCharacters() { return( tbc ); };
  double getTotalNumberofCustomSeperators() { return( tcsbc ); };
  double getReservedWordRatio() { return( reserved_word_ratio ); };
  double getUniqueReservedWordRatio() { return( unique_reserved_word_ratio ); };
  double getUniqueToNormalReservedWordRatio() { return( unique_to_normal_reserved_word_ratio ); };
  double getSeperators_CharactersRatio() { if (tbc) return( tsbc/tbc ); else return( 0 ); };
  double getCustomSeperators_CharactersRatio() { if (tbc) return( tcsbc/tbc ); else return( 0 ); };
  double getCustomSeperators_SeperatorsRatio() { if (tsbc) return( tcsbc/tsbc ); else return( 0 ); };
  void freeReservedWordsHistogram( double *histo ) { if (histo) delete []histo; };
  int getReservedWordsHistogram( unsigned *num_elem, double **histo );
  int getReservedWordsHistogram( std::string *str );
 protected:
  struct egstr
   {
    bool operator()(char *s1, char *s2) const
    {
      return strcmp(s1, s2)< 0;
    }
   };
  parsefile *parser;
  seperator *sep;
  reservedword *rw;
  std::map<char *,long,egstr> reservedword_histogram;
  double tsbc; // The total number of seperators
  double tbc;  // The total number of characters
  double tcsbc; // The total number of custom seperators
  long num_reserved_words;
  long num_unique_reserved_words;
  double reserved_word_ratio;
  double unique_reserved_word_ratio;
  double unique_to_normal_reserved_word_ratio;
 private:
  int installed;
};
#endif
