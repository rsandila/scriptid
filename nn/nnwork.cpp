/****************************************************************************

    Neural Network Library
    Copyright (C) 1998 Daniel Franklin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.
              
 ****************************************************************************/

#include "nnwork.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>

// This class implements a simple three-layer backpropagation network.

// Calculate the sigmoid function. This is the transfer function of the nodes.
/*
float sigmoid (float data)
{
	return (1. / (1. + exp (-data)));
}
*/

// Constructors: create the network.
// Arguments are the dims of the network

nnwork::nnwork (int ninput, int nhidden, int noutput)
{
  output=NULL; 
  output_weight_delta=NULL;
  hidden_weight_delta=NULL;
  input_size = ninput;
  hidden_size = nhidden;
  output_size = noutput;
  srand (time (0));
  
  hidden_nodes = new nnlayer (hidden_size, input_size);
  assert (hidden_nodes);
  output_nodes = new nnlayer (output_size, hidden_size);
  assert (output_nodes);
}

// Empty network

nnwork::nnwork ()
{
  output=NULL; 
  output_weight_delta=NULL;
  hidden_weight_delta=NULL;
  input_size = 0;
  hidden_size = 0;
  output_size = 0;
  
  hidden_nodes = 0;
  output_nodes = 0;
}

// Load straight from a file.

nnwork::nnwork (char *filename)
{
  output=NULL; 
  output_weight_delta=NULL;
  hidden_weight_delta=NULL;
  input_size = 0;
  hidden_size = 0;
  output_size = 0;
  
  hidden_nodes = 0;
  output_nodes = 0;
  
  if (!load (filename))
    std::cerr << "Error: File failed to load." << std::endl;
}

// Destructor: destroys the network.

nnwork::~nnwork ()
{
  if (output) delete []output;
  if (output_weight_delta) delete []output_weight_delta;
  if (hidden_weight_delta) delete []hidden_weight_delta;
  output=NULL; 
  output_weight_delta=NULL;
  hidden_weight_delta=NULL;
  if (output_nodes) delete output_nodes;
  if (hidden_nodes) delete hidden_nodes;
  output_nodes=NULL;
  hidden_nodes=NULL;
}

// The following function is useful if you want to find out the dimensions
// of a network loaded from a file, or whatever. ALL adds up everything.

int nnwork::get_layersize (int layer)
{
  switch (layer) 
    {
    case (ALL):
      return input_size + hidden_size + output_size;
      
    case (INPUT):
      return input_size;
      
    case (HIDDEN):
      return hidden_size;
      
    case (OUTPUT):
      return output_size;
      
    default: 
      std::cerr << "Warning: no such layer: " << layer << std::endl;
      return -1;
    }
}

// Training routine for the network. Uses data as input, compares output with
// desired output, computes errors, adjusts weights attached to each node,
// then repeats until the mean squared error at the output is less than 
// max_MSE. The learning rate is eta.

