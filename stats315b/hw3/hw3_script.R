library(nnet)
dir = "C:/Users/wxia/Desktop/learning-stuff/312B-2016Spring/data"

setwd(dir)
rm(list = ls())

spam_predictors =  read.csv("Spam/Spam_Names.txt",skip = 33, sep =":",strip.white = TRUE, header=FALSE, colClasses = "character")
spam_names = c(spam_predictors[,1],'type')

spam_df = read.csv("Spam/Spam_Data.txt", header = FALSE)
colnames(spam_df) = spam_names

#data pre-process
spam_df_processed = array(dim = c(nrow(spam_df), ncol(spam_df)))
spam_df_processed = as.data.frame(spam_df_processed)
for (i in 1:57) {
  spam_df_processed[,i]<-scale(spam_df[,i])
}

spam_df_processed[, 58] = spam_df[, 58]
colnames(spam_df_processed) = spam_names

spam_df_processed$type = class.ind(spam_df_processed$type)
set.seed(100)
trainIndex = sample(1:nrow(spam_df), nrow(spam_df_processed) * 0.9)

spam_test = spam_df_processed[-trainIndex,]

nHidden = 10
nDiffsets = 15
nnRate = array(dim = c(nHidden, nDiffsets))
maxRate = 0

for (i in seq(1, nHidden, by = 1)) {
  for (j in seq(1, nDiffsets, by = 1)) {
    set.seed(j*i)
    index = sample(trainIndex, length(trainIndex), replace = FALSE)
    spam_train = spam_df_processed[index, ]
    nn = nnet(type ~ ., data = spam_train, size = i,
              softmax = TRUE, maxit = 1000, trace = F,
              rang = 0.5)
    test_predict = predict(nn, spam_test, type = 'class')
    
    right = sum(test_predict == (spam_test$type)[, 2])
    # misclassification rate
    rate = 1 - right / nrow(spam_test)
    nnRate[i, j] = rate
  }
}

plot(1:nHidden, rowMeans(nnRate), type = 'l')
bestHidden = which.min(rowMeans(nnRate))

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
  rate = 1 - right / nrow(spam_test)
  wdRate[i] = rate
  i = i + 1
}

index = which.min(wdRate)
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
  rate = 1 - right / nrow(spam_test)
  wdfineRate[i] = rate
  i = i + 1  
}
index2 = which.min(wdfineRate)
wdfinetuning = (index-1) * 0.1 + 0.2 * index2 / 100
min(wdfineRate)

#(c)
nn = nnet(type~. , data = spam_train, size = bestHidden, softmax = TRUE,
          maxit = 1000, rang = 0.5, decay = wdfinetuning, trace = F)

test_predict = predict(nn, spam_test, type = 'raw')
spamIndex = test_predict[, 2] > 0.98
test_01 = seq(0, 0, length.out = nrow(spam_test))
test_01[spamIndex] = 1

t = table(spam_test$type[, 2], test_01)
