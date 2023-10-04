#include "neuronLayer.h"

namespace DC
{
	NeuronLayer::NeuronLayer(int NumberOfNeurons, int NumberOfInputsPerNeuron)
	{
		numberOfNeurons = NumberOfNeurons;
		for (int i = 0; i < NumberOfNeurons; i++)
		{
			neurons.push_back(Neuron(NumberOfInputsPerNeuron));
		}
	}
}