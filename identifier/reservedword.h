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
#ifndef __RESERVEDWORD_H__
#define __RESERVEDWORD_H__

#include <vector>

//#define _GNU_SOURCE
#include <search.h>

/* The max number of reserved words that can be handled */
#define RESERVEDWORD_HASHSIZE 10240

class reservedword
{
 public:
  reservedword( char *fname );
  virtual ~reservedword();
  int isOk() { return( installed ); };
  int isReservedWord( std::string word );
  std::string getReservedWord( unsigned idx );
  unsigned getNumReservedWords() { return( wordlist.size() ); };
  void dump( std::ostream &outp );
 protected:
  unsigned num_reserved_words;
  struct hsearch_data hashtab;
  std::vector<std::string> wordlist;
 private:
  int installed;
};
#endif
