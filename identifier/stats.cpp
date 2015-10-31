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
#include "stats.h"

stats::stats( std::string basedir, std::string language, std::string file )
{
  installed=0;
  sep=NULL;
  rw=NULL;
  parser=NULL;
  num_reserved_words=0;
  num_unique_reserved_words=0;
  tsbc=0.0;
  tbc=0.0;  
  tcsbc=0.0; 
  reserved_word_ratio=0.0;
  unique_reserved_word_ratio=0.0;
  unique_to_normal_reserved_word_ratio=0.0;

  std::string rwname, sepname;
  int result;
  std::string wordtmp;
  long num;
  std::map<char *,long,egstr> ::iterator mait;
  char *tmpchr;

  rwname=basedir;
  rwname+="/";
  rwname+=language;
  rwname+=".words";
  rw=new reservedword( (char *)(rwname.c_str()) );
  if (!rw || !rw->isOk()) return;
  sepname=basedir;
  sepname+="/";
  sepname+=language;
  sepname+=".seperator";
  sep=new seperator( (char *)(sepname.c_str()) );
  if (!sep || !sep->isOk()) return;
  parser=new parsefile( (char *)(file.c_str()), sep );
  if (!parser || !parser->isOk()) return;
  tsbc=parser->getTotalSeperatorByteCount();
  tbc=parser->getTotalCharacterCount();
  tcsbc=parser->getCustomSeperatorByteCount();
  // check how many uniquewords and how many words are reserved!
  for (int cnt=0;cnt<rw->getNumReservedWords();cnt++)
    {
      wordtmp=rw->getReservedWord( cnt );
      tmpchr=new char[wordtmp.size()+1];
      if (!tmpchr) return;
      strcpy( tmpchr, wordtmp.c_str() );
      reservedword_histogram.insert( std::pair<char *,long>( tmpchr, 0 ) );
    }
  if (rw->getNumReservedWords()!=reservedword_histogram.size())
    {
      std::cerr << rwname << " contains duplicate words! " << rw->getNumReservedWords() << " vs. " << reservedword_histogram.size() << std::endl;
    return;
    }
  for (int cnt=0;cnt<parser->getNumUniqueWords();cnt++)
    {
      result=parser->getUniqueWord( cnt, &wordtmp );
      if (!result)
	{
	  if (rw->isReservedWord( wordtmp ))
	    {
	      num_unique_reserved_words+=1;
	    }
	}
    }
  for (int cnt=0;cnt<parser->getNumWords();cnt++)
    {
      result=parser->getWord( cnt, &wordtmp );
      if (!result)
	{
	  if (rw->isReservedWord( wordtmp ))
	    {
	      num_reserved_words+=rw->isReservedWord( wordtmp ); 
	      mait=reservedword_histogram.find( (char *)(wordtmp.c_str()) );
	      if (mait==reservedword_histogram.end())
		{ // should never happen!
		  std::cerr << "Really weird thing happened with reserved words" << std::endl;
		  return;
		}
	      (*mait).second+=1;
	    }
	}
    }
  if (parser->getNumWords())
    {
      reserved_word_ratio=(double)((double)num_reserved_words/(double)parser->getNumWords());
      unique_reserved_word_ratio=(double)((double)num_unique_reserved_words/(double)parser->getNumWords());
    }
  if (reserved_word_ratio)
    {
      unique_to_normal_reserved_word_ratio=unique_reserved_word_ratio/reserved_word_ratio;
    }
  installed=1;
}

stats::~stats()
{
  std::map<char *,long,egstr>::iterator mit;
  mit=reservedword_histogram.begin();
  while (mit!=reservedword_histogram.end())
    {
      if ((*mit).first) delete [](*mit).first;
      mit++;
    }
  reservedword_histogram.clear();
  if (sep) delete sep;
  sep=NULL;
  if (rw) delete rw;
  rw=NULL;
  if (parser) delete parser;
  parser=NULL;
}

void stats::dump()
{
}

int stats::getReservedWordsHistogram( unsigned *num_elem, double **histo )
{
  double *x, total;
  std::map<char *,long,egstr>::iterator mit;
  
  if (!isOk() || !num_elem || !histo) return( -1 );
  *histo=NULL;
  *num_elem=reservedword_histogram.size();
  x=new double[*num_elem];
  if (!x) return( -1 );
  if (reservedword_histogram.empty())
    {
      for (unsigned cnt=0;cnt<*num_elem;cnt++)
	{
	  x[cnt]=0.0;
	}
      *histo=x;
      return( 0 );
    }
  mit=reservedword_histogram.begin();
  total=0.0;
  for (unsigned cnt=0;cnt<*num_elem;cnt++)
    {
      //      std::cout << (*mit).first << ", " << (*mit).second << std::endl;
      x[cnt]=(double)((*mit).second);
      total+=x[cnt];
      mit++;
    }
  //  std::cout << "total = " << total << std::endl;
  if (total)
    {
      for (unsigned cnt=0;cnt<*num_elem;cnt++)
	{
	  x[cnt]/=total;
	}
    }
  *histo=x;
  return( 0 );
}

int stats::getReservedWordsHistogram( std::string *str )
{
  double *x;
  unsigned n;
  char buf[1024];

  str->clear();
  if (getReservedWordsHistogram( &n, &x ))
    {
      return( -1 );
    }
  for (unsigned int cnt=0;cnt<n;cnt++)
    {
      snprintf( buf, 1024, "%f", x[cnt] );
      if (cnt) (*str)=(*str)+", ";
      (*str)=(*str)+buf;
    }
  freeReservedWordsHistogram( x );
  return( 0 );
}
