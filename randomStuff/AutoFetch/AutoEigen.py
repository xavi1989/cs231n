import datetime
import numpy as np

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
                       call_threshold = 0.2, put_threshold = 0.2,
                       up_threshold = 0.8,
                       down_threshold = 0.3):
        self.stocks = []
        self.symbols = Symbols
        self.expiries = Expiries
        self.up_threshold = up_threshold
        self.down_threshold = down_threshold

        nStock = len(Symbols)
        nOption = len(Expiries)

        self.estimate_result = np.empty((nStock, 3 + 5 * nOption))

        today = datetime.date.today()
        self.estimate_result[:, 0] = str(today)

        for sym in Symbols:
            self.stocks.append(AutoStock.AutoStock(sym, Expiries, call_threshold, put_threshold))

    def start(self):
        for stock in self.stocks:
            print ('+' * 20 + 'Processing stock: ' + stock.symbol + '  +' * 20)
            stock.fetch_data()
            stock.estimate_process()
            stock.save_raw_data()

    def estimateTrend(self):
        pass

if __name__ == '__main__':
    Symbols = ['AAPL', 'GOOG', 'NVDA', 'AMD', 'QCOM', 'TSLA', 'TWTR', 'FB', 'BIDU', 'QIWI', 'SNAP', 'YELP', 'SQ']
    Expiries = [datetime.date(2017, 11, 17), datetime.date(2017, 12, 17)]

    print ('\n\n' + '+' * 20 + 'Testing AutoEigen' + '+' * 20)
    autoEigen = AutoEigen(Symbols, Expiries)

    print ('\n\n' + '+' * 20 + 'Testing AutoEigen start' + '+' * 20)
    autoEigen.start()
