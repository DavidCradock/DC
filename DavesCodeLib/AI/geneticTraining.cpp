#include "geneticTraining.h"
#include "../Common/error.h"
#include "../Math/mathUtilities.h"
#include <algorithm>

namespace DC
{
	Genome GeneticsTraining::rouletteWheelSelection(const std::vector<Genome>& population) const
	{
		// Make sure the vector contains stuff
		ErrorIfFalse(population.size(), L"GeneticsTraining::rouletteWheelSelection() failed as the given Genome vector was empty.");

		// Compute total fitness of all the elements in the vector
		double dTotalFitness = 0;
		for (unsigned int ui = 0; ui < population.size(); ui++)
		{
			dTotalFitness += population[ui].fitness;
		}

		// Now compute a random value between 0 and dTotalFitness.
		// Think of this as a portion of the entire wheel.
		double dPortion = randDouble() * dTotalFitness;

		// Now go through the vector of genomes, adding their fitness scores as we go
		// and if the sub total is greater than the above dTotalFitness,
		// we've found our index.
		double dSubTotal = 0;
		Genome selectedGenome = population[0];
		for (unsigned int ui = 0; ui < population.size(); ui++)
		{
			dSubTotal += population[ui].fitness;
			if (dSubTotal > dPortion)
			{
				selectedGenome = population[ui];
				break;
			}
		}
		return selectedGenome;
	}

	void GeneticsTraining::computeFitness(const std::vector<double>& populationFitness, double& worstFitness, double& averageFitness, double& bestFitness, double& totalFitness) const
	{
		worstFitness = 999999999999999;
		bestFitness = -999999999999999;
		totalFitness = 0;
		for (unsigned int ui = 0; ui < populationFitness.size(); ui++)
		{
			// Total fitness
			totalFitness += populationFitness[ui];

			// Worst fitness
			if (worstFitness > populationFitness[ui])
				worstFitness = populationFitness[ui];

			// Best fitness
			if (bestFitness < populationFitness[ui])
				bestFitness = populationFitness[ui];
		}

		averageFitness = 0;
		if (populationFitness.size())
			averageFitness = totalFitness / populationFitness.size();
	}

	void GeneticsTraining::computeFitness(const std::vector<Genome>& population, double& worstFitness, double& averageFitness, double& bestFitness, double& totalFitness) const
	{
		worstFitness = 999999999999999;
		bestFitness = -999999999999999;
		totalFitness = 0;
		for (unsigned int ui = 0; ui < population.size(); ui++)
		{
			// Total fitness
			totalFitness += population[ui].fitness;

			// Worst fitness
			if (worstFitness > population[ui].fitness)
				worstFitness = population[ui].fitness;

			// Best fitness
			if (bestFitness < population[ui].fitness)
				bestFitness = population[ui].fitness;
		}

		averageFitness = 0;
		if (population.size())
			averageFitness = totalFitness / population.size();
	}

	void GeneticsTraining::crossover(
		std::vector<double>& mumWeights,
		std::vector<double>& dadWeights,
		std::vector<double>& childAWeights,
		std::vector<double>& childBWeights,
		double crossoverRate) const
	{
		// Error checking to make sure vector sizes are equal
		if (mumWeights.size() != dadWeights.size())
			ErrorIfTrue(1, L"GeneticsTraining::crossover() failed. The passed parent vectors are not the same size.");

		// Determine whether we should perform the crossover...
		// First by calculating a random value an determining whether it is below the given crossover rate.
		// And second, if both parents are the same
		if (randDouble() > crossoverRate || mumWeights == dadWeights)
		{
			childAWeights = mumWeights;
			childBWeights = dadWeights;
			return;
		}

		// If we get here, mum and dad are different and a crossover is to be performed.

		// Determine a random cross over point
		int iCrossoverPoint = randInt(0, (int)mumWeights.size() - 1);

		// Clear the child weights
		childAWeights.clear();
		childBWeights.clear();

		// Give the children weights from mum and dad, before the crossover point
		for (int i = 0; i < iCrossoverPoint; i++)
		{
			// Child A gets all of mum's weights, before the crossover point
			childAWeights.push_back(mumWeights[i]);

			// The other child, all of dad's, before the crossover point
			childBWeights.push_back(dadWeights[i]);
		}
		// Give the children weights from dad and mum, after the crossover point
		for (int i = iCrossoverPoint; i < mumWeights.size(); i++)
		{
			// Child A gets all of dad's weights, after the crossover point
			childAWeights.push_back(dadWeights[i]);

			// The other child, all of mum's, after the crossover point
			childBWeights.push_back(mumWeights[i]);
		}
	}

