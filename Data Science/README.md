# Predicting Downloads without Reviews
Predicting Downloads without Reviews

데이터과학 수업의 텀 프로젝트.

어떤 안드로이드 어플의 다운로드 수를 앱에 대한 리뷰 없이 예측.

말도 안되는 모델이지만, 정확도가 zeroR 보단 높음.

## Libraries

1. scikit-learn

2. BeautifulSoup4

3. requests

        pip install sklearn
    
        pip install BeautifulSoup4
    
        pip install requests
    
## Run
아래 순서로 실행.

1. getAppUrlList.py    (tested on python3, windows, cmd)

2. getAppData.py       (tested on python3, windows, cmd)

3. cleanAppData.py     (tested on python3, windows, cmd)

4. trainAppData.py     (tested on python2, linux, ipython)

5. useAppModel.py      (tested on python2, linux, ipython)

## Problems

한글 출력이 되지 않을 수 있음.

Sometimes, you can not print Korean (Python2 only)

이로 인해 category 를 feature 로 사용할 경우 오류가 생길 수 있음.

And it can cause error to use category feature.

만약 category 를 feature 로 사용하기 위해선 cleanAppData.py 를 python3, windows 환경에서 실행해야 함.

If you want to use category feature, you have to run cleanAppData.py in Python3, Windows.
