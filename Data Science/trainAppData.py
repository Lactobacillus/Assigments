#-*- coding: utf-8 -*-

import sys
import pickle
import random
import sklearn
from sklearn.svm import SVC

reload(sys)
sys.setdefaultencoding('utf-8')

data = list()
train = list()
CV = list()

X_train = list()
Y_train = list()
X_CV = list()
Y_CV = list()
X_test = list()
Y_test = list()

# load pickled data : pickledX, pickledY
try:

	with open('pickledX', 'rb') as f:

		X = pickle.load(f)

except:

	print('Error : file read (pickledX)')
	sys.exit(0)

	pass

try:

	with open('pickledY', 'rb') as f:

		Y = pickle.load(f)

except:

	print('Error : file read (pickledY)')
	sys.exit(0)

	pass

# split data to train, CV, test
# train : CV : test = 6 : 2 : 2
for idx in range(0, len(X)):

	data.append([X[idx], Y[idx]])

while len(train) < 0.6 * len(X):

	idx = random.randint(0, len(data) - 1)
	
	train.append(data[idx])
	del data[idx]

while len(CV) < 0.2 * len(X):

	idx = random.randint(0, len(data) - 1)
	
	CV.append(data[idx])
	del data[idx]

for temp in train:

	X_train.append(temp[0])
	Y_train.append(temp[1])

for temp in CV:

	X_CV.append(temp[0])
	Y_CV.append(temp[1])

for temp in data:

	X_test.append(temp[0])
	Y_test.append(temp[1])

# find best model by cross validation
best = SVC(C = 1, decision_function_shape = 'ovo')
best.fit(X_train, Y_train)

for i in range(1, 5):

	# function : ovo
	model = SVC(C = i, decision_function_shape = 'ovo')
	model.fit(X_train, Y_train)
	pred_cv = model.predict(X_CV)
	prf = sklearn.metrics.precision_recall_fscore_support(Y_CV, pred_cv)

	if best.score(X_CV, Y_CV) < model.score(X_CV, Y_CV):

		best = model
	
	print('function : ovo, C : '+ str(i) + ', score : ' + str(model.score(X_CV, Y_CV)))
	print(prf[2])

for i in range(1, 5):

	# function : ovr
	model = SVC(C = i, decision_function_shape = 'ovr')
	model.fit(X_train, Y_train)
	pred_cv = model.predict(X_CV)
	prf = sklearn.metrics.precision_recall_fscore_support(Y_CV, pred_cv)

	if best.score(X_CV, Y_CV) < model.score(X_CV, Y_CV):

		best = model
	
	print('function : ovr, C : '+ str(i) + ', score : ' + str(model.score(X_CV, Y_CV)))
	print(prf[2])

# final score of best model
print('final model')
print(best)
print('final score of CV set : ' + str(best.score(X_CV, Y_CV)))
print('final score of test set : ' + str(best.score(X_test, Y_test)))

# save best model
try:

	with open('model', 'wb') as f:

		pickle.dump([best, best.score(X_test, Y_test)], f, protocol = 2)

except:

	pass