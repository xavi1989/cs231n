"""

Framework for Bracket orders
by FemtoTrader <femto.trader@gmail.com>
Copyright (c) 2015-01-11

Done:
master order as market order and 2 child order
 - stop loss order: stop order
 - take profit order: limit order
store date_open and date_close
implements master order as pending
Trailing stop (modify stop order)
Bug fixed: Fix MAE MFE when entry is SELL (should be MAE<0 MFE>0)
Pluggable features for bracket orders
Trailing stop as "BOFeature" (additional feature for bracket orders)
BreakEven as "BOFeature" (additional feature for bracket orders)
features (BOFeature or collection BOFeatures) as parameter of order_send
expiration of pending orders as BOFeature
order modify (price, price_limit, price_stop)

ToDo:
price_close in add_feature ? pb with update
idem price_open

Pluggable features for bo_blotter (order statistics like MAE MFE)
MAE MFE statistics as "BOBlotterFeature"
    feature = MAE_MFE_BOBlotter_Feature()
    blotter.add_feature(name, feature)
    # ensure name doesn't exists in _d_features
    Need first to define BOFeature:AdvancedStats and after BlotterFeature:AdvancedStats

test is filled (with market orders, pending orders)
hidden trailing stop
calculate exit and entry efficiency
exchange for master order should be passed to child orders
partial close
test without SL and TP
order_modify (expiration is not supported)
price_rel(price, pct)
risk management
calculate current risk
manage leverage
modify_limit and modify_stop without canceling
  not possible
  test if order was cancelled correctly before putting an other order
secure bracket order blotter:
    compare bracket order blotter with get_open_orders()
    opened_orders = get_open_orders()
    group by security and order style
    and sum
    -volume pending = volume market (?)
OCO OCA order (a much cleaner approach)

"""

import pandas as pd
import numpy as np
from math import copysign
from collections import OrderedDict
from datetime import timedelta
#import inspect

def not_implemented_error(msg=""):
    raise(NotImplementedError(msg))

def sign(x):
    """Returns sign of x
    either +1 or -1
    """
    return(copysign(1, x))

def is_buy(order):
    """Returns a boolean
    if vol>=0 is_buy is True
    else (vol<0) is_buy is False
    """
    return(copysign(1, order.amount)>=0)

def rel_value(value, pct):
    """Returns a value (price) with +/- pct (%)"""
    return(value*(1+pct/100.0))

def price_SL_TP_rel(price, volume, relative_stop_loss, relative_take_profit):
    """Returns SL and TP price from price, volume and relative SL and TP"""
    price_stop_loss = rel_value(price, -sign(volume)*relative_stop_loss)
    price_take_profit = rel_value(price, sign(volume)*relative_take_profit)
    return(price_stop_loss, price_take_profit)

def dt_td_to_dt(x, dt):
    """Returns a datetime
    if x is a datetime it returns dt+x
    else it returns dt (assuming it's a datetime)"""
    try:
        x.total_seconds()
        # if it doesn't raise exception it should be a timedelta
        # we shoudl use isinstance(x, timedelta) but it's forbidden
        return(dt + x)
    except:
        # we assume x is a datetime
        return(x)
    
def repr_execution_style(style, _is_buy=True):
    pending = (style.get_limit_price(_is_buy) is not None) or (style.get_stop_price(_is_buy) is not None)
    if not pending:
        s = "<Exec MarketOrder>"
    else:
        s = "<Exec PendingOrder pLim=%s pStop=%s>" % (style.get_limit_price(_is_buy), style.get_stop_price(_is_buy))
    return(s)

def enumerate_reversed(L):
   """Returns a reversed iterator"""
   for index in reversed(xrange(len(L))):
      yield index, L[index]

    

# ======
# Signal / Slot : from https://github.com/Numergy/signalslot/


class SignalSlotException(Exception):
    """Base class for all exceptions of this module."""
    def raise_exc(self, m):
        raise(Exception(m))

class SlotMustAcceptKeywords(SignalSlotException):
    """
    Raised when connecting a slot that does not accept ``**kwargs`` in its
    signature.
    """
    def __init__(self, signal, slot):
        m = 'Cannot connect %s to %s because it does not accept **kwargs' % (
            slot, signal)
        self.raise_exc(m)


class QueueCantQueueNonSignalInstance(SignalSlotException):
    """
    Raised when trying to queue something else than a
    :py:class:`~signalslot.signal.Signal` instance.
    """
    def __init__(self, queue, arg):
        m = 'Cannot queue %s to %s because it is not a Signal instance' % (
            arg, queue)
        self.raise_exc(m)


# ====


"""
Module defining the Signal class.
"""


class Signal(object):
    """
    Define a signal by instanciating a :py:class:`Signal` object, ie.:

    >>> conf_pre_load = Signal()

    Optionaly, you can declare a list of argument names for this signal, ie.:

    >>> conf_pre_load = Signal(args=['conf'])

    Any callable can be connected to a Signal, it **must** accept keywords
    (``**kwargs``), ie.:

    >>> def yourmodule_conf(conf, **kwargs):
    ...     conf['yourmodule_option'] = 'foo'
    ...

    Connect your function to the signal using :py:meth:`connect`:

    >>> conf_pre_load.connect(yourmodule_conf)

    Emit the signal to call all connected callbacks using
    :py:meth:`emit`:

    >>> conf = {}
    >>> conf_pre_load.emit(conf=conf)
    >>> conf
    {'yourmodule_option': 'foo'}

    Note that you may disconnect a callback from a signal if it is already
    connected:

    >>> conf_pre_load.is_connected(yourmodule_conf)
    True
    >>> conf_pre_load.disconnect(yourmodule_conf)
    >>> conf_pre_load.is_connected(yourmodule_conf)
    False
    """
    def __init__(self, args=None, name=None):
        self.slots = []
        self.args = args or []
        self.name = name

    def connect(self, slot):
        """
        Connect a callback ``slot`` to this signal.
        """
        #if inspect.getargspec(slot).keywords is None:
        #    raise exceptions.SlotMustAcceptKeywords(self, slot)

        if not self.is_connected(slot):
            self.slots.append(slot)

    def is_connected(self, slot):
        """
        Check if a callback ``slot`` is connected to this signal.
        """
        return slot in self.slots

    def disconnect(self, slot):
        """
        Disconnect a slot from a signal if it is connected else do nothing.
        """
        if self.is_connected(slot):
            self.slots.pop(self.slots.index(slot))

    def emit(self, **kwargs):
        """
        Emit this signal which will execute every connected callback ``slot``,
        passing keyword arguments.

        If a slot returns anything other than None, then :py:meth:`emit` will
        return that value preventing any other slot from being called.

        >>> need_something = Signal()
        >>> def get_something(**kwargs):
        ...     return 'got something'
        ...
        >>> def make_something(**kwargs):
        ...     print('I will not be called')
        ...
        >>> need_something.connect(get_something)
        >>> need_something.connect(make_something)
        >>> need_something.emit()
        'got something'
        """
        for slot in self.slots:
            result = slot(**kwargs)

            if result is not None:
                return result

    def equal(self, other):
        """
        Return True if other has the same slots connected.

        >>> a = Signal()
        >>> b = Signal()
        >>> a == b
        True
        >>> def slot(**kwargs):
        ...    pass
        ...
        >>> a.connect(slot)
        >>> a == b
        False
        >>> b.connect(slot)
        >>> a == b
        True
        """
        return self.slots == other.slots

    def __repr__(self):
        return '<signalslot.Signal: %s>' % (self.name or 'NO_NAME')