float nnwork::train (float data [], float desired [], float max_MSE, float eta, float lasterror )
{
  //  float *output, *output_weight_delta, *hidden_weight_delta;
  float MSE, MSE_max = max_MSE * 2., sum; // slight speed enhancement
  float err;
  int i, j, k;
  float owdk, tmpfl;
  float *nkw;
  struct neuron *opnod, *hinod;

  /*	if (input_size == 0 || hidden_size == 0 || output_size == 0) {
	std::cerr << "Warning: stupid dimensions. No action taken." << std::endl;
	return( nan );
	}*/
  
  if (!output)
    {
      output = new float [output_size];
      assert (output);
    }
  if (!output_weight_delta)
    {
      output_weight_delta = new float [output_size];
      assert (output_weight_delta);
    }
  if (!hidden_weight_delta)
    {
      hidden_weight_delta = new float [hidden_size];
      assert (hidden_weight_delta);
    }
  run (data, output);
		
// Firstly calculate the output layer error terms

  for (k = 0, MSE = 0; k < output_size; k++) 
    {
      owdk = desired [k] - output [k];
      tmpfl = output[k];

      //      err += fabsf( owdk );
      MSE += owdk * owdk;      
      owdk *= tmpfl * (1 - tmpfl);
      output_weight_delta [k] = owdk;
    }

  err = MSE;

  if (MSE > MSE_max)
    {
      
      opnod = output_nodes->nodes;
      hinod = hidden_nodes->nodes;
      for (j = 0; j < hidden_size; j++) 
	{
	  for (k = 0, sum = 0; k < output_size; k++)
	    {
	      //	  sum += output_weight_delta [k] * output_nodes -> nodes [k].weights [j];
	      sum += output_weight_delta [k] * opnod[k].weights [j];
	    }
	  
	  //      hidden_weight_delta [j] = sum * hidden_nodes -> nodes [j].output * (1 - hidden_nodes -> nodes [j].output);
	  tmpfl = hinod[j].output;
	  hidden_weight_delta [j] = sum * tmpfl * (1 - tmpfl);
	}

// Now update the output weights.
	
      for (k = 0; k < output_size; k++)
	{
	  owdk=output_weight_delta[k];
	  nkw=opnod[k].weights;
	  for (j = 0; j < hidden_size; j++)
	    {
	      //	  output_nodes -> nodes [k].weights [j] += eta * output_weight_delta [k] * hidden_nodes -> nodes [j].output;
	      //	  output_nodes -> nodes [k].weights [j] += eta * owdk * hidden_nodes -> nodes [j].output;
	      nkw[j] += eta * owdk * hinod[j].output;
	    }
	}
      
      // And the hidden weights.
      
      for (j = 0; j < hidden_size; j++)
	{
	  nkw = hinod[j].weights;
	  owdk=hidden_weight_delta[j];
	  for (i = 0; i < input_size; i++)
	    {
	      //	  hidden_nodes -> nodes [j].weights [i] += eta * hidden_weight_delta [j] * data [i];
	      nkw[i] += eta * owdk * data [i];
	    }
	}
    }
  //  delete [] output;
  //  delete [] output_weight_delta;
  //  delete [] hidden_weight_delta;
  return( err );
}

// Should only be applied after the network has been suitably trained. When
// it is ready, data are applied at the input, and the output (basically a
// boolean output but could be different) goes into result. Naturally, data
// and result are the same size as the input and output vectors respectively.

void nnwork::run (float data [], float result [])
{
  int i, j, k;
  float sum;
  float *nodesj, *nodesk;
  struct neuron *tmpneur;
  /*  if (input_size == 0 || hidden_size == 0 || output_size == 0) {
      std::cerr << "Warning: stupid dimensions. No action taken." << std::endl;
      return;
      }*/
  
  tmpneur = hidden_nodes->nodes;

  for (j = 0; j < hidden_size; j++) 
    {
      sum = 0;
      
      // Calculate the output value
      nodesj=tmpneur[j].weights;
      
      for (i = 0; i < input_size; i++)
	{
	  //	      sum += hidden_nodes -> nodes [j].weights [i] * data [i];
	  sum+= nodesj[i]*data[i];
	}
      
      //      hidden_nodes -> nodes [j].output = sigmoid (sum);
      tmpneur[j].output = sigmoid (sum);
    }

  for (k = 0; k < output_size; k++) 
    {
      sum = 0;
	    
// Calculate the output value
      tmpneur=hidden_nodes->nodes;
      nodesk=output_nodes->nodes[k].weights;
      for (j = 0; j < hidden_size; j++)
	{
	  //	  sum += output_nodes -> nodes [k].weights [j] *
	  //	    hidden_nodes -> nodes [j].output;
	  sum += nodesk[j]*tmpneur[j].output;
	}
      result [k] = sigmoid (sum);
    }
}

/* 
	Restore the values of the connection weights from a file. Format:

nnlib network weights data file version 1.0
Size: input_nodes hidden_nodes output_nodes
Hidden layer weights:
wh_0,0 wh_0,1 wh_0,2 ... wh_0,input_nodes-1
wh_1,0 wh_1,1 wh_1,2 ... wh_1,input_nodes-1
wh_2,0 wh_2,1 wh_2,2 ... wh_2,input_nodes-1
...
wh_hidden_nodes-1,0 wh_hidden_nodes-1,1 wh_hidden_nodes-1,2 ... wh_hidden_nodes,input_nodes-1
Output layer weights:
wo_0,0 wo_0,1 wo_0,2 ... wo_0,hidden_nodes-1
wo_1,0 wo_1,1 wo_1,2 ... wo_1,hidden_nodes-1
wo_2,0 wo_2,1 wo_2,2 ... wo_2,hidden_nodes-1
...
wo_output_nodes-1,0 wo_output_nodes-1,1 wo_output_nodes-1,2 ... wo_output_nodes-1,hidden_nodes-1


	THERE IS MINIMAL ERROR CHECKING!!! Please don't manually edit the 
	files.
*/

