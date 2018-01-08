# Four in a Row
# functions
# Cho Youngwoo
# 2014210085
# Artificial intelligence 2016

import random
from multiprocessing import Process, Queue
from operator import itemgetter

# MIN_TRIAL : default trial of MCTS per process
# MAP 		: save status of gameHistories

MIN_TRIAL = 5400000
TRIAL_RATIO = [0, 0, 1, 1, 1, 1, 1, 0.999999999, 0.999999996, 0.999999987, 0.999999961, 0.99999989, 0.999999689, 0.999999188, 0.999997889, 0.999994851, 0.999987542, 0.999971507, 0.999935504, 0.999861248, 0.999705274, 0.999402615, 0.99880708, 0.997720236, 0.99571783, 0.992287536, 0.986381152, 0.97691191, 0.961728792, 0.939069312, 0.905448538, 0.859106754, 0.796048614, 0.716715035, 0.619251366, 0.509611153, 0.391053451, 0.275615689, 0.170672269, 0.088816401, 0.034245822, 0.007610183]
WEIGHTED_TRIAL = list()
MAP = dict()

for i in range(0, 42):

	WEIGHTED_TRIAL.append(int(float(5 * MIN_TRIAL) / 6.0 * TRIAL_RATIO[i] + float(MIN_TRIAL) / 42.0 / 5.0))

print WEIGHTED_TRIAL


def checkStatus(gameMap):

	# not finish	=> 0
	# Computer win  => 1
	# Human win 	=> 2

	for row in gameMap:
		for col in range(0, 4):
			# check row
			temp = row[col] + row[col + 1] + row[col + 2] + row[col + 3]

			if temp == 12:
				# Computer win : four in a row
				return 1

			elif temp == 28:
				# Human win : four in a row
				return 2

	for row in range(0, 3):
		for col in range(0, 7):
			# check column
			temp = gameMap[row][col] + gameMap[row + 1][col] + gameMap[row + 2][col] + gameMap[row + 3][col]

			if temp == 12:
				# Computer win : four in a column
				return 1

			elif temp == 28:
				# Human win : four in a column
				return 2

	for row in range(0, 3):
		for col in range(0, 4):
			# check diagonal
			temp1 = gameMap[row][col] + gameMap[row + 1][col + 1] + gameMap[row + 2][col + 2] + gameMap[row + 3][col + 3]
			temp2 = gameMap[5 - row][col] + gameMap[4 - row][col + 1] + gameMap[3 - row][col + 2] + gameMap[2 - row][col + 3]

			if temp1 == 12 or temp2 == 12:
				# Computer win : four in a diagonal
				return 1

			elif temp1 == 28 or temp2 == 28:
				# Human win : four in a diagonal
				return 2
	# no finish
	return 0

def MCTS(gameMap, gameHistory, turnComputer, win, lose):

	move = random.randint(0, 6)
	row = gameHistory.count(str(move))

	if row > 5:

		return 0

	gameHistory += str(move)
	gameLength = len(gameHistory)

	if gameLength % 2 == turnComputer:
		# Computer's turn
		gameMap[5 - row][move] = 3

	else:
		# Human's turn
		gameMap[5 - row][move] = 7

	if gameHistory in MAP:

		status = MAP[gameHistory]

	else:

		status = checkStatus(gameMap)
		MAP[gameHistory] = int(status)

	if status == 0:

		MCTS(gameMap, gameHistory, turnComputer, win, lose)

	elif status == 1:

		win.append(gameHistory)

	elif status == 2:

		lose.append(gameHistory)

	return 0

def search(move, gameMap, gameHistory, turnComputer, result_queue):

	data = list()
	winList = list()
	loseList = list()

	random.seed()

	row = gameHistory.count(str(move))
	gameHistory += str(move)
	gameMap[5 - row][move] = 3

	while len(winList) + len(loseList) < WEIGHTED_TRIAL[len(gameHistory)]:

		MCTS(gameMap, gameHistory, turnComputer, winList, loseList)

	data = [move, -len(winList), len(loseList)]
	result_queue.put(data)

	return 0

def searchLight(move, gameMap, gameHistory, turnComputer, result_queue):

	data = list()
	winList = list()
	loseList = list()

	random.seed()

	row = gameHistory.count(str(move))
	gameHistory += str(move)
	gameMap[5 - row][move] = 3

	while len(winList) + len(loseList) < 10000:

		MCTS(gameMap, gameHistory, turnComputer, winList, loseList)

	data = [move, -len(winList), len(loseList)]
	result_queue.put(data)

	return 0

def determineBest(results):

	# choose best move by sorting
	integrate = [[0, 0, 0], [1, 0, 0], [2, 0 ,0], [3, 0 ,0], [4, 0, 0], [5, 0, 0], [6, 0, 0]]

	for element in results:

		integrate[element[0]][1] += element[1]
		integrate[element[0]][2] += element[2]
		
	return sorted(results, key = itemgetter(1, 2))[0][0]

def semiCandidate(temps):

	size = len(temps)
	candidate = list()

	if size > 4:

		size = 4

	temp = sorted(temps, key = itemgetter(1, 2))

	for i in range(0, size):

		candidate.append(temp[i][0])

	return candidate