# ====
    
    
 
class Feature(object):
    """Abstraction class of feature"""
    is_collection = False
    default_name = "default_feature_name"
    
    _data = None
    
    def init(self, name):
        self.name = name
        self._data = OrderedDict()

    @property
    def name(self):
        """Get feature name
        if name is None default_name will be use"""
        if self._name is None:
            return(self.default_name)
        else:
            return(self._name)

    @name.setter
    def name(self, new_name):
        """Set feature name"""
        self._name = new_name        

class BOFeatures(object):
    """Bracket order features collection
    
    A collection of additional features to apply to a bracket order
    Features can be 
     - TrailingStop
     - BreakEven
     - or anything that applies to
    a bracket order and which need to be update when incoming price
    
    Features are BOFeature
    this collection is BOFeatures
    """

    is_collection = True
    
    _d_features = None #OrderedDict    
    
    def __init__(self, bo=None):
        self.bo = bo
        self._d_features = OrderedDict()
        
    def delete(self, name):
        """Delete a feature using its name"""
        try:
            del self._d_features[name]
            return(True)
        except:
            return(False)

    def get(self, name):
        """Returns a feature
        
        __getitem__ will be more "Pythonic"
        but it's disabled"""
        try:
            return(self._d_features[name])
        except:
            log.error("Can't get feature '%s'" % name)
            return  
        
    def items(self):
        return(self._d_features.items())     
        
    def add(self, feature):
        """Add a feature to this collection"""
        
        if self.bo is not None:
            feature.attach(self.bo)
            
        bo_feature_name = feature.name
        
        if bo_feature_name not in self._d_features:
            log.info("Add feature '%s'" % feature)
            self._d_features[bo_feature_name] = feature
            return(True)
        else:
            log.error("Feature name '%s' ever exists - you must delete it before" % bo_feature_name)
            return(False)  
        
    def attach(self, bo):
        """Attach a bracket order to each feature"""
        self.bo = bo
        for name, feature in self._d_features.items():
            feature.attach(bo)

    def update_at_init(self, price, dt):
        """Update each feature with init price"""
        for name, feature in self._d_features.items():
            feature.update_at_init(price, dt)

    def update(self, price, dt):
        """Update each feature with new price"""
        for name, feature in self._d_features.items():
            feature.update(price, dt)
   
    @staticmethod
    def create(feature, bo=None):
        """Create BOFeatures (collection) when a feature is given
        (collection of only one feature)
        and returns features if that's ever a collection"""
        if feature is None:
            features = BOFeatures(bo)
            return(features)
    
        else:
            
            if feature.is_collection:
                return(feature)
            else:
                features = BOFeatures(bo)
                features.add(feature)
                return(features)

class BOFeature(Feature):
    """Abstract class to manage additional feature of a bracket order
    such as trailing stop or breakeven
    
    I'm using data dict instead of using attributes
    because __dict__ is locked for security issue
    but I need to access attributes by string in account_df
    """
    
    default_name = 'UndefinedBOFeature'
    _name = None
    bo = None
    
    #_data = None #OrderedDict() # data of a feature are stored in a dict in order to easily
        
    def attach(self, bo):
        """Attach a given bracket order to this feature"""
        self.bo = bo
        if self.bo is not None:
            log.info("Attach bo feature '%s' to bo#%s" % (self.name, self.bo.ticket))
            return(True)
        else:
            log.info("bo feature '%s' not attached to bo" % (self.name))
            return(False)
            
    def update_at_init(self, price, dt):
        """Update feature with init price"""
        if self.bo is not None:
            log.info("Update bo feature '%s' at init for bo#%s with price=%s dt=%s" % (self.name, self.bo.ticket, price, dt))
        else:
            log.info("Update bo feature '%s' at init with price=%s dt=%s (no bo attached)" % (self.name, price, dt))        
    def update(self, price, dt):
        """Update feature with new price"""
        log.info("Update bo feature '%s' for bo#%s with price=%s dt=%s" % (self.name, self.bo.ticket, price, dt))

    def __repr__(self):
        """Returns string representation of BOFeature"""
        if self.bo is None:
            s = "<BOFeature:%s not attached to bo!>" % (self.name)            
        else:
            s = "<BOFeature:%s attached to %s>" % (self.name, self.bo)
        return(s)

    def get(self, key):
        """Returns feature data using 'key'
        
        __getitem__ will be more "Pythonic"
        but it's disabled"""
        try:
            return(self._data[key])
        except:
            log.error("Can't get data '%s'" % key)
            return
        
    def set(self, key, value):
        """Set feature data 'value' using 'key'
        
        __setitem__ will be more "Pythonic"
        but it's disabled"""
        self._data[key] = value
        
    def items(self):
        return(self._data.items())

    
class TrailingStop(BOFeature):
    """Bracket order feature to manage trailing stop"""
    
    default_name = 'TrailingStop'
    
    def __init__(self, price_trailing_diff, name=None):
        """Initialization of trailing stop with trailing price difference"""
        self.init(name)
        
        self.set('price_trailing_diff', price_trailing_diff)
        self.set('price_trailing', None)

        self.stop_modified = Signal()

    def __repr__(self):
        """Returns string representation of BOFeature"""
        price_trailing_diff = self.get('price_trailing_diff')
        if self.bo is None:
            s = "<BOFeature:%s not attached to bo!>" % (self.name)            
        else:
            s = "<BOFeature:%s price_diff=%s attached to bo#%s>" % (self.name, price_trailing_diff, self.bo.ticket)
        return(s)        
        
    def update(self, price, dt):
        """Update feature with new price"""
                
        price_trailing_diff = self.get('price_trailing_diff')
        price_trailing = self.get('price_trailing')
        
        if self.bo.price_diff_d is not None:
            if self.bo.price_diff_d>price_trailing_diff:
                new_stop = price-self.bo.direction*price_trailing_diff
                if price_trailing is None:
                    self.modify_stop(new_stop) # ToFix! at first update we should only modify stop when it's closer than actual stop
                else:
                    if self.bo.direction*(new_stop-price_trailing)>0:
                        self.modify_stop(new_stop)
                
    def modify_stop(self, new_stop):
        """Modify stop price"""
        log.info("<BOFeature:%s modify stop to %s" % (self.name, new_stop))
        self.set('price_trailing', new_stop) # hidden stop
        self.bo.modify_stop(new_stop)
        self.stop_modified.emit(bo=self.bo)


