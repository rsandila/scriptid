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
#include "../nn/nnwork.h"
#include "scriptid.h"

stats *initialize( char *libdir, char *language, char *file, int verbose )
{
  stats *tmp;
  tmp=new stats( libdir, language, file );
  if (!tmp)
    {
      if (verbose)
	{
	  std::cerr << "Unable to allocate stats" << std::endl;
	}
      return( NULL );
    }
  if (!tmp->isOk())
    {
      if (verbose)
	{
	  std::cerr << "Unable to initialize stats" << std::endl;
	}
      return( NULL );
    }
  return( tmp );
}

int do_id( char *libdir, char *language, stats *id, int verbose )
{
  double *h;
  unsigned num_elem, INPUT_SIZE;
  nnwork *trainer;
  std::string nnname;
  float *inp, outp;
  int result;

  nnname=libdir;
  nnname+='/';
  nnname+=language;
  nnname+=".nnw";

  INPUT_SIZE=0;
  trainer=new nnwork( (char *)nnname.c_str() );
  if (!trainer)
    {
      if (verbose)
	{
	  std::cerr << "Unable to allocate neural network" << std::endl;
	}
      return( 2 );
    }
  INPUT_SIZE=trainer->get_layersize( INPUT );
  if (!INPUT_SIZE)
    {
      if (verbose)
	{
	  std::cerr << "Neural network data file does not make sense" << std::endl;
	}
      delete trainer;
      return( 2 );
    }
  inp=new float[INPUT_SIZE];
  if (!inp)
    {
      if (verbose)
	{
	  std::cerr << "Unable to allocate temporary storage" << std::endl;
	}
      delete trainer;
      return( 2 );
    }
  inp[0]=id->getReservedWordRatio();
  inp[1]=id->getUniqueReservedWordRatio();
  inp[2]=id->getUniqueToNormalReservedWordRatio();
  inp[3]=id->getSeperators_CharactersRatio();
  inp[4]=id->getCustomSeperators_CharactersRatio();
  inp[5]=id->getCustomSeperators_SeperatorsRatio();
  if (id->getReservedWordsHistogram( &num_elem, &h )) 
    {
      if (verbose)
	{
	  std::cerr << "Unable to get histogram!" << std::endl;
	}
      delete trainer;
      delete []inp;
      return( 2 );
    }
  if (num_elem!=(INPUT_SIZE-6))
    {
      if (verbose)
	{
	  std::cerr << "Data files and neural network size differs " << num_elem << " vs. " << INPUT_SIZE - 6 << std::endl;
	}
      delete trainer;
      delete []inp;
      id->freeReservedWordsHistogram( h );
      return( 2 );
    }
  for (int cnt=0;cnt<num_elem;cnt++)
    inp[6+cnt]=h[cnt];
  id->freeReservedWordsHistogram( h );
  if (verbose>1)
    {
      int line;
      line=0;
      std::cout << "Neural Net Input: " << std::endl;
      for (int cnt=0;cnt<INPUT_SIZE;cnt++)
	{
	  std::cout << (double)inp[cnt] << ", ";
	  line+=1;
	  if (line>7)
	    {
	      line=0;
	      std::cout << std::endl;
	    }
	}
      std::cout << std::endl;
    }
  trainer->run( inp, &outp );
  if (verbose)
    {
      std::cout << "Neural Net Output: ";
      std::cout << outp;
    }
  if (outp>0.5) 
    {
      result=0;
      if (verbose)
	{
	  std::cout << " (Plain Text)";
	}
    }
  else 
    {
      result=1;
      if (verbose)
	{
	  std::cout << " (Script)";
	}
    }
  if (verbose)
    {
      std::cout << std::endl;      
    }
  return( result );
}