def chooseCandidate(gameMap, gameHistory, who):

	candidate = list()
	temp = [1, 1, 1, 1, 1, 1, 1]

	# choose candidates of move which is not fool
	if who == 'c':
		# choose candidates for computer
		for moveOne in range(0, 7):

			rowOne = gameHistory.count(str(moveOne))

			if rowOne <= 5:
				# foresee next move, Computer's turn
				gameHistory += str(moveOne)
				gameMap[5 - rowOne][moveOne] = 3

				if checkStatus(gameMap) == 1:
					# if there is a case of CCCC in next move, computer must choose it
					for idx in range(0, 7):

						temp[idx] = 0

					temp[moveOne] = 1

				for moveTwo in range(0, 7):
					# if there is a case of HHHH, and computer must not choose it 
					rowTwo = gameHistory.count(str(moveTwo))

					if rowTwo <= 5:
						# foresee next next move, Human's turn
						gameHistory += str(moveTwo)
						gameMap[5 - rowTwo][moveTwo] = 7

						if checkStatus(gameMap) == 2:

							temp[moveOne] = 0

						gameMap[5 - rowTwo][moveTwo] = 0
						gameHistory = gameHistory[0:-1]

				gameHistory = gameHistory[0:-1]
				gameMap[5 - rowOne][moveOne] = 0

			else:

				temp[moveOne] = 0

	else:
		# choose candidate for human
		for moveOne in range(0, 7):

			rowOne = gameHistory.count(str(moveOne))

			if rowOne <= 5:
				# foresee next move, Human's turn
				gameHistory += str(moveOne)
				gameMap[5 - rowOne][moveOne] = 7

				if checkStatus(gameMap) == 2:
					# if there is a case of HHHH in next move, computer must choose it
					for idx in range(0, 7):

						temp[idx] = 0

					temp[moveOne] = 1

				for moveTwo in range(0, 7):
					# if there is a case of CCCC, and computer must not choose it 
					rowTwo = gameHistory.count(str(moveTwo))

					if rowTwo <= 5:
						# foresee next next move, Computer's turn
						gameHistory += str(moveTwo)
						gameMap[5 - rowTwo][moveTwo] = 3

						if checkStatus(gameMap) == 1:

							temp[moveOne] = 0

						gameMap[5 - rowTwo][moveTwo] = 0
						gameHistory = gameHistory[0:-1]

				gameHistory = gameHistory[0:-1]
				gameMap[5 - rowOne][moveOne] = 0

			else:

				temp[moveOne] = 0

	for idx in range(0, 7):

		if temp[idx] == 1:

			candidate.append(idx)

	return candidate

def turnOfComputer(gameMap, gameHistory, turnComputer):

	event = 0
	jobs = list()
	temp_jobs = list()
	results = list()
	temps = list()
	candidate = list()
	result_queue = Queue()
	temp_queue = Queue()

	if len(gameHistory) == 0:
		# the first turn, computer can move to 0, 1, 5 ,6
		# Computer choose 1 or 5
		move = random.randint(0, 1) * 4 + 1
		gameHistory += str(move)
		gameMap[5][move] = 3
		print('My best choice is to move ' + str(move))

		return gameMap, gameHistory, event

	elif len(gameHistory) == 1:
		# the second turn, Computer choose 2, 3, 4, 5
		move = random.randint(0, 3) + 2
		gameHistory += str(move)
		gameMap[5][move] = 3
		print('My best choice is to move ' + str(move))

		return gameMap, gameHistory, event

	else:
		# turn 3 ~ 42, Computer choose move by MCTS
		candidate = chooseCandidate(gameMap, gameHistory, 'c')

		if len(candidate) == 0:

			event = 1

			return gameMap, gameHistory, event

		for move in candidate:

			job = Process(target = search, args = (move, gameMap, gameHistory, turnComputer, result_queue, ))
			job.start()
			temp_jobs.append(job)

		for job in temp_jobs:

			job.join()
			temps.append(temp_queue.get())

		candidate = semiCandidate(temps)

		for move in candidate:

			for i in range(0, int(40 / len(candidate))):

				job = Process(target = search, args = (move, gameMap, gameHistory, turnComputer, result_queue, ))
				job.start()
				jobs.append(job)
			
		for job in jobs:

			job.join()
			results.append(result_queue.get())

		best = determineBest(results)
		row = gameHistory.count(str(best))
		gameHistory += str(best)
		gameMap[5 - row][best] = 3
		print('My best choice is ' + str(best))

		return gameMap, gameHistory, event

def turnOfHuman(gameMap, gameHistory):

	while 1:
		# check input and move
		move = input("What's your clever move?\nGive me a number of move (0 ~ 6) : ")
		gameHistory += str(move)
		row = gameHistory.count(str(move)) - 1

		if 0 <= move and move <= 6 and row <= 5:
			
			break

		else:

			gameHistory = gameHistory[0:-1]

	gameMap[5 - row][move] = 7

	return gameMap, gameHistory

def printMap(gameMap, gameHistory):

	for row in gameMap:

		temp = ''

		for element in row:

			if element == 3:
				# Computer's
				temp = temp + ' C '

			elif element == 7:
				# Human's
				temp = temp + ' H '

			else:
				# empty
				temp = temp + ' * '

		print(temp)

	print(' 0  1  2  3  4  5  6 ')
	print('Turn : ' + str(len(gameHistory)))
	print('History : ' + gameHistory + '\n')
