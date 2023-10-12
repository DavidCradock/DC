#include "genome.h"

namespace DC
{
	Genome::Genome()
	{
		fitness = 0;
	}

	Genome::Genome(std::vector<double>& neuralNetworkWeights, double networkFitness)
	{
		fitness = networkFitness;
		weights = neuralNetworkWeights;
	}

	Genome::Genome(const NeuralNetworkFeedForward& neuralNetwork, double networkFitness)
	{
		fitness = networkFitness;
		weights = neuralNetwork.getNeuronWeights();
	}

	void Genome::set(std::vector<double>& neuralNetworkWeights, double networkFitness)
	{
		fitness = networkFitness;
		weights = neuralNetworkWeights;
	}

	void Genome::set(const NeuralNetworkFeedForward& neuralNetwork, double networkFitness)
	{
		fitness = networkFitness;
		weights = neuralNetwork.getNeuronWeights();
	}
}