	void GeneticsTraining::crossoverBetweenNeurons(
		std::vector<double>& mumWeights,
		std::vector<double>& dadWeights,
		std::vector<double>& childAWeights,
		std::vector<double>& childBWeights,
		const std::vector<int>& splitPoints,
		double crossoverRate) const
	{
		// Error checking to make sure vector sizes are equal
		if (mumWeights.size() != dadWeights.size())
			ErrorIfTrue(1, L"GeneticsTraining::crossoverBetweenNeurons() failed. The passed parent vectors are not the same size.");
		// Error checking to make sure the given split points vector is of adequate size
		ErrorIfTrue(splitPoints.size() < 2, L"GeneticsTraining::crossoverBetweenNeurons() failed. The passed splitPoints vector's size is too small. Must be at least 2.");

		// Determine whether we should perform the crossover...
		// First by calculating a random value an determining whether it is below the given crossover rate.
		// And second, if both parents are the same
		if (randDouble() > crossoverRate || mumWeights == dadWeights)
		{
			// Just pass on the parents as the children.
			childAWeights = mumWeights;
			childBWeights = dadWeights;
			return;
		}

		// If we get here, mum and dad are different and a crossover is to be performed.

		// Using the vector holding where we are allowed to split the weights, so that they do not split a portion of a single neuron,
		// determine two crossover points
		int iRand1 = randInt(0, (int)splitPoints.size() - 2);
		int iCrossoverPoint1 = splitPoints[iRand1];
		int iRand2 = randInt(iRand1+1, (int)splitPoints.size() - 1);
		int iCrossoverPoint2 = splitPoints[iRand2];

		// Clear the child weights
		childAWeights.clear();
		childBWeights.clear();

		// Give the children weights from mum and dad
		for (int i = 0; i < (int)mumWeights.size(); i++)
		{
			// If outside of the crossover points
			if (i < iCrossoverPoint1 || i > iCrossoverPoint2)
			{
				// Child A gets all of mum's weights
				childAWeights.push_back(mumWeights[i]);

				// The other child, all of dad's
				childBWeights.push_back(dadWeights[i]);
			}
			else
			{
				// Child A gets all of dad's weights
				childAWeights.push_back(dadWeights[i]);

				// The other child, all of mum's
				childBWeights.push_back(mumWeights[i]);
			}
		}
	}

	void GeneticsTraining::mutate(std::vector<double>& networkWeights, double mutationProbability, double mutationMaxAmount) const
	{
		ErrorIfFalse(networkWeights.size(), L"GeneticsTraining::mutate() failed. Given network weights vector of zero size.");
		// Go through each weight and perhaps mutate it
		for (int i = 0; i < (int)networkWeights.size(); i++)
		{
			if (randDouble() < mutationProbability)
			{
				networkWeights[i] += randomClampedDouble() * mutationMaxAmount;
			}
		}
	}

	std::vector<Genome> GeneticsTraining::createNewGeneration(
		const std::vector<Genome>& genomes,
		const std::vector<int>& splitsPoints,
		double mutationProbability,
		double mutationMaxAmount,
		double crossoverRate,
		int numberOfElite,
		int numCopiesOfEachElite) const
	{
		// Error checking
		ErrorIfFalse(genomes.size(), L"GeneticsTraining::createNewGeneration() failed, as it was given a vector of Genome objects of zero size.");
		ErrorIfTrue(numberOfElite < 0 || numCopiesOfEachElite < 0, L"GeneticsTraining::createNewGeneration() failed, as it was given values less than zero for numberOfElite or numCopiesOfEachElite.");

		// The old population of genomes
		std::vector<Genome> vecOldPop = genomes;

		// Compute worst, average, best and total fitness of the entire population
		double dWorstFitness, dAverageFitness, dBestFitness, dTotalFitness;
		computeFitness(vecOldPop, dWorstFitness, dAverageFitness, dBestFitness, dTotalFitness);

		// Create the new population of genomes
		std::vector<Genome> vecNewPop;

		// Insert the elite
		insertElite(vecOldPop, vecNewPop, numberOfElite, numCopiesOfEachElite);

		while (vecNewPop.size() < vecOldPop.size())
		{
			// Use roulette wheel selection to choose a mum and dad
			Genome mum = rouletteWheelSelection(vecOldPop);
			Genome dad = rouletteWheelSelection(vecOldPop);

			// Perform some crossover to create the babies
			Genome baby1;
			Genome baby2;
			crossoverBetweenNeurons(mum.weights, dad.weights, baby1.weights, baby2.weights, splitsPoints, crossoverRate);

			// Now mutate the babies (Yeuck!)
			mutate(baby1.weights, mutationProbability, mutationMaxAmount);
			mutate(baby2.weights, mutationProbability, mutationMaxAmount);

			// Now add the new babies into the new population of genomes
			vecNewPop.push_back(baby1);

			// We check the new population size as the population size may be an odd number and if we add two
			// babies, we'll create an oversized population.
			if (vecNewPop.size() < vecOldPop.size())
				vecNewPop.push_back(baby2);
		}

		// Error checking
		return vecNewPop;
	}

	void GeneticsTraining::insertElite(std::vector<Genome>& oldPopulation, std::vector<Genome>& newPopulation, int numberOfElite, int numberOfCopiesOfEachElite) const
	{
		// Sort the old population by fitness so that the fitest are towards the end of the vector
		std::sort(oldPopulation.begin(), oldPopulation.end());

		// For each number of elite to insert
		for (int i = 0; i < numberOfElite; i++)
		{
			// Add X copies of each elite
			for (int j = 0; j < numberOfCopiesOfEachElite; j++)
			{
				// Make sure there is room for the elite
				if (newPopulation.size() < oldPopulation.size())
				{
					newPopulation.push_back(oldPopulation[oldPopulation.size() - 1 - i]);
				}
				else
				{
					break;
				}
			}
		}
	}
}