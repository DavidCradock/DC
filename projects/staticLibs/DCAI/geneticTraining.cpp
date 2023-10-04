#include "geneticTraining.h"
#include "../DCCommon/error.h"
#include "../DCMath/utilities.h"
#include <algorithm>

namespace DC
{
	Genome GeneticsTraining::rouletteWheelSelection(const std::vector<Genome>& Population) const
	{
		// Make sure the vector contains stuff
		ErrorIfFalse(Population.size(), L"GeneticsTraining::rouletteWheelSelection() failed as the given Genome vector was empty.");

		// Compute total fitness of all the elements in the vector
		double dTotalFitness = 0;
		for (unsigned int ui = 0; ui < Population.size(); ui++)
		{
			dTotalFitness += Population[ui].fitness;
		}

		// Now compute a random value between 0 and dTotalFitness.
		// Think of this as a portion of the entire wheel.
		double dPortion = randDouble() * dTotalFitness;

		// Now go through the vector of genomes, adding their fitness scores as we go
		// and if the sub total is greater than the above dTotalFitness,
		// we've found our index.
		double dSubTotal = 0;
		Genome selectedGenome = Population[0];
		for (unsigned int ui = 0; ui < Population.size(); ui++)
		{
			dSubTotal += Population[ui].fitness;
			if (dSubTotal > dPortion)
			{
				selectedGenome = Population[ui];
				break;
			}
		}
		return selectedGenome;
	}

	void GeneticsTraining::computeFitness(const std::vector<double>& PopulationFitness, double& WorstFitness, double& AverageFitness, double& BestFitness, double& TotalFitness) const
	{
		WorstFitness = 999999999999999;
		BestFitness = -999999999999999;
		TotalFitness = 0;
		for (unsigned int ui = 0; ui < PopulationFitness.size(); ui++)
		{
			// Total fitness
			TotalFitness += PopulationFitness[ui];

			// Worst fitness
			if (WorstFitness > PopulationFitness[ui])
				WorstFitness = PopulationFitness[ui];

			// Best fitness
			if (BestFitness < PopulationFitness[ui])
				BestFitness = PopulationFitness[ui];
		}

		AverageFitness = 0;
		if (PopulationFitness.size())
			AverageFitness = TotalFitness / PopulationFitness.size();
	}

	void GeneticsTraining::computeFitness(const std::vector<Genome>& Population, double& WorstFitness, double& AverageFitness, double& BestFitness, double& TotalFitness) const
	{
		WorstFitness = 999999999999999;
		BestFitness = -999999999999999;
		TotalFitness = 0;
		for (unsigned int ui = 0; ui < Population.size(); ui++)
		{
			// Total fitness
			TotalFitness += Population[ui].fitness;

			// Worst fitness
			if (WorstFitness > Population[ui].fitness)
				WorstFitness = Population[ui].fitness;

			// Best fitness
			if (BestFitness < Population[ui].fitness)
				BestFitness = Population[ui].fitness;
		}

		AverageFitness = 0;
		if (Population.size())
			AverageFitness = TotalFitness / Population.size();
	}

	void GeneticsTraining::crossover(
		std::vector<double>& MumWeights,
		std::vector<double>& DadWeights,
		std::vector<double>& ChildAWeights,
		std::vector<double>& ChildBWeights,
		double CrossoverRate) const
	{
		// Error checking to make sure vector sizes are equal
		if (MumWeights.size() != DadWeights.size())
			ErrorIfTrue(1, L"GeneticsTraining::crossover() failed. The passed parent vectors are not the same size.");

		// Determine whether we should perform the crossover...
		// First by calculating a random value an determining whether it is below the given crossover rate.
		// And second, if both parents are the same
		if (randDouble() > CrossoverRate || MumWeights == DadWeights)
		{
			ChildAWeights = MumWeights;
			ChildBWeights = DadWeights;
			return;
		}

		// If we get here, mum and dad are different and a crossover is to be performed.

		// Determine a random cross over point
		int iCrossoverPoint = randInt(0, (int)MumWeights.size() - 1);

		// Clear the child weights
		ChildAWeights.clear();
		ChildBWeights.clear();

		// Give the children weights from mum and dad, before the crossover point
		for (int i = 0; i < iCrossoverPoint; i++)
		{
			// Child A gets all of mum's weights, before the crossover point
			ChildAWeights.push_back(MumWeights[i]);

			// The other child, all of dad's, before the crossover point
			ChildBWeights.push_back(DadWeights[i]);
		}
		// Give the children weights from dad and mum, after the crossover point
		for (int i = iCrossoverPoint; i < MumWeights.size(); i++)
		{
			// Child A gets all of dad's weights, after the crossover point
			ChildAWeights.push_back(DadWeights[i]);

			// The other child, all of mum's, after the crossover point
			ChildBWeights.push_back(MumWeights[i]);
		}
	}

