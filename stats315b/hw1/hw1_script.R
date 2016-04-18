library(rpart)
library(rpart.plot)
dir = "/home/wxia/wei_project/cs231n/stats315b"

setwd(dir)
rm(list = ls())

set.seed(100)
df.incomedata = read.csv("Marketing/Income_Data.txt", header = FALSE)

incomedata_names = c("income", "sex", "marital", "age", "edu", "occu", "timelive",
                     "dual_income", "num.in.house", "p.under18", "house.status", "hometype", "ethnic", "language")

names(df.incomedata) = incomedata_names

factors = c(2, 3, 6, 8, 11, 12, 13, 14)
df.incomedata[, factors] = lapply(df.incomedata[, factors], factor)

temp = rpart.control(minsplit = 40, cp = 0)
fit = rpart(income ~ ., data = df.incomedata,
            method = 'class', control = temp)

plotcp(fit)
plot(fit)

fit.prune = prune(fit, cp = 0.002)
plot(fit.prune)
text(fit.prune)

#problem 2
df.housetypedata = read.csv("Marketing/Housetype_Data.txt", header = FALSE)
housetype_names = c("typeofhouse", "sex", "marital", "age", "edu", "occu", "income", "timelive",
                     "dual_income", "num.in.house", "p.under18", "house.status", "ethnic", "language")
names(df.housetypedata) = housetype_names

factors = c(1, 2, 3, 6, 9, 12, 13, 14)
df.housetypedata[, factors] = lapply(df.housetypedata[, factors], factor)

temp = rpart.control(cp = 0)
fit.housetype = rpart(typeofhouse ~ ., data = df.housetypedata,
            method = 'class', control = temp)
plotcp(fit.housetype)
plot(fit.housetype)

temp = rpart.control(cp = 1.7596e-03)
pruned.housetype = rpart(typeofhouse ~ ., data = df.housetypedata,
                      method = 'class', control = temp)
plot(pruned.housetype)
text(pruned.housetype)

# error rate

