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
#include <string>
#include <vector>
#include <time.h>
#include "nnwork.h"
#include "nnparam.h"

#define __RANDOM_ORDER__

int load_data( float ***input, int *num_input, float **output )
{
  float *inptemp;
  float *outptemp;
  std::vector<float> tempstore_bad;
  std::vector<float> tempstore_good;
  char buf[1024000];
  int pos, num_good, num_bad;
  std::string strpart;
  std::string strwork;
  float x;
  char c;
  long line;

  if (!input || !output || !num_input) return( 1 );
  std::ifstream goodinp( GOODNAME );
  std::ifstream badinp( BADNAME );
  line=0;
  num_good=0;
  num_bad=0;
  std::cout << "Counting lines... ";
  std::cout.flush();
  do 
    {
      badinp.get( buf, 1024000, '\n' );
      if (!badinp.fail())
	{
	  line+=1;
	  num_bad+=1;
	}
      else
	{
    	  badinp.clear();
	}
      badinp.get( c ); // remove \n at end of line
      //      std::cerr << "c = " << c << std::endl;
    } while (!badinp.eof()  && !badinp.fail() && !badinp.bad());
  if (!num_bad)
    {
      std::cerr << BADNAME << " is empty!" << std::endl;
      return( 6 );
    }
  do 
    {
      goodinp.get( buf, 1024000, '\n' );
      if (!goodinp.fail())
	{
	  line+=1;
	  num_good+=1;
	}
      else
	{
    	  goodinp.clear();
	}
      goodinp.get( c ); // remove \n at end of line
      //      std::cerr << "c = " << c << std::endl;
    } while (!goodinp.eof()  && !goodinp.fail() && !goodinp.bad());
  if (!num_good)
    {
      std::cerr << GOODNAME << " is empty!" << std::endl;
      return( 7 );
    }  
  std::cout << line << std::endl;
  std::cout << num_good << " good samples, " << num_bad << " bad samples" << std::endl;
  (*num_input)=line;
  line=0;
  (*input)=new float *[*num_input];
  if (!(*input)) return( 3 );
  (*output)=new float[*num_input];
  if (!(*output)) return( 4 );
  goodinp.close();
  badinp.close();
  goodinp.open( GOODNAME );
  badinp.open( BADNAME );
  do
    {
      badinp.get( buf, 1024000, '\n' );
      if (!badinp.fail())
	{
	  (*output)[line]=0.1;
	  (*input)[line]=new float[INPUT_SIZE];
	  if (!(*input)[line])
	    {
	      std::cerr << "Out of memory line " << line << std::endl;
	      return( 5 );
	    }
	  pos=0;
	  strwork=buf;
	  for (int cnt=0;cnt<(INPUT_SIZE);cnt++)
	    {
	      pos=strwork.find( ",", pos ); // find 
	      if (pos==strwork.npos) 
		{
		  std::cerr << BADNAME << " line too short! " << line << ", " << cnt << std::endl;
		  return( 3 );
		}
	      strpart=strwork.substr( pos+1, strwork.size()-(pos+1) );
	      x=atof( strpart.c_str() );
	      if (isnan(x))
		{
		  std::cerr << "NAN detected in " << BADNAME << " line " << line << std::endl;
		  return( 1 );
		}	  
	      (*input)[line][cnt]=x;
	      pos+=1;
	    }
	  line+=1;
	}
      else
	{
    	  badinp.clear();
	}
      badinp.get( c ); // remove \n at end of line
    } while (!badinp.eof()  && !badinp.fail() && !badinp.bad());
  do
    {
      goodinp.get( buf, 1024000, '\n' );
      if (!goodinp.fail())
	{
	  (*output)[line]=0.9;
	  (*input)[line]=new float[INPUT_SIZE];
	  if (!(*input)[line])
	    {
	      std::cerr << "Out of memory line " << line - num_bad << std::endl;
	      return( 5 );
	    }
	  pos=0;
	  strwork=buf;
	  for (int cnt=0;cnt<(INPUT_SIZE);cnt++)
	    {
	      pos=strwork.find( ",", pos ); // find 
	      if (pos==strwork.npos) 
		{
		  std::cerr << GOODNAME << " line too short! " << line - num_bad << ", " << cnt << std::endl;
		  return( 3 );
		}
	      strpart=strwork.substr( pos+1, strwork.size()-(pos+1) );
	      x=atof( strpart.c_str() );
	      if (isnan(x))
		{
		  std::cerr << "NAN detected in " << GOODNAME << " line " << line << std::endl;
		  return( 1 );
		}	  
	      (*input)[line][cnt]=x;
	      pos+=1;
	    }
	  line+=1;
	}
      else
	{
    	  goodinp.clear();
	}
      goodinp.get( c ); // remove \n at end of line
    } while (!goodinp.eof()  && !goodinp.fail() && !goodinp.bad());
  if (line!=(num_good+num_bad))
    {
      std::cerr << "Short reading actual data by " << line << std::endl;
      return( 5 );
    }
  return( 0 );
}

