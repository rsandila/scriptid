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
#ifndef __SEPERATOR_H__
#define __SEPERATOR_H__

#define SEPERATOR_LIST_SIZE 256

#define SEPERATOR_TYPE_CUSTOM  2
#define SEPERATOR_TYPE_BUILTIN 1
#define SEPERATOR_TYPE_NONE    0

class seperator
{
 public:
  seperator( char *fname );
  virtual ~seperator();
  int isOk() { return( installed ); };
  int getNextWordandSeperator( char *in, char **word_end, char **seperator_end, int *sepType );
  int getNextWordandSeperator( char *in, char *word, unsigned wordlen, char *seperator, unsigned seperatorlen, int *sepType, char **nextword );
  int removeBuiltinSeperators( char *in, char *out, unsigned outlen );
  void dump( std::ostream &outp );
 protected:
  char list[SEPERATOR_LIST_SIZE];
 private:
  int installed;
};
#endif
