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
#include "seperator.h"

seperator::seperator( char *fname )
{
  installed=0;
  memset( list, SEPERATOR_TYPE_NONE, SEPERATOR_LIST_SIZE );

  std::ifstream inp( fname );
  if (!inp) return;
 
  char buf[1024];
  do
    {
      inp.get( buf, 1024, '\n' );
      char c;
      inp.get(c);
      if (c=='\n')
	{ // end of line removed
	  char *tmp;
	  tmp=buf;
	  while (*tmp) 
	    {
	      list[*tmp]=SEPERATOR_TYPE_CUSTOM;
	      tmp+=1;
	    }
	}
      else
	{ // line too long?
	  return;
	}
    } while (!inp.eof());
  list['\n']=SEPERATOR_TYPE_BUILTIN;
  list['\t']=SEPERATOR_TYPE_BUILTIN;
  list['\r']=SEPERATOR_TYPE_BUILTIN;
  list[' ']=SEPERATOR_TYPE_BUILTIN;
  installed=1;
}

seperator::~seperator()
{
}

int seperator::getNextWordandSeperator( char *in, char *word, unsigned wordlen, char *seperator, unsigned seperatorlen, int *sepType, char **nextword )
{
  char *wrdtmp, *septmp, *sepst;
  int result;
  
  if (!nextword) return( -1 );
  wrdtmp=septmp=NULL;

  result=getNextWordandSeperator( in, &wrdtmp, &septmp, sepType );
  if (result) return( result );

  sepst=wrdtmp;
  if (!sepst) sepst=in;
  if (wrdtmp && (wrdtmp-in)>=wordlen) return( -1 );
  if ((septmp-sepst)>=seperatorlen) return( -1 );
  
  memset( word, 0, wordlen );
  memset( seperator, 0, seperatorlen );

  if (wrdtmp) memcpy( word, in, wrdtmp-in+1 );
  memcpy( seperator, sepst+1, septmp-sepst-1 );
  for (int i=0;i<strlen( word );i++) word[i]=std::toupper( word[i] );
  *nextword=septmp;
  return( 0 );
}

int seperator::getNextWordandSeperator( char *in, char **word_end, char **seperator_end, int *sepType )
{
  if (!isOk() || !in || !word_end || !seperator_end || !sepType) return( -1 );
  char *tmp;
  tmp=in;
  *word_end=NULL;
  *seperator_end=NULL;
  *sepType=SEPERATOR_TYPE_NONE;
  while (*tmp)
    {
      if (list[*tmp])
	{
	  *word_end=tmp-1;  
	  if (*word_end<in)
	    { // string started with seperator
	      *word_end=NULL;
	    }
	  break;
	}
      tmp+=1;
    }
  // either end of string or found a seperator
  if (!(*tmp))
    { // end of string or word with no seperator at end
      if (in!=tmp)
	{ // there has been some word
	  *word_end=tmp-1;
	  *seperator_end=*word_end+1;
	  return( 0 );
	}
      return( -1 );
    }
  *sepType=list[*tmp];
  while (*tmp && list[*tmp])
    { // while seperators
      if (list[*tmp]>(*sepType)) *sepType=list[*tmp];
      tmp+=1;
    }
  *seperator_end=tmp;
  return( 0 );
}

int seperator::removeBuiltinSeperators( char *in, char *out, unsigned outlen )
{
  if (!isOk() || !in || !out || !outlen) return( -1 );
  char *itmp, *otmp;
  itmp=in;
  otmp=out;
  while (*itmp && (otmp<(out+outlen)))
    {
      if (list[*itmp]!=SEPERATOR_TYPE_BUILTIN)
	{
	  *otmp=*itmp;
	  otmp+=1;
	}
      itmp+=1;
    }
  *otmp=0;
  return( 0 );
}

void seperator::dump( std::ostream &outp )
{
  outp << "Seperators" << std::endl;
  for (int cnt=0;cnt<SEPERATOR_LIST_SIZE;cnt+=16)
    {
      outp.width( 4 );
      //      outp.setf(std::ios::hex,std::ios::basefield);
      outp << std::hex << cnt;
      //      outp.setf(std::ios::dec,std::ios::basefield);
      outp << " " << (int)list[cnt   ] << " " << (int)list[cnt +1];
      outp << " " << (int)list[cnt+ 2] << " " << (int)list[cnt+ 3];
      outp << " " << (int)list[cnt+ 4] << " " << (int)list[cnt+ 5];
      outp << " " << (int)list[cnt+ 6] << " " << (int)list[cnt+ 7];
      outp << " " << (int)list[cnt+ 8] << " " << (int)list[cnt+ 9];
      outp << " " << (int)list[cnt+10] << " " << (int)list[cnt+11];
      outp << " " << (int)list[cnt+12] << " " << (int)list[cnt+13];
      outp << " " << (int)list[cnt+14] << " " << (int)list[cnt+15];
      outp << std::endl;
    }
}
