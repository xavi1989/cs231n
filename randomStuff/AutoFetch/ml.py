from sklearn.multiclass import OneVsOneClassifier
from sklearn.svm import LinearSVC
from sklearn import svm
from sklearn.metrics import confusion_matrix
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
import os
import numpy as np
import pandas as pd
from collections import Counter
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris

from imblearn.datasets import make_imbalance
from imblearn.under_sampling import RandomUnderSampler

import stats_excel

def loop(N):
    rate = np.zeros((N))
    for i in range(0, N):
        cmd = 'python3 stats_excel.py -s %s' % i
        print ('executing cmd: %s' % cmd)
        os.system(cmd)
        print ('cmd done')

        filename = 'data/option-11-20To12-15.csv'
        df = pd.read_csv(filename, sep = ' ').as_matrix()
        #print (df[0:5, :])

        X = df[:, [2, 3, 4, 5, 7]]
        base = X[:, 3].reshape((-1, 1))
        X[:, 0:4] = X[:, 0:4] / base
        #print (X[0:5, :])
        #print (X[:, 4])

        Y = df[:, 8]
        y1 = df[:, 7] > 0
        y2 = df[:, 8] == 'up'

        #print ([y1, y2])
        rate[i] = np.sum(y1 == y2) / len(y1)
        print (rate[i])
    print (rate)

def dataset_split(X, y, train_percent = 0.8, seed = 42):
    #X_train, X_test, y_train, y_test = train_test_split(X, y, testsize, randomstate)
    np.random.seed(seed)
    N = len(y)
    perm = np.random.permutation(range(N))

    train_num = int(N * train_percent)
    test_num = 1 - train_num

    X_train = X[perm[0 : train_num], :]
    y_train = y[perm[0 : train_num]]
    X_test  = X[perm[train_num : N], :]
    y_test  = y[perm[train_num : N]]

    #print (np.hstack((X_train, y_train.reshape((-1, 1)))))
    print (np.sum(y) / len(y))
    print (np.hstack((X_test, y_test.reshape((-1, 1)))))

    return X_train, X_test, y_train, y_test

def model_create(classifier = 'svm'):
    model = None
    if classifier == 'svm':
        model = svm.SVC(C = 1.0, degree = 3, kernel = 'linear')
    elif classifier == 'logistic':
        model = LogisticRegression(C = 1.0, penalty='l2')

    print (model)
    return model

def model_train(model, X, y):
    model.fit(X, y)

def model_predict(model, X):
    y = model.predict(X)
    return y

def plot_pie(y):
    target_stats = Counter(y)
    labels = list(target_stats.keys())
    sizes = list(target_stats.values())
    explode = tuple([0.1] * len(target_stats))

    fig, ax = plt.subplots()
    ax.pie(sizes, explode=explode, labels=labels, shadow=True,
           autopct='%1.1f%%')
    ax.axis('equal')

    plt.show()

if __name__ == '__main__':
    '''
    N = 15
    cmd = 'python3 stats_excel.py -s %s' % N
    print ('executing cmd: %s' % cmd)
    os.system(cmd)
    print ('cmd done')
    '''

    filename = 'data/option-11-20To12-15.csv'
    df = pd.read_csv(filename, sep = ' ').as_matrix()
    y1 = df[:, 7] > 0
    y2 = df[:, 8] == 'up'

    rate = np.sum(y1 == y2) / len(y1)
    print (rate)

    print (df[0:5, :])

    rate1 = np.clip((df[:, 4] - df[:, 3]) / (df[:, 2] - df[:, 3]), 0, 1)
    rate2 = np.clip((df[:, 5] - df[:, 3]) / (df[:, 2] - df[:, 3]), 0, 1)

    rate = np.vstack([rate1, rate2, df[:, 8].reshape((1, -1))])
    print (rate[:, 0:5])

    index_up = rate[2, :] == 'up'
    index_mid = rate[2, :] == 'mid'
    index_dw = rate[2, :] == 'dw'

    data_up = rate[:, index_up]
    data_mid = rate[:, index_mid]
    data_dw = rate[:, index_dw]

    x = np.arange(0, 1.2, 0.01)
    plt.axis([0, 1.2, 0, 1.2])
    plt.plot(x, [0.5] * len(x), 'b--')
    plt.plot([0.5] * len(x), x, 'b--')
    plt.plot(data_up[0, :], data_up[1, :], 'go')
    plt.plot(data_dw[0, :], data_dw[1, :], 'rx')
    plt.show()

    '''
    # ML
    y = df[:, 8]
    classnames, y = np.unique(y, return_inverse=True)
    X = df[:, [2, 3, 4, 5, 7]]

    #plot_pie(y)
    print ('counter is {}'.format(Counter(y)))

    ratio = {0: 8, 1: 11}
    X, y = make_imbalance(X, y, ratio=ratio)

    #split data
    X_train, X_test, y_train, y_test = dataset_split(X, y, train_percent = 0.8, seed = 2)

    # model create
    model = model_create('svm')

    # model fit on train data
    model_train(model, X_train, y_train)

    # model predict on train data
    y_train_pred = model_predict(model, X_train)
    print (np.sum(y_train == y_train_pred) / len(y_train))

    # model predict on test data
    y_test_pred = model_predict(model, X_test)
    print (np.sum(y_test == y_test_pred) / len(y_test))
    
    table = confusion_matrix(y_test, y_test_pred)
    print (table)
    '''

    '''
    classnames, y = np.unique(Y, return_inverse=True)
    print (classnames)
    print (y)

    y_pred = OneVsOneClassifier(LinearSVC(random_state=0)).fit(X, y).predict(X)
    print (np.vstack((y, y_pred)))
    table = confusion_matrix(y, y_pred)
    print (table)
    '''