class BreakEven(BOFeature):
    """Bracket order feature to manage breakeven"""
    
    default_name = 'BreakEven'
    
    def __init__(self, price_diff, price_offset, name=None):
        """Initialization of breakeven with
        price_diff and price_offset
        
        price_diff is price difference to trigger breakeven
           (when price_diff_d > price_diff)
        price_offset (default=0)
        """
        self.init(name)

        self.set('price_diff', price_diff)
        self.set('price_offset', price_offset)
        self.set('reached', False)
        
        self.stop_modified = Signal()
        
    def __repr__(self):
        """Returns string representation of BOFeature"""
        price_diff = self.get('price_diff')
        price_offset = self.get('price_offset')
        s = "<BOFeature:%s pBEdiff=%s pBEoffset=%s>" % (self.name, price_diff, price_offset)
        return(s)

    def update(self, price, dt):
        """Update feature with new price"""
        reached = self.get('reached')
        price_diff = self.get('price_diff')
        price_offset = self.get('price_offset')
        #log.info("Update bo feature '%s' at price change with price=%s dt=%s" % (self.name, price, dt))
        #log.info(self.bo)
        if self.bo.price_diff_d is not None and not reached:
            if self.bo.price_diff_d >= price_diff:
                self.set('reached', True)
                new_stop = self.bo.price_open + self.bo.direction * price_offset
                log.info("<BOFeature:%s BE reached: price stop set to %s" % (self.name, new_stop))
                self.modify_stop(new_stop)
                return(self.bo.ticket)
        else:
            return
                        
    def modify_stop(self, new_stop):
        """Modify stop price"""
        log.info("<BOFeature:%s modify stop to %s" % (self.name, new_stop))
        self.bo.modify_stop(new_stop)
        self.stop_modified.emit(bo=self.bo)

class Expiration(BOFeature):
    
    default_name = 'Expiration'
    
    def __init__(self, dt_exp, name=None):
        self.init(name)
        
        dt_exp = self.dt_td_to_dt(dt_exp, get_datetime()) # if dt is a timedelta it returns a datetime
        self.set('dt_exp', dt_exp)

        self.reached = Signal() # expiration reached
        
    def update(self, price, dt):
        #log.info("Expiration Update")
        
        dt_exp = self.get('dt_exp')
        
        if self.bo is not None:
            if self.bo.is_cancellable:
                if dt_exp is not None:
                    if dt>=dt_exp:
                        log.info("bo#%s: cancel bracket order because expiration reached (dt=%s expiration=%s)" % (self.bo.ticket, dt, dt_exp))
                        self.bo.date_close = dt
                        cancelled = self.bo.cancel()
                        self.reached.emit(bo=self.bo)
                        return(cancelled)
        #else:
        #    log.warn("Can't set expiration=%s to a market order (only possible with pending order) or a not cancellable order (a pending order which triggered)" % dt_exp)
        #    return                   
                    
    def dt_td_to_dt(self, x, dt_current):
        """Returns a datetime
        if x is a datetime it returns dt+x
        else it returns dt (assuming it's a datetime)"""
        try:
            x.total_seconds()
            # if it doesn't raise exception it should be a timedelta
            # we shoudl use isinstance(x, timedelta) but it's forbidden
            return(dt_current + x)
        except:
            # we assume x is a datetime
            return(x)                    
                    
# https://www.quantopian.com/posts/bracket-orders 

