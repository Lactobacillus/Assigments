import sys
import urllib.request
from bs4 import BeautifulSoup
from collections import Counter
import pickle

appRawData = list()
category = list()
error = list()
urlFree = 'https://play.google.com/store/apps/collection/topselling_free'
urlPaid = 'https://play.google.com/store/apps/collection/topselling_paid'

pageFree = urllib.request.urlopen(urlFree)
soupFree = BeautifulSoup(pageFree)
soupFree = list(set(soupFree.find_all('a', 'card-click-target')))

pagePaid = urllib.request.urlopen(urlPaid)
soupPaid = BeautifulSoup(pagePaid)
soupPaid = list(set(soupPaid.find_all('a', 'card-click-target')))

# get free app ranking
for url in soupFree:

	while 1:
		
		try:
	
			url = 'https://play.google.com' + url['href']
			page = urllib.request.urlopen(url)
			soup = BeautifulSoup(page)
			soup = soup.find('a', 'document-subtitle category')
			category.append(soup.span.text)

			break

		except:

			print('Error in : ' + str(url))

			pass

	print(soup.span.text)

# get paid app ranking
for url in soupPaid:

	while 1:

		try:

			url = 'https://play.google.com' + url['href']
			page = urllib.request.urlopen(url)
			soup = BeautifulSoup(page)
			soup = soup.find('a', 'document-subtitle category')
			category.append(soup.span.text)

			break

		except:

			print('Error in : ' + str(url))
			pass

	print(soup.span.text)

# count free / paid app ranking
top100 = Counter(category)

# save top100
try:

	f = open('top100.txt', 'w')

	for key, element in top100.items():
		
		f.write("'" + str(key) + "' : " + str(element) + ', ')

	f.close()

except:

	pass

# load data : appList.txt
try:

	f = open('appList.txt','r')

	appList = eval('[' + f.read() + ']')

	f.close

except:

	print('Error : file read (appList.txt)')
	sys.exit(0)

	pass

# parse app information
for url in appList:

	try:

		app = dict()

		page = urllib.request.urlopen(url)
		soup = BeautifulSoup(page)

		# get developer name of app
		temp = soup.find('a', 'document-subtitle primary')
		app['developer'] = temp.span.text
		app['developerURL'] = 'https://play.google.com' + temp['href']

		# get category of app
		temp = soup.find('a', 'document-subtitle category')
		app['category'] = temp.span.text

		# get price of app
		# free : 0
		# paid : price of app
		temp = soup.find('button', 'price buy id-track-click id-track-impression')
		temp = temp.find_all('span')[-1].text

		if '설치' in temp:

			app['price'] = 0

		else:

			temp = temp.replace('₩', '')
			temp = temp.replace('구매', '')
			temp = temp.replace(' ', '')
			temp = temp.replace(',', '')
			app['price'] = int(temp)

		# get age requirement of app
		temp = soup.find('div', 'content', itemprop = 'contentRating').text
		
		if '18' in temp:

			app['ageRequirement'] = 1

		else:

			app['ageRequirement'] = 0

		# get downloads of app
		# value is range of downloads
		temp = soup.find('div', 'content', itemprop = 'numDownloads')
		app['downloads'] = temp.text.replace(' ', '')

		# get information of 'in app purchase' of app
		# value is 0 or 1
		temp = soup.find_all('div', 'inapp-msg')
		app['inAppPurchase'] = len(temp)

		# get file size of app
		# if there is not enough information, value is 0
		temp = soup.find('div', 'content', itemprop = 'fileSize')
		
		if temp == None:

			app['appSize'] = 0

		elif '기기' in temp.text:

			app['appSize'] = 0

		else:

			temp = temp.text.replace(',', '')

			if 'K' in temp or 'k' in temp:

				temp = temp.replace('K', '')
				temp = temp.replace('k', '')
				app['appSize'] = int(float(temp) * 1000.0)

			elif 'M' in temp or 'm' in temp:

				temp = temp.replace('M', '')
				temp = temp.replace('m', '')
				app['appSize'] = int(float(temp) * 1000000.0)

			else:

				temp = temp.replace('G', '')
				temp = temp.replace('g', '')
				app['appSize'] = int(float(temp) * 1000000000.0)

		# get devloper's star
		devPage = urllib.request.urlopen(app['developerURL'])
		devSoup = BeautifulSoup(devPage)

		starNum = 0
		temp = devSoup.find_all('div', 'tiny-star star-rating-non-editable-container')

		for star in temp:

			starNum += float(star['aria-label'][10:13])

		try:

			app['developerStar'] = round(starNum / float(len(temp)), 1)

		except:

			app['developerStar'] = 0

		# get popular app information
		# it means the number of apps that have same category
		# and it represents trend of app market
		try:

			app['inTop100'] = top100[app['category']]

		except:

			app['inTop100'] = 0

		# get deveploper's brand power
		# we assume developer's brand power is proportional to the number of google searches
		#########################################################################################
		#googlePage = urllib.request.urlopen('https://www.google.co.kr/search?q="' + str() + '"')
		#googleSoup = BeautifulSoup(googlePage)

		#temp = googleSoup.find('div', id = 'resultStats').text
		#temp = temp.replace('검색결과 약 ', '')
		#temp = temp.replace('개', '')
		#temp = temp.replace(',', '')

		#app['developerSearch'] = int(temp)
		#########################################################################################
		# we use another parse code

		# save these data in appRawData
		try:

			print(app)
			appRawData.append(app)

		except:

			pass

	except:

		error.append(url)
		pass

# save exception app list
try:

	f = open('error.txt', 'w')

	for err in error:

		f.write("'" + str(err) + "',\n")

	f.close()

except:

	pass

# save appRawData
try:

	f = open('appRawData.txt', 'w')

	for app in appRawData:

		f.write("'" + str(app) + "',\n")

	f.close()

except:

	pass

try:

	with open('pickled', 'wb') as f:

		pickle.dump(appRawData, f)

except:

	pass