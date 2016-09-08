"""
This is a template algorithm on Quantopian for you to adapt and fill in.
"""
from quantopian.algorithm import attach_pipeline, pipeline_output
from quantopian.pipeline import Pipeline
from quantopian.pipeline.data.builtin import USEquityPricing
from quantopian.pipeline.factors import AverageDollarVolume

def PrintCurrentPortfolio(context, Data, title=None):
    """
        print current portfolio status
    """
    print title
    print "    "+str(get_datetime())
    print "    "+"cash: " + str(context.portfolio.cash)
    if context.portfolio.cash < 0:
        print "error: cash cannot be negative !!!!!"
    # stock1
    price1 = Data.current(context.stock1.stockSymbol, 'price')
    price2 = Data.current(context.stock2.stockSymbol, 'price')
    openPrice1 = context.stock1.openPrice
    openPrice2 = context.stock2.openPrice
    percent1 = "{0:.0f}%".format((price1 - openPrice1) / openPrice1 * 100)
    percent2 = "{0:.0f}%".format((price2 - openPrice2) / openPrice2 * 100)

        
    print "    "+"stock: "+context.stock1.stockName+" Amount: "+str(context.stock1.amount) +"  Price: "+ str(price1) + " Percent:" + percent1 + " isTrigger: "+str(context.stock1.isTrigger)
    # stock2
    print "    "+"stock: "+context.stock2.stockName+" Amount: "+str(context.stock2.amount) +"  Price: "+ str(price2) + " Percent:" + percent2 + " isTrigger: "+str(context.stock2.isTrigger)

    # porfolio value
    print "    "+"porfolio value " + str(context.portfolio.portfolio_value)
    print "    "+'\n\n'

class Transaction(object):
    """
        transaction object will keep the information of each buy or sell operation
        member variable:
            bool isOpen;
            string stockName;
            int orderId;
            float tradePrice;
            int targetAmount;
            int filled;

        member function:
            log
    """

    def __init__(self, orderId=None, name=None):
        self.orderId = orderId
        self.stockName = name
        self.isOpen = 1
        self.done = 0

    def log(self):
        time = get_datetime()
        print "<<<<<<<<<<<< transaction <<<<<<<<<<<<<"
        print "<<<<<< time: " + time + "<<<<<<<<"
        print "<< name: " + self.stockName + "  price: " + str(self.tradePrice) + " amount: " + str(self.targetAmount)
        print ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
        print "\n"

    def WaitForComplete(self, stock=None):
        """
            change state when complete
        """
        msg = get_order(self.orderId)
        if msg:
            if msg.amount == msg.filled:
                # close the transaction, update stock
                self.isOpen = 0
                stock.amount = stock.amount + msg.amount
                print "transaction done target: " + str(msg.amount) + "  filled: "+ str(msg.filled)
                self.done = 1

                if stock.amount < 0:
                    print "error cannot < 0 + " + stock.stockName
                

class Stock(object):
    """
        Stock member variable:
            string stockName
            symbol stockSymbol
            float openPrice
            int amount
            bool isTrigger
            Transaction trans

       Stock member function:
            
    """

    def __init__(self, name=""):
        if name is not "":
            self.stockName = name

        self.openPrice = 0
        self.amount = 0
        self.isTrigger = 0
        self.trans = Transaction()

    """
        sell stock with specified share
    """
    def sellShare(self, amount=None, Data=None):
        if amount is None:
            amount = self.amount

        if amount == 0:
            return
        
        print "sell transaction create "
        self.trans.isOpen = 1
        self.trans.stockName = self.stockName
        self.trans.orderId = order(self.stockSymbol, -amount)
        self.trans.tradePrice = Data.current(self.stockSymbol, 'price')

    """
        buy stock with amount of money
    """
    def buyValue(self, value=None, Data=None):
        if value is None:
            value = self.value

        if value == 0:
            return
        
        print "buy transaction create "
        self.trans.isOpen = 1
        self.trans.stockName = self.stockName
        self.trans.orderId = order_value(self.stockSymbol, value)
        self.trans.tradePrice = Data.current(self.stockSymbol, 'price')

def initialize(context):
    """
        state = 0: no stock position
        state = 1: transaction pending
        state = 2: hold two stock
        state = 3: hold one stock

        nextState: the state after pending
    """
    context.state = 0
    context.nextState = 0
    context.stock1 = Stock(name="LABD")
    # work around https://www.quantopian.com/posts/how-do-i-get-price-from-a-symbol-a-variable
    context.stock1.stockSymbol = symbol('LABD')

    context.stock2 = Stock(name="LABU")
    context.stock2.stockSymbol = symbol('LABU')

    context.isTrigger = 0

    """
        schedule order at the market open day
    """
    schedule_function(func=morningOrder,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_open(minutes=5))

    schedule_function(func=checkSell,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_close(minutes=30))

    schedule_function(func=rebuy,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_close(minutes=20))
        
    schedule_function(func=printClose,
                      date_rule=date_rules.every_day(),
                      time_rule=time_rules.market_close())


def morningOrder(context, data):
    print "Phase 1: morningOrder "
    context.stock1.openPrice = data.current(context.stock1.stockSymbol, 'price')
    context.stock2.openPrice = data.current(context.stock2.stockSymbol, 'price')

    # reset trigger event
    context.stock1.isTrigger = 0
    context.stock2.isTrigger = 0
    context.isTrigger = 0
    
    current_cash = context.portfolio.cash
    if context.state == 0:
        print "Phase 1: MorningOrder Transaction Create"
        # https://www.quantopian.com/posts/negative-cash
        # allocate 90% of case
        context.stock1.buyValue(value=current_cash / 2 * 0.9, Data=data)
        context.stock2.buyValue(value=current_cash / 2 * 0.9, Data=data)
        context.state = 1 # pending state
        context.nextState = 2

