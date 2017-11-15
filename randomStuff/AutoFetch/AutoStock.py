import datetime
import os

#local
import utils.finance_get as fin
import process_data as process_data

class AutoStock():
    '''
        symbol name
        current_price
        expiry list of expiry date of option
        call_threshold
        put_threshold
        dataframe_call = {} with expiry as key
        dataframe_put  = {} with expiry as key
        estimate_price = {} with expiry as key
    '''
    def __init__(self, Symbol, Expiry, call_threshold = 0.2, put_threshold = 0.2):
        self.symbol = Symbol
        self.expiry = Expiry
        self.call_threshold = call_threshold
        self.put_threshold  = put_threshold

        self.dataframe_call = {}
        self.dataframe_put = {}
        self.estimate_price = {}

    def fetch_data(self):
        for date in self.expiry:
            self.dataframe_call[date] = fin.get_stock_call_option(self.symbol, date)
            self.dataframe_put[date]  = fin.get_stock_put_option(self.symbol, date)

    def estimate_process(self):
        self.current_price = fin.get_stock_price(self.symbol)

        for date in self.expiry:
            self.estimate_price[date] = process_data.combine_estimate(
                                            self.dataframe_call[date],
                                            self.dataframe_put[date],
                                            self.current_price,
                                            self.call_threshold,
                                            self.put_threshold)

    def get_estimate(self):
        return self.estimate_price

    def save_raw_data(self):
        pwd = os.getcwd()
        if not os.path.isdir(pwd + '/data'):
            os.makedirs(pwd + '/data')

        now = datetime.date.today()

        for date in self.expiry:
            filename = 'data/' + self.symbol + ' Today_' + str(now) + '_call' + str(date) + '.csv'
            self.dataframe_call[date].to_csv(filename, sep = '\t', encoding='utf-8')

if __name__ == '__main__':
    Symbol = 'AAPL'
    Date1 = datetime.date(2017, 11, 17)
    Date2 = datetime.date(2017, 12, 17)
    Expiry = [Date1, Date2]

    stock = AutoStock(Symbol, Expiry)

    print ('\n\n' + '+' * 20 + 'Testing fetch_data' + '+' * 20)
    stock.fetch_data()

    for date in Expiry:
        print (stock.dataframe_call[date].iloc[0:3, 0:3])
        print (stock.dataframe_put[date].iloc[0:3, 0:3])

    print ('\n\n' + '+' * 20 + 'Testing fetch_data' + '+' * 20)
    stock.estimate_process()

    print ("stock current price is: ", stock.current_price)
    for date in Expiry:
        print (stock.estimate_price[date])
        
    print ('\n\n' + '+' * 20 + 'Testing save_raw_data' + '+' * 20)
    stock.save_raw_data()

    print ('\n\n' + '+' * 20 + 'Testing get_estimate' + '+' * 20)
    p = stock.get_estimate()
    print (p)
