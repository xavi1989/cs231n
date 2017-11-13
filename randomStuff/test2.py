import pandas as pd
import numpy as np
import datetime
from pandas_datareader import data, wb
from pandas_datareader.data import Options
from yahoo_finance import Share
import json
import requests

def get_stock_price(Symbol):

    try:
        rsp = requests.get('https://finance.google.com/finance?q=' + Symbol + '&output=json')
    except:
        return None

    if rsp == None:
        return None

    if rsp.status_code in (200,):

        # This magic here is to cut out various leading characters from the JSON 
        # response, as well as trailing stuff (a terminating ']\n' sequence), and then
        # we decode the escape sequences in the response
        # This then allows you to load the resulting string
        # with the JSON module.

        try:
            fin_data = json.loads(rsp.content[6:-2].decode('unicode_escape'))
        except:
            return None

        '''
        # print out some quote data
        print('Opening Price: {}'.format(fin_data['op']))
        print('Price/Earnings Ratio: {}'.format(fin_data['pe']))
        print('52-week high: {}'.format(fin_data['hi52']))
        print('52-week low: {}'.format(fin_data['lo52']))
        '''
        range_high = float(fin_data['hi'].replace(',', ''))
        range_low = float(fin_data['lo'].replace(',', ''))

        return (range_high + range_low) / 2
    else:
        return None

def cal_price(Symbol, Date):
    cur_price = get_stock_price(Symbol)

    if cur_price == None:
        cur_price = 0

    expiry = Date

    amd = Options(Symbol, 'yahoo')

    data = amd.get_call_data(expiry=expiry)
    #print (data.iloc[0:5, [1, 2, 6]])

    index = np.array(data.index.tolist())

    strike = index[:, 0]

    vol = data.iloc[:, [1, 2, 5, 6]]

    volumn_threshold = 0.1 * np.mean(vol['Vol'])

    mask = vol['Vol'] > volumn_threshold

    #mask2: if strike + vol < current price, then even volumn is 0 is acceptable
    mask2 = (strike + vol['Ask']) < cur_price

    mask |= mask2

    totalPrice_call = np.sum((strike + vol['Ask']) * vol['Open_Int'] * mask)
    totalVol_call = np.sum(vol['Open_Int'] * mask)

    Price_call = totalPrice_call / totalVol_call

    data = amd.get_put_data(expiry=expiry)
    #print (data.iloc[0:5, [1, 2, 6]])
    index = np.array(data.index.tolist())

    strike = index[:, 0]

    vol = data.iloc[:, [1, 2, 5, 6]]

    volumn_threshold = 0.25 * np.mean(vol['Vol'])

    mask = vol['Vol'] > volumn_threshold

    #mask2: if strike - vol > current price, then even volumn is 0 is acceptable
    mask2 = (strike - vol['Ask']) > cur_price

    mask |= mask2

    #strike - price
    totalPrice_put = np.sum((strike - vol['Ask']) * vol['Open_Int'] * mask)
    totalVol_put = np.sum(vol['Open_Int'] * mask)

    Price_put = totalPrice_put / totalVol_put

    return Price_call, Price_put, (totalPrice_put + totalPrice_call) / (totalVol_put + totalVol_call), cur_price

Symbols = ['AAPL', 'GOOG', 'NVDA', 'AMD', 'QCOM', 'TSLA', 'TWTR', 'FB', 'BIDU', 'QIWI', 'SNAP', 'YELP', 'SQ']
Date1 = datetime.date(2017, 11, 17)
Date2 = datetime.date(2017, 12, 17)

N = len(Symbols)

info1 = np.zeros((N, 4))
info2 = np.zeros((N, 4))

for i in range(N):
    a, b, c, d = cal_price(Symbols[i], Date1)
    info1[i, :] = np.array([a, b, c, d])
    info1 = np.around(info1, decimals=3)

    a, b, c, d = cal_price(Symbols[i], Date2)
    info2[i, :] = np.array([a, b, c, d])
    info2 = np.around(info2, decimals=3)

sym = np.array(Symbols)

info1 = np.hstack((sym.reshape((-1, 1)), info1))
info2 = np.hstack((sym.reshape((-1, 1)), info2))

print ("+" * 25 + str(Date1) + "+" * 25)
print (info1)
print ("+" * 25 + str(Date2) + "+" * 25)
print (info2)
