#include "neuralNetFeedForward.h"
#include "../Math/mathUtilities.h"
#include "../Common/error.h"

namespace DC
{




	NeuralNetworkFeedForward::NeuralNetworkFeedForward()
	{
		create(1, 1, 0, 0);
	}

	NeuralNetworkFeedForward::NeuralNetworkFeedForward(int numberOfInputsPARAM, int numberOfOutputsPARAM, int numberOfLayersPARAM, int numberOfNeuronsPerLayerPARAM)
	{
		create(numberOfInputsPARAM, numberOfOutputsPARAM, numberOfLayersPARAM, numberOfNeuronsPerLayerPARAM);
	}

	void NeuralNetworkFeedForward::create(int numberOfInputsPARAM, int numberOfOutputsPARAM, int numberOfLayersPARAM, int numberOfNeuronsPerLayerPARAM)
	{
		// Check for valid parameters
		ErrorIfTrue(numberOfInputsPARAM < 1, L"NeuralNetworkFeedForward::create() given invalid number of inputs. Must be at least 1");
		ErrorIfTrue(numberOfOutputsPARAM < 1, L"NeuralNetworkFeedForward::create() given invalid number of outputs. Must be at least 1");
		ErrorIfTrue(numberOfLayersPARAM < 0, L"NeuralNetworkFeedForward::create() given invalid number of layers. Must be at least 0");
		ErrorIfTrue(numberOfNeuronsPerLayerPARAM < 1 && numberOfLayersPARAM > 0, L"NeuralNetworkFeedForward::create() given invalid number of neurons per layer. Must be at least 1 if numberOfLayers is > 0");

		// Clear everything
		layers.clear();

		// Set given parameters
		numberOfInputs = numberOfInputsPARAM;
		numberOfOutputs = numberOfOutputsPARAM;
		numberOfLayers = numberOfLayersPARAM;
		numberOfNeuronsPerLayer = numberOfNeuronsPerLayerPARAM;

		// Set default bias
		bias = -1;

		// Set default sigmoid response
		sigmoidResponse = 1;

		if (numberOfLayers)
		{
			// Create the first layer of neurons which have the number of inputs set to the number of inputs of the network.
			layers.push_back(NeuronLayer(numberOfNeuronsPerLayer, numberOfInputs));

			// Create the other layers
			// iNumLayers -1 because we've already created a layer above
			for (int i = 0; i < numberOfLayers - 1; i++)
			{
				// Create the layer, using the number of inputs set to the number of neurons per layer
				layers.push_back(NeuronLayer(numberOfNeuronsPerLayer, numberOfNeuronsPerLayer));
			}

			// Create the output layer
			layers.push_back(NeuronLayer(numberOfOutputs, numberOfNeuronsPerLayer));
		}
		else
		{
			// Create the output layer
			layers.push_back(NeuronLayer(numberOfOutputs, numberOfInputs));
		}
	}

	std::vector<double> NeuralNetworkFeedForward::getNeuronWeights(void) const
	{
		std::vector<double> vecWeights;

		// For each layer (+1 as we are also returning the output layer)
		for (int iLayer = 0; iLayer < numberOfLayers + 1; iLayer++)
		{
			// For each neuron in each layer
			for (int iNeuron = 0; iNeuron < layers[iLayer].numberOfNeurons; iNeuron++)
			{
				// For each weight in each neuron, not including the bias of the neuron
				for (int iWeight = 0; iWeight < layers[iLayer].neurons[iNeuron].numberOfInputs; iWeight++)
				{
					vecWeights.push_back(layers[iLayer].neurons[iNeuron].inputWeights[iWeight]);
				}
			}
		}
		return vecWeights;
	}

	int NeuralNetworkFeedForward::getNumberOfWeights(void) const
	{
		int iNumberOfWeights = 0;
		// For each layer (+1 as we are also returning the output layer)
		for (int iLayer = 0; iLayer < numberOfLayers + 1; iLayer++)
		{
			// For each neuron in each layer
			for (int iNeuron = 0; iNeuron < layers[iLayer].numberOfNeurons; iNeuron++)
			{
				// For each weight in each neuron, not including the bias of the neuron
				for (int iWeight = 0; iWeight < layers[iLayer].neurons[iNeuron].numberOfInputs; iWeight++)
				{
					iNumberOfWeights++;
				}
			}
		}
		return iNumberOfWeights;
	}

