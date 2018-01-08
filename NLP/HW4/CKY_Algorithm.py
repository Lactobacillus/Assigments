# CKY_Algorithm.py
# Natural Language Processing 2017
# Author : Cho Youngwoo
# 2017.05.16

import os
import sys

def readFile(fileName):

	# 개행문자로 구분되는 text 의 리스트를 반환
	
	with open(os.path.join(os.path.dirname(sys.argv[0]), fileName), 'r') as f:

		text = f.readlines()

	return text

def loadGrammer(rawText):

	# text 로 된 grammer 를 정리
	# 하나의 grammer 에서 두 개 이상의 production rule 이 있을 경우 '|' 로 구분
	# 하나의 production rule 에 두 개 이상의 string 이 들어갈 경우 ',' 로 구분
	# 공백문자는 신경쓰지 않음
	# epsilon 은 $ 로 표시
	# start symbol 은 항상 S
	# ex) S -> NP, VP
	# ex) NP -> DET, N | NP, PP 

	g = dict()

	for line in rawText:

		line = line.replace(' ', '').replace('\n', '').split('->')

		if not line[0] in g:

			g[line[0]] = list()

		for prod in line[1].split('|'):

			g[line[0]].append(prod.split(','))

	return g

def showGrammer(g):

	# grammer 를 표시

	print(':: Grammer')

	for key, value in g.items():

		t = key + ' -> '

		for vL in value:

			for v in vL:

				t = t + v + ' '

			t = t + '| '

		print('>> ' + t[:-2])

	print('')

def showTable(t):

	# table 을 표시

	length = 0
	lineLen = 0

	for r in t:

		for c in r:

			if isinstance(c, list) and length < len(str(c)) - 4 * len(c):

				length = len(str(c)) - 4 * len(c)

	print(':: table')

	for r in t:

		line = ''

		for c in range(1, len(r)):

			if r[c] == None:

				line = line + '[' + ' ' * (length + 3) + ']' + '\t'

			else:

				temp = ''

				for w in r[c]:

					temp = temp + w + ' '

				line = line + '[' + temp[:-1].center(length + 3, ' ') + ']' + '\t'

		print('>> ' + line.expandtabs(length + 7))

	print('')

def cky(g, s):

	# CKY Algorithm
	# 2 차원 테이블에서 작동하는 CKY Algorithm 의 pseudo code 를 구현
	# 주어진 sentence 가 grammer 에 accepted 되면 True 와 parse tree 를 리턴
	# 속하지 않으면 False 을 리턴

	sList = s.split(' ')
	table = [[None for i in range(0, len(sList) + 1)] for j in range(0, len(sList))]
	
	for i in range(1, len(sList) + 1):

		for key, value in g.items():

			for rule in value:

				if len(rule) == 1 and rule[0] == sList[i - 1]:

					table[i - 1][i] = [key]

	for j in range(2, len(sList) + 1):
		
		for i in reversed(range(0, j - 1)):

			for k in range(i + 1, j):

				try:

					for key, value in g.items():

						for rule in value:

							for t1 in table[i][k]:

								for t2 in table[k][j]:

									if [t1] + [t2] == rule:

										if table[i][j] == None:

											table[i][j] = list()

										table[i][j].append(key)
										table[i][j] = list(set(table[i][j]))

				except:

					pass
	
	if isinstance(table[0][len(sList)], list) and 'S' in table[0][len(sList)]:

		return (True, table)

	else:

		return (False, table)

def main():

	# main 함수, 진짜 main 은 아님
	# sentence 앞에 '#' 있으면 그 문장은 무시함

	grammer = loadGrammer(readFile(sys.argv[1]))
	inputText = readFile(sys.argv[2])

	showGrammer(grammer)

	for text in inputText:

		if text[0] != '#':

			result = cky(grammer, text.replace('\n', ''))
			
			print(':: Sentence >> ' + text.replace('\n', ''))

			if result[0]:

				print(':: Result >> Accepted')

			else:

				print(':: Result >> Rejected')

			showTable(result[1])

if __name__ == '__main__':

	# 진짜 main
	# 인자에 grammer 파일과 input 파일의 이름이 들어온 경우에만 실행
	# grammer 파일과 input 파일은 실행 파일과 같은 경로에 존재해야 함

	if len(sys.argv) != 3:

		print('Usage : CKY_Algorithm.py <grammer.txt> <input.txt>')

		sys.exit(0)

	else:

		main()

		sys.exit(0)