# Put any initialization logic here.  The context object will be passed to
# the other methods in your algorithm.
class BracketOrder(object):
    """Class to manage bracket orders
    
    A bracket order is composed of
    a parent order (either market or pending order)
    
    2 child pending orders
    - limit order (take profit TP)
    - stop order (stop loss SL)
    """
    
    ticket = None # bracket order ticket

    price_created = None
    price_open = None
    
    volume = None

    symbol = None
    price_stop = None
    price_limit = None    
    
    #date_created = None # property
    date_open = None
    date_close = None
    date_closed = None

    # for stats
    price_close = None
    
    order_id_master = None
    order_id_limit = None
    order_id_stop = None
        
    comment = ""
    magic = 0
    
    style = None
   
    #price_diff = None
    MAE = 0 # maximum adverse excursion
    MFE = 0 # maximum favorable excursion
        
    features = None
    
    was_triggered = False
        
    def __init__(self, bo_blotter, symbol, style, volume, price_stop, price_limit, comment, magic, features):
        """Bracket order init"""
        self.bo_blotter = bo_blotter
            
        self.symbol = symbol
        self.volume = volume
        self.price_stop = price_stop
        self.price_limit = price_limit        
        
        self.comment = comment
        self.magic = magic
        
        self.style = style # style of master order
                
        self.features = features
        
        self.triggered = Signal()
        self.was_triggered = False
        
        self.limit_reached = Signal()
        self.stop_reached = Signal()
        self.closed = Signal()
        self.cancelled = Signal()
        
    @property
    def direction(self):
        """Returns direction of bracket order (1 or -1)"""
        return(copysign(1, self.volume))
    
    @property
    def is_buy(self):
        """Returns bool
        True if first (master) order is buy"""
        return(copysign(1, self.volume)>0)

    @property
    def is_market(self):
        """Returns bool
        True if first (master) order is a market order"""
        return(not self.is_pending)        

    @property
    def is_pending(self):
        """Returns bool
        True if first (master) order is a pending order
        (it's also for a pending order which was trigered)"""
        return(self.style.get_limit_price(self.is_buy) is not None or self.style.get_stop_price(self.is_buy) is not None)
    
    @property
    def is_cancellable(self):
        """Returns bool
        True if first (master) order is a pending order which is still cancellable"""
        return(self.is_pending and self.order_master.filled==0)
    
    @property
    def is_filled(self):
        """Returns bool
        True if first (master) order is filled (totally)"""
        return(self.order_master.amount==self.order_master.filled)

    @property
    def is_triggered(self):
        """Returns bool
        True if first (master) order is triggered"""
        return(self.order_master.limit_reached or self.order_master.stop_reached)
    
    @property
    def amount(self):
        """Returns number of shares asked (buy or sell) in this order"""
        return(self.order_master.amount)

    @property
    def filled(self):
        """Returns number of shares which fill order"""
        return(self.order_master.filled)    

    @property
    def date_created(self):
        return(self.order_master.created)    
    
    @property
    def price_diff(self):
        """Returns price difference between close and open price"""
        try:
            return(self.price_close - self.price_open)
        except:
            return

    @property
    def price_diff_d(self):
        """Returns price difference between close and open price
        with direction"""
        try:
            return(self.direction*(self.price_close - self.price_open))
        except:
            return
        
    @property
    def price_diff_rel(self):
        """Returns relative price difference between close and open price
        with open price as reference"""        
        try:
            return(self.price_diff / self.price_open)
        except:
            return

    @property
    def price_diff_rel_d(self):
        """Returns relative price difference between close and open price
        with open price as reference
        and direction"""        
        try:
            return(self.price_diff_d / self.price_open)
        except:
            return
        
    @property
    def MAE_rel(self):
        """Returns relative MAE (maximum adverse excursion)"""
        try:
            return(self.MAE / self.price_open)
        except:
            return

    @property
    def MFE_rel(self):
        """Returns relative MFE (maximum favorable excursion)"""
        try:
            return(self.MFE / self.price_open)
        except:
            return

    @property
    def profit(self):
        """Returns profit"""
        try:
            return(self.volume * self.price_diff)
        except:
            return(0.0)
        
    def send(self, price, dt):
        """Sends order(s)"""
                    
        self.price_created = price
                
        self.order_id_master = order(self.symbol, self.volume, style=self.style)
        #log.info(self.style.get_limit_price(is_buy(self.volume)))
        
        self.order_id_stop = self._create_stop(self.price_stop)
        
        self.order_id_limit = self._create_limit(self.price_limit)
                
        #ticket = (self.order_id_master, self.order_id_limit, self.order_id_stop)
        #ticket = "-".join(ticket)
       
        self.ticket = self.bo_blotter._ticket()

        self.features = BOFeatures.create(self.features, self) # create a collection of features
        #self.features.attach(self) # attach bracket order to feature
        self.features.update_at_init(price, dt)
        
        return(self.ticket)
    
    @property
    def order_master(self):
        """Returns order master"""
        return(get_order(self.order_id_master))

    @property
    def order_limit(self):
        """Returns order limit"""
        return(get_order(self.order_id_limit))

    @property
    def order_stop(self):
        """Returns order stop"""
        return(get_order(self.order_id_stop))
    
    def __repr__(self):
        """Returns string representation of a bracket order"""
        s = """<BracketOrder #%s %s %s
 style=%s
 price_created=%s price_open=%s price_limit=%s price_stop=%s
 order_master: %s
 order_limit: %s
 order_stop: %s\n>""" % (self.ticket, self.volume, self.symbol, repr_execution_style(self.style, self.is_buy), self.price_created, self.price_open, self.price_limit, self.price_stop, self.order_master, self.order_limit, self.order_stop)
        return(s)

    def _create_stop(self, price_stop):
        """Creates a stop order"""
        if self.price_stop is not None:
            return(order(self.symbol, -self.volume, style=StopOrder(self.price_stop))) # , exchange=self.order_master.style.exchange
        else:
            return(None)
        
    def modify_stop(self, price_stop):
        """Modifies stop order"""
        if price_stop!=self.price_stop:
            log.info("bo#%s: modify stop price to %s" % (self.ticket, price_stop))
            order_id = self.order_id_stop
            cancel_order(order_id) # DANGEROUS! it should be atomic operation!
            #self.order_stop.cancel()
            order_id = order(self.symbol, -self.volume, style=StopOrder(price_stop))
            self.order_id_stop = order_id
            self.price_stop = price_stop
            return(order_id)
        else:
            return

        #o = get_order(order_id)
        #if o:
        #    cancel_order(order_id) # DANGEROUS! it should be atomic operation!
        #    self.order_id_stop = None
        
        #order_id = self.order_id_stop
        #o = get_order(order_id)
        #if not o:
        #    order_id = order(self.symbol, -self.volume, style=StopOrder(price_stop))
        #    self.order_id_stop = order_id
        #    self.price_stop = price_stop
        #    return(order_id)
        
        #return
              
    def _create_limit(self, price_limit):
        """Creates a limit order"""
        if self.price_limit is not None:
            return(order(self.symbol, -self.volume, style=LimitOrder(self.price_limit)))
        else:
            return
    
    def modify_limit(self, price_limit):
        """Modifies limit order"""
        if price_limit!=self.price_limit:
            if price_limit is not None:
                log.info("modify limit price of #%s to %s" % (self.ticket, price_limit))
                order_id = self.order_id_limit
                cancel_order(order_id) # DANGEROUS! it should be atomic operation!
                order_id = order(self.symbol, -self.volume, style=LimitOrder(price_limit))
                self.order_id_limit = order_id
                self.price_limit = price_limit
                return(order_id)
            else:
                return
        else:
            return
    
    def modify_price(self, price):
        """Modify price of master pending order"""
        if price is not None and self.is_cancellable:
            log.info("bo#%s: modify price (pending) order " % self.ticket)
            not_implemented_error("Can't modify price for now (only for pending orders which wasn't triggered")
            order_id = self.order_id_master
            cancel_order(order_id) # DANGEROUS! it should be atomic operation!
            #style = self.style
            #if self.is_limit:
            #elif self.is_stop:
            #elif self.is_stop_limit
            #order_id = order(self.symbol, self.volume, style=new_style))
            
        else:
            return

    def modify_expiration(self, new_expiration):
        """Modify price of master pending order"""
        if self.is_cancellable:
            if self.expiration!=new_expiration:
                log.info("bo#%s: modify expiration of pending order" % self.ticket)
                dt = get_datetime()
                new_expiration = dt_td_to_dt(new_expiration, dt)
                not_implemented_error("Can't modify modify expiration of pending order")
        else:
            return
        

    def cancel(self):
        """Cancels bracket order (cancel master order, limit and stop order)"""
        if self.is_market:
            log.info("bo#%s: can't cancel order (market)" % self.ticket)
            return(False)
        else:
            log.info("bo#%s: cancel master order, limit and stop order" % self.ticket)
            if self.is_cancellable:
                cancel_order(self.order_master)
                cancel_order(self.order_limit)
                cancel_order(self.order_stop)
                self.cancelled.emit(bo=self)
                self.bo_blotter._move_cancelled_order(self)
                return(True)
            else:
                log.info("bo#%s: can't cancel order (not cancellable)" % self.ticket)
                return(False)

    def close(self, volume):
        """Close bracket order"""
        log.info("bo#%s: order closed with vol=%s" % (self.ticket, volume))
        if volume is not None:
            not_implemented_error("Partial close is not implemented")
        else:
            volume_close = -self.volume
        #if is_pending_order(style):
        #    not_implemented_error("Can't close pending order")
        #else:
        #if not bo.is_filled():
        
        self.order_id_close = order(self.symbol, volume_close)
        
        if self.order_id_limit is not None:
            cancel_order(self.order_id_limit)
        if self.order_id_stop is not None:
            cancel_order(self.order_id_stop)
        
        self.date_close = get_datetime()

        self.closed.emit(bo=self)
        
        self.bo_blotter._move_closed_order(self)
        
        return(self.ticket)

    def add_feature(self, feature):
        attached = feature.attach(self)
        if attached:
            added = self.features.add(feature)
            #if self.price_close is None:
            #    print "price is none"
            #print self.price_close
            return(added)
            #if added:
            #    price = self.data[self.symbol].price
            #    dt = get_datetime()
            #    feature.update_at_init(price, dt)
            #    return()
            #else:
            #    return
        else:
            return    

    def _watch_orders(self, price, dt):
        """Watch orders and cancel them if necesserary
        Move cancelled order from 'trades' to 'cancelled'"""
        
        if not self.was_triggered and self.is_triggered:
            log.info("bo#%s: triggered" % (self.ticket))
            self.triggered.emit(bo=self)
            self.was_triggered = True
        
        if self.order_stop.stop_reached:
            # Stop order is reached so limit order need to be cancelled
            cancel_order(self.order_id_limit)
            log.info("bo#%s: cancel limit order %r because stop order reached %r" % (self.ticket, self.order_id_limit, self.order_id_stop))
            self.stop_reached.emit(bo=self)
            
        if self.order_limit.limit_reached:
            # Limit order is reached so stop order need to be cancelled
            cancel_order(self.order_id_stop) 
            log.info("bo#%s: cancel stop order %r because limit order reached %r" % (self.ticket, self.order_id_stop, self.order_id_limit))
            self.limit_reached.emit(bo=self)
            
        if self.order_limit.limit_reached or self.order_stop.stop_reached:
            self.date_closed = dt    
            self.closed.emit(bo=self)
            self.bo_blotter._move_closed_order(self)

    def _update_order_statistics(self, price, dt):
        """Updates statistics of this bracket order
        BO statistics are price_open, price_close
        BO (advanced) statistics are for example MAE and MFE"""
                
        if self.price_open is None:
            if self.order_master.filled>0:
                #log.info('ok')
                self.price_open = price
                self.date_open = dt
            
            #if self.order_master.amount==self.order_master.filled:
            #    self.price_open = self.data[self.symbol].price
            #    

        #log.info("%s/%s" % (bo.order_master.amount, bo.order_master.filled))
            
        self.price_close = price
            
        if self.price_diff_d>self.MFE:
            self.MFE = self.price_diff_d
            
        if self.price_diff_d<self.MAE:
            self.MAE = self.price_diff_d
            
    def update(self, price, dt):
        #self.price = price
        self._update_order_statistics(price, dt)
        self.features.update(price, dt)
        self._watch_orders(price, dt)
            