extern "C" int identify( char *libdir, char *language, char *file, int verbose )
{
  stats *sid;
  int result;

  if (!libdir || !language || !file) return( -1 );
  sid=initialize( libdir, language, file, verbose );
  if (!sid)
    {
      return( -1 );
    }
  result=do_id( libdir, language, sid, verbose );
  delete sid;
  sid=NULL;
  if (result>1) return( -1 );
  return( result );
}

extern "C" int isTextFile( char *fname )
{ // If less than 20% of the first 1kB is ascii it is a text file, otherwise binary
  FILE *inp;
  char buf[1024];
  int read, cnt, total;

  if (!fname) return( -1 );
  inp=fopen( fname, "rb" );
  if (!inp) return( -1 );
  read=fread( buf, 1, 1024, inp );
  fclose( inp );
  if (read<0) return( -1 );
  if (!read) return( 0 );
  for (total=0,cnt=0;cnt<read;cnt++)
    {
      if (isupper( buf[cnt] ) || islower( buf[cnt] ) || isspace( buf[cnt] ) || ispunct( buf[cnt] ) || 
	  isdigit( buf[cnt] )) total+=1;
    }
  if (((float)total/(float)read)<0.8) return( 0 );
  return( 1 );
}

extern "C" int isHTMLwithScript( char *fname, char *scriptname, unsigned scriptname_length )
{
  FILE *inp;
  char *tmppos;
  char *language;
  char *typel;
  char *fbuf;
  long fsize;
  long read;

  inp=fopen( fname, "rb" );
  if (!inp) return( -1 );
  fsize=ftell( inp );
  fseek( inp, 0L, SEEK_END );
  fsize=ftell( inp ) - fsize;
  fseek( inp, 0L, SEEK_SET );
  fbuf=new char[fsize+1];
  if (!fbuf) { fclose( inp ); return( -1 ); };
  read=fread( fbuf, 1, fsize, inp );
  fclose( inp );
  if (read!=fsize)
    {
      delete []fbuf;
      return( -1 );
    }
  if (strcasestr( fbuf, "<html" ) && (strcasestr( fbuf, "<body" ) || strcasestr( fbuf, "<head" )))
    { // most likely html
      tmppos=strcasestr( fbuf, "<script " );
      if (tmppos)
	{ // contains some script
	  language=strcasestr( tmppos, "language=" );
	  typel=strcasestr( tmppos, "type=" );
	  if (typel)
	    {
	      typel+=5;
	      tmppos=typel;
	      if ((*tmppos)=='"') { tmppos+=1; typel+=1; };
	      while ((isalpha( *tmppos ) || (*tmppos)=='/') && (*tmppos)!=0) tmppos+=1;
	      if (!(*tmppos))
		{
		  delete []fbuf;
		  return( 0 );
		}
	      if ((tmppos - typel+1)>scriptname_length)
		{
		  delete []fbuf;
		  return( -1 );
		}
	      memset( scriptname, 0, scriptname_length );
	      memcpy( scriptname, typel, tmppos - typel );
	      delete []fbuf;
	      return( 1 );
	    }
	  if (language)
	    {
	      language+=9;
	      tmppos=language;
	      if ((*tmppos)=='"') { tmppos+=1; language+=1; };
	      while ((isalpha( *tmppos ) || (*tmppos)=='/') && (*tmppos)!=0) tmppos+=1;
	      if (!(*tmppos))
		{
		  delete []fbuf;
		  return( 0 );
		}
	      if ((tmppos - language+1)>scriptname_length)
		{
		  delete []fbuf;
		  return( -1 );
		}
	      memset( scriptname, 0, scriptname_length );
	      memcpy( scriptname, language, tmppos - language );
	      delete []fbuf;
	      return( 1 );
	    }
	  // if neither language or type specified? Most likely microsoft, so it probably contains vbscript
	  delete []fbuf;
	  snprintf( scriptname, scriptname_length, "unknown" );
	  return( 1 );
	}
      delete []fbuf;
      return( 0 );
    }
  delete []fbuf;
  return( 0 );
}


