#include "neuron.h"
#include "../DCMath/utilities.h"

namespace DC
{
	Neuron::Neuron(int NumberOfInputs)
	{
		// We need an additional weight for the bias of this neuron
		numberOfInputs = NumberOfInputs + 1;
		for (int i = 0; i < NumberOfInputs + 1; i++)
		{
			// Set weights to random values between -1 and +1
			inputWeights.push_back(randomClampedDouble());
		}
	}
}