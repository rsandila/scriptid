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
#ifndef __SCRIPTID_H__
#define __SCRIPTID_H__

#define scriptid_default_lib_dir "/usr/local/lib/scriptid";
#define scriptid_default_language "vbscript"

/* This function is used to identify a specific file as either script or plain text
   Parameters:
      libdir - The path to all the shared data files used by this class.
      language - The name of the language to identify
      file - The file to identify
      verbose - 0 no error messages printed to std::cerr. 1 all messages printed.
   Return Values:
      0 - Text file
      1 - Script file
     -1 - Error
 */
extern "C" int identify( char *libdir, char *language, char *file, int verbose );

/*
  This function is used to determine whether a file is a text file or not. It can
  be used to filter files before identify is called
  Parameters:
    fname - The full path to the file to be checked
 Return Values:
     0 - Not text
     1 - Text 
    -1 - Error 
 */
extern "C" int isTextFile( char *fname );
#endif