class BOBlotter(object):
    """
    BBOBlotter is a class to manage a blotter for Bracket Orders
    This blotter is gives a ticket when order are send, modified, received
    it manages statistics for each bracket orders
    """
    
    _m_ticket_initial = 100000 # initial bo ticket number
    _m_ticket = -1
    
    context = None # trading context
    data = None # price data
    
    _d_orders = None
            
    def __init__(self, context):
        """Initialize bracket order blotter"""
        self.context = context
        
        self._m_ticket = self._m_ticket_initial

        self._d_orders = {}
        self._d_orders['trades'] = OrderedDict()
        self._d_orders['history'] = OrderedDict()
        self._d_orders['cancelled'] = OrderedDict()       
        
    def _ticket(self):
        """Get a new ticket number for a bracket order
        This is an internal ticket number"""
        self._m_ticket += 1
        return(self._m_ticket)
    
    def update(self, context, data):
        """Updates blotter with context and data"""
        self.context = context
        self.data = data

        dt = get_datetime()

        for tkt, bo in self._d_orders['trades'].items():
            price = self.data[bo.symbol].price
            bo.update(price, dt)
                        
    def order_send(self, symbol, style, volume, price_stop_loss, price_take_profit, comment, magic, features=None): # =MarketOrder()
        """Send a bracket order
        symbol: symbol (Security)
        volume: number of shares
        price_stop_loss: stop loss price
        price_take_profit: take profit price"""
        
        dt = get_datetime()
        price = self.data[symbol].price
        
        log.info("bo_blotter: order_send %s %s price=%s SL=%s TP=%s %s" % (volume, symbol, price, price_stop_loss, price_take_profit, repr_execution_style(style))) # repr_execution_style ToFix
        
        if volume == 0:
            log.error("Don't bother placing orders for 0 shares.")
            return

        bo = BracketOrder(self, symbol, style, volume, price_stop_loss, price_take_profit, comment, magic, features)                
        #order_id_master = bo.send(price, dt)
        bo.send(price, dt)

        #log.info("bo_blotter: bo#%s order_send %s %s price=%s SL=%s TP=%s %s" % (bo.ticket, volume, symbol, price, price_stop_loss, price_take_profit, repr_execution_style(style))) # repr_execution_style ToFix

        self._d_orders['trades'][bo.ticket] = bo

        return(bo.ticket)

    def _order_close(self, bo, volume=None):
        """Close parent order and cancel pending limit and stop orders
        volume: default close volume is None for full close
            or numerical value for partial close"""
        log.info("bo_blotter: order_close bracket order bo#%s with vol=%s" % (bo.ticket, volume))
        ticket = bo.close(volume)
        return(ticket)
    
    def order_close(self, ticket, volume=None):
        """Close parent order and cancel pending limit and stop orders
        volume: default close volume is None for full close
            or numerical value for partial close"""
        bo = self.bo_by_ticket(ticket)
        if bo:
            bo = self._d_orders['trades'][ticket]
            ticket = self._order_close(bo, volume)
            return(ticket)
        else:
            return

    def _order_cancel(self, bo):
        """Cancel bracket order given a bo object
        only valid if parent order is a pending order"""
        log.info("bo_blotter: order_cancel bracket order bo#%s" % bo.ticket)        
        cancelled = bo.cancel()
        return(cancelled)
        
    def order_cancel(self, ticket):
        """Cancel bracket order given a ticket
        only valid if parent order is a pending order"""
        bo = self.bo_by_ticket(ticket)
        if bo:
            cancelled = self._order_cancel(bo)
            return(cancelled)
        else:
            return

    def _order_modify(self, bo, price, price_stop, price_limit, expiration=None):
        """Modify bracket order
        price: price of a pending order
        price_stop: price of stop loss (stop order)
        price_limit: price of take profit (limit order)
        expiration: datetime expiration
        """
        log.info("bo_blotter: order_modify bracket order bo#%s" % bo.ticket)
        bo.modify_price(price)
        bo.modify_stop(price_stop)
        bo.modify_limit(price_limit)
        bo.modify_expiration(expiration)
        return(bo.ticket)
        
    def order_modify(self, ticket, price, price_stop, price_limit, expiration=None):
        """Modify bracket order
        price: price of a pending order
        price_stop: price of stop loss (stop order)
        price_limit: price of take profit (limit order)
        expiration: datetime exiration
        """
        if expiration is not None:
            not_implemented_error("expiration must be None")
            return
        
        bo = self.bo_by_ticket(ticket)
        if bo:
            ticket = self._order_modify(bo, price, price_stop, price_limit, expiration)
            return(ticket)
        else:
            return
        
    def _move_cancelled_order(self, bo):
        """Move cancelled bracket order from 'trades' to 'cancelled'"""
        return(self._move_order_from_to(bo, 'trades', 'cancelled'))    

    def _move_closed_order(self, bo):
        """Move closed bracket order from 'trades' to 'history'"""
        return(self._move_order_from_to(bo, 'trades', 'history'))    
            
    def _move_order_from_to(self, bo, source='trades', destination='history'):
        """Move order from source to destination
        source is generally 'trades'
        destination is generally 'history' (but it can be 'cancelled')"""
        ticket = bo.ticket
        log.info("bo_blotter: bo#%s: move from '%s' to '%s'" % (ticket, source, destination))
        self._d_orders[destination][ticket]= self._d_orders[source][ticket]
        del self._d_orders[source][ticket]
                
    def bo_by_ticket(self, ticket, typ='trades'):
        """Returns a bracket order by ticket"""
        if ticket in self._d_orders[typ]:
            bo = self._d_orders[typ][ticket]
            return(bo)
        else:
            log.warn("bo_blotter: ticket '%s' doesn't exists in '%s'" % (ticket, typ))
            return

    def bo_by_pos(self, position, typ='trades'):
        """Returns a bracket order by position"""
        try:
            bo = self._d_orders[typ].items()[position][1]
            # self._d_orders[typ].items() is a list of tuple
            # [(bo_ticket1, bo1), (bo_ticket2, bo2)]
            return(bo)
        except:
            log.warn("bo_blotter: position '%s' doesn't exists in '%s'" % (position, typ))
            return
    
    @property
    def orders(self):
        """Returns an OrderedDict with bo ticket as keys and values
        - opened or pending BO trades but not closed trades"""
        return(self._d_orders['trades'])

    @property
    def orders_total(self):
        """Returns the number of market bracket order and pending bracket orders."""
        return(len(self._d_orders['trades'])) 

    @property
    def orders_history(self):
        """Returns an OrderedDict with bo ticket as keys and values
        - closed trades"""        
        return(self._d_orders['history'])    

    @property
    def orders_history_total(self):
        """Returns the number of bracket orders in history"""
        return(len(self._d_orders['history'])) 

    @property
    def account_df_trades(self, improve=False):
        """Returns a DataFrame with
        - opened or pending BO trades but not closed trades"""
        return(self.account_df('trades', improve))

    @property
    def account_df_history(self, improve=False):
        """Returns a DataFrame with
        - closed trades"""
        return(self.account_df('history', improve))    
    
    def account_df(self, typ='trades', improve=False):        
        """Returns a DataFrame with
        - opened or pending BO trades but not closed trades if typ='trades'
        - closed trades if typ='history'
        - cancelled traded if typ:'cancelled'"""
        cols = ['date_open', 'date_close', 'symbol', 'style', 'volume', 'price_open', 'price_stop', 'price_limit', 'price_close', 'comment', 'magic', 'order_id_master', 'order_id_stop', 'order_id_limit', 'direction', 'price_diff', 'price_diff', 'price_diff_d', 'price_diff_rel', 'price_diff_rel_d', 'MAE', 'MFE', 'MAE_rel', 'MFE_rel', 'price_trailing_diff', 'profit']
        d = self._d_orders[typ]
        if len(d)>0:
            df = pd.DataFrame(d.values(), index=d.keys())
            df = df.rename(columns={0: 'bo'})
            df['date_created'] = df['bo'].map(lambda o: o.date_created)
            df['date_open'] = df['bo'].map(lambda o: o.date_open)
            df['date_close'] = df['bo'].map(lambda o: o.date_close)
            df['date_closed'] = df['bo'].map(lambda o: o.date_closed)
            df['symbol'] = df['bo'].map(lambda o: o.symbol)
            #df['style'] = df['bo'].map(lambda o: o.style)
            df['volume'] = df['bo'].map(lambda o: o.volume)
            df['price_open'] = df['bo'].map(lambda o: o.price_open)
            df['price_stop'] = df['bo'].map(lambda o: o.price_stop)
            df['price_limit'] = df['bo'].map(lambda o: o.price_limit)
            df['price_close'] = df['bo'].map(lambda o: o.price_close)
            df['comment'] = df['bo'].map(lambda o: o.comment)
            df['magic'] = df['bo'].map(lambda o: o.magic)
            #df['order_id_master'] = df['bo'].map(lambda o: o.order_id_master)
            #df['order_id_stop'] = df['bo'].map(lambda o: o.order_id_stop)
            #df['order_id_limit'] = df['bo'].map(lambda o: o.order_id_limit)

            df['direction'] = df['bo'].map(lambda o: o.direction)

            df['price_diff'] = df['bo'].map(lambda o: o.price_diff)
            df['price_diff_d'] = df['bo'].map(lambda o: o.price_diff_d)
            df['price_diff_rel'] = df['bo'].map(lambda o: o.price_diff_rel)
            df['price_diff_rel_d'] = df['bo'].map(lambda o: o.price_diff_rel_d)
            
            df['MAE'] = df['bo'].map(lambda o: o.MAE)
            df['MFE'] = df['bo'].map(lambda o: o.MFE)
            
            #df['MAE_rel'] = df['MAE'] / df['price_open']
            #df['MFE_rel'] = df['MFE'] / df['price_open']
            df['MAE_rel'] = df['bo'].map(lambda o: o.MAE_rel)
            df['MFE_rel'] = df['bo'].map(lambda o: o.MFE_rel)
            

            #df['profit'] = df['volume'] * df['price_diff'].fillna(0)
            df['profit'] = df['bo'].map(lambda o: o.profit)
            #df['profit_rel'] = df['bo'].map(lambda o: o.profit_rel)
            
            if improve:
                try:
                    df = improve_account_df_with_additional_data(df)
                except Exception as e:
                    log.error("Can't improve account df with additional data")
                    log.error("Reason: %s" % str(e))
            
            #del df['bo']            
            
            return(df)
        else:
            return(pd.DataFrame(columns=cols))

    def add_bo_feature(self, ticket, bo_feature):
        bo = self.bo_by_ticket(ticket)
        added = bo.add_feature(bo_feature)
        return(added)

    def add_blotter_feature(self, blotter_feature):
        not_implemented_error("Blotter feature not implemented")
        #bo = self.bo_by_ticket(ticket)
        #return(self.features.add(blotter_feature)) 
    

