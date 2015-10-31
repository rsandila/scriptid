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
#include "scriptid.h"
#include "stats.h"
#include "../version.h"

/*
 Parameters for program
   --stat file - Prints stats for file
   --id   file - Identify file
   --language name - Language name to use for identifying things 
   --libdir name - Base directory for data files
   --version
   --help
 */

stats *initialize( char *libdir, char *language, char *file, int verbose );

void print_version()
{
  std::cout << PROJECTNAME << " version " << PROJECTVER << std::endl;
  std::cout << "Copyright @ 2003 Robert Sandilands <rsandila@netscape.net> License: LGPL" << std::endl << std::endl;
}

void print_usage( char *argv0 )
{
  print_version();
  std::cerr << "Usage: " << argv0 << "[--stat file|--id file] [--language name] [--libdir location]" << std::endl;
  std::cerr << "\tEither --stat or --id must always be defined." << std::endl;
  std::cerr << "\t\t--stat prints statistics of file, --id identifies file" << std::endl;
  std::cerr << "\t--language specifies which language must be used to identify." << std::endl;
  std::cerr << "\t\tThe default is " << scriptid_default_language << std::endl;
  std::cerr << "\t--libdir specifies the location of the data files. " << std::endl << "\t\t";
  std::cerr << scriptid_default_lib_dir;
  std::cerr << " is the default." << std::endl;
  std::cerr << "\t--help to see this screen" << std::endl;
  std::cerr << "\t--version to see the version number" << std::endl;
}

int parse_param( int argc, char **argv, char **stat, char **id, char **language, char **libdir )
{
  if (!stat || !id || !language || !libdir) return( -1 );

  *stat=NULL;
  *id=NULL;
  *language=scriptid_default_language;
  *libdir=scriptid_default_lib_dir;

  if (argc<2)
    {
      print_usage( argv[0] );
      return( -1 );
    }
  for (int cnt=1;cnt<argc;cnt++)
    {
      if (!strcmp( argv[cnt], "--stat" ))
	{
	  (*stat)=argv[cnt+1];
	  cnt+=1;
	  continue;
	}
      if (!strcmp( argv[cnt], "--id" ))
	{
	  (*id)=argv[cnt+1];
	  cnt+=1;
	  continue;
	}
      if (!strcmp( argv[cnt], "--language" ))
	{
	  (*language)=argv[cnt+1];
	  cnt+=1;
	  continue;
	}
      if (!strcmp( argv[cnt], "--libdir" ))
	{
	  (*libdir)=argv[cnt+1];
	  cnt+=1;
	  continue;
	}
      if (!strcmp( argv[cnt], "--help" ))
	{
	  print_usage( argv[0] );
	  return( -1 );
	}
      if (!strcmp( argv[cnt], "--version" ))
	{
	  print_version();
	  return( -1 );
	}
      std::cerr << "Unknown parameter " << argv[cnt] << std::endl;
      print_usage( argv[0] );
      return( -1 );
    }
  if ((!(*id) && !(*stat)) || ((*id) && (*stat)))
    {
      std::cerr << "Either of --id or --stat must be defined. Not both or none." << std::endl;
      return( -1 );
    }
  return( 0 );
}

int do_stat( stats *id, char *file )
{
  std::string histo;
  if (!file || !id || !id->isOk())
    {
      return( 3 );
    }
  std::cout << file;
  std::cout << ", " << id->getReservedWordRatio();
  std::cout << ", " << id->getUniqueReservedWordRatio();
  std::cout << ", " << id->getUniqueToNormalReservedWordRatio();
  std::cout << ", " << id->getSeperators_CharactersRatio();
  std::cout << ", " << id->getCustomSeperators_CharactersRatio();
  std::cout << ", " << id->getCustomSeperators_SeperatorsRatio();
  id->getReservedWordsHistogram( &histo );
  std::cout << ", " << histo;
  std::cout << std::endl;
  return( 0 );
}

int main( int argc, char **argv )
{
  char *stat, *id, *language, *libdir;
  int result;

  if (parse_param( argc, argv, &stat, &id, &language, &libdir ))
    {
      return( -1 );
    }
  if (stat)
    {
      stats *sid;
      result=isTextFile( stat );
      if (result<=0)
	{
	  std::cerr << stat << " is not a text file or does not exist." << std::endl;
	  return( 3 );
	}
      sid=initialize( libdir, language, stat, 1 );
      if (!sid)
	{
	  return( 2 );
	}

      if (stat)
	{
	  result=do_stat( sid, stat );
	  delete sid;
	  sid=NULL;
	  return( result );
	}
    }
  if (id)
    {
      char scriptname[1024];

      result=isTextFile( id );
      if (result<0)
	{
	  std::cerr << id << " does not exist." << std::endl;
	  return( -1 );
	}
      if (!result)
	{
	  std::cerr << id << " is a binary file." << std::endl;
	  return( 0 );
	}
      result=isHTMLwithScript( id, scriptname, 1024 );
      if (result<0)
	{
	  std::cerr << id << " caused error in isHTMLwithScript" << std::endl;
	}
      if (result>0)
	{
	  std::cout << "HTML with script language " << scriptname << " detected." << std::endl;
	  if (strcasestr( scriptname, language ))
	    {
	      return( 1 );
	    }
	  else
	    {
	      return( 0 );
	    }
	}
      result=identify( libdir, language, id, 1 );
      return( result );
    }
  return( 255 );
}
