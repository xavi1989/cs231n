import pandas_datareader.data as web
import datetime
import time
import os
import pandas as pd

import stock_dataset as stockData

#https://finance.google.com/finance/historical?q=NYSE:AAPL&startdate=2017-11-10&enddate=2017-12-15&output=csv

def get_stock_history_data(symbol, start, end):
    try:
        f = web.DataReader(symbol, 'yahoo', start, end)
        time.sleep(2)
    except:
        return None

    return f

def get_stock_history_data2(symbol, start, end):
    try:
        rsp = requests.get('https://finance.google.com/finance/historical?q=%s:%s&startdate=%s&enddate=%s&output=csv' % (EX, Symbol, str(start), str(end)))
    except:
        return None

    print (rsp)
    return None

def get_stocks_history_data(Symbols, start, end):
    start = datetime.datetime(2017, 11, 15)
    end   = datetime.date.today()
    pwd = os.getcwd()
    foldername = pwd + '/../data/history/'
    if not os.path.isdir(foldername):
        os.makedirs(foldername)
    for symbol in Symbols:
        f = get_stock_history_data(symbol, start, end)
        if f is None:
            print ('%s failed ....' % symbol)
            continue
        print (symbol)
        filename = foldername + symbol + '_history.xlsx'
        print (filename)
        writer = pd.ExcelWriter(filename, engine='openpyxl')
        f.to_excel(writer, 'Sheet1')
        writer.save()

def get_stock_price_from_excel(symbol, excel_name, start, end, priceType = 'Close'):
    df = pd.read_excel(excel_name, sheetname = 'Sheet1')
    a = 0
    a = (df[df['Date'] == start]['Close'])
    return a.as_matrix()

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

    f = get_stock_history_data2(symbol, start, end)

    start, end, jump = stock_data_gain(symbol, start, end)
    print ([start, end, jump])

    pwd = os.getcwd()
    foldername = pwd + '/../data/history/'
    excel_name = foldername + symbol + '_history.xlsx'
    start = datetime.datetime(2017, 11, 24)
    end   = datetime.datetime(2017, 11, 24)
    get_stock_price_from_excel(symbol, excel_name, start, end)

'''
    Symbols = []
    for k in sorted(stockData.Table):
        Symbols += [k]

    Symbols = ['VIPS']
    start = datetime.datetime(2017, 11, 15)
    end   = datetime.date.today()
    get_stocks_history_data(Symbols, start, end);
'''
