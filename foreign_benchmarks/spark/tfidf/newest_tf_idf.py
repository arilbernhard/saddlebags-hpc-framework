#taken from https://github.com/apache/spark/blob/master/examples/src/main/python/mllib/tf_idf_example.py

import datetime
from pyspark import SparkContext
from pyspark.mllib.feature import HashingTF
from pyspark.mllib.feature import IDF

a = datetime.datetime.now()

sc = SparkContext()
documents = sc.wholeTextFiles("../wikipedia/large_wiki").map(lambda (file,contents): contents.split(" "))
tf = HashingTF().transform(documents)
tf.cache()

idf = IDF().fit(tf)
tfidf = idf.transform(tf)

test = tfidf.take(1)

b = datetime.datetime.now()
c = b-a

print("TOTAL TIME " + str(c.total_seconds()*1000))