def improve_account_df_with_additional_data(df):
    for key in ['price_trailing_diff', 'price_trailing']:
        df[key] = df['bo'].map(lambda o: o.features.get('TrailingStop').get(key))
    return(df)

#def supplement_account_df(self, df, feature_name=None):
#    """Returns account DataFrame with additional columns from BOFeature
#    Adds price_trailing_diff and price_trailing columns"""
#    log.info("Adds new columns to df using %s" % feature_name)            
#    for key in self._data:
#        df[key] = df['bo'].map(lambda o: o.features.get(feature_name).get(key))   
#    return(df)

def slot_test_triggered(**kwargs):
    bo = kwargs['bo']
    log.info("Slot bo#%s triggered" % bo.ticket)    
    
def slot_test_limit_reached(**kwargs):
    bo = kwargs['bo']
    log.info("Slot bo#%s limit_reached" % bo.ticket)

def slot_test_stop_reached(**kwargs):
    bo = kwargs['bo']
    log.info("Slot bo#%s stop_reached" % bo.ticket)

def slot_expiration_reached(**kwargs):
    bo = kwargs['bo']
    log.info("Slot bo#%s expiration_reached" % bo.ticket)
    
def initialize(context):
    context.symb = symbol('AAPL') # sid(24) # AAPL
    context.magic = 123 # magic number
    
    context.first_time = True
    context.finalize = True
    
    context.bo_blotter = BOBlotter(context)
    #bo_blotter_feature = ...
    #context.bo_blotter.add_feature(bo_blotter_feature)
    
    context.i = 0

    context.relative_take_profit = 15.0 # %
    context.relative_stop_loss = 20.0 # %
    
    context.relative_trailing = 5.0 # %
    
    context.relative_breakeven = 6.0 # %
    context.relative_breakeven_offset = 1.0 # %
    
    price_init = 60
    context.price_stop_loss = price_init
    context.price_take_profit = price_init
    context.price_open = price_init
    context.price_breakeven = price_init

    context.rec_data = OrderedDict() # record data in an ordered dict
    
    context.debug = True # CAUTION! for contest or live set to False
    
    set_benchmark(context.symb)

    #schedule_function(summary, date_rules.every_day())
    #https://www.quantopian.com/posts/run-summary

