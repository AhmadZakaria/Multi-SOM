// ROFL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>
#include <list>


const int NEURONS = 100;
const int DIMENSIONS = 10;
const int EXPAMPLES = 100;
const double INITSIGMA = 1;

class ROFL {
private:
	int CountDimensions;
	int CountNeurons;
	int p = 2.0;
	int NumberNeurons = 0;
	double StepsizeSigma = 0.5;
	double StepsizeCenter = 0.5;
	double Centers[NEURONS][DIMENSIONS];
	double Sigmas[NEURONS];
	double TrainingExamples[EXPAMPLES][DIMENSIONS];

public:
	explicit ROFL(int countDimensions, int countNeurons) {
		CountDimensions = countDimensions;
		CountNeurons = countNeurons;
	}

	double InitSigma()
	{
		return INITSIGMA;
	}

	double MeanSigma()
	{
		double sumSigma = 0;

		for (int neuron = 0; neuron < NumberNeurons; neuron++)
		{
			sumSigma += Sigmas[neuron];
		}

		return sumSigma / NumberNeurons;
	}

	double Distance(int neuron, int pattern)
	{

		double temp[10];

		for (int dimension = 0; dimension < CountDimensions; dimension++)
		{
			temp[dimension] = (Centers[neuron][dimension] - TrainingExamples[pattern][dimension])*(Centers[neuron][dimension] - TrainingExamples[pattern][dimension]);
		}

		double sum = 0;
		for (int dimension = 0; dimension < CountDimensions; dimension++)
		{
			sum += temp[dimension];
		}


		return sqrt(sum);
	}

	void AdaptNeuron(int pattern, int acceptingNeuron)
	{
		double distance = Distance(acceptingNeuron, pattern);

		for (int dimension = 0; dimension < CountDimensions; dimension++)
		{
			Centers[acceptingNeuron][dimension] = Centers[acceptingNeuron][dimension] + StepsizeCenter * (TrainingExamples[pattern][dimension] - Centers[acceptingNeuron][dimension]);
		}

		Sigmas[acceptingNeuron] = Sigmas[acceptingNeuron] + StepsizeSigma * (distance - Sigmas[acceptingNeuron]);
	}

	void CreateNewNeuron(int pattern)
	{
		for (int dimension = 0; dimension < CountDimensions; dimension++)
		{
			Centers[NumberNeurons][dimension] = TrainingExamples[pattern][dimension];
		}

		if(NumberNeurons == 0)
		{
			Sigmas[NumberNeurons] = InitSigma();
		}
		else
		{
			Sigmas[NumberNeurons] = MeanSigma();
		}


		NumberNeurons++;
	}

	std::list<int> FindAcceptingNeurons(int pattern, std::list<int> acceptingNeurons)
	{
		for (int neuron = 0; neuron < NumberNeurons; neuron++)
		{
			double radius = p * Sigmas[neuron];
			double tempSum = 0;
			for(int dimension = 0; dimension < CountDimensions;dimension++)
			{
				tempSum += pow(TrainingExamples[pattern][dimension] - Centers[neuron][dimension],2);
			}
			if(sqrt(tempSum) < radius)
			{
				acceptingNeurons.push_back(neuron);
			}
		}

		return acceptingNeurons;
	}

	int FindWinner(int pattern, std::list<int> acceptingNeurons)
	{
		int closestNeuron = 0;
				
		double minDistance = -1;

		for(int acceptingNeuron: acceptingNeurons)
		{
			double currentDistance = Distance(acceptingNeuron, pattern);

			if(currentDistance <minDistance || minDistance==-1)
			{
				minDistance = currentDistance;
				closestNeuron = acceptingNeuron;
			}
		}
		return closestNeuron;
	}

	void Train(double trainingExamples[EXPAMPLES][DIMENSIONS], int numberExamples)
	{

		for (int i = 0; i<EXPAMPLES; i++)
		{
			memcpy(&TrainingExamples[i], &trainingExamples[i], sizeof(trainingExamples[0]));
		}

		for (int pattern = 0; pattern < numberExamples; pattern++)
		{
			std::list<int> acceptingNeurons;

			acceptingNeurons = FindAcceptingNeurons(pattern, acceptingNeurons);

			if(acceptingNeurons.size() == 0)
			{
				if(NumberNeurons < NEURONS)
				{
					CreateNewNeuron(pattern);
				}
			}
			else if(acceptingNeurons.size() == 1)
			{	
				AdaptNeuron(pattern, acceptingNeurons.back());
			}
			else
			{
				int closestNeuron = FindWinner(pattern, acceptingNeurons);
				AdaptNeuron(pattern, closestNeuron);
			}

		}
	}

	

};

