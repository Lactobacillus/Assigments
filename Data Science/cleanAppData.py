import sys
import time
import random
import requests
import pickle

X = list()
Y = list()

sizeList = list()
starList = list()
inTop100List = list()
priceList = list()

metaData = dict()

# load data : appRawData.txt
try:

	f = open("appRawData.txt", 'r')

	appRawData = f.read()
	appRawData = appRawData.replace("'{", "{")
	appRawData = appRawData.replace("}'", "}")
	appRawData = '[' + appRawData + ']'
	appRawData = eval(appRawData)

	f.close()

except:

	print('Error : file read (appRawData.txt)')
	sys.exit(0)

	pass

# calculate average app size, developer star
# and change invalid value into average
for app in appRawData:

	priceList.append(app['price'])
	inTop100List.append(app['inTop100'])

	if app['appSize'] != 0:

		sizeList.append(app['appSize'])

	if app['developerStar'] != 0:

		starList.append(app['developerStar'])

avgSize = int(sum(sizeList) / len(sizeList))
avgStar = round(sum(starList) / float(len(starList)), 1)

for app in appRawData:

	if app['appSize'] == 0:

		app['appSize'] = avgSize

	if app['developerStar'] == 0:

		app['developerStar'] = avgStar

# get deveploper's brand power
# we assume developer's brand power is proportional to the number of google searches
#################################################################################################
#for app in appRawData:
#
#	while 1:
#
#		try:
#
#			googleURL = 'https://www.google.co.kr/search?q=' + str('"' + app['developer'] + '"')
#			googlePage = requests.get(googleURL)
#			googleSoup = BeautifulSoup(googlePage.text)
#
#			temp = googleSoup.find('div', id = 'resultStats').text
#			temp = temp.replace('검색결과 약 ', '')
#			temp = temp.replace('개', '')
#			temp = temp.replace(',', '')
#
#			app['developerSearch'] = int(temp)
#
#			time.sleep(float(random.randrange(1, 20)) / 10.0)
#
#			break
#
#		except:
#
#			pass
#################################################################################################
# we use another parse code

# load data : googleSearch.txt
try:

	f = open("googleSearch.txt", 'r')

	googleSearch = f.readlines()

	f.close()

except:

	print('Error : file read (googleSearch.txt)')
	sys.exit(0)

	pass

# add googleSearch data to developerSearch in appRawData
idx = 0
for app in appRawData:

	app['developerSearch'] = int(googleSearch[idx])

	idx = idx + 1

# metaData have index of category and max value of each feature
metaData['category'] = {'의료' : 0, '엔터테인먼트' : 1, '사진' : 2, '퍼즐' : 3, '시뮬레이션' : 4, '도구' : 5, '교통' : 6, '카드' : 7, '어드벤처' : 8, '여행 및 지역정보' : 9, '퀴즈' : 10, '금융' : 11, '아케이드' : 12, '커뮤니케이션' : 13, '도서 및 참고자료' : 14, '소셜' : 15, '보드' : 16, '음악' : 17, '비즈니스' : 18, '날씨' : 19, '교육' : 20, '전략' : 21, '라이프스타일' : 22, '캐주얼 게임' : 23, '카지노' : 24, '생산성' : 25, '미디어 및 동영상' : 26, '자동차 경주' : 27, '액션' : 28, '음악 및 오디오' : 29, '데코레이션' : 30, '만화' : 31, '롤플레잉' : 32, '쇼핑' : 33, '뉴스 및 잡지' : 34, '건강/운동' : 35, '라이브러리 및 데모' : 36, '스포츠' : 37, '단어' : 38}
metaData['max'] = {'appSize' : max(sizeList), 'developerStar' : max(starList), 'inTop100' : max(inTop100List), 'price' : max(priceList), 'developerSearch' : max(googleSearch)}

# feature scaling
for app in appRawData:
#
#	try :
#
#		app['inTop100'] = float(app['inTop100']) / float(metaData['max']['inTop100'])
#
#	except:
#
#		app['inTop100'] = 0.0
#
#	app['appSize'] = float(app['appSize']) / float(metaData['max']['appSize'])
#	app['developerStar'] = float(app['developerStar']) / float(metaData['max']['developerStar'])
#	app['inTop100'] = float(app['inTop100']) / float(metaData['max']['inTop100'])
#	app['price'] = float(app['price']) / float(metaData['max']['price'])
	app['developerSearch'] = float(app['developerSearch']) / float(metaData['max']['developerSearch'])

# make data for training
for app in appRawData:

	x = list()

	#for i in range(0, 39):

	#	x.append(0)

	#x[metaData['category'][app['category']]] = 1
	#x.append(app['appSize'])
	x.append(app['developerStar'])
	x.append(app['inTop100'])
	x.append(app['price'])
	x.append(app['inAppPurchase'])
	x.append(app['ageRequirement'])
	#x.append(app['developerSearch'])

	y = app['downloads']

	X.append(x)
	
	# we merge some ranges of downloads
	if y in ('1-5','5-10','10-50','50-100','100-500','500-1,000'):

		Y.append('1-1,000')

	elif y in ('1,000-5,000','5,000-10,000'):

		Y.append('1,000-10,000')

	elif y in ('10,000-50,000','50,000-100,000'):

		Y.append('10,000-100,000')

	elif y in ('100,000-500,000','500,000-1,000,000','1,000,000-5,000,000'):

		Y.append('100,000-5,000,000')
        
	elif y in ('5,000,000-10,000,000','10,000,000-50,000,000'):

		Y.append('5,000,000-50,000,000')

	elif y in ('500,000,000-1,000,000,000', '1,000,000,000-5,000,000,000', '50,000,000-100,000,000','100,000,000-500,000,000'):

		Y.append('50,000,000-5,000,000,000')

	else:

		Y.append(y)
	
# save X, Y : txt file and pickle data
try:

	f = open('X.txt', 'w')

	for x in X:

		f.write("'" + str(x) + "',\n")

	f.close()

except:

	pass

try:

	f = open('Y.txt', 'w')

	for y in Y:

		f.write("'" + str(y) + "',\n")

	f.close()

except:

	pass

try:

	with open('pickledX', 'wb') as f:

		pickle.dump(X, f, protocol = 2)

except:

	pass

try:

	with open('pickledY', 'wb') as f:

		pickle.dump(Y, f, protocol = 2)

except:

	pass
