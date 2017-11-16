import json
import requests
import time

import utils.stock_dataset as stockData

#https://finance.google.com/finance?q=NYSE:SNAP&output=json

def get_quote_localimpl(Symbol):
    Table = stockData.Table

    if Symbol in Table:
        EX = Table[Symbol]
    else:
        print ("cannot find Exchange for the Symbol " + Symbol)
        return None;

    try:
        rsp = requests.get('https://finance.google.com/finance?q=' + EX + ':' + Symbol + '&output=json')
    except:
        return None

    # sleep 2 seconds
    time.sleep(2)

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

        return round((range_high + range_low) / 2, 3)
    else:
        return None


if __name__ == '__main__':
    Symbols = ['AAPL', 'GOOG', 'NVDA', 'AMD', 'QCOM', 'TSLA', 'TWTR', 'FB', 'BIDU', 'QIWI', 'SNAP', 'YELP', 'SQ']

    for sym in Symbols:
        price = get_quote_localimpl(sym)
        print (sym + ":" + str(price))
