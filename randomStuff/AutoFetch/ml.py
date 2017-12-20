from sklearn.multiclass import OneVsOneClassifier
from sklearn.svm import LinearSVC
from sklearn.metrics import confusion_matrix

import numpy as np
import pandas as pd

if __name__ == '__main__':
    filename = 'data/option-11-20To12-15.csv'
    df = pd.read_csv(filename, sep = ' ').as_matrix()
    print (df[0:5, :])

    X = df[:, 2:6]
    base = X[:, 3].reshape((-1, 1))
    X = X / base
    print (X[0:5, :])

    Y = df[:, 7]
    Y[Y == 'up'] = 'rnd'
    Y[Y == 'dw'] = 'rnd'

    classnames, y = np.unique(Y, return_inverse=True)
    print (classnames)
    print (y)

    y_pred = OneVsOneClassifier(LinearSVC(random_state=0)).fit(X, y).predict(X)
    print (np.vstack((y, y_pred)))
    table = confusion_matrix(y, y_pred)
    print (table)
