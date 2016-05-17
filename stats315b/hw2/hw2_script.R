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

gbm.a = gbm(type~., data = spam.train, n.trees = 2500, distribution = "bernoulli", cv.folds = 5, interaction.depth = 4, shrinkage = 0.05)
best.iter_cv = gbm.perf(gbm.a, method="cv")
gbm.prob = predict(gbm.a, spam.train, type = "response", n.trees = best.iter_cv)
index = which(gbm.prob > 0.5)
gbm.predict = gbm.prob
gbm.predict[index] = 1
gbm.predict[-index] = 0

wrong = sum(gbm.predict != spam.train$type)
missRate = wrong / dim(spam.train)[1]

table(spam.train$type, gbm.predict)

#test data
gbm.prob.test = predict(gbm.a, spam.test, type = "response", n.trees = best.iter_cv)
index.test = which(gbm.prob.test > 0.5)
gbm.predict.test = gbm.prob.test
gbm.predict.test[index.test] = 1
gbm.predict.test[-index.test] = 0

wrong = sum(gbm.predict.test != spam.test$type)
missRate.train = wrong / dim(spam.train)[1]
table(spam.test$type, gbm.predict.test)
#6b
index = which(gbm.prob > 0.35)
gbm.predict = gbm.prob
gbm.predict[index] = 1
gbm.predict[-index] = 0

wrong = sum(gbm.predict != spam.train$type)
missRate = wrong / dim(spam.train)[1]

table(spam.train$type, gbm.predict)

index.test = which(gbm.prob.test > 0.02)
gbm.predict.test = gbm.prob.test
gbm.predict.test[index.test] = 1
gbm.predict.test[-index.test] = 0

wrong = sum(gbm.predict.test != spam.test$type)
missRate.train = wrong / dim(spam.train)[1]
table(spam.test$type, gbm.predict.test)

summary(gbm.a)
# Problem 7
cal.raw = read.csv("Marketing/California_Data.txt", header = FALSE)
calnames = c("med.value", "med.income", "house.medAge", "avg.room", "avg.bedroom",
             "population", "Avg.Occupancy", "Latitude", "Longitude")
colnames(cal.raw) = calnames
#false_index = which(cal.raw$Avg.Occupancy > 20)
#cal.info = cal.raw[-false_index, ]
cal.info = cal.raw

cal.train = sample(1:nrow(cal.info), nrow(cal.info) * 0.8)

cal.boost = gbm(med.value~., data = cal.info[cal.train, ], distribution = "gaussian", n.trees = 2500, cv.folds = 5, interaction.depth = 4, shrinkage = 0.05)
best.iter_cv = gbm.perf(cal.boost, method="cv")
#predict on the test data
yhat.cal = predict(cal.boost, newdata = cal.info[-cal.train, ], n.trees = best.iter_cv)
mean((yhat.cal - cal.info$med.value[-cal.train])^2)

par(mfrow = c(1, 1))
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

income.boost = gbm(income~., data = income.data, distribution = "gaussian", n.trees = 2500, interaction.depth = 4, cv.folds = 5, shrinkage = 0.05)
best.iter_cv = gbm.perf(income.boost, method="cv")
income.predict = predict(income.boost, newdata = income.data, n.trees = best.iter_cv)
mean((income.predict - income.data$income)^2)
par(mfrow = c(1, 1))
summary(income.boost)

par(mfrow = c(1, 1))
boxplot(income.data$income~income.data$sex, data = income.data)

#problem 9
occupation.data = read.csv("Marketing/Occupation_Data.txt", header = FALSE)
occupationNames = c("occupation", "home.type", "sex", "marital.status", "age",
                    "edu", "annual.income", "time.live", "dual.income",
                    "num.person", "p.under18", "house.status", "ethnic", "language")
colnames(occupation.data) = occupationNames
factors = c(1, 2, 3, 4, 5, 6, 9, 12, 13, 14)
occupation.data[, factors] = lapply(occupation.data[, factors], factor)

index.train = sample(1:nrow(occupation.data), nrow(occupation.data) * 0.8)
occupation.boost = gbm(occupation~., data = occupation.data[index.train, ], distribution = "multinomial", n.trees = 2500, interaction.depth = 4, cv.folds = 5, shrinkage = 0.05)

test.predict = predict(occupation.boost, newdata = occupation.data[-index.train, ], type = "response", n.trees = 2500)
test.label = apply(test.predict, 1, which.max)

t = table(test.label, occupation.data[-index.train, ]$occupation)
right1 = diag(t)
total = apply(t, 1, sum)
1 - right1 / total

summary(occupation.boost)
