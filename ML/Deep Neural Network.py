# Deep Neural Network.py
# Machine Learning 2016
# Author : Cho Youngwoo
# 2016.12.12

# Code was written for Python3
# libraries
# 1. numpy
# 2. matplotlib
# you can install all these libraries with scipy
#     Windows (cmd) : pip install scipy
#     Linux (shell) : sudo pip install scipy

import time
import copy
import pickle
import numpy as np
import matplotlib.pyplot as pyplot
from multiprocessing import Pool

def getDataSet(fileName):

	# open file and seperate into X, Y
	with open(fileName, 'r') as f:

		x = list()
		y = list()
		lines = f.readlines()
		
		for line in lines:

			x.append([float(temp) for temp in ' '.join(line.split()).replace('\n', '').split(' ')[:-1]])
			y.append(int(line.split(' ')[-1]))

		return {'X' : np.transpose(np.array(x)), 'Y' : np.array(y).reshape(1, len(y))}

class LayerRBM():

	h_bias = None
	w = None

	def __init__(self, v = 13, h = 13, alpha = 0.000001):

		self.h_bias = (np.random.rand(h, 1) - 0.5)
		self.w = (np.random.rand(h, v) - 0.5)
		self.alpha = alpha

	def preTrain(self, x, v_bias, epoch = 5):

		v_bias = v_bias.reshape(x.shape[0], 1)
		
		for e in range(0, epoch):

			order = [i for i in range(0, int(x.shape[1]))]
			np.random.shuffle(order)

			for idx in order:
				
				v0 = x.copy()[:, idx].reshape(x.shape[0], 1)
				h0 = self.sigmoid(self.w.dot(v0) + self.h_bias)
				v1 = self.sigmoid(self.w.T.dot(h0) + v_bias)
				h1 = self.sigmoid(self.w.dot(v1) + self.h_bias)

				self.w += self.alpha * np.transpose(v0.dot(h0.T) - v1.dot(h1.T))
				self.h_bias += self.alpha * (h0 - h1)
				v_bias[:,0] += (self.alpha * (v0 - v1))[:,0]

		return None

	def forwardPropProb(self, x):
		
		return self.sigmoid(np.dot(self.w, x) + self.h_bias.reshape(self.w.shape[0], 1).repeat(x.shape[1], 1))

	def forwardPropRBM(self, x):

		prob = self.forwardPropProb(x)

		return np.greater(prob, np.random.random(prob.shape)).astype(int)

	def sigmoid(self, x):

		return 1 / (1 + np.exp(-x))

