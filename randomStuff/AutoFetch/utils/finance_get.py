import numpy as np
from pandas_datareader.data import Options
import datetime

#local
import utils.get_quote as gq

OptionBackEnd = 'yahoo'

def get_stock_price(Symbol):
    if Symbol == None:
        return    

    info = gq.get_quote_localimpl(Symbol)

    return info

def get_stock_call_option(Symbol, Expiry):
    if not Symbol:
        return None

    if not Expiry:
        return None

    option = Options(Symbol, OptionBackEnd)
    data = option.get_call_data(expiry=Expiry)

    return data
    
def get_stock_put_option(Symbol, Expiry):
    if not Symbol:
        return None

    if not Expiry:
        return None

    option = Options(Symbol, OptionBackEnd)
    data = option.get_put_data(expiry=Expiry)

    return data

if __name__ == '__main__':
    Symbols = ['AAPL', 'GOOG', 'NVDA', 'AMD', 'QCOM', 'TSLA', 'TWTR', 'FB', 'BIDU', 'QIWI', 'SNAP', 'YELP', 'SQ']

    print ('+' * 20 + 'Testing get_stock_price' + '+' * 20)
    for sym in Symbols:
        price = get_stock_price(sym)
        print (sym + ":" + str(price))

    print ('\n\n' + '+' * 20 + 'Testing get_stock_call_option' + '+' * 20)
    Symbol = 'AAPL'
    Date = datetime.date(2017, 11, 17)
    data = get_stock_call_option(Symbol, Date)
    print (data.iloc[0:5, 0:5])

    print ('\n\n' + '+' * 20 + 'Testing get_stock_put_option' + '+' * 20)
    Symbol = 'AAPL'
    Date = datetime.date(2017, 11, 17)
    data = get_stock_put_option(Symbol, Date)
    print (data.iloc[0:5, 0:5])
