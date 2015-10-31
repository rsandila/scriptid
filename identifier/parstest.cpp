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
#include <cstdlib>
#include <iostream>
#include <iosfwd>
#include "seperator.h"
#include "parsefile.h"
#include "reservedword.h"

void sep_dump_test( seperator *sep )
{
  sep->dump( std::cout );
}

void sep_string_test( seperator *sep )
{
  char *septest_str="  \tThis is a test string with<= some junk+in it\n";
  char word[1024], seper[1024], *endstr;
  int sepType;
  int result;

  std::cout << "Seperator String Test" << std::endl;
  endstr=septest_str;
  do
    {
      result=sep->getNextWordandSeperator( endstr, word, 1024, seper, 1024, &sepType, &endstr );
      if (!result)
	{
	  std::cout << "Word = [" << word << "] Seperator = [" << seper << "] Type = " << sepType << std::endl;
	}
    } while (!result);
  std::cout << "End of Seperator String Test" << std::endl;
}

void parsefile_test( seperator *septest )
{
  parsefile pftest( "parstest.cpp", septest );
  if (!pftest.isOk())
    {
      std::cerr << "Parsefile did not initialize" << std::endl;
      return;
    }
  pftest.dump( std::cout );
}

void reservedword_test()
{
  reservedword rw( "../data/vbscript.words" );
  if (!rw.isOk())
    {
      std::cerr << "ReservedWords did not initialize" << std::endl;
      return;
    }
  rw.dump( std::cout );
  if (rw.isReservedWord( "ninkonpoop" ))
    {
      std::cerr << "First ReservedWord test failed" << std::endl;
    }
  else
    {
      std::cout << "ReservedWord:BadWord worked" << std::endl;
    }
  if (rw.isReservedWord( "Class_Initialize" ))
    {
      std::cout << "ReservedWord:GoodWord worked" << std::endl;
    }
  else
    {
      std::cout << "ReservedWord:GoodWord failed" << std::endl;
    }
}

int main( int argc, char *argv )
{
  seperator septest( "../data/vbscript.seperator" );
  if (!septest.isOk())
    {
      std::cerr << "Failed to load ../data/vbscript.seperator" << std::endl;
      return( 1 );
    }
  sep_dump_test( &septest );
  sep_string_test( &septest );
  parsefile_test( &septest );
  reservedword_test();
  return( 0 );
}

/* test test test test test test */