class DeepBeliefNet():

	X = None
	Y = None
	layers = list()

	def __init__(self, structure = [13, 13, 13, 1], alpha = 0.0001):
		# example of structure == [13, 13, 13, 1]
		# output layer   : 1
		# hidden layer 2 : 13
		# hidden layer 1 : 13
		# input          : 13
		for idx in range(0, len(structure) - 1):

			self.layers.append(LayerRBM(structure[idx], structure[idx + 1]))
		
		self.structure = structure
		self.alpha = alpha
		
	def setData(self, x, y):

		self.X = x.copy()
		self.Y = y.copy()

		return None

	def preTrain(self, epoch = 5):

		if type(self.X) == None or type(self.Y) == None:

			print('[Error] set data before initialize layer')

		start = time.time()
		print('[Info] pre-training started')

		for idx in range(0, len(self.structure) - 1):

			pre_start = time.time()

			if idx == 0:

				avg = np.mean(self.X, axis = 1).reshape(self.X.shape[0], 1)
				v = np.greater(self.X, avg).astype(int)
				self.layers[0].preTrain(v, np.zeros(shape = (self.structure[0], 1)), epoch)
				v = self.layers[0].forwardPropRBM(self.X)

			else:

				self.layers[idx].preTrain(v, self.layers[idx - 1].h_bias, epoch)
				v = self.layers[idx].forwardPropRBM(v)

			print('>>>>>> layer {0} pre-trained'.format(idx + 1))
			print('>>>>>> - elipsed time : ' + str(time.time() - pre_start)[:6])

		print('[Info] pre-training finished')
		print('>>>>>> total elapsed time : ' + str(time.time() - start)[:6])
		print('')

		return None

	def setWeight(self, layer, weight):

		if self.layers[layer].w.shape != weight.shape:

			print('[Error] the shape of weight is not equal')
			print('>>>>>> original shape : ' + str(self.layers[layer].w.shape))
			print('>>>>>> given shape    : ' + str(weight.shape))
			print('')

		else:

			self.layers[layer].w = weight
			print('[Info] the shape of layer ' + str(layer) + ' is changed')

		return None

	def train(self, epoch = 5, momentum = 0):

		if isinstance(self.X, np.ndarray) == False or isinstance(self.Y, np.ndarray) == False:

			print('[Error] There is no data to train')

		else:

			start = time.time()
			print('[Info] train is started')

			for e in range(0, epoch):

				epoch_start = time.time()

				L = len(self.layers)
				order = [i for i in range(0, int(self.X.shape[1]))]
				history = [np.zeros(self.layers[l].w.shape) for l in range(0, L)]
				np.random.shuffle(order)

				for idx in order:

					x = self.X.copy()[:, idx].reshape(self.X.shape[0], 1)
					y = self.Y.copy()[:, idx].reshape(self.Y.shape[0], 1)
					h = list()

					for l in range(0, L):

						if l == 0:

							h.append(self.layers[0].forwardPropProb(x))

						else:

							h.append(self.layers[l].forwardPropProb(h[l - 1]))

					for l in reversed(range(0, L)):

						if l == L - 1:

							err = np.subtract(y, h[l]) * h[l] * (1 - h[l])
							update = self.alpha * err.dot(h[l - 1].T)

						elif l == 0:

							err = self.layers[l + 1].w.T.dot(err) * h[l] * (1 - h[l])
							update = self.alpha * err.dot(x.T)
							
						else:

							err = self.layers[l + 1].w.T.dot(err) * h[l] * (1 - h[l])
							update = self.alpha * err.dot(h[l - 1].T)
							
						self.layers[l].w = (self.layers[l].w + update + momentum * history[l]).copy()
						history[l] = update.copy()
		
				r = self.test()
				acc = float(r['TP'] + r['TN']) / float(r['TP'] + r['TN'] + r['FP'] + r['FN'])

				print('>>>>>> epoch ' + str(e + 1))
				print('>>>>>> - training accuracy : ' + str(acc)[:6])
				print('>>>>>> - elapsed time      : ' + str(time.time() - epoch_start)[:6])

			print('[Info] train is finished')
			print('>>>>>> total elapsed time : ' + str(time.time() - start)[:6])
			print('')

		return None

	def forwardPropagation(self, x = None):

		if isinstance(x, np.ndarray):

			prob = self.layers[0].forwardPropProb(x)

		else:

			prob = self.layers[0].forwardPropProb(self.X)

		for idx in range(1, len(self.layers)):

			prob = self.layers[idx].forwardPropProb(prob)

		return prob

	def predict(self, x = None):

		if isinstance(x, np.ndarray):

			return np.greater(self.forwardPropagation(x), 0.5).astype(int)

		else:

			return np.greater(self.forwardPropagation(self.X), 0.5).astype(int)

	def test(self, x = None, y = None, predicted = None):

		tp = 0
		tn = 0
		fp = 0
		fn = 0

		if isinstance(x, np.ndarray) == False or isinstance(y, np.ndarray) == False:

			x = self.X.copy()
			y = self.Y.copy()

		if isinstance(predicted, np.ndarray) == False:

			predicted = self.predict(x)
		
		for idx in range(0, y.shape[1]):

			if predicted[0, idx] == y[0, idx] and predicted[0, idx] == 1:

				tp += 1

			elif predicted[0, idx] == y[0, idx] and predicted[0, idx] == 0:

				tn += 1

			elif predicted[0, idx] != y[0, idx] and predicted[0, idx] == 1:

				fp += 1

			elif predicted[0, idx] != y[0, idx] and predicted[0, idx] == 0:

				fn += 1

			else:

				pass

		return {'TP' : tp, 'TN' : tn, 'FP' : fp, 'FN' : fn}

	def drawROC(self, x, y, resolution):

		fpr = list()
		fnr = list()
		prior_prob = 0

		if isinstance(x, np.ndarray) == False:

			prob = self.layers[0].forwardPropProb(self.X)

		else:

			prob = self.layers[0].forwardPropProb(x)

		for idx in range(1, len(self.layers)):

			prob = self.layers[idx].forwardPropProb(prob)

		for i in range(0, resolution + 1):

			predicted = np.greater(prob, prior_prob).astype(int)
			result = self.test(x, y, predicted)

			fpr.append(float(result['FP']) / float(result['TN'] + result['FP']))
			fnr.append(1.0 - (float(result['FN']) / float(result['TP'] + result['FN'])))
			prior_prob += (1.0 / float(resolution))

		# ploting ROC curve
		pyplot.plot(fpr, fnr)
		pyplot.plot([0, 1], [1, 0])
		pyplot.plot([0, 1], [0, 1])
		pyplot.title('ROC curve - ' + str(self.structure))
		pyplot.xlabel('FP rate')
		pyplot.ylabel('FN rate')
		pyplot.show()

		return None

	def saveModel(self, fileName = None):

		if fileName == None:

			fileName = str(self.structure)

		try:

			with open(fileName, 'wb') as f:

				pickle.dump(self.layers, f)
				print('[Info] Saved model : ', fileName)

		except:

			print('[Error] Can not save model')

		return layers

	def loadModelFromFile(self, fileName):

		try:

			with open(fileName, 'rb') as f:

				layers = pickle.load(f)

			print('[Info] Load model successfully')

		except:

			print('[Error] Can not open model')

		return layers

	def showInfo(self):

		print('[Info] information of model')
		print('>>>>>> shape of data X        : ' + str(self.X.shape))
		print('>>>>>> shape of data Y        : ' + str(self.Y.shape))
		print('>>>>>> structure of model     : ' + str(self.structure))
		print('>>>>>> input dimension        : ' + str(self.structure[0]))
		print('>>>>>> output dimension       : ' + str(self.structure[-1]))
		print('>>>>>> number of layers       : ' + str(len(self.structure) - 1))
		print('>>>>>> learning rate of model : ' + str(self.alpha))
		print('')

		return None
	