#def _handle_data(context, data):
#    try:
#        secured_handle_data(context, data)
#    except Exception  as e:
#        log.error("%r" % e)
#        try:
#            if context.debug:
#                raise(e)
#        except KeyError:
#            pass
#        return
    
# Will be called on every trade event for the securities you specify. 
def handle_data(context, data):
    # Implement your algorithm logic here.

    # data[sid(X)] holds the trade event data for that security.
    # context.portfolio holds the current portfolio state.

    # Place orders with the order(SID, amount) method.

    # TODO: implement your own logic here.
    dt = get_datetime()
    
    bo_blotter = context.bo_blotter
    bo_blotter.update(context, data)

    if context.first_time:

        if dt.year==2012 and dt.month==3 and dt.day>=15:
            p = data[context.symb].price
            symb = context.symb
            log.info("price=%s" % p)

            volume = 1000
            (context.price_stop_loss, context.price_take_profit) = price_SL_TP_rel(p, volume, context.relative_stop_loss, context.relative_take_profit)
            #context.price_stop_loss = p*(1-sign(volume)*context.SL_rel/100.0)
            #context.price_take_profit = p*(1+sign(volume)*context.TP_rel/100.0)
            context.price_open = p
            comment = "MyComment"
            
            #market @ 88.385
            #buy stop @ 95 order placed above price (we hope that price keeps going up)
            #buy limit @ 80 order place below price (we hope that price is falling, bonce and goes up)
            #sell stop @ 80 order placed below price (we hope that price keeps going down)
            #sell limit @ 95 order place above price (we hope that price is rising, bonce and goes down)
            
            
            features = None
            features = BOFeatures()
            #features.add(Expiration(timedelta(days=600)))
            #features.add(TrailingStop(context.relative_trailing))
            

            #feature = None
            #feature = Expiration(timedelta(days=3)) # should be cancelled because expiration reached
            feature = Expiration(timedelta(days=600)) # shouldn't be cancelled because expiration not reached
            features.add(feature)
            
            # BOFeature: Trailing stop
            #feature = TrailingStop(context.relative_trailing)
            #feature = TrailingStop(context.relative_trailing, name="TrailSt")
            
            # BOFeature: Breakeven
            price_breakeven_diff = p * context.relative_breakeven / 100.0
            context.price_breakeven = p + sign(volume)*price_breakeven_diff
            price_breakeven_offset = p * context.relative_breakeven_offset / 100.0
            #feature = BreakEven(price_breakeven_diff, price_breakeven_offset, name="BreakEv")
            
            # Open BUY
            #tkt = bo_blotter.order_send(symb, MarketOrder(), volume, context.price_stop_loss, context.price_take_profit, comment, context.magic, features) # features=TrailingStop(context.relative_trailing) or features=features with features=BOFeature()
            #tkt = bo_blotter.order_send(symb, StopOrder(95), volume, context.price_stop_loss, context.price_take_profit, comment, context.magic, features)
            tkt = bo_blotter.order_send(symb, LimitOrder(80), volume, context.price_stop_loss, context.price_take_profit, comment, context.magic, features)
            
            # Open SELL (CAUTION: change volume sign BEFORE)
            #volume = -1000
            #(context.price_stop_loss, context.price_take_profit) = price_SL_TP_rel(p, volume, context.relative_stop_loss, context.relative_take_profit)
            #tkt = bo_blotter.order_send(symb, MarketOrder(), volume, context.price_stop_loss, context.price_take_profit, comment, context.magic, features)
            #tkt = bo_blotter.order_send(symb, StopOrder(77), volume, context.price_stop_loss, context.price_take_profit, comment, context.magic, features)
            #tkt = bo_blotter.order_send(symb, LimitOrder(95), volume, context.price_stop_loss, context.price_take_profit, comment, context.magic, features)            
            
            bo = bo_blotter.bo_by_ticket(tkt)
            bo.triggered.connect(slot_test_triggered)
            bo.limit_reached.connect(slot_test_limit_reached)
            bo.stop_reached.connect(slot_test_stop_reached)
            bo.features.get('Expiration').reached.connect(slot_expiration_reached)
            #not_implemented_error()
            
            # Expiration
            #feature = Expiration(timedelta(days=3)) # should be cancelled because expiration reached
            #feature = Expiration(timedelta(days=600)) # shouldn't be cancelled because expiration not reached
            #bo_blotter.add_bo_feature(tkt, feature)
            
            # Trailing stop
            #feature = TrailingStop(context.relative_trailing)
            #bo_blotter.add_bo_feature(tkt, feature)
            
            # BreakEven
            #feature = BreakEven(price_breakeven_diff, price_breakeven_offset)
            #bo_blotter.add_bo_feature(tkt, feature)
            
            log.info("bo#%s: bracket order opened" % tkt)
            #print(tkt)
            #context.orders[order_id] = True
            context.first_time = False
            context.closed = False

    elif dt.year==2012 and dt.month==11 and dt.day>=15:
            if not context.closed:
                tkt = bo_blotter.order_close(100001)
                context.closed = True
            #tkt = 100001
            #log.info(bo_blotter.orders)
            #for tkt, bo in bo_blotter.orders.items():
            #    if bo.is_pending:
            ##    if bo.is_cancelable:
            #        bo_blotter.order_cancel(tkt)

    else:
        pass

    if not context.first_time:
        pass
        #bo1 = bo_blotter.bo_by_pos(0)
        #bo2 = bo_blotter.bo_by_pos(1)
        
        #print bo1
        #print bo2
    
        
    context.i += 1

    opened_orders = get_open_orders()
    #if len(opened_orders)>0:
    #    log.info("open_orders:\n%s" % opened_orders)
    
    df_trades = bo_blotter.account_df_trades
    if len(df_trades)>0:
        tkt = 100001
        #bo = bo_blotter.bo_by_ticket(tkt)
        #pSL1 = bo.price_stop
        #log.info(pSL1)

        #log.info(data[context.symb].price)
        #bo = bo_blotter.bo_by_ticket(100001)
        #log.info(bo.order_master)

        pass
        #log.info("trades:\n%s" % df_trades)
        #s_trade = df_trades.iloc[0]
        #log.info("trade[0]:\n%s" % s_trade)
        #log.info("Close manually")
        #if dt.year==2012 and dt.month==9 and dt.day>=1:
        #    tkt = bo_blotter.order_close(100001)
        #    log.info("Close ticket=%s" % tkt)
        
    else:
        pass
        #pSL1 = context.price_stop_loss
    
    #try:
    #    MAE = s_trade['MAE']
    #except:
    #    MAE = 0
        
    #try:
    #    MFE = s_trade['MFE']
    #except:
    #    MFE = 0

    try:
        MAE_rel = df_trades['MAE_rel'].sum()
    except:
        MAE_rel = 0        
        
    try:
        MFE_rel = df_trades['MFE_rel'].sum()
    except:
        MFE_rel = 0       

    try:
        price_diff_rel_d = df_trades['price_diff_rel_d'].sum()
    except:
        price_diff_rel_d = 0

    # End of backtest    
    if dt.year==2014 and dt.month==1 and dt.day==15 and context.finalize:
        context.finalize = False
        finalize(context, data)
   
    #context.rec_data['price'] = data[context.symb].price
    #context.rec_data['pOpen'] = context.price_open
    #context.rec_data['pSL0'] = context.price_stop_loss
    #context.rec_data['pTP0'] = context.price_take_profit
    #context.rec_data['pSL1'] = pSL1
    #context.rec_data['pBE'] = context.price_breakeven
    #context.rec_data['pBEoff'] = context.price_breakeven + price_breakeven_offset
    
    #record(**context.rec_data)
    #record(*sum(context.rec_data.iteritems(), ()))
    record('price', data[context.symb].price, 'pOpen', context.price_open, 'pSL0', context.price_stop_loss, 'pTP0', context.price_take_profit) # should preserve order
       
    #record(price=data[context.symb].price, pOpen=context.price_open, pSL0=context.price_stop_loss, pTP0=context.price_take_profit, pSL1=pSL1)
    #, pBE=context.price_breakeven

    #record(MAE=MAE, MFE=MFE)
    #record(MAE_rel=MAE_rel, MFE_rel=MFE_rel, price_diff_rel_d=price_diff_rel_d)
    
    #record(balance=context.account.????)
    
    #record(leverage=context.account.leverage)
    #if context.i == 20:
    #   not_implemented_error()

    
