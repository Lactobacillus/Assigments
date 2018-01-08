# youngPS.py
# Operating System 2016
# Author : Cho Youngwoo
# 2016.06.09

# load libraries
import os
import sys
import time

# initialize time : t
# default directory : /proc/
t = 0.0
PATH = '/proc/'

# lists to compare wss
history = list()
isChangeWSS = dict()

# for 140 sec
while t < 140.0:

	pidList = list()
	processDict = dict()

	# for all files or directories
	for name in os.listdir(PATH):

		# if name is integer, it is pid
		# name can be file and directory 
		try:

			pidList.append(int(name))

		except:

			pass

	# open /proc/<pid>/status and /proc/<pid>/statm
	for pid in pidList:

		temp = dict()

		# open files
		try:

			fStatus = open(os.path.join(PATH, str(pid), 'status'), 'r')
			fStatm = open(os.path.join(PATH, str(pid), 'statm'), 'r')

			rawStatus = fStatus.readlines()
			rawStatm = fStatm.readlines()

		except:

			pass

		# status -> VmSize : virtual memory size
		# status -> VmRSS : physical memery size (non swap)
		# statm -> share : shared page size
		# status -> VmHWM : if change, wss is changed

		# get name, virtual, physical from status
		name = rawStatus[0].replace('Name:\t', '').replace('\n', '')
		virtual = rawStatus[12].replace('VmSize:\t', '').replace('kB\n', '').replace(' ', '')
		wss = rawStatus[15].replace('VmHWM:\t', '').replace('kB\n', '').replace(' ', '')
		physical = rawStatus[16].replace('VmRSS:\t', '').replace('kB\n', '').replace(' ', '')

		# get shared from statm
		rawStatm = rawStatm[0].split(' ')
		shared = str(int(rawStatm[2]) * 4)

		# save data
		if not 'SigQ' in virtual:

			temp['name'] = name + '\t'
			temp['virtual'] = virtual + '\t'
			temp['physical'] = physical + '\t'
			temp['shared'] = shared + '\t'
			temp['wss'] = wss

		else:

			temp['name'] = name + '\t'
			temp['virtual'] = '\t'
			temp['physical'] = '\t'
			temp['shared'] = shared + '\t'
			temp['wss'] = '\t'

		# check WSS has been changed
		try:

			if history[len(history) - 1][pid]['wss'] != temp['wss']:

				print(pid)
				isChangeWSS[pid] = 2

		except:

			isChangeWSS[pid] = 0

		processDict[pid] = temp
		
		# close files
		fStatus.close()
		fStatm.close()

	history.append(processDict)

	# print data
	print('PID\t'.expandtabs(10) + 'Name\t'.expandtabs(30) + 'Virtual\t'.expandtabs(15) + 'Physical\t'.expandtabs(15) + 'Shared\t'.expandtabs(15) + 'WSS\t')

	for pid in pidList:

		if isChangeWSS[pid] > 0:

			print(isChangeWSS[pid])

			string = '*'
			isChangeWSS[pid] = isChangeWSS[pid] - 1

		else:

			string = ''

		try:

			print(str(str(pid) + '\t').expandtabs(10) + processDict[pid]['name'].expandtabs(30) + processDict[pid]['virtual'].expandtabs(15) + processDict[pid]['physical'].expandtabs(15) + processDict[pid]['shared'].expandtabs(15) + string)

		except:

			print(str(str(pid) + '\t').expandtabs(10) + processDict[pid]['name'].expandtabs(30) + processDict[pid]['virtual'].expandtabs(15) + processDict[pid]['physical'].expandtabs(15) + processDict[pid]['shared'].expandtabs(15))

	# increasing time and sleep
	t = t + 0.5
	time.sleep(0.5)
	
	os.system('clear')