def report(result):

	tp = str(result['TP']) + '\t'
	tn = str(result['TN']) + '\t'
	fp = str(result['FP']) + '\t'
	fn = str(result['FN']) + '\t'
	acc = float(result['TP'] + result['TN']) / float(result['TP'] + result['TN'] + result['FP'] + result['FN'])
	err = float(result['FP'] + result['FN']) / float(result['TP'] + result['TN'] + result['FP'] + result['FN'])

	# display accuracy / error and confusion matrix
	print('[Info] Result of experiment')
	print('>>>>>> Model Accuracy  : ' + str(acc)[:6] + '%')
	print('>>>>>> Empirical Error : ' + str(err)[:6] + '%')
	print('')
	print('[Info] Confusion Matrix')
	print('>>>>>>    ' + '\t'.expandtabs(10) + 'True : 1\t'.expandtabs(10) + 'True : 0\t'.expandtabs(10))
	print('>>>>>> Pred. : 1\t'.expandtabs(10) + tp.expandtabs(10) + fp.expandtabs(10))
	print('>>>>>> Pred. : 0\t'.expandtabs(10) + fn.expandtabs(10) + tn.expandtabs(10))
	print('')

	return None

def compareModel(params):
	# params[0] : structure
	# params[1] : train_set
	# params[2] : test_set
	model = DeepBeliefNet(structure = params[0])
	model.setData(params[1][0]['X'], params[1][0]['Y'])
	model.preTrain(epoch = 5)
	model.train(epoch = 15, momentum = 50)
	
	result = model.test(params[2][0]['X'], params[2][0]['Y'])
	acc = float(result['TP'] + result['TN']) / float(result['TP'] + result['TN'] + result['FP'] + result['FN'])

	return {'Structure' : params[0], 'Accuracy' : acc, 'Model' : copy.deepcopy(model)}

