#include "genome.h"

namespace DC
{
	Genome::Genome()
	{
		fitness = 0;
	}

	Genome::Genome(std::vector<double>& NeuralNetworkWeights, double NetworkFitness)
	{
		fitness = NetworkFitness;
		weights = NeuralNetworkWeights;
	}

	Genome::Genome(const NeuralNetworkFeedForward& NeuralNetwork, double NetworkFitness)
	{
		fitness = NetworkFitness;
		weights = NeuralNetwork.getNeuronWeights();
	}

	void Genome::set(std::vector<double>& NeuralNetworkWeights, double NetworkFitness)
	{
		fitness = NetworkFitness;
		weights = NeuralNetworkWeights;
	}

	void Genome::set(const NeuralNetworkFeedForward& NeuralNetwork, double NetworkFitness)
	{
		fitness = NetworkFitness;
		weights = NeuralNetwork.getNeuronWeights();
	}
}