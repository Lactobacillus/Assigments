import getpass
import requests
import urllib.request
from bs4 import BeautifulSoup

urlList = ['https://play.google.com/store/apps/category/HEALTH_AND_FITNESS','https://play.google.com/store/apps/category/EDUCATION','https://play.google.com/store/apps/category/TRANSPORTATION','https://play.google.com/store/apps/category/FINANCE','https://play.google.com/store/apps/category/WEATHER','https://play.google.com/store/apps/category/NEWS_AND_MAGAZINES','https://play.google.com/store/apps/category/PERSONALIZATION','https://play.google.com/store/apps/category/TOOLS','https://play.google.com/store/apps/category/BOOKS_AND_REFERENCE','https://play.google.com/store/apps/category/APP_WALLPAPER','https://play.google.com/store/apps/category/LIBRARIES_AND_DEMO','https://play.google.com/store/apps/category/LIFESTYLE','https://play.google.com/store/apps/category/COMICS','https://play.google.com/store/apps/category/GAME_EDUCATIONAL','https://play.google.com/store/apps/category/GAME_WORD','https://play.google.com/store/apps/category/GAME_ROLE_PLAYING','https://play.google.com/store/apps/category/GAME_BOARD','https://play.google.com/store/apps/category/GAME_SPORTS','https://play.google.com/store/apps/category/GAME_SIMULATION','https://play.google.com/store/apps/category/GAME_ARCADE','https://play.google.com/store/apps/category/GAME_ACTION','https://play.google.com/store/apps/category/GAME_ADVENTURE','https://play.google.com/store/apps/category/GAME_MUSIC','https://play.google.com/store/apps/category/GAME_RACING','https://play.google.com/store/apps/category/GAME_STRATEGY','https://play.google.com/store/apps/category/GAME_CARD','https://play.google.com/store/apps/category/GAME_CASINO','https://play.google.com/store/apps/category/GAME_CASUAL','https://play.google.com/store/apps/category/GAME_TRIVIA','https://play.google.com/store/apps/category/GAME_PUZZLE','https://play.google.com/store/apps/category/MEDIA_AND_VIDEO','https://play.google.com/store/apps/category/BUSINESS','https://play.google.com/store/apps/category/PHOTOGRAPHY','https://play.google.com/store/apps/category/PRODUCTIVITY','https://play.google.com/store/apps/category/SOCIAL','https://play.google.com/store/apps/category/SHOPPING','https://play.google.com/store/apps/category/SPORTS','https://play.google.com/store/apps/category/ENTERTAINMENT','https://play.google.com/store/apps/category/TRAVEL_AND_LOCAL','https://play.google.com/store/apps/category/APP_WIDGETS','https://play.google.com/store/apps/category/MUSIC_AND_AUDIO','https://play.google.com/store/apps/category/MEDICAL','https://play.google.com/store/apps/category/COMMUNICATION','https://play.google.com/store/apps/category/FAMILY?age=AGE_RANGE1','https://play.google.com/store/apps/category/FAMILY?age=AGE_RANGE2','https://play.google.com/store/apps/category/FAMILY?age=AGE_RANGE3','https://play.google.com/store/apps/category/FAMILY_EDUCATION','https://play.google.com/store/apps/category/FAMILY_BRAINGAMES','https://play.google.com/store/apps/category/FAMILY_ACTION','https://play.google.com/store/apps/category/FAMILY_PRETEND','https://play.google.com/store/apps/category/FAMILY_MUSICVIDEO','https://play.google.com/store/apps/category/FAMILY_CREATE']

targetList = list()
appList = list()

# we have to login google to see all apps in categories
googleID = input('google ID : ')
googlePW = getpass.getpass()

class SessionGoogle:
    def __init__(self, url_login, url_auth, login, pwd):
        self.ses = requests.session()
        login_html = self.ses.get(url_login)
        soup_login = BeautifulSoup(login_html.content).find('form').find_all('input')
        dico = {}
        for u in soup_login:
            if u.has_attr('value'):
                dico[u['name']] = u['value']
        # override the inputs with out login and pwd:
        dico['Email'] = login
        dico['Passwd'] = pwd
        self.ses.post(url_auth, data=dico)

    def get(self, URL):
        return self.ses.get(URL).text

url_login = "https://accounts.google.com/ServiceLogin"
url_auth = "https://accounts.google.com/ServiceLoginAuth"
session = SessionGoogle(url_login, url_auth, googleID, googlePW)

# parse categories of app
for url in urlList:

    while 1:

        try:

            page = session.get(url)
            soup = BeautifulSoup(page)
            buttons = soup.find_all('a', 'see-more play-button small id-track-click apps id-responsive-see-more')

            break

        except:

            print(url)
            pass
    
    print(len(buttons))

    for button in buttons:

        targetList.append('https://play.google.com' + button['href'])

# save categories of app
try:

    f = open('targetList.txt', 'w')

    for url in targetList:

        f.write("'" + str(url) + "',\n")

    f.close()

    print('finish targetList')

except:

    pass

# parse url of apps
for url in targetList:

    while 1:

        try:

            page = session.get(url)
            soup = BeautifulSoup(page)
            apps = soup.find_all('a', 'card-click-target')            

            break

        except:
            
            print(url)
            pass

    print(len(apps))

    for app in apps:

        appList.append('https://play.google.com' + app['href'])

# remove duplicated elements
appList = list(set(appList))

# save url of apps
try:

    f = open('appList.txt', 'w')

    for url in appList:

        f.write("'" + str(url) + "',\n")

    f.close()

except:

    pass