#pragma once
#include "neuron.h"

namespace DC
{
	// A neuron layer
	// This holds a layer of neurons
	class NeuronLayer
	{
	public:
		// Constructor.
		// Sets the number of neurons in this layer.
		NeuronLayer(int NumberOfNeurons, int NumberOfInputsPerNeuron);

		// Holds each neuron in this layer
		std::vector<Neuron> neurons;

		// This holds the number of neurons in this layer.
		// We could just call mvecNeurons.size(), but for speed purposes,
		// we store the value here.
		int numberOfNeurons;
	};
}