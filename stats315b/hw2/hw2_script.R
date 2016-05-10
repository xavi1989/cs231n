library("gbm")
dir = "/home/wxia/wei_project/cs231n/stats315b"

setwd(dir)
rm(list = ls())

set.seed(100)

spam.train = read.csv("Marketing/Spam_Train.txt", header = FALSE)
spam.test = read.csv("Marketing/Spam.Test.txt", header = FALSE)

spamnames = c("make", "address", "all", "3d", "our", "over", "remove",
          "internet","order", "mail", "receive", "will",
          "people", "report", "addresses","free", "business",
          "email", "you", "credit", "your", "font","000","money",
          "hp", "hpl", "george", "650", "lab", "labs",
          "telnet", "857", "data", "415", "85", "technology", "1999",
          "parts","pm", "direct", "cs", "meeting", "original", "project",
          "re","edu", "table", "conference", ";", "(", "[", "!", "$", "#",
          "CAPAVE", "CAPMAX", "CAPTOT","type")

colnames(spam.train) = spamnames 
colnames(spam.test) = spamnames 

gbm.a = gbm(type~., data = spam.train, n.trees = 2500, distribution = "bernoulli")
gbm.prob = predict(gbm.a, spam.train, type = "response", n.trees = 2500)
index = which(gbm.prob > 0.5)
gbm.predict = gbm.prob
gbm.predict[index] = 1
gbm.predict[-index] = 0

wrong = sum(gbm.predict != spam.train$type)
missRate = wrong / dim(spam.train)[1]

table(spam.train$type, gbm.predict)

#test data
gbm.prob.test = predict(gbm.a, spam.test, type = "response", n.trees = 2500)
index.test = which(gbm.prob.test > 0.5)
gbm.predict.test = gbm.prob.test
gbm.predict.test[index.test] = 1
gbm.predict.test[-index.test] = 0

wrong = sum(gbm.predict.test != spam.test$type)
missRate.train = wrong / dim(spam.train)[1]
table(spam.test$type, gbm.predict.test)


# Problem 7
cal.raw = read.csv("Marketing/California_Data.txt", header = FALSE)
calnames = c("med.value", "med.income", "house.medAge", "avg.room", "avg.bedroom",
             "population", "Avg.Occupancy", "Latitude", "Longitude")
colnames(cal.raw) = calnames
false_index = which(cal.raw$Avg.Occupancy > 10)
cal.info = cal.raw[-false_index, ]

cal.train = sample(1:nrow(cal.info), nrow(cal.info) * 0.8)

cal.boost = gbm(med.value~., data = cal.info[cal.train, ], distribution = "gaussian", n.trees = 2500, interaction.depth = 4)

#predict on the test data
yhat.cal = predict(cal.boost, newdata = cal.info[-cal.train, ], n.trees = 2500)
mean((yhat.cal - cal.info$med.value[-cal.train])^2)

summary(cal.boost)

par(mfrow = c(1, 2))
plot(cal.boost, i="med.income")
plot(cal.boost, i="Avg.Occupancy")

# Problem 8
income.data = read.csv("Marketing/Income_Data.txt", header = FALSE)

incomedata_names = c("income", "sex", "marital", "age", "edu", "occu", "timelive",
                     "dual_income", "num.in.house", "p.under18", "house.status", "hometype", "ethnic", "language")

names(income.data) = incomedata_names

factors = c(2, 3, 6, 8, 11, 12, 13, 14)
income.data[, factors] = lapply(income.data[, factors], factor)

income.boost = gbm(income~., data = income.data, distribution = "bernoulli", n.trees = 2500, interaction.depth = 4)