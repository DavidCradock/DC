#include "neuronLayer.h"

namespace DC
{
	NeuronLayer::NeuronLayer(int numberOfNeuronsPARAM, int numberOfInputsPerNeuronPARAM)
	{
		numberOfNeurons = numberOfNeuronsPARAM;
		for (int i = 0; i < numberOfNeuronsPARAM; i++)
		{
			neurons.push_back(Neuron(numberOfInputsPerNeuronPARAM));
		}
	}
}