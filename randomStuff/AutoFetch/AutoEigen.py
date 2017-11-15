import datetime
import numpy as np
import pandas as pd

#local import
import AutoStock

class AutoEigen():
    '''
        Symbols is list
        Expiries is list
        up_threshold
        down_threshold

        AutoStock stocks []
        estimate_result = np.array[N, 3 + 5 * numOf(Expiries)]
        Date Symbol TodayPrice OptionDate_1 callEst_1 putEst_1 combEst_1 Trend_1 ...
    '''
    def __init__(self, Symbols, Expiries,
                       call_threshold = 0.1, put_threshold = 0.25,
                       up_threshold = 0.67,
                       down_threshold = 0.33):
        self.stocks = []
        self.symbols = Symbols
        self.expiries = Expiries
        self.up_threshold = up_threshold
        self.down_threshold = down_threshold

        nStock = len(Symbols)
        nOption = len(Expiries)

        colnames = ['Date', 'Symbol', 'TodayPrice']
        for i in range(nOption):
            colnames += ['OpDate_' + str(i), 'callEst_' + str(i), 'putEst_' + str(i), 'combEst_' + str(i), 'Trend_' + str(i)]

        self.estimate_result = pd.DataFrame(index = [i for i in range(nStock)], columns = colnames, dtype = float)
        # modify Date, Symbol, OpDate Trend to be string type
        colnames = ['Date', 'Symbol']
        for i in range(nOption):
            colnames += ['OpDate_' + str(i), 'Trend_' + str(i)]

        for c in colnames:
            self.estimate_result[c] = self.estimate_result[c].astype(str)

        # set Date
        today = datetime.date.today()
        self.estimate_result['Date'] = str(today)
        # set Symbol
        for i in range(nStock):
            self.estimate_result.at[i, 'Symbol'] = str(Symbols[i])

        #set Expiry date
        for i in range(nOption):
            self.estimate_result['OpDate_' + str(i)] = str(Expiries[i])

        for sym in Symbols:
            self.stocks.append(AutoStock.AutoStock(sym, Expiries, call_threshold, put_threshold))

    def start(self):
        for stock in self.stocks:
            print ('+' * 20 + 'Processing stock: ' + stock.symbol + '+' * 20)
            stock.fetch_data()
            stock.estimate_process()
            stock.save_raw_data()

    def estimateTrend(self):
        #populate stock estimate
        for i, stock in enumerate(self.stocks):
            self.estimate_result.at[i, 'TodayPrice'] = stock.current_price
            p = stock.get_estimate()
            for j, date in enumerate(self.expiries):
                self.estimate_result.at[i, 'callEst_' + str(j)] = p[self.expiries[j]][0]
                self.estimate_result.at[i, 'putEst_' + str(j)] = p[self.expiries[j]][1]
                self.estimate_result.at[i, 'combEst_' + str(j)] = p[self.expiries[j]][2]

        # estimate trend
        for i, stock in enumerate(self.stocks):
            for j, date in enumerate(self.expiries):
                callEst = self.estimate_result.at[i, 'callEst_' + str(j)]
                putEst  = self.estimate_result.at[i, 'putEst_' + str(j)]
                combEst = self.estimate_result.at[i, 'combEst_' + str(j)]

                interval = callEst - putEst
                trend = 'Flat'
                if (combEst - putEst) / interval > self.up_threshold:
                    trend = 'Up'
                if (callEst - combEst) / interval < self.down_threshold:
                    trend = 'Down'

                self.estimate_result.at[i, 'Trend_' + str(j)] = trend

        print (self.estimate_result.head())

if __name__ == '__main__':
    #Symbols = ['AAPL', 'GOOG', 'NVDA', 'AMD', 'QCOM', 'TSLA', 'TWTR', 'FB', 'BIDU', 'QIWI', 'SNAP', 'YELP', 'SQ']
    Symbols = ['AAPL']
    Expiries = [datetime.date(2017, 11, 17), datetime.date(2017, 12, 17)]

    print ('\n\n' + '+' * 20 + 'Testing AutoEigen' + '+' * 20)
    autoEigen = AutoEigen(Symbols, Expiries)

    print ('\n\n' + '+' * 20 + 'Testing AutoEigen start' + '+' * 20)
    autoEigen.start()
    autoEigen.estimateTrend()