def checkSell(context, data):
    if context.isTrigger == 0:
        return

    PrintCurrentPortfolio(context, data, title="Phase 3: checkSell")

    context.stock1.sellShare(Data=data)
    context.stock2.sellShare(Data=data)
    context.state = 1 # pending state
    context.nextState = 0

def rebuy(context, data):
    # buy back the stock
    # TODO: check all the transaction has been completed
    if context.isTrigger == 0:
        return

    PrintCurrentPortfolio(context, data, title="Phase 4: before rebuy")
    current_cash = context.portfolio.cash

    context.stock1.buyValue(value=current_cash / 2 * 0.9, Data=data)
    context.stock2.buyValue(value=current_cash / 2 * 0.9, Data=data)
    context.state = 1
    context.nextState = 2

def checkOpenTransaction(context, data):
    if context.stock1.trans.isOpen == 1:
        print "stock1 transaction is not completed !!!!"
        msg = get_order(context.stock1.trans.orderId)

    if context.stock2.trans.isOpen == 1:
        print "stock1 transaction is not completed !!!!"
        msg = get_order(context.stock2.trans.orderId)    


def printClose(context, data):
    PrintCurrentPortfolio(context, data, title="Phase 5: PrintClose")

    # check open transaction
    checkOpenTransaction(context, data)

    print "\n"


def event_trigger(context, data):
    price1 = data.current(context.stock1.stockSymbol, 'price')
    price2 = data.current(context.stock2.stockSymbol, 'price')
    openPrice1 = context.stock1.openPrice
    openPrice2 = context.stock2.openPrice
    percent1 = "{0:.0f}%".format((price1 - openPrice1) / openPrice1 * 100)
    percent2 = "{0:.0f}%".format((price2 - openPrice2) / openPrice2 * 100)
    
    if price1 < context.stock1.openPrice * 0.93:
        context.stock1.isTrigger = 1

    if price2 < context.stock2.openPrice * 0.93:
        context.stock2.isTrigger = 1

    if context.stock1.isTrigger == 1 or context.stock2.isTrigger == 1:
        context.isTrigger = 1
        print "Phase 2:"
        print "       stock1 event trigger current price: " + str(price1) + " openPrice " + str(context.stock1.openPrice) + "percent " + percent1
        print "       stock2 event trigger current price: " + str(price2) + " openPrice " + str(context.stock2.openPrice) + "percent " + percent2

def trackTheOtherStock(context, data):
    price1 = data.current(context.stock1.stockSymbol, 'price')
    price2 = data.current(context.stock2.stockSymbol, 'price')
    openPrice1 = context.stock1.openPrice
    openPrice2 = context.stock2.openPrice
    percent1 = "{0:.0f}%".format((price1 - openPrice1) / openPrice1 * 100)
    percent2 = "{0:.0f}%".format((price2 - openPrice2) / openPrice2 * 100)

    if context.stock1.isTrigger == 1:
        # keep an eye on stock2
        if price2 < context.stock2.openPrice * 0.93:
            context.stock2.isTrigger = 1
            context.stock2.sellShare(Data=data)

    if context.stock2.isTrigger == 1:
        # keep an eye on stock1
        if price1 < context.stock1.openPrice * 0.93:
            context.stock1.isTrigger = 1
            context.stock1.sellShare(Data=data)

    if context.stock1.isTrigger == 1 and context.stock2.isTrigger == 1:
        context.isTrigger = 1
        print "Phase exception:"
        print "       stock1 event trigger current price: " + str(price1) + " openPrice " + str(context.stock1.openPrice) + "percent " + percent1
        print "       stock2 event trigger current price: " + str(price2) + " openPrice " + str(context.stock2.openPrice) + "percent " + percent2
        context.state = 1
        context.nextState = 0

def handle_data(context,data):
    """
    Called every minute.
    """

    #print context.state
    if context.state == 1:
        # wait open transaction to complete
        if context.stock1.trans.isOpen == 1:
            context.stock1.trans.WaitForComplete(stock=context.stock1)

        if context.stock2.trans.isOpen == 1:
            context.stock2.trans.WaitForComplete(stock=context.stock2)

        if context.nextState == 2:
            # two transaction pending
            if context.stock1.trans.done == 1 and context.stock2.trans.done == 1:
                context.stock1.trans.done = 0
                context.stock2.trans.done = 0
                context.state = context.nextState
        else:
            # one transaction pending
        
            if context.stock1.trans.done == 1:
                context.stock1.trans.done = 0
                context.state = context.nextState

            if context.stock2.trans.done == 1:
                context.stock2.trans.done = 0
                context.state = context.nextState

    elif context.state == 0:
        # do nothing
        pass
    elif context.state == 2:
        # check event trigger
        # current only trigger once per day TODO            
        if context.isTrigger == 0:
            pass
        else:
            return
    
        event_trigger(context, data)

        if context.stock1.isTrigger == 1 and context.stock2.isTrigger == 1:
            print "error cannot both be triggered !!!"

        if context.stock1.isTrigger == 1:
            context.stock1.sellShare(Data=data)
            
        if context.stock2.isTrigger == 1:
            context.stock2.sellShare(Data=data)
        
        if context.isTrigger == 1:
            context.state = 1
            context.nextState = 3

    elif context.state == 3:
        # sell limit the other stock
        trackTheOtherStock(context, data)

