import numpy as np
import datetime
import pandas as pd

#local
import utils.finance_get as fin

def slice_data_frame(Data):
    if Data is None:
        raise ValueError

    index = np.array(Data.index.tolist())
    N = len(index)
    data = np.zeros((N, 6))

    # strike
    data[:, 0] = index[:, 0]
    # 0 -> Last 1 -> Bid 2 -> Ask 5 -> Volumn 6 -> Open_Int
    data[:, 1:6] = Data.iloc[:, [0, 1, 2, 5, 6]]

    dataframe = pd.DataFrame(data)
    dataframe.columns = ['Strike', 'Last', 'Bid', 'Ask', 'Vol', 'Open_Int']

    return dataframe

def estimate_call_data(Data, currentPrice, vol_threshold = 0.2):
    if Data is None:
        raise ValueError

    if currentPrice is None:
        raise ValueError

    df = slice_data_frame(Data)

    volumn_threshold = vol_threshold * np.mean(df['Vol'])

    mask = df['Vol'] > volumn_threshold

    #mask2: if strike + vol < current price, then even volumn is 0 is acceptable
    mask2 = (df['Strike'] + df['Bid']) < currentPrice

    mask |= mask2

    totalMoney = np.sum((df['Strike'] + df['Bid']) * df['Open_Int'] * mask)
    totalNum = np.sum(df['Open_Int'] * mask)
    price = totalMoney / totalNum

    price = round(price, 3)
    return price, totalMoney, totalNum

def estimate_put_data(Data, currentPrice, vol_threshold = 0.2):
    if Data is None:
        raise ValueError

    if currentPrice is None:
        raise ValueError

    df = slice_data_frame(Data)

    volumn_threshold = vol_threshold * np.mean(df['Vol'])

    mask = df['Vol'] > volumn_threshold
    #mask2: if strike - vol > current price, then even volumn is 0 is acceptable
    mask2 = (df['Strike'] - df['Bid']) > currentPrice

    mask |= mask2
    #strike - price
    totalMoney = np.sum((df['Strike'] - df['Bid']) * df['Open_Int'] * mask)
    totalNum = np.sum(df['Open_Int'] * mask)
    price = totalMoney / totalNum

    price = round(price, 3)
    return price, totalMoney, totalNum

def combine_estimate(callData, putData, currentPrice,
                     callVol_threshold = 0.2, putVol_threshold = 0.2):
    if callData is None:
        raise ValueError

    if putData is None:
        raise ValueError

    if currentPrice is None:
        raise ValueError

    call_price, call_Sum, call_Num = estimate_call_data(callData, currentPrice, vol_threshold = callVol_threshold)
    put_price,  put_Sum,  put_Num  = estimate_put_data( putData,  currentPrice, vol_threshold = putVol_threshold)

    comb_price = (call_Sum + put_Sum) / (call_Num + put_Num)

    return comb_price

if __name__ == '__main__':
    print ('\n\n' + '+' * 20 + 'Testing slice_data_frame' + '+' * 20)
    Symbol = 'AAPL'
    Date = datetime.date(2017, 11, 17)
    data_call = fin.get_stock_call_option(Symbol, Date)

    data = slice_data_frame(data_call)
    print (data.iloc[0:5, :])

    print ('\n\n' + '+' * 20 + 'Testing estimate_call_data' + '+' * 20)
    Symbol = 'AAPL'
    Date = datetime.date(2017, 11, 17)
    currentPrice = fin.get_stock_price(Symbol)
    data_call = fin.get_stock_call_option(Symbol, Date)
    estimate_price = estimate_call_data(data_call, currentPrice, vol_threshold = 0.1)
    print ("estimate based on call is: " + str(estimate_price))

    print ('\n\n' + '+' * 20 + 'Testing estimate_put_data' + '+' * 20)
    Symbol = 'AAPL'
    Date = datetime.date(2017, 11, 17)
    currentPrice = fin.get_stock_price(Symbol)
    data_put = fin.get_stock_put_option(Symbol, Date)
    estimate_price = estimate_put_data(data_put, currentPrice, vol_threshold = 0.25)
    print ("estimate based on put is: " + str(estimate_price))

    print ('\n\n' + '+' * 20 + 'Testing combine_estimate' + '+' * 20)
    Symbol = 'AAPL'
    Date = datetime.date(2017, 11, 17)
    currentPrice = fin.get_stock_price(Symbol)
    data_put = fin.get_stock_put_option(Symbol, Date)
    estimate_price = combine_estimate(data_call, data_put, currentPrice, callVol_threshold = 0.1, putVol_threshold = 0.25)
    print ("combined estimate is: " + str(estimate_price))