# see https://www.quantopian.com/posts/run-summary    
def stats_account(df):
    #df['price_diff_rel_d'] = df['direction']*df['price_diff_rel']
    return(df[['price_diff_rel_d', 'MAE_rel', 'MFE_rel']].sum())

def summary_bo_blotter(context, data):
    bo_blotter = context.bo_blotter
    
    df_trades = bo_blotter.account_df('trades')
    log.info("trades:\n%s" % df_trades)

    df_history = bo_blotter.account_df('history')
    log.info("history:\n%s" % df_history)
    stats = stats_account(df_history)
    log.info("history_stats:\n%s" % stats)
    if len(df_history)>0:
        log.info("history[0]:\n%s" % df_history.iloc[0])
        
    #log.info("history['price_trailing_diff']:\n%s" % df_history['price_trailing_diff'])
    #log.info("history['price_trailing']:\n%s" % df_history['price_trailing'])
    
    df_cancelled = bo_blotter.account_df('cancelled')
    log.info("cancelled:\n%s" % df_cancelled)
    if len(df_cancelled)>0:
        log.info("cancelled[0]:\n%s" % df_cancelled.iloc[0])
    
    
def finalize(context, data):
    log.info("End of backtest")
    log.info("open_orders:\n%s" % get_open_orders())
    summary_bo_blotter(context, data)
