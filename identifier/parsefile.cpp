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
#include <algorithm>
#include "parsefile.h"

inline bool lt_stringcmp(std::string c1, std::string c2) { return ( c1.compare( c2 ) ); }

parsefile::parsefile( char *fname, seperator *sep )
{
  char buf[1024];
  char word[1024], seper[1024];
  char *endstr;
  int result;
  int sepType;
  char c;

  total_seperator_count=0;
  custom_seperator_count=0;
  total_character_count=0;
  installed=0;
  mysep=sep;

  if (!fname || !sep || !sep->isOk()) return;

  std::ifstream inp( fname );
  if (!inp) return;
  
  do
    {
      inp.get( buf, 1024, '\n' );
      if (!inp.fail())
	{
	  total_character_count+=strlen( buf );
	  //	  std::cerr << "STR[" << buf << "]" << std::endl;
	  endstr=buf;
	  do 
	    {
	      result=mysep->getNextWordandSeperator( endstr, word, 1024, seper, 1024, &sepType, &endstr );
	      if (!result)
		{

		  if (strlen( word )>0)
		    {
		      for (int i;i<strlen( word );i++) word[i]=std::toupper( word[i] );
		      //		      std::cerr << "W[" << word << "]";
		      words.push_back( word );
		    }
		  if (sepType==SEPERATOR_TYPE_CUSTOM)
		    {
		      int res2;
		      char cseper[1024];
		      total_seperator_count+=strlen( seper );
		      res2=mysep->removeBuiltinSeperators( seper, cseper, 1024 );
		      //		      std::cerr << "S[" << seper << "] " << sepType;
		      if (!res2) 
			{
			  custom_seperator_count+=strlen( cseper );
			  seperators.push_back( cseper );
			}
		    }
		  //		  std::cerr << std::endl;
		}
	    } while (!result && *endstr);
	}
      else
	{
	  inp.clear();
	}
      inp.get( c ); // remove \n at end of line
      //      std::cerr << "c = " << c << std::endl;
    } while (!inp.eof()  && !inp.fail() && !inp.bad());
  // fill uniquewords
  std::vector<std::string> tmpwords;

  tmpwords=words;
  sort( tmpwords.begin(), tmpwords.end() );
  //  sort( words.begin(), words.end() );

  for (int cnt=0;cnt<tmpwords.size()-1;cnt++)
    if (tmpwords[cnt]!=tmpwords[cnt+1])
      {
	uniquewords.push_back( tmpwords[cnt] );
      }
  if (tmpwords.size()>1)
    {
      if (tmpwords.back()!=uniquewords.back())
	{
	  uniquewords.push_back( tmpwords.back() );
	}
    }
  installed=1;
}

parsefile::~parsefile()
{
}

int parsefile::getNumWords()
{
  if (!isOk()) return( -1 );
  return( words.size() );
}

int parsefile::getWord( unsigned index, std::string *word )
{
  if (!isOk() || index>=getNumWords() || !word) return( -1 );
  (*word)=words[index];
  return( 0 );
}

int parsefile::getNumUniqueWords()
{
  if (!isOk()) return( -1 );
  return( uniquewords.size() );
}

int parsefile::getUniqueWord( unsigned index, std::string *word )
{
  if (!isOk() || index>=getNumUniqueWords() || !word) return( -1 );
  (*word)=uniquewords[index];
  return( 0 );
}


int parsefile::getNumSeperators()
{
  if (!isOk()) return( -1 );
  return( seperators.size() );
}

int parsefile::getSeperator( unsigned index, std::string sep )
{
  if (!isOk() || index>=getNumSeperators()) return( -1 );
  sep=seperators[index];
  return( 0 );
}

void parsefile::dump(  std::ostream &outp )
{
  outp << "Parsefile::Seperators" << std::endl;
  for (int cnt=0;cnt<getNumSeperators();cnt++)
    {
      outp << "[" << seperators[cnt] << "]" << std::endl;
    }
  outp << "Parsefile::Words" << std::endl;
  for (int cnt=0;cnt<getNumWords();cnt++)
    {
      outp << "[" << words[cnt] << "]" << std::endl;
    }
  outp << "Parsefile::UniqueWords" << std::endl;
  for (int cnt=0;cnt<getNumUniqueWords();cnt++)
    {
      outp << "[" << uniquewords[cnt] << "]" << std::endl;
    }
  outp << "Parsefile::dump fin." << std::endl;
}