int nnwork::load (char *filename)
{
	std::ifstream weights (filename);
	int i, j, k;
	int num_input, num_hidden, num_output;
	char temp [50];

	if (weights.good ()) {
		weights.getline (temp, 50, '\n');
		
		if (strcmp (temp, "nnlib network weights data file version 1.0") == 0) {
			weights >> temp >> num_input >> num_hidden >> num_output;

			if (strcmp (temp, "Size:") != 0) {
				std::cerr << "Error in " << filename << ": no \"Size:\" line." << std::endl;
				weights.close ();
				return 0;
			}
			
// If required, resize the network	

			input_size = num_input;
			
			if (hidden_size != num_hidden || output_size != num_output) 
				std::cerr << "Resizing neural network." << std::endl;

			if (hidden_size != num_hidden) {
				hidden_size = num_hidden;
				if (hidden_nodes) delete hidden_nodes;
				hidden_nodes = new nnlayer (hidden_size, input_size);
				assert (hidden_nodes);
			}
			
			if (output_size != num_output) {
				output_size = num_output;
				if (output_nodes) delete output_nodes;
				output_nodes = new nnlayer (output_size, hidden_size);
				assert (output_nodes);
			}
			
// Read the hidden weights
			weights.getline (temp, 2, '\n');
			weights.getline (temp, 50, '\n');
			
			if (strcmp (temp, "Hidden layer weights:") == 0)
				for (j = 0; j < hidden_size; j++)
					for (i = 0; i < input_size; i++) {
						weights >> hidden_nodes -> nodes [j].weights [i];
						if (!weights.good ()) {
							std::cerr << "Premature EOF on data file." << std::endl;
							weights.close ();
							return 0;
						}
					}
			else {
				std::cerr << "Error in " << filename << ": no  \"Hidden layer weights:\" line." << std::endl;
				weights.close ();
				return 0;
			}

// And the output weights
			weights.getline (temp, 2, '\n');
			weights.getline (temp, 50, '\n');
			
			if (strcmp (temp, "Output layer weights:") == 0)
				for (k = 0; k < output_size; k++)
					for (j = 0; j < hidden_size; j++) {
						weights >> output_nodes -> nodes [k].weights [j];
						if (!weights.good ()) {
							std::cerr << "Premature EOF on data file." << std::endl;
							weights.close ();
							return 0;
						}
					}
			else {
				std::cerr << "Error in " << filename << ": no  \"Output layer weights:\" line." << std::endl;
				weights.close ();
				return 0;
			}

			weights.close ();
		} else {
			std::cerr << "Unsupported file version, or garbage in file." << std::endl;
			return 0;
		}
		
		return 1;
	} else
		return 0;
}

// Save the current weights to <filename>. Suggested extension is .nnw

int nnwork::save (char *filename)
{
	std::ofstream weights (filename);
	int i, j, k;

	if (input_size == 0 || hidden_size == 0 || output_size == 0) {
		std::cerr << "Warning: silly dimensions. No action taken." << std::endl;
		return 0;
	}

	if (weights.good ()) {
		weights << "nnlib network weights data file version 1.0" << std::endl;
		weights << "Size: " << input_size << " " << hidden_size << " " << output_size << std::endl;
		
// Write out the hidden weights

		weights << "Hidden layer weights:" << std::endl;

		for (j = 0; j < hidden_size; j++) {
			for (i = 0; i < input_size; i++)
				weights << hidden_nodes -> nodes [j].weights [i] << "\t";
			weights << std::endl;
		}
		
		weights << "Output layer weights:" << std::endl;

// And the output weights
			
		for (k = 0; k < output_size; k++) {
			for (j = 0; j < hidden_size; j++)
				weights << output_nodes -> nodes [k].weights [j] << "\t";
			weights << std::endl;
		}

		weights.close ();
		
		return 1;
	} else
		return 0;
}