def crossValidation(process_nubmer, train, test):

	candidates = list()

	#candidates.append([[13, 11, 1], [train], [test]])
	#candidates.append([[13, 12, 1], [train], [test]])
	#candidates.append([[13, 13, 1], [train], [test]])
	#candidates.append([[13, 14, 1], [train], [test]])
	#candidates.append([[13, 15, 1], [train], [test]])

	#candidates.append([[13, 11, 11, 1], [train], [test]])
	#candidates.append([[13, 11, 12, 1], [train], [test]])
	#candidates.append([[13, 11, 13, 1], [train], [test]])
	#candidates.append([[13, 11, 14, 1], [train], [test]])
	#candidates.append([[13, 11, 15, 1], [train], [test]])

	#candidates.append([[13, 12, 11, 1], [train], [test]])
	#candidates.append([[13, 12, 12, 1], [train], [test]])
	#candidates.append([[13, 12, 13, 1], [train], [test]])
	#candidates.append([[13, 12, 14, 1], [train], [test]])
	#candidates.append([[13, 12, 15, 1], [train], [test]])

	#candidates.append([[13, 13, 11, 1], [train], [test]])
	#candidates.append([[13, 13, 12, 1], [train], [test]])
	candidates.append([[13, 13, 13, 1], [train], [test]])
	#candidates.append([[13, 13, 14, 1], [train], [test]])
	#candidates.append([[13, 13, 15, 1], [train], [test]])

	#candidates.append([[13, 14, 11, 1], [train], [test]])
	#candidates.append([[13, 14, 12, 1], [train], [test]])
	#candidates.append([[13, 14, 13, 1], [train], [test]])
	#candidates.append([[13, 14, 14, 1], [train], [test]])
	#candidates.append([[13, 14, 15, 1], [train], [test]])

	#candidates.append([[13, 15, 11, 1], [train], [test]])
	#candidates.append([[13, 15, 12, 1], [train], [test]])
	#candidates.append([[13, 15, 13, 1], [train], [test]])
	#candidates.append([[13, 15, 14, 1], [train], [test]])
	#candidates.append([[13, 15, 15, 1], [train], [test]])

	print('[Info] Cross Validation started')

	processor = Pool(process_nubmer)
	models = processor.map(compareModel, candidates)

	best_strcuture = None
	best_accuracy = 0
	best_model = None

	for m in models:

		if m['Accuracy'] > best_accuracy:

			best_strcuture = m['Structure']
			best_accuracy = m['Accuracy']
			best_model = m['Model']

	print('[Info] result of Cross Validation')
	print('>>>>>> Model Structure : ' + str(best_strcuture))
	print('>>>>>> Model Accuracy  : ' + str(best_accuracy)[:6] + '%')
	print('')

	try:

		with open(('best_' + str(best_strcuture)), 'wb') as f:

			pickle.dump(self.layers, f)
			
		print('[Info] Saved model : ', fileName)

	except:

		print('[Error] Can not save model')

	try:

		with open('models', 'wb') as f:

			pickle.dump(models)

		print('[Info] Saved models : ', fileName)

	except:

		print('[Error] Can not save models')

	return {'Structure' : best_strcuture, 'Model' : best_model}

def main():

	# open and get files
	train_set = getDataSet('trn.txt')
	test_set = getDataSet('tst.txt')
	full_X = np.concatenate((train_set['X'], test_set['X']), axis = 1)
	full_Y = np.concatenate((train_set['Y'], test_set['Y']), axis = 1)

	# cross validation of models
	#best = crossValidation(2, train_set, test_set)

	# train and test model
	#model = DeepBeliefNet(structure = best['Structure'])
	model = DeepBeliefNet(structure = [13, 13, 13, 1])
	model.setData(train_set['X'], train_set['Y'])
	model.preTrain(epoch = 5)
	model.train(epoch = 15, momentum = 50)
	model.showInfo()

	# report result and draw ROC curve
	experiment = model.test(full_X, full_Y)
	report(experiment)
	model.drawROC(full_X, full_Y, 100)

if __name__ == '__main__':

	main()
