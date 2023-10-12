#include "neuron.h"
#include "../Math/mathUtilities.h"

namespace DC
{
	Neuron::Neuron(int numberOfInputsPARAM)
	{
		// We need an additional weight for the bias of this neuron
		numberOfInputs = numberOfInputsPARAM + 1;
		for (int i = 0; i < numberOfInputsPARAM + 1; i++)
		{
			// Set weights to random values between -1 and +1
			inputWeights.push_back(randomClampedDouble());
		}
	}
}