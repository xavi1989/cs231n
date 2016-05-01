from cs231n.layers import *
from cs231n.fast_layers import *


def affine_relu_forward(x, w, b):
  """
  Convenience layer that perorms an affine transform followed by a ReLU

  Inputs:
  - x: Input to the affine layer
  - w, b: Weights for the affine layer

  Returns a tuple of:
  - out: Output from the ReLU
  - cache: Object to give to the backward pass
  """
  a, fc_cache = affine_forward(x, w, b)
  out, relu_cache = relu_forward(a)
  cache = (fc_cache, relu_cache)
  return out, cache


def affine_relu_backward(dout, cache):
  """
  Backward pass for the affine-relu convenience layer
  """
  fc_cache, relu_cache = cache
  da = relu_backward(dout, relu_cache)
  dx, dw, db = affine_backward(da, fc_cache)
  return dx, dw, db


def affine_batchnorm_relu_forward(x, w, b, gamma, beta, bn_params):
  out, fc_cache = affine_forward(x, w, b)
  out, bn_cache = batchnorm_forward(out, gamma, beta, bn_params)
  out, relu_cache = relu_forward(out)
  cache = (fc_cache, bn_cache, relu_cache)
  return out, cache

def affine_batchnorm_relu_backward(dout, cache):
  fc_cache, bn_cache, relu_cache = cache
  dbatch = relu_backward(dout, relu_cache)

  da, dgamma, dbeta = batchnorm_backward(dbatch, bn_cache)

  dx, dw, db = affine_backward(da, fc_cache)
  return dx, dw, db, dgamma, dbeta  


def conv_relu_forward(x, w, b, conv_param):
  """
  A convenience layer that performs a convolution followed by a ReLU.

  Inputs:
  - x: Input to the convolutional layer
  - w, b, conv_param: Weights and parameters for the convolutional layer
  
  Returns a tuple of:
  - out: Output from the ReLU
  - cache: Object to give to the backward pass
  """
  a, conv_cache = conv_forward_fast(x, w, b, conv_param)
  out, relu_cache = relu_forward(a)
  cache = (conv_cache, relu_cache)
  return out, cache


def conv_relu_backward(dout, cache):
  """
  Backward pass for the conv-relu convenience layer.
  """
  conv_cache, relu_cache = cache
  da = relu_backward(dout, relu_cache)
  dx, dw, db = conv_backward_fast(da, conv_cache)
  return dx, dw, db


def conv_relu_pool_forward(x, w, b, conv_param, pool_param):
  """
  Convenience layer that performs a convolution, a ReLU, and a pool.

  Inputs:
  - x: Input to the convolutional layer
  - w, b, conv_param: Weights and parameters for the convolutional layer
  - pool_param: Parameters for the pooling layer

  Returns a tuple of:
  - out: Output from the pooling layer
  - cache: Object to give to the backward pass
  """
  a, conv_cache = conv_forward_fast(x, w, b, conv_param)
  s, relu_cache = relu_forward(a)
  out, pool_cache = max_pool_forward_fast(s, pool_param)
  cache = (conv_cache, relu_cache, pool_cache)
  return out, cache


def conv_relu_pool_backward(dout, cache):
  """
  Backward pass for the conv-relu-pool convenience layer
  """
  conv_cache, relu_cache, pool_cache = cache
  ds = max_pool_backward_fast(dout, pool_cache)
  da = relu_backward(ds, relu_cache)
  dx, dw, db = conv_backward_fast(da, conv_cache)
  return dx, dw, db

def conv_batchnorm_relu_forward(X, w, b, conv_param, gamma, beta, bn_param):
  layer_in = X
  layer_out, cache_l1 = conv_forward_fast(layer_in, w, b, conv_param)
  layer_in, cache_l2 = spatial_batchnorm_forward(layer_out, gamma, beta, bn_param)
  layer_out, cache_l3 = relu_forward(layer_in)

  cache = (cache_l1, cache_l2, cache_l3)

  return layer_out, cache

def conv_batchnorm_relu_backward(dout, cache):
  cache_l1, cache_l2, cache_l3 = cache

  din = relu_backward(dout, cache_l3)
  dout, dgamma, dbeta = spatial_batchnorm_backward(din, cache_l2)
  din, dw, db = conv_backward_fast(dout, cache_l1)

  return dout, dw, db, dgamma, dbeta

def conv_batchnorm_relu_pool_forward(X, w, b, conv_param, gamma, beta, bn_param, pool_param):
  layer_in = X
  layer_out, cache_l1 = conv_forward_fast(layer_in, w, b, conv_param)
  layer_in, cache_l2 = spatial_batchnorm_forward(layer_out, gamma, beta, bn_param)
  layer_out, cache_l3 = relu_forward(layer_in)
  layer_in, cache_l4 = max_pool_forward_fast(layer_out, pool_param)

  cache = (cache_l1, cache_l2, cache_l3, cache_l4)

  return layer_in, cache

def conv_batchnorm_relu_pool_backward(dout, cache):
  cache_l1, cache_l2, cache_l3, cache_l4 = cache

  din = max_pool_backward_fast(dout, cache_l4)
  dout = relu_backward(din, cache_l3)
  din, dgamma, dbeta = spatial_batchnorm_backward(dout, cache_l2)
  dout, dw, db = conv_backward_fast(din, cache_l1)

  return dout, dw, db, dgamma, dbeta

