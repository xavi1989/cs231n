import numpy as np
from random import shuffle

def softmax_loss_naive(W, X, y, reg):
  """
  Softmax loss function, naive implementation (with loops)

  Inputs have dimension D, there are C classes, and we operate on minibatches
  of N examples.

  Inputs:
  - W: A numpy array of shape (D, C) containing weights.
  - X: A numpy array of shape (N, D) containing a minibatch of data.
  - y: A numpy array of shape (N,) containing training labels; y[i] = c means
    that X[i] has label c, where 0 <= c < C.
  - reg: (float) regularization strength

  Returns a tuple of:
  - loss as single float
  - gradient with respect to weights W; an array of same shape as W
  """
  # Initialize the loss and gradient to zero.
  loss = 0.0
  dW = np.zeros_like(W)

  #############################################################################
  # TODO: Compute the softmax loss and its gradient using explicit loops.     #
  # Store the loss in loss and the gradient in dW. If you are not careful     #
  # here, it is easy to run into numeric instability. Don't forget the        #
  # regularization!                                                           #
  #############################################################################
  for j in xrange(X.shape[0]):
    score = X[j, :].dot(W)
    score = np.exp(score)
    nor = np.sum(score)
    score = score / nor
    loss += (-np.log(score[y[j]]))

    for i in xrange(W.shape[1]):
        dW[:, i] += (score[i] - (i == y[j])) * X[j, :]

  loss = loss / X.shape[0]
  reg_loss = 0.5 * reg * np.sum(W * W)
  loss = loss + reg_loss

  dW = dW / X.shape[0]
  dW = dW + reg * W

  #############################################################################
  #                          END OF YOUR CODE                                 #
  #############################################################################

  return loss, dW


def softmax_loss_vectorized(W, X, y, reg):
  """
  Softmax loss function, vectorized version.

  Inputs and outputs are the same as softmax_loss_naive.
  """
  # Initialize the loss and gradient to zero.
  loss = 0.0
  dW = np.zeros_like(W)
  num_train = X.shape[0]
  #############################################################################
  # TODO: Compute the softmax loss and its gradient using no explicit loops.  #
  # Store the loss in loss and the gradient in dW. If you are not careful     #
  # here, it is easy to run into numeric instability. Don't forget the        #
  # regularization!                                                           #
  #############################################################################
  score = X.dot(W)
  score = np.exp(score)
  nor = np.sum(score, axis = 1)
  score = score.T / nor
  #score C * D
  m_loss = np.choose(y, score)
  loss = -np.sum(np.log(m_loss))
  loss = loss / num_train
  reg_loss = 0.5 * reg * np.sum(W * W)
  loss = loss + reg_loss

  # dW
  dW1 = X.T.dot(score.T)
  mask = np.zeros_like(score)
  mask[y, xrange(num_train)] = 1
  dW2 = X.T.dot(mask.T)

  dW = dW1 - dW2
  dW = dW / num_train
  reg_dW = reg * W
  dW = dW + reg_dW
  #############################################################################
  #                          END OF YOUR CODE                                 #
  #############################################################################

  return loss, dW

