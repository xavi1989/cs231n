import numpy as np
import datetime
import pandas as pd
from openpyxl import load_workbook
import os

#local
import utils.finance_get as fin
import process_data as process_data
import utils.stock_dataset as stockData

def get_option_index(data, price):
    index_array = np.array(data.index.values)
    for i in range(len(index_array) - 1):
        price1 = index_array[i][0]
        price2 = index_array[i + 1][0]
        if price >= price1 and price <= price2:
            if price <= (price1 + price2) / 2:
                return i
            else:
                return i + 1

    price2 = index_array[len(index_array) - 1][0]
    if (price - price2) < 1:
        return len(index_array) - 1
    else:
        return None

def query_option_price(data_call, data_put, price_up, price_down):
    price_neutral = (price_up + price_down) / 2
    index1 = np.array(data_call.index.values)
    index2 = np.array(data_put.index.values)

    index_up = get_option_index(data_call, price_up)
    index_down = get_option_index(data_put, price_down)

    if index_up is None or index_down is None:
        return None

    d1 = index1[index_up]
    d2 = index2[index_down]

    option_price1 = data_call.loc[d1, 'Ask']
    option_price2 = data_put.loc[d2, 'Ask']

    option_price = option_price1 + option_price2
    # break even price
    price1 = price_up + option_price1
    pencentage1 = (price1 - price_neutral) / price_neutral
    # one percentage gain
    gain1 = (price_neutral * 0.01) / option_price

    # break even price
    price2 = price_down - option_price2
    pencentage2 = -(price_neutral - price2) / price_neutral
    # one percentage gain
    gain2 = (price_neutral * 0.01) / option_price

    return [price_neutral, option_price1, pencentage1, gain1, option_price2, pencentage2, gain2]

def straddle_option(data_call, data_put, estimate_price, percentage):
    price_up = estimate_price * (1 + percentage)
    price_down = estimate_price * (1 - percentage)

    ret = query_option_price(data_call, data_put, price_up, price_down)
    return ret

def save_estimate_to_excel(dataframe):
    #sudo pip3 install openpyxl
    pwd = os.getcwd()
    if not os.path.isdir(pwd + '/data'):
        os.makedirs(pwd + '/data')

    # check if file exist
    filename = pwd + '/data/EstimateResult.xlsx'
    writer = pd.ExcelWriter(filename, engine='openpyxl') 
    if os.path.isfile(filename):
        # load existing excel
        book = load_workbook(filename)
        writer.book = book
        writer.sheets = dict((ws.title, ws) for ws in book.worksheets)

    # write new sheet with name as today's date
    now = datetime.date.today()
    name = str(now) + 'straddle'
    dataframe.to_excel(writer, name)
    writer.save()

def straddle_process():
    Symbols = []
    for k in sorted(stockData.Table):
        Symbols += [k]
    Date = datetime.date(2017, 12, 17)
    Percentages = [0.03, 0.05, 0.1, 0.15, 0.2]

    colnames = ['Symbol', 'Option_Date', 'Price_neutral', 'PercentJump', 'Option_price1', 'Percent1', 'Price1', 'Gain1', 'Option_price2', 'Percent2', 'Price2', 'Gain2']
    straddle_result = pd.DataFrame(index = [i for i in range(len(Symbols) * len(Percentages))], columns = colnames, dtype = float)
    # modify Date, Symbol, OpDate Trend to be string type
    colnames = ['Option_Date']

    for c in colnames:
        straddle_result[c] = straddle_result[c].astype(str)

    straddle_result['Option_Date'] = str(Date)

    for i, sym in enumerate(Symbols):
        print("processing stock " + str(sym))

        data_call = fin.get_stock_call_option(sym, Date)
        data_put = fin.get_stock_put_option(sym, Date)
        currentPrice = fin.get_stock_price(sym)

        if data_call is None or data_put is None:
            continue

        if data_call.shape[0] == 0 or data_put.shape[0] == 0:
            continue

        _, _, estimate_price = process_data.combine_estimate(data_call, data_put, currentPrice, callVol_threshold = 0.25, putVol_threshold = 0.25)

        for j, percent in enumerate(Percentages):
            ret = straddle_option(data_call, data_put, currentPrice, percent)
            if ret is None:
                ret = [0] * 7

            straddle_result.at[len(Percentages) * i + j, ['Symbol']] = str(sym)
            straddle_result.at[len(Percentages) * i + j, ['Price_neutral']] = ret[0]
            straddle_result.at[len(Percentages) * i + j, ['PercentJump']] = percent
            straddle_result.at[len(Percentages) * i + j, ['Option_price1']] = ret[1]
            straddle_result.at[len(Percentages) * i + j, ['Percent1']] = ret[2]
            straddle_result.at[len(Percentages) * i + j, ['Price1']] = ret[0] * (1 + ret[2])
            straddle_result.at[len(Percentages) * i + j, ['Gain1']] = ret[3]
            straddle_result.at[len(Percentages) * i + j, ['Option_price2']] = ret[4]
            straddle_result.at[len(Percentages) * i + j, ['Percent2']] = ret[5]
            straddle_result.at[len(Percentages) * i + j, ['Price2']] = ret[0] * (1 + ret[5])
            straddle_result.at[len(Percentages) * i + j, ['Gain2']] = ret[6]

    save_estimate_to_excel(straddle_result)
