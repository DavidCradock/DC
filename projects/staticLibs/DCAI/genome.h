#pragma once
#include <vector>
#include "neuralNetFeedForward.h"

namespace DC
{
	// This is used with the GeneticsTraining class with methods which are used to create an entire new population of networks from an
	// existing one.
	// We typically create a vector of these, transfer the weights for each neural network, along with it's fitness into each one.
	// Then we use the vector of objects of this class to create a next generation from the existing population.
	// For example (psuedo code ahead):
	// std::vector<Genome> vecGenomes;
	// for (each neural network)
	// {
	//		Genome genome(someExistingNeuralNetwork, itsFitnessScore);
	//		vecGenomes.push_back(genome);
	// }
	// GeneticsTraining genTrainer;
	// std::vector<Genome> vecNewPopulation = genTrainer.someMethodWhichCreatesANewPopulation(vecGenomes);
	// Then we can use the vecNewPopulation to update the existing neural networks and reset their fitness for the next generation.
	class Genome
	{
	public:
		// Default constructor, sets everything to zero
		Genome();

		// Constructor which copies the given values into itself
		Genome(std::vector<double>& NeuralNetworkWeights, double NetworkFitness);

		// Constructor which copies the given values into itself
		Genome(const NeuralNetworkFeedForward& neuralNetwork, double NetworkFitness);

		// Sets this genome to the given values.
		void set(std::vector<double>& NeuralNetworkWeights, double NetworkFitness);

		// Sets this genome to hold the given fitness and weights stored in the given neural network
		void set(const NeuralNetworkFeedForward& NeuralNetwork, double NetworkFitness);

		// This should be filled with the weights from a neural network by calling it's getNeuronWeights() method.
		std::vector<double>	weights;

		// This should be set to hold the final fitness score of the entity owning the neural network.
		double fitness;

		// Operator overload '<' for sorting a vector of this class's objects by fitness
		friend bool operator<(const Genome& lhs, const Genome& rhs)
		{
			return (lhs.fitness < rhs.fitness);
		}
	};
}