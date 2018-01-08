#-*- coding: utf-8 -*-

import sys
import pickle
import sklearn
import requests
from sklearn.svm import SVC
from bs4 import BeautifulSoup
from collections import Counter

reload(sys)
sys.setdefaultencoding('utf-8')

X_input = list()
metaData = dict()

metaData['category'] = {'의료' : 0, '엔터테인먼트' : 1, '사진' : 2, '퍼즐' : 3, '시뮬레이션' : 4, '도구' : 5, '교통' : 6, '카드' : 7, '어드벤처' : 8, '여행 및 지역정보' : 9, '퀴즈' : 10, '금융' : 11, '아케이드' : 12, '커뮤니케이션' : 13, '도서 및 참고자료' : 14, '소셜' : 15, '보드' : 16, '음악' : 17, '비즈니스' : 18, '날씨' : 19, '교육' : 20, '전략' : 21, '라이프스타일' : 22, '캐주얼 게임' : 23, '카지노' : 24, '생산성' : 25, '미디어 및 동영상' : 26, '자동차 경주' : 27, '액션' : 28, '음악 및 오디오' : 29, '데코레이션' : 30, '만화' : 31, '롤플레잉' : 32, '쇼핑' : 33, '뉴스 및 잡지' : 34, '건강/운동' : 35, '라이브러리 및 데모' : 36, '스포츠' : 37, '단어' : 38}
metaData['max'] = {'appSize' : max(sizeList), 'developerStar' : max(starList), 'inTop100' : max(inTop100List), 'price' : max(priceList), 'developerSearch' : max(googleSearch)}

# load data : top100.txt
try:

	f = open('top100.txt', 'r')

	top100 = '{' + f.read() + '}'
	top100 = eval(top100)
	top100 = Counter(top100)

	f.close()

except:

	print('Error : file read (top100)')
	sys.exit(0)

	pass

# load pickled data : model
try:

	with open('model', 'rb') as f:

		modelInfo = pickle.load(f)
		model = modelInfo[0]
		score = modelInfo[1]

except:

	print('Error : file read (model)')
	sys.exit(0)

	pass

# print model informaiton
print(model)
print('score : ' + str(score))

# X_input
try:

	print(metaData['category'])
	category = input('category (number) : ')

	if not category in metaData['category'].values():

		raise ValueError('invalid category')

	#for i in range(0, 39):

	#	X_input.append(0)

	#X_input[category] = 1

except:

	print('Error : wrong category')
	sys.exit(0)

	pass

try:

	appSize = input('appSize (integer byte) : ')
	#X_input.append(int(appSize))

except:

	print('Error : wrong appSize')
	sys.exit(0)

	pass

try:

	developerStar = input("developerStar (average star of developer's another app, float) : ")

	if float(developerStar) > 5.0 or float(developerStar) < 0.0:

		raise ValueError('range of developer star is 0 to 5')

	X_input.append(round(float(appSize), 1))

except:

	print('Error : wrong developerStar')
	sys.exit(0)

	pass

X_input.append(top100[category])

try:

	price = input('price (integer >= 0) : ')

	if int(price) < 0:

		raise ValueError('price can not be negative integer')

	X_input.append(int(price))

except:

	print('Error : wrong price')
	sys.exit(0)

	pass

try:

	inAppPurchase = input('inAppPurchase (0 or 1) : ')

	if not (int(inAppPurchase) == 0 or int(inAppPurchase) == 1):

		raise ValueError('invalid value')

	X_input.append(int(inAppPurchase))

except:

	print('Error : wrong inAppPurchase')
	sys.exit(0)

	pass

try:

	ageRequirement = input('ageRequirement (0 or 1) : ')

	if not (int(ageRequirement) == 0 or int(ageRequirement) == 1):

		raise ValueError('invalid value')

	X_input.append(int(ageRequirement))

except:

	print('Error : wrong ageRequirement')
	sys.exit(0)

	pass

try:

	developerName = raw_input('developerName to search in google (no special letters)')
	googleURL = 'https://www.google.co.kr/search?q=' + str('"' + str(developerName) + '"')
	googlePage = requests.get(googleURL)
	googleSoup = BeautifulSoup(googlePage.text)

	temp = googleSoup.find('div', id = 'resultStats').text
	temp = temp.replace('검색결과 약 ', '')
	temp = temp.replace('개', '')
	temp = temp.replace(',', '')

	#X_input.append(int(temp))

except:

	print('Error : wrong developerSearch')
	print('check special letters or internet status')
	sys.exit(0)

	pass

try:

	predict = model.predict(X_input)

	print('\ninput data : ' + str(X_input))
	print('predicted : ' + str(predict))

except:

	print('check the number of features')
	print('modify features in cleanAppData.py / useAppModel.py')
	sys.exit(0)

	pass