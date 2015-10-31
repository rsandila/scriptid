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
#include <iostream>
#include <iosfwd>
#include <fstream>
#include "reservedword.h"

reservedword::reservedword( char *fname )
{
  installed=0;
  memset( &hashtab, 0, sizeof(hashtab) );
  if (!hcreate_r( RESERVEDWORD_HASHSIZE, &hashtab )) return;

  std::ifstream inp( fname );
  if (!inp) return;
 
  char buf[1024];
  ENTRY mentry, *ep;

  do
    {
      inp.get( buf, 1024, '\n' );
      for (int cnt=0;cnt<strlen( buf );cnt++) 
	{
	  buf[cnt]=std::toupper( buf[cnt] );
	}
      if (!inp.fail())
	{
	  wordlist.push_back( buf );
	  mentry.key=(char *)(wordlist.back().c_str());
	  mentry.data=(char *)(wordlist.back().c_str());
	  hsearch_r( mentry, ENTER, &ep, &hashtab );
	}
      else
	{ // skip empty lines
	  inp.clear();
	}
      char c;
      inp.get(c);
    } while (!inp.eof() && inp.good());
  installed=1;
}

reservedword::~reservedword()
{
  hdestroy_r( &hashtab );
}

int reservedword::isReservedWord( std::string word )
{
  if (!isOk() || word.size()>=1024) return( -1 );
  char keytemp[1024];
  ENTRY *ep, msearch;
  for (int cnt=0;cnt<word.size();cnt++)
    {
      keytemp[cnt]=std::toupper( word[cnt] );
    }
  keytemp[word.size()]=0;
  msearch.key=(char *)keytemp;
  ep=NULL;
  hsearch_r( msearch, FIND, &ep, &hashtab );
  if (ep) return( 1 );
  return( 0 );
}

void reservedword::dump( std::ostream &outp )
{
  outp << "ReservedWord::Wordlist" << std::endl;
  for (int cnt=0;cnt<wordlist.size();cnt++)
    outp << wordlist[cnt] << std::endl;
}

std::string reservedword::getReservedWord( unsigned idx )
{
  if (idx>wordlist.size()) return( "" );
  return( wordlist[idx] );
}
