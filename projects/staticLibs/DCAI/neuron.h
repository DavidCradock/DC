#pragma once
#include <vector>

namespace DC
{
	// A neuron of a neural network.
	// It represents a single neuron and holds the weights for each input into the neuron
	// as well as the neuron's bias.
	class Neuron
	{
	public:
		// Constructor.
		// Sets the number of inputs this neuron has, creates each neuron
		// and bias value for the neuron and sets them all to have random
		// values between -1 and 1
		Neuron(int numberOfInputs);

		// Holds the weight values for each of the inputs this neuron has.
		// It also holds the bias of the neuron at the end of the vector.
		std::vector<double> inputWeights;

		// This holds the number of inputs and weights this neuron has.
		// It actually has the number of inputs + 1, as we need an additional weight
		// for the bias of the neuron.
		// We could just call vecInputsWeights.size(), but for speed purposes,
		// we store the value here.
		int numberOfInputs;
	};
}