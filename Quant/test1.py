"""
This is a template algorithm on Quantopian for you to adapt and fill in.
"""
from quantopian.algorithm import attach_pipeline, pipeline_output
from quantopian.pipeline import Pipeline
from quantopian.pipeline.data.builtin import USEquityPricing
from quantopian.pipeline.factors import AverageDollarVolume

def PrintCurrentPortfolio(context, data):
    """
        print current portfolio status
    """
    print get_datetime()
    print "cash: " + str(context.portfolio.cash)
    print "stock LABD " + str(context.portfolio.positions[context.stock1].amount)
    print "           open price " + str(context.OpenPrice1) + " current price " + str(data.current(context.stock1, 'price'))
    print "istriggered  " + str(context.trigger1)
    
    
    print "stock LABU " + str(context.portfolio.positions[context.stock2].amount)
    print "           open price " + str(context.OpenPrice2) + " current price " + str(data.current(context.stock2, 'price'))
    print "istriggered  " + str(context.trigger2)
    print "porfolio value " + str(context.portfolio.portfolio_value)
    print '\n\n'
 
def initialize(context):
    """
        state = 0: no stock position
        state = 1: pair stocks have been bought
        state = 2: threshold has been triggered

    """
    context.state = 0
    context.stock1 = symbol("LABD")
    context.stock2 = symbol("LABU")
    context.OpenPrice1 = 0
    context.OpenPrice2 = 0
    context.trigger1 = 0
    context.trigger2 = 0

    """
        schedule order at the market open day
    """
    schedule_function(func=morningOrder,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_open())

    schedule_function(func=rebalance,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_close(minutes=30))

    schedule_function(func=rebuy,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_close(minutes=20))
        
    schedule_function(func=printClose,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_close())

def rebuy(context, data):
    # buy back the stock
    # TODO: need some time to sell the stock
    if context.trigger1 == 1 or context.trigger2 == 1:
        #checkOrderExecute(context, data)
        pass
    else:
        return
    
    print "rebuy: \n"
    PrintCurrentPortfolio(context, data)
    current_cash = context.portfolio.cash
    context.order_id1 = order_value(context.stock1, current_cash / 2)
    context.order_id2 = order_value(context.stock2, current_cash / 2)
    context.state = 1
    
def printClose(context, data):
    print "printClose: \n"
    PrintCurrentPortfolio(context, data)
    print "\n\n\n\n"
    
def morningOrder(context, data):
    context.OpenPrice1 = data.current(context.stock1, 'price')
    context.OpenPrice2 = data.current(context.stock2, 'price')

    context.trigger1 = 0
    context.trigger2 = 0
    current_cash = context.portfolio.cash
    if(context.state == 0):
        # order each stock 1000 dollars
        context.order_id1 = order_value(context.stock1, current_cash / 2)
        context.order_id2 = order_value(context.stock2, current_cash / 2)
        context.state = 1
        
    print "morning Order: \n"
    PrintCurrentPortfolio(context, data)
    # portfolia object
    #print "stock 1 amount" + str(context.portfolio.positions[context.stock1].amount)
    #print "stock 2 amount" + str(context.portfolio.positions[context.stock2].amount)

def rebalance(context, data):
    if context.trigger1 == 1 or context.trigger2 == 1:
        #checkOrderExecute(context, data)
        pass
    else:
        return
    
    print "before do rebalance \n"
    PrintCurrentPortfolio(context, data)

    if context.portfolio.positions[context.stock1].amount > 0:
        order_target(context.stock1, 0)

    if context.portfolio.positions[context.stock2].amount > 0:
        order_target(context.stock2, 0)

def checkOrderExecute(context, data):
    if context.trigger1 == 1:
        id1 = get_order(context.id1)
        if id1:
            # log the order amount and the amount that is filled  
            message = 'Order for {amount} has {filled} shares filled.'  
            message = message.format(amount=id1.amount, filled=id1.filled)  
            log.info(message)
            
    if context.trigger2 == 1:
        id2 = get_order(context.id2)
        if id2:
            # log the order amount and the amount that is filled  
            message = 'Order for {amount} has {filled} shares filled.'  
            message = message.format(amount=id2.amount, filled=id2.filled)  
            log.info(message)

def event_trigger(context, data):
    price1 = data.current(context.stock1, 'price')
    price2 = data.current(context.stock2, 'price')
    
    # event triggered?
    if price1 < context.OpenPrice1 * 0.93:
        context.trigger1 = 1
        print "stock1 event trigger " + str(price1) + "  " + str(context.OpenPrice1) + "\n\n"
    if price2 < context.OpenPrice2 * 0.93:
        context.trigger2 = 1
        print "stock2 event trigger " + str(price2) + "  " + str(context.OpenPrice2) + "\n\n"

    
    
def handle_data(context,data):
    """
    Called every minute.
    """
            
    if context.trigger1 == 1 or context.trigger2 == 1:
        #checkOrderExecute(context, data)
        return
    price1 = data.current(context.stock1, 'price')
    price2 = data.current(context.stock2, 'price')
    
    event_trigger(context, data)

    if context.trigger1 == 1 and context.trigger2 == 1:
        print "error cannot both be triggered"
        print context.trigger1
        print context.trigger2
        print str(price1) + "  " + str(context.OpenPrice1)
        print str(price2) + "  " + str(context.OpenPrice2)


    if context.trigger1:
        context.id1 = order_target(context.stock1, 0)
        
    if context.trigger2:
        context.id2 = order_target(context.stock2, 0)
        
    if context.trigger1 == 1 or context.trigger2 == 1:
        context.state = 2

