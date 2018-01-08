import os
import sensegram
from gensim.models import word2vec

wv = word2vec.Word2Vec.load_word2vec_format(os.path.join(os.getcwd(), 'wiki.words'), binary=True)
sv = sensegram.SenseGram.load_word2vec_format(os.path.join(os.getcwd(), 'wiki.senses.w2v'), binary = True)
wsd_model = sensegram.WSD(sv, wv, window=5, method='sim', filter_ctx=3)

sv.get_senses("table")

sv.most_similar("table#0")
sv.most_similar("table#1")

wsd_model.dis_text("They bought a table and chairs for kitchen", "table", 14, 19)

sv.get_senses("change")

sv.most_similar("change#0")
sv.most_similar("change#1")
sv.most_similar("change#2")

wsd_model.dis_text("No change is given", "change", 3, 9)
wsd_model.dis_text("I noticed the change in her facial expression", "change", 14, 19)