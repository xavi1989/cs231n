import numpy as np

from cs231n.layers import *
from cs231n.fast_layers import *
from cs231n.layer_utils import *


class ThreeLayerConvNet(object):
  """
  A three-layer convolutional network with the following architecture:
  
  conv - batch - relu - 2x2 max pool - affine - batch - relu - affine - softmax
  
  The network operates on minibatches of data that have shape (N, C, H, W)
  consisting of N images, each with height H and width W and with C input
  channels.
  """
  
  def __init__(self, input_dim=(3, 32, 32), num_filters=32, filter_size=7,
               hidden_dim=100, num_classes=10, weight_scale=1e-3, reg=0.0,
               bn_params = {'mode': 'train'}, dtype=np.float32):
    """
    Initialize a new network.
    
    Inputs:
    - input_dim: Tuple (C, H, W) giving size of input data
    - num_filters: Number of filters to use in the convolutional layer
    - filter_size: Size of filters to use in the convolutional layer
    - hidden_dim: Number of units to use in the fully-connected hidden layer
    - num_classes: Number of scores to produce from the final affine layer.
    - weight_scale: Scalar giving standard deviation for random initialization
      of weights.
    - reg: Scalar giving L2 regularization strength
    - dtype: numpy datatype to use for computation.
    """
    self.params = {}
    self.reg = reg
    self.dtype = dtype

    ############################################################################
    # TODO: Initialize weights and biases for the three-layer convolutional    #
    # network. Weights should be initialized from a Gaussian with standard     #
    # deviation equal to weight_scale; biases should be initialized to zero.   #
    # All weights and biases should be stored in the dictionary self.params.   #
    # Store weights and biases for the convolutional layer using the keys 'W1' #
    # and 'b1'; use keys 'W2' and 'b2' for the weights and biases of the       #
    # hidden affine layer, and keys 'W3' and 'b3' for the weights and biases   #
    # of the output affine layer.                                              #
    ############################################################################
    W1 = np.random.normal(0,weight_scale,[num_filters, input_dim[0], filter_size, filter_size])
    b1 = np.zeros(num_filters)
    gamma1 = np.random.normal( 1 , 1e-3, num_filters)
    beta1 = np.zeros( num_filters )

    W2 = np.random.normal(0,weight_scale,[num_filters*input_dim[1]*input_dim[2]/4, hidden_dim])
    b2 = np.zeros(hidden_dim)
    gamma2 = np.random.normal( 1 , 1e-3, hidden_dim)
    beta2 = np.zeros( hidden_dim )

    W3 = np.random.random([hidden_dim, num_classes]) * weight_scale
    b3 = np.zeros(num_classes)

    self.params['W1'] = W1
    self.params['b1'] = b1
    self.params['gamma1'] = gamma1
    self.params['beta1'] = beta1
    self.bn_params = {}
    self.bn_params['mode'] = bn_params['mode']

    self.params['W2'] = W2
    self.params['b2'] = b2
    self.params['gamma2'] = gamma2
    self.params['beta2'] = beta2

    self.bn_params2 = {}
    self.bn_params2['mode'] = bn_params['mode']

    self.params['W3'] = W3
    self.params['b3'] = b3

    ############################################################################
    #                             END OF YOUR CODE                             #
    ############################################################################

    for k, v in self.params.iteritems():
      self.params[k] = v.astype(dtype)
     
 
  def loss(self, X, y=None):
    """
    Evaluate loss and gradient for the three-layer convolutional network.
    
    Input / output: Same API as TwoLayerNet in fc_net.py.
    """
    W1, b1, gamma1, beta1 = self.params['W1'], self.params['b1'], self.params['gamma1'], self.params['beta1']
    W2, b2, gamma2, beta2 = self.params['W2'], self.params['b2'], self.params['gamma2'], self.params['beta2']
    W3, b3 = self.params['W3'], self.params['b3']
    
    reg = self.reg
    # pass conv_param to the forward pass for the convolutional layer
    filter_size = W1.shape[2]
    conv_param = {'stride': 1, 'pad': (filter_size - 1) / 2}

    # pass pool_param to the forward pass for the max-pooling layer
    pool_param = {'pool_height': 2, 'pool_width': 2, 'stride': 2}

    bn_params = self.bn_params
    bn_params2 = self.bn_params2

    scores = None
    ############################################################################
    # TODO: Implement the forward pass for the three-layer convolutional net,  #
    # computing the class scores for X and storing them in the scores          #
    # variable.                                                                #
    ############################################################################
    layer_in = X
    layer_out, cache_conv = conv_batchnorm_relu_pool_forward(layer_in, W1, b1, conv_param, gamma1, beta1, bn_params, pool_param)

    layer_in, cache_l1 = affine_batchnorm_relu_forward(layer_out, W2, b2, gamma2, beta2, bn_params2)

    layer_out, cache_l2 = affine_forward(layer_in, W3, b3)

    scores = layer_out
    ############################################################################
    #                             END OF YOUR CODE                             #
    ############################################################################
    
    if y is None:
      return scores
    
    loss, grads = 0, {}
    ############################################################################
    # TODO: Implement the backward pass for the three-layer convolutional net, #
    # storing the loss and gradients in the loss and grads variables. Compute  #
    # data loss using softmax, and make sure that grads[k] holds the gradients #
    # for self.params[k]. Don't forget to add L2 regularization!               #
    ############################################################################
    loss, dout = softmax_loss(layer_out, y)

    reg_loss = 0.5 * reg * (np.sum(W1*W1) + np.sum(W2*W2) + np.sum(W3*W3))
    loss += reg_loss

    din, dW3, db3 = affine_backward(dout, cache_l2)

    dout, dW2, db2, dgamma2, dbeta2 = affine_batchnorm_relu_backward(din, cache_l1)

    din, dW1, db1, dgamma1, dbeta1 = conv_batchnorm_relu_pool_backward(dout, cache_conv)

    dW1 += reg * W1
    dW2 += reg * W2
    dW3 += reg * W3

    grads['W1'] = dW1
    grads['W2'] = dW2
    grads['W3'] = dW3

    grads['b1'] = db1
    grads['b2'] = db2
    grads['b3'] = db3

    grads['gamma1'] = dgamma1
    grads['gamma2'] = dgamma2

    grads['beta1'] = dbeta1
    grads['beta2'] = dbeta2
    ############################################################################
    #                             END OF YOUR CODE                             #
    ############################################################################
    
    return loss, grads


