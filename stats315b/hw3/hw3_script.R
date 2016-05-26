library(nnet)
dir = "C:/Users/wxia/Desktop/learning-stuff/312B-2016Spring/data"

setwd(dir)
rm(list = ls())

spam_predictors =  read.csv("Spam/Spam_Names.txt",skip = 33, sep =":",strip.white = TRUE, header=FALSE, colClasses = "character")
spam_names = c(spam_predictors[,1],'type')

spam_df = read.csv("Spam/Spam_Data.txt", header = FALSE)
colnames(spam_df) = spam_names

spam_df$type = class.ind(spam_df$type)
set.seed(100)
testIndex = sample(1:nrow(spam_df), nrow(spam_df) * 0.1)

spam_test = spam_df[testIndex,]
spam_train = spam_df[-testIndex, ]

nHidden = 10
nDiffsets = 5
nnRate = array(dim = c(nHidden, nDiffsets))
maxRate = 0

for (i in seq(1, nHidden, by = 1)) {
  nn = nnet(type ~ ., data = spam_train, size = i,
            softmax = TRUE, maxit = 1000, trace = F)
  test_predict = predict(nn, spam_test)
  index = test_predict[, 2] > 0.5
  test_01[index] = 1
  test_01[-index] = 0
  
  right = sum(test_01 == (spam_test$type)[, 2])
  # right classification rate
  rate = right / nrow(spam_test)
  nnRate[1][1] = rate
  if(rate > maxRate) {
    maxRate = rate
    bestModel = nn
  }
}