	void NeuralNetworkFeedForward::replaceWeights(const std::vector<double>& newWeights)
	{
		int iIndex = 0;

		// For each layer (+1 as we are also replacing the output layer)
		for (int iLayer = 0; iLayer < numberOfLayers + 1; iLayer++)
		{
			// For each neuron in each layer
			for (int iNeuron = 0; iNeuron < layers[iLayer].numberOfNeurons; iNeuron++)
			{
				// For each weight in each neuron, not including the bias of the neuron
				for (int iWeight = 0; iWeight < layers[iLayer].neurons[iNeuron].numberOfInputs; iWeight++)
				{
					layers[iLayer].neurons[iNeuron].inputWeights[iWeight] = newWeights[iIndex++];
				}
			}
		}
	}

	std::vector<double> NeuralNetworkFeedForward::update(std::vector<double>& inputs)
	{
		ErrorIfTrue(inputs.size() != numberOfInputs, L"NeuralNetworkFeedForward::update() failed. Was given a vector of inputs which are not equal to in size of the network's number of inputs");
		std::vector<double> vecOutputs;

		int cWeight = 0;

		// For each layer (+1 as we are also updating the output layer)
		for (int iLayer = 0; iLayer < numberOfLayers + 1; iLayer++)
		{
			// If we've used the inputs, we need to set the inputs to be the outputs of the previous layer
			if (iLayer > 0)
				inputs = vecOutputs;

			// Clear the outputs, as we're going to calculate them below
			vecOutputs.clear();

			// Used to count through each weight in the inputs below.
			cWeight = 0;

			int iNumInputs = 0;

			// For each neuron in each layer
			// multiply the input by the weight for that input, then use the sigmoid
			// function to smooth it out to get the final output.
			for (int iNeuron = 0; iNeuron < layers[iLayer].numberOfNeurons; iNeuron++)
			{
				double dAccumulatedInput = 0;
				iNumInputs = layers[iLayer].neurons[iNeuron].numberOfInputs;

				// For each weight, not including the bias (the -1)
				for (int iWeight = 0; iWeight < iNumInputs - 1; iWeight++)
				{
					dAccumulatedInput += layers[iLayer].neurons[iNeuron].inputWeights[iWeight] * inputs[cWeight++];
				}

				// Multiply by the bias
				dAccumulatedInput += layers[iLayer].neurons[iNeuron].inputWeights[iNumInputs - 1] * bias;

				// Pass the final computed input value, multiplied by the weights, with the bias applied, to the sigmoid function to
				// obtain the final output value and store that in the vector of outputs.
				vecOutputs.push_back(sigmoid(dAccumulatedInput, sigmoidResponse));

				cWeight = 0;
			}
		}
		return vecOutputs;
	}

	void NeuralNetworkFeedForward::setWeightBias(double weightBias)
	{
		bias = weightBias;
	}

	void NeuralNetworkFeedForward::setSigmoidResponse(double response)
	{
		ErrorIfTrue(areDoublesEqual(0, response), L"NeuralNetworkFeedForward::setSigmoidResponse() failed. The given value should not be zero. Otherwise a divide by zero error occurs.");
		sigmoidResponse = response;
	}

	std::vector<int> NeuralNetworkFeedForward::calculateSplitPoints(void)
	{
		std::vector<int> vecSplitPoints;

		int iWeightCounter = 0;

		// For each layer (+1 as we are also giving the output layer)
		for (int iLayer = 0; iLayer < numberOfLayers + 1; iLayer++)
		{
			// For each neuron in each layer
			for (int iNeuron = 0; iNeuron < layers[iLayer].numberOfNeurons; iNeuron++)
			{
				// For each weight in each neuron, not including the bias of the neuron
				for (int iWeight = 0; iWeight < layers[iLayer].neurons[iNeuron].numberOfInputs; iWeight++)
				{
					iWeightCounter++;
				}
				vecSplitPoints.push_back(iWeightCounter - 1);
			}
		}
		return vecSplitPoints;
	}
}