class MultiLayerConvNet(object):
  """
  A Multi-layer convolutional network with the following architecture:
  
  [conv - batch - relu - 2x2 max pool] - [conv - batch - relu]*(N-1) -
  [affine - batch - relu]*M - [affine - softmax]
  
  The network operates on minibatches of data that have shape (N, C, H, W)
  consisting of N images, each with height H and width W and with C input
  channels.
  """
  
  def __init__(self, input_dim=(3, 32, 32), num_filters=32, filter_size=7,
               hidden_dim=100, dropout=0, num_classes=10, weight_scale=1e-3,
               reg=0.0, bn_params = {'mode': 'train'}, dtype=np.float32, seed=None):
    """
    Initialize a new network.
    
    Inputs:
    - input_dim: Tuple (C, H, W) giving size of input data
    - num_filters: Number of filters to use in the convolutional layer, can be a vector
    - filter_size: Size of filters to use in the convolutional layer, same len as num_filter
    - hidden_dim: Number of units to use in the fully-connected hidden layer, can be a vector
    - num_classes: Number of scores to produce from the final affine layer.
    - weight_scale: Scalar giving standard deviation for random initialization
      of weights.
    - reg: Scalar giving L2 regularization strength
    - dtype: numpy datatype to use for computation.
    """

    self.num_convLayer = 1
    if isinstance(num_filters, list):
      self.num_convLayer = len(num_filters)
    else:
      num_filters = [num_filters]
      filter_size = [filter_size]

    self.use_dropout = dropout > 0

    self.num_fullLayer = 1
    if isinstance(hidden_dim, list):
      self.num_fullLayer = len(hidden_dim)
    else:
      hidden_dim = [hidden_dim]

    self.conv_params = {}
    self.bn_params = {}
    self.dropout_param = {}
    self.pool_param = {}
    self.reg = reg
    self.dtype = dtype

    self.params = {}
    # conv filter parameters
    # np.random.normal(0, scale = weight_scale, N, C, HH, WW) * num_convLayer
    # conv_params 'stride' 'pad' * num_convLayer
    # batchnorm bn_params 'mode' = 'train' * num_convLayer, gamma, beta
    # pool_param 'pool_height' 'pool_width' 'stride' * 1

    # first layer
    self.params['W1'] = np.random.normal(0, weight_scale, [num_filters[0], 3, filter_size[0], filter_size[0]])
    self.params['b1'] = np.zeros(num_filters[0])
    self.conv_params[0] = {}
    self.conv_params[0] = {'stride': 1, 'pad': (filter_size[0] - 1) / 2}
    #batch
    self.params['gamma1'] = np.random.normal(1, weight_scale, num_filters[0])
    self.params['beta1']  = np.zeros(num_filters[0])
    self.bn_params[0] = {}
    self.bn_params[0]['mode'] = bn_params['mode']
      
    #pool
    self.pool_param = {'pool_height': 2, 'pool_width': 2, 'stride': 2}

    #remaining conv layer
    for i in range(1, self.num_convLayer, 1):
      # conv part
      stringW = 'W%s' %(i+1)
      stringb = 'b%s' %(i+1)

      self.params[stringW] = np.random.normal(0, weight_scale, [num_filters[i], num_filters[i-1], filter_size[i], filter_size[i]])
      self.params[stringb] = np.zeros(num_filters[i])
      self.conv_params[i] = {}
      self.conv_params[i] = {'stride': 1, 'pad': (filter_size[i] - 1) / 2}

      # batch
      sgamma = "gamma%s" %(i+1)
      sbeta  = "beta%s"  %(i+1)
      self.params[sgamma] = np.random.normal(1, weight_scale, num_filters[i])
      self.params[sbeta]  = np.zeros(num_filters[i])
      self.bn_params[i] = {}
      self.bn_params[i]['mode'] = bn_params['mode']

    #hidden layer
    for i in range(self.num_convLayer, self.num_convLayer + self.num_fullLayer, 1):
      #affine
      stringW = "W%s" %(i+1)
      stringb = "b%s" %(i+1)
      if i == self.num_convLayer:
        predim = input_dim[1] * input_dim[2] * num_filters[i-1] / (self.pool_param['pool_height'] * self.pool_param['pool_width'])
      else:
        predim = hidden_dim[i-self.num_convLayer - 1]

      afterdim = hidden_dim[i-self.num_convLayer]

      self.params[stringW] = np.random.normal(0, weight_scale, [predim, afterdim])
      self.params[stringb]  = np.zeros(afterdim)

      #batch
      sgamma = "gamma%s" %(i+1)
      sbeta  = "beta%s"  %(i+1)
      self.params[sgamma] = np.random.normal(1, weight_scale, afterdim)
      self.params[sbeta]  = np.zeros(afterdim)
      self.bn_params[i] = {}
      self.bn_params[i]['mode'] = bn_params['mode']

      # drop out
      if self.use_dropout:
          self.dropout_param[i] = {}
          self.dropout_param[i] = {'mode': 'train', 'p': dropout}
          if seed is not None:
              self.dropout_param[i]['seed'] = seed


    #last layer, score layer
    #affine
    stringW = "W%s" %(self.num_convLayer + self.num_fullLayer+1)
    stringb = "b%s" %(self.num_convLayer + self.num_fullLayer+1)
    self.params[stringW] = np.random.normal(0, weight_scale, [hidden_dim[-1], num_classes])
    self.params[stringb] = np.zeros(num_classes) 

    for k, v in self.params.iteritems():
      self.params[k] = v.astype(dtype)

  def loss(self, X, y=None):
    """
    Evaluate loss and gradient for the three-layer convolutional network.
    
    Input / output: Same API as TwoLayerNet in fc_net.py.
    """
    X = X.astype(self.dtype)
    mode = 'test' if y is None else 'train'

    # Set train/test mode for batchnorm params and dropout param since they
    # behave differently during training and testing.
    for k, v in self.bn_params.iteritems():
        if v is not None:
            v['mode'] = mode

    for k, v in self.dropout_param.iteritems():
        if v is not None:
            v['mode'] = mode

    reg = self.reg
    caches = {}
    caches_dropout = {}
    layer_in = X
    w2_sum = 0
    #forward pass

    #first layer
    w2_sum += np.sum(self.params['W1'] * self.params['W1'])
    layer_out, caches[0] = conv_batchnorm_relu_pool_forward(layer_in, self.params['W1'], self.params['b1'], self.conv_params[0], self.params['gamma1'], self.params['beta1'], self.bn_params[0], self.pool_param)
    layer_in = layer_out

    # remainder conv layers
    for i in range(1, self.num_convLayer, 1):
      stringW = "W%s" %(i+1)
      stringb = "b%s" %(i+1)
      sgamma = "gamma%s" %(i+1)
      sbeta  = "beta%s"  %(i+1)

      w2_sum += np.sum(self.params[stringW] * self.params[stringW])
      layer_out, caches[i] = conv_batchnorm_relu_forward(layer_in, self.params[stringW], self.params[stringb], self.conv_params[i], self.params[sgamma], self.params[sbeta], self.bn_params[i])
      layer_in = layer_out


    #hidden layer
    for i in range(self.num_convLayer, self.num_convLayer + self.num_fullLayer, 1):
      stringW = "W%s" %(i+1)
      stringb = "b%s" %(i+1)
      sgamma = "gamma%s" %(i+1)
      sbeta  = "beta%s"  %(i+1)

      w2_sum += np.sum(self.params[stringW] * self.params[stringW])
      layer_out, caches[i] = affine_batchnorm_relu_forward(layer_in, self.params[stringW], self.params[stringb], self.params[sgamma], self.params[sbeta], self.bn_params[i])
      layer_in = layer_out

      # drop out
      if self.use_dropout:
          layer_out, caches_dropout[i] = dropout_forward(layer_in, self.dropout_param[i])
          layer_in = layer_out

    #last layer
    stringW = "W%s" %(self.num_convLayer + self.num_fullLayer+1)
    stringb = "b%s" %(self.num_convLayer + self.num_fullLayer+1)
    w2_sum += np.sum(self.params[stringW] * self.params[stringW])
    layer_out, caches[self.num_convLayer + self.num_fullLayer] = affine_forward(layer_in, self.params[stringW], self.params[stringb])

    scores = layer_out
    # If test mode return early
    if mode == 'test':
      return scores

    # backward
    loss, grads = 0.0, {}
    sftm_loss, sftm_grad = softmax_loss(scores, y)
    loss = sftm_loss
    loss += 0.5 * self.reg * w2_sum

    dout = sftm_grad
    # last layer
    stringW = "W%s" %(self.num_convLayer + self.num_fullLayer + 1)
    stringb = "b%s" %(self.num_convLayer + self.num_fullLayer + 1)

    din, grads[stringW], grads[stringb] = affine_backward(dout, caches[self.num_convLayer + self.num_fullLayer])
    grads[stringW] += reg * self.params[stringW]
    dout = din

    # hidden layer
    for i in range(self.num_convLayer + self.num_fullLayer - 1, self.num_convLayer-1, -1):
      if self.use_dropout:
          din = dropout_backward(dout, caches_dropout[i])
          dout = din

      stringW = "W%s" %(i+1)
      stringb = "b%s" %(i+1)
      sgamma = "gamma%s" %(i+1)
      sbeta  = "beta%s"  %(i+1)

      din, grads[stringW], grads[stringb], grads[sgamma], grads[sbeta] = affine_batchnorm_relu_backward(dout, caches[i])
      grads[stringW] += reg * self.params[stringW]
      dout = din

    # remainder conv layers
    for i in range(self.num_convLayer-1, 0, -1):
      stringW = "W%s" %(i+1)
      stringb = "b%s" %(i+1)
      sgamma = "gamma%s" %(i+1)
      sbeta  = "beta%s"  %(i+1)

      din, grads[stringW], grads[stringb], grads[sgamma], grads[sbeta] =  conv_batchnorm_relu_backward(dout, caches[i])
      grads[stringW] += reg * self.params[stringW]
      dout = din

    # first layer
    stringW = "W1"
    stringb = "b1"
    sgamma = "gamma1"
    sbeta  = "beta1"
    din, grads[stringW], grads[stringb], grads[sgamma], grads[sbeta] = conv_batchnorm_relu_pool_backward(dout, caches[0])
    grads[stringW] += reg * self.params[stringW]

    return loss, grads
