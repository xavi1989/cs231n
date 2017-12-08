import pandas_datareader.data as web
import datetime

def get_stock_history_data(symbol, start, end):
    try:
        f = web.DataReader(symbol, 'yahoo', start, end)
    except:
        return None
    return f

'''
    find the growth so far.
    Min of first 3 day as start point
    today price as end point
    return the growth percentage
'''
def stock_data_gain(symbol, start, end):
    # get stock price
    f = get_stock_history_data(symbol, start, end)

    if f is None:
        return 0, 0, 0

    # process
    price = f.iloc[:, 3]
    start = min(price[0:2]) if len(price) > 2 else min(price)
    end = price[-1]

    jump = round((end - start) / start, 2)

    return start, end, jump

if __name__ == '__main__':
    symbol ='AAPL'
    start = datetime.datetime(2017, 11, 27)
    end   = datetime.date.today()

    f = get_stock_history_data(symbol, start, end)

    start, end, jump = stock_data_gain(symbol, start, end)
    print ([start, end, jump])
