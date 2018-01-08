# Four in a Row
# main
# Cho Youngwoo
# 2014210085
# Artificial intelligence 2016

import timeit
import FIAR_functions as game

turn = 0
turnComputer = 0
event = 0
state = 0

# gameHistory data
# string like '0441562....' (MAX length : 42) 
gameHistory = ''

# gameMap data
# 0 : empty
# 1 : computer's
# 2 : Humans's
gameMap = [[0 for i in range(0,7)] for j in range(0, 6)]

# main
if __name__ == '__main__':
	
	while 1:
		# check input
		turnComputer = int(input('You first? or Computer first?\nYou : 1, Computer : 2 => ')) - 1

		if turnComputer == 0 or turnComputer == 1:

			break

	while state == 0 and event == 0:
		
		game.printMap(gameMap, gameHistory)

		if len(gameHistory) % 2 != turnComputer:
			# Computer's turn
			timeNow = timeit.default_timer()
			gameMap, gameHistory, event = game.turnOfComputer(gameMap, gameHistory, turnComputer)
			time = timeit.default_timer() - timeNow
			print('Elapsed time : ' + str(time))

		else:
			# Human's turn
			gameMap, gameHistory = game.turnOfHuman(gameMap, gameHistory)

		state = game.checkStatus(gameMap)

	game.printMap(gameMap, gameHistory)

	if event == 1:
		# Computer resign
		print('Computer resigns! You win!')

	elif state == 1:
		# Computer win
		print('Nice match! Computer wins.')

	elif state == 2:
		# Human win
		print('Nice match! You win!')

	else:
		# draw
		print("It's draw!")

	print('Game History is ' + gameHistory)