void cleanup( float **inp, int inp_size, float *outp )
{
  if (inp)
    {
      for (int cnt=0;cnt<inp_size;cnt++)
	{
	  if (inp[cnt]) delete [](inp[cnt]);
	}
      delete []inp;
    }
  if (outp) delete []outp;
}

int learn_net( float **inp, int num_input, float *outp, char *fname )
{
  float run_outp;
  int *used;
  float tot_err, min_err;
  int learn_cycle, learn_item, find_iter;
  float last_error;
#ifdef __TIMING__
  clock_t start, end;
#endif
  last_error=1.0;
  tot_err=2.0;
  nnwork trainer( INPUT_SIZE, MIDDLE_SIZE, OUTPUT_SIZE );

  if (fname)
    {
      std::cout << "Load training data " << fname << std::endl;
      trainer.load( fname );
    }
  learn_cycle=0;
  //  learn_rate=0.00001;
  std::cout << "Starting training with " << num_input << " entries" << std::endl;
  used=NULL;
  min_err=-1;
  do
    {
#ifdef __TIMING__
      start=clock();
#endif
#ifdef __RANDOM_ORDER__
      if (used) delete []used;
      used=new int[num_input];
      if (!used)
	{
	  std::cerr << "Out of memory!" << std::endl;
	  return( 2 );
	}
      memset( used, 0, sizeof(int)*num_input ); 
#endif
      last_error=tot_err;
      tot_err=0;

      for (int cnt=0;cnt<num_input;cnt++)
	  {
#ifdef __RANDOM_ORDER__
	    find_iter=0;
	    do 
	      {
		learn_item=(int)ceil(((float)rand()/(float)RAND_MAX)*(num_input-1));
		find_iter+=1;
	      } while (used[learn_item] && find_iter<50);
	    if (find_iter>=50)
	      {
		for (learn_item=0;learn_item<num_input && used[learn_item];learn_item++);
	      }
	    used[learn_item]=1;
#else
	    learn_item=cnt;
#endif
	  //	  std::cout << learn_item << std::endl;
	  //	  std::cout << "item " << cnt << std::endl;
	  tot_err+=trainer.train( inp[learn_item], &(outp[learn_item]), DELTA, LEARN_ERROR, 1.0 /*(float)random()/(float)RAND_MAX*/ );
	  /*	  if (!(cnt%100)) 
	    {
	      std::cout << ".";
	      std::cout.flush();
	      }*/
	}
      //      tot_err=0;
      //      std::cout << ":";
      //      std::cout.flush();
      //      last_error=tot_err;
      //      for (int cnt=0;cnt<num_input;cnt++)
      //	{
      //	  trainer.run( inp[cnt], &run_outp );
      //	  tot_err+=fabsf( run_outp - outp[cnt] );
	  /*	  if (!(cnt%100)) 
	    {
	      std::cout << ",";
	      std::cout.flush();
	      }*/
      //	}
      if (min_err==-1) min_err=tot_err;
      //      std::cout << std::endl;
#ifdef __TIMING__
      end = clock();
#endif
      std::cout << "Learn Cycle: " << ++learn_cycle;
#ifdef __TIMING__
      std::cout << " Time: " << (float)(end - start)/(float)CLOCKS_PER_SEC;
      std::cout << " NN Mflops: " << (float)(num_input)*(float)(MIDDLE_SIZE+1)*(float)INPUT_SIZE*(float)CLOCKS_PER_SEC/((float)(end - start)*1e6);
#endif
      std::cout << " Error: " << tot_err;
      std::cout << std::endl;
#ifdef __TIMING__
      start = clock();
#endif
      trainer.save( "temp.nnw" );
      if (min_err>tot_err)
	{
	  trainer.save( "min.nnw" );
	  min_err=tot_err;
	}
#ifdef __TIMING__
      end = clock();
      //      std::cout << "Saving .nnw files " << (float)(end - start)/(float)CLOCKS_PER_SEC << std::endl; 
#endif
      //      getchar();
    } while (tot_err>1 || isnan(tot_err));
  trainer.save( "learn.nnw" );
  for (int cnt=0;cnt<num_input;cnt++)
    {
      trainer.run( inp[cnt], &run_outp );
      std::cout << cnt << " " << run_outp << " " << outp[cnt] << std::endl;
    }
  return( 0 );
}