	void GeneticsTraining::crossoverBetweenNeurons(
		std::vector<double>& MumWeights,
		std::vector<double>& DadWeights,
		std::vector<double>& ChildAWeights,
		std::vector<double>& ChildBWeights,
		const std::vector<int>& SplitPoints,
		double CrossoverRate) const
	{
		// Error checking to make sure vector sizes are equal
		if (MumWeights.size() != DadWeights.size())
			ErrorIfTrue(1, L"GeneticsTraining::crossoverBetweenNeurons() failed. The passed parent vectors are not the same size.");
		// Error checking to make sure the given split points vector is of adequate size
		ErrorIfTrue(SplitPoints.size() < 2, L"GeneticsTraining::crossoverBetweenNeurons() failed. The passed SplitPoints vector's size is too small. Must be at least 2.");

		// Determine whether we should perform the crossover...
		// First by calculating a random value an determining whether it is below the given crossover rate.
		// And second, if both parents are the same
		if (randDouble() > CrossoverRate || MumWeights == DadWeights)
		{
			// Just pass on the parents as the children.
			ChildAWeights = MumWeights;
			ChildBWeights = DadWeights;
			return;
		}

		// If we get here, mum and dad are different and a crossover is to be performed.

		// Using the vector holding where we are allowed to split the weights, so that they do not split a portion of a single neuron,
		// determine two crossover points
		int iRand1 = randInt(0, (int)SplitPoints.size() - 2);
		int iCrossoverPoint1 = SplitPoints[iRand1];
		int iRand2 = randInt(iRand1+1, (int)SplitPoints.size() - 1);
		int iCrossoverPoint2 = SplitPoints[iRand2];

		// Clear the child weights
		ChildAWeights.clear();
		ChildBWeights.clear();

		// Give the children weights from mum and dad
		for (int i = 0; i < (int)MumWeights.size(); i++)
		{
			// If outside of the crossover points
			if (i < iCrossoverPoint1 || i > iCrossoverPoint2)
			{
				// Child A gets all of mum's weights
				ChildAWeights.push_back(MumWeights[i]);

				// The other child, all of dad's
				ChildBWeights.push_back(DadWeights[i]);
			}
			else
			{
				// Child A gets all of dad's weights
				ChildAWeights.push_back(DadWeights[i]);

				// The other child, all of mum's
				ChildBWeights.push_back(MumWeights[i]);
			}
		}
	}

	void GeneticsTraining::mutate(std::vector<double>& NetworkWeights, double MutationProbability, double MutationMaxAmount) const
	{
		ErrorIfFalse(NetworkWeights.size(), L"GeneticsTraining::mutate() failed. Given network weights vector of zero size.");
		// Go through each weight and perhaps mutate it
		for (int i = 0; i < (int)NetworkWeights.size(); i++)
		{
			if (randDouble() < MutationProbability)
			{
				NetworkWeights[i] += randomClampedDouble() * MutationMaxAmount;
			}
		}
	}

	std::vector<Genome> GeneticsTraining::createNewGeneration(
		const std::vector<Genome>& genomes,
		const std::vector<int>& SplitsPoints,
		double MutationProbability,
		double MutationMaxAmount,
		double CrossoverRate,
		int NumberOfElite,
		int NumCopiesOfEachElite) const
	{
		// Error checking
		ErrorIfFalse(genomes.size(), L"GeneticsTraining::createNewGeneration() failed, as it was given a vector of Genome objects of zero size.");
		ErrorIfTrue(NumberOfElite < 0 || NumCopiesOfEachElite < 0, L"GeneticsTraining::createNewGeneration() failed, as it was given values less than zero for NumberOfElite or NumCopiesOfEachElite.");

		// The old population of genomes
		std::vector<Genome> vecOldPop = genomes;

		// Compute worst, average, best and total fitness of the entire population
		double dWorstFitness, dAverageFitness, dBestFitness, dTotalFitness;
		computeFitness(vecOldPop, dWorstFitness, dAverageFitness, dBestFitness, dTotalFitness);

		// Create the new population of genomes
		std::vector<Genome> vecNewPop;

		// Insert the elite
		insertElite(vecOldPop, vecNewPop, NumberOfElite, NumCopiesOfEachElite);

		while (vecNewPop.size() < vecOldPop.size())
		{
			// Use roulette wheel selection to choose a mum and dad
			Genome mum = rouletteWheelSelection(vecOldPop);
			Genome dad = rouletteWheelSelection(vecOldPop);

			// Perform some crossover to create the babies
			Genome baby1;
			Genome baby2;
			crossoverBetweenNeurons(mum.weights, dad.weights, baby1.weights, baby2.weights, SplitsPoints, CrossoverRate);

			// Now mutate the babies (Yeuck!)
			mutate(baby1.weights, MutationProbability, MutationMaxAmount);
			mutate(baby2.weights, MutationProbability, MutationMaxAmount);

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

	void GeneticsTraining::insertElite(std::vector<Genome>& OldPop, std::vector<Genome>& NewPop, int NumberOfElite, int NumCopiesOfEachElite) const
	{
		// Sort the old population by fitness so that the fitest are towards the end of the vector
		std::sort(OldPop.begin(), OldPop.end());

		// For each number of elite to insert
		for (int i = 0; i < NumberOfElite; i++)
		{
			// Add X copies of each elite
			for (int j = 0; j < NumCopiesOfEachElite; j++)
			{
				// Make sure there is room for the elite
				if (NewPop.size() < OldPop.size())
				{
					NewPop.push_back(OldPop[OldPop.size() - 1 - i]);
				}
				else
				{
					break;
				}
			}
		}
	}
}