library(nnet)
dir = "/home/wxia/wei_project/cs231n/stats315b/"

setwd(dir)
rm(list = ls())

spam_predictors =  read.csv("Marketing/Spam_Names.txt",skip = 33, sep =":",strip.white = TRUE, header=FALSE, colClasses = "character")
spam_names = c(spam_predictors[,1],'type')

spam_df = read.csv("Marketing/Spam_Data.txt", header = FALSE)
colnames(spam_df) = spam_names

#data pre-process
meanSpam = apply(spam_df[, 1:57], 2, mean)
sdSpam = sqrt(apply(spam_df[, 1:57], 2, var))

spam_df_processed = (spam_df[, 1:57] - meanSpam) / sdSpam
spam_df_processed = cbind(spam_df_processed, 'type' = spam_df[, 58])

spam_df_processed$type = class.ind(spam_df_processed$type)
set.seed(100)
testIndex = sample(1:nrow(spam_df), nrow(spam_df_processed) * 0.1)

spam_test = spam_df_processed[testIndex,]
spam_train = spam_df_processed[-testIndex, ]

nHidden = 10
nDiffsets = 5
nnRate = array(dim = c(nHidden, nDiffsets))
maxRate = 0

for (i in seq(1, nHidden, by = 1)) {
  for (j in seq(1, nDiffsets, by = 1)) {
    set.seed(j*i)
    nn = nnet(type ~ ., data = spam_train, size = i,
              softmax = TRUE, maxit = 1000, trace = F,
              rang = 0.5)
    test_predict = predict(nn, spam_test, type = 'class')
    
    right = sum(test_predict == (spam_test$type)[, 2])
    # right classification rate
    rate = right / nrow(spam_test)
    nnRate[i, j] = rate
    if(rate > maxRate) {
      maxRate = rate
      bestModel = nn
    }
  }
}

bestHidden = which.max(rowMeans(nnRate))

wdLength = 10
wdRate  = array(dim = c(wdLength, 1))
i = 1
for (wd in seq(0, 1, length.out = wdLength)) {
  nn = nnet(type~., data = spam_train, size = bestHidden,
            softmax = TRUE, maxit = 1000, trace = F,
            rang = 0.5, decay = wd)
  test_predict = predict(nn, spam_test, type = 'class')
  
  right = sum(test_predict == (spam_test$type)[, 2])
  # right classification rate
  rate = right / nrow(spam_test)
  wdRate[i] = rate
  i = i + 1
}

index = which.max(wdRate)
#weight decay fine tuning
wdLength = 100
wdfineRate = array(dim = c(wdLength, 1))
i = 1
for(wd in seq((index-1)*0.1, (index+1)*0.1, length.out = wdLength)) {
  nn = nnet(type~., data = spam_train, size = bestHidden,
            softmax = TRUE, maxit = 1000, trace = F,
            rang = 0.5, decay = wd)
  test_predict = predict(nn, spam_test, type = 'class')
  
  right = sum(test_predict == (spam_test$type)[, 2])
  # right classification rate
  rate = right / nrow(spam_test)
  wdRate[i] = rate
  i = i + 1  
}
