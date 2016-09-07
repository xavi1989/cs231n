"""
This is a template algorithm on Quantopian for you to adapt and fill in.
"""
from quantopian.algorithm import attach_pipeline, pipeline_output
from quantopian.pipeline import Pipeline
from quantopian.pipeline.data.builtin import USEquityPricing
from quantopian.pipeline.factors import AverageDollarVolume
 
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

def morningOrder(context, data):
    context.OpenPrice1 = data[context.stock1].price
    context.OpenPrice2 = data[context.stock2].price

    current_cash = context.portfolio.cash
    if(context.state == 0):
        # order each stock 1000 dollars
        context.order_id1 = order_value(context.stock1, current_cash / 2)
        context.order_id2 = order_value(context.stock2, current_cash / 2)
        context.state = 1

    # portfolia object
    #print "stock 1 amount" + str(context.portfolio.positions[context.stock1].amount)
    #print "stock 2 amount" + str(context.portfolio.positions[context.stock2].amount)

def rebalance(context, data):
    if context.portfolio.positions[context.stock1].amount > 0:
        order_target(context.stock1, 0)

    if context.portfolio.positions[context.stock2].amount > 0:
        order_target(context.stock2, 0)

    # buy back the stock
    # TODO: need some time to sell the stock
    current_cash = context.portfolio.cash
    context.order_id1 = order_value(context.stock1, current_cash / 2)
    context.order_id2 = order_value(context.stock2, current_cash / 2)
    context.state = 1

def handle_data(context,data):
    """
    Called every minute.
    """
    price1 = data[context.stock1].price
    price2 = data[context.stock2].price

    # event triggered?
    if price1 < context.OpenPrice1 * 0.95:
        context.trigger1 = 1
    if price2 < context.OpenPrice2 * 0.95:
        context.trigger2 = 1

    if context.trigger1 == 1 and context.trigger2 == 1:
        print "error cannot both be triggered"

    if context.trigger1:
        order_target(context.stock1, 0)

    if context.trigger2:
        order_target(context.stock2, 0)

    if context.trigger1 == 1 or context.trigger2 == 1:
        context.state = 2

