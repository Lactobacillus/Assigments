# Bayes' Classifier.py
# Machine Learning 2016
# Author : Cho Youngwoo
# 2016.10.31

# Code was written for Python3
# libraries
# 1. numpy
# 2. matplotlib
# you can install all these libraries with scipy
#     Windows (cmd) : pip install scipy
#     Linux (shell) : sudo pip install scipy

import numpy as np
import matplotlib.pyplot as pyplot

def getDataSet(fileName):

	# open file and seperate into X, Y
	with open(fileName, 'r') as f:

		x = list()
		y = list()
		lines = f.readlines()
		
		for line in lines:

			x.append([float(temp) for temp in ' '.join(line.split()).replace('\n', '').split(' ')[:-1]])
			y.append(int(line.split(' ')[-1]))

		return {'X' : np.array(x), 'Y' : np.array(y)}

def makeModel(data):

	prior = list()
	expectation = list()
	covariance = list()

	# calculate prior probabilities, expectation, covariance of input data
	prior.append((data['Y'].shape[0] - np.sum(data['Y'])) / data['Y'].shape[0])
	prior.append(np.sum(data['Y']) / data['Y'].shape[0])

	expectation.append(np.delete(data['X'], np.nonzero(data['Y'] == 1), 0).mean(0))
	expectation.append(np.delete(data['X'], np.nonzero(data['Y'] == 0), 0).mean(0))

	covariance.append(np.cov(np.delete(data['X'], np.nonzero(data['Y'] == 1), 0), rowvar = False))
	covariance.append(np.cov(np.delete(data['X'], np.nonzero(data['Y'] == 0), 0), rowvar = False))

	return {'Exp' : np.array(expectation), 'Cov' : np.array(covariance), 'Prior' : np.array(prior)}

def multivariate_gaussian(exp, cov, x):

	# PDF of multivariate gaussian distribution
	term1 = np.power(np.sqrt(2 * np.pi), 13) * np.sqrt(np.linalg.det(cov))
	term2 = np.subtract(x, exp)

	return np.exp(-0.5 * np.dot(np.dot(term2.T, np.linalg.inv(cov)), term2)) / term1

def predict(model, x):

	# compare probabilities to choose proper class
	prob0 = multivariate_gaussian(model['Exp'][0], model['Cov'][0], x) * model['Prior'][0]
	prob1 = multivariate_gaussian(model['Exp'][1], model['Cov'][1], x) * model['Prior'][1]
	
	if prob0 > prob1:

		return 0

	else:

		return 1

def test(model, dataset):

	tp = 0
	tn = 0
	fp = 0
	fn = 0
	predicted = list()

	for instance in dataset['X']:

		predicted.append(predict(model, instance))

	predicted = np.array(predicted)

	# counting TP, TN, FP, FN by comparing predicted and true Y value
	for idx in range(0, len(predicted)):

		if predicted[idx] == dataset['Y'][idx] and predicted[idx] == 1:

				tp = tp + 1

		elif predicted[idx] == dataset['Y'][idx] and predicted[idx] == 0:

				tn = tn + 1

		elif predicted[idx] != dataset['Y'][idx] and predicted[idx] == 1:

				fp = fp + 1

		elif predicted[idx] != dataset['Y'][idx] and predicted[idx] == 0:

				fn = fn + 1

		else:

			pass

	return {'TP' : tp, 'TN' : tn, 'FP' : fp, 'FN' : fn}

def drawROC(model, dataset, resolution):

	fpr = list()
	fnr = list()

	model['Prior'][0] = 0
	model['Prior'][1] = 1

	for i in range(0, resolution + 1):

		# changing prior probabilities, calculate FP rate and FN rate
		trial = test(model, dataset)

		fpr.append(trial['FP'] / (trial['TN'] + trial['FP']))
		fnr.append(1 - (trial['FN'] / (trial['TP'] + trial['FN'])))

		model['Prior'][0] = model['Prior'][0] + (1 / resolution)
		model['Prior'][1] = 1 - model['Prior'][0]

	# ploting ROC curve
	pyplot.plot(fpr, fnr)
	pyplot.plot([0, 1], [1, 0])
	pyplot.plot([0, 1], [0, 1])
	pyplot.title('ROC curve')
	pyplot.xlabel('FP rate')
	pyplot.ylabel('FN rate')
	pyplot.show()

	return None

def report(result):

	TP = str(result['TP']) + '\t'
	TN = str(result['TN']) + '\t'
	FP = str(result['FP']) + '\t'
	FN = str(result['FN']) + '\t'
	acc = (result['TP'] + result['TN']) / (result['TP'] + result['TN'] + result['FP'] + result['FN'])
	err = (result['FP'] + result['FN']) / (result['TP'] + result['TN'] + result['FP'] + result['FN'])

	# display accuracy / error and confusion matrix
	print('========== Result ==========')
	print('Model Accuracy  : ' + str(acc)[:6] + '%')
	print('Empirical Error : ' + str(err)[:6] + '%')
	print('===== Confusion Matrix =====')
	print('\t'.expandtabs(10) + 'True : 1\t'.expandtabs(10) + 'True : 0\t'.expandtabs(10))
	print('Pred. : 1\t'.expandtabs(10) + TP.expandtabs(10) + FP.expandtabs(10))
	print('Pred. : 0\t'.expandtabs(10) + FN.expandtabs(10) + TN.expandtabs(10))

	return None

def main():

	# open and get files
	trainSet = getDataSet('trn.txt')
	testSet = getDataSet('tst.txt')

	# train and test model
	model = makeModel(trainSet)
	experiment = test(model, testSet)

	# report result and draw ROC curve
	report(experiment)
	drawROC(model, testSet, 100)

if __name__ == '__main__':

	main()