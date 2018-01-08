# young_wit.py
# Natural Language Processing 2017
# Author : Cho Youngwoo
# 2017.06.21

from wit import Wit
import random

ACCESS_TOKEN = 'WRYYPG6SU6V2RCZZZ4NU3BXK3I3WFRPL'


def send(request, response):
	
	print(response['text'])

def track(request):

	context = request['context']
	entities = request['entities']

	if context.get('order_number') is not None:

		del context['order_number']

	if len(str(entities['order_number'][0]['value'])) != 8:

		# 잘못된 order number 를 받은 경우
		context['wrong'] = True

		if context.get('sent') is not None:

			del context['sent']

		if context.get('ready') is not None:

			del context['ready']

	else:

		# 정상적인 order number 를 받은 경우
		context['order_number'] = str(entities['order_number'][0]['value'])

		if random.randrange(0, 2) % 2 == 0:

			# 상품을 배송한 상황 가정
			context['sent'] = True
			context['tracking_number'] = str(random.randrange(0, 99999999))

			if context.get('wrong') is not None:

				del context['wrong']

			if context.get('ready') is not None:

				del context['ready']

		else:

			# 상품을 아직 배송하지 않은 상황 가정
			context['ready'] = True

			if context.get('wrong') is not None:

				del context['wrong']

			if context.get('sent') is not None:

				del context['sent']

	return context

actions = {

	'send': send,
	'track': track,
}

client = Wit(access_token = ACCESS_TOKEN, actions = actions)
client.interactive()