int test_net( float **inp, int num_input, float *outp, char *fname )
{
  float run_outp;
  long wrong;
  double err;
  if (!inp || !num_input || !outp || !fname) return( 1 );

  nnwork trainer( fname );
  wrong=0;
  err=0;
  for (int cnt=0;cnt<num_input;cnt++)
    {
      std::cout << cnt << " ";
      if (cnt==(num_input-1))
	{
	  for (int cnt2=0;cnt2<INPUT_SIZE;cnt2++)
	    {
	      std::cout << inp[cnt][cnt2] << " ";
	    }
	}
      trainer.run( inp[cnt], &run_outp );
      std::cout << " " << run_outp << " " << outp[cnt];
      if (fabs(run_outp-outp[cnt])>0.3) 
	{
	  wrong+=1;
	  std::cout << " ERROR";
	}
      std::cout << std::endl;
      err+=fabs(run_outp-outp[cnt]);
    }
  std::cout << std::endl;
  std::cout << wrong << " points badly identified" << std::endl;
  std::cout << "Total Error = " << err << std::endl;
  return( 0 );
}

int test_single( char *fname )
{
  float run_outp;
  nnwork trainer( fname );
  float test[INPUT_SIZE]={ 
                          0.209239, 0.0679348, 0.324675, 0.106729, 0.0441389, 0.413559, 0.000000, 0.051948,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.012987, 0.000000, 0.000000, 0.012987, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.012987, 
			  0.051948, 0.012987, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.038961,
			  0.000000, 0.000000, 0.051948, 0.038961, 0.000000, 0.000000, 0.012987, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.012987, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.012987, 0.000000, 0.090909, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.012987, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.012987, 
			  0.000000, 0.012987, 0.012987, 0.012987, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.012987, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.012987, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.038961, 
			  0.000000, 0.000000, 0.000000, 0.064935, 0.000000, 0.000000, 0.000000, 0.090909, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.155844, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.142857, 0.000000, 
			  0.000000, 0.000000, 0.000000 };
  trainer.run( test, &run_outp );
  std::cout << "test = " << run_outp << std::endl;
  std::cout << "error = " << fabs( 0.1 - run_outp ) << std::endl;
  return( 0 );
}

int main( int argc, char **argv )
{
  float **inp, *outp;
  int result;
  int num_input;

  if (!argv[1] || argv[1][0]!='-')
    {
      std::cerr << "Usage: " << argv[0] << " -[l|t] fname" << std::endl << "\t-l is learn, -t is test" << std::endl << "\t Where fname is optional for -l and required for -t. For -l it means a place to start learning." << std::endl << "\tFor -t it means what network weights to use for testing." << std::endl;
      return( 1 );
    }
  switch (argv[1][1])
    {
    case 'T':
      break;
    default:
      if (load_data( &inp, &num_input, &outp ))
	{
	  std::cerr << "Error loading input data!" << std::endl;
	  return( 1 );
	}
    };
  std::cout << "Loaded training data" << std::endl;
  switch (argv[1][1])
    {
    case 'l':
      result=learn_net( inp, num_input, outp, argv[2] );
      break;
    case 't':
      if (!argv[2])
	{
	  std::cerr << "Please specify nn weights to load" << std::endl;
	  result=2;
	  break;
	}
      result=test_net( inp, num_input, outp, argv[2] );
      break;
    case 'T':
      if (!argv[2])
	{
	  std::cerr << "Please specify nn weights to load" << std::endl;
	  result=3;
	  break;
	}
      result=test_single( argv[2] );
      break;
    default:
      std::cerr << "Unknown parameter " << argv[1] << std::endl;
      result=1;
    }
  cleanup( inp, num_input, outp );
  return( result );
}
