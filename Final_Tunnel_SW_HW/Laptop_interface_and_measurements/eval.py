import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit


def model(d, c):
    return -20 * np.log10(d) - c

if __name__ == "__main__":
    
    df1 = pd.read_csv("Rulletest1/målinger_20250707_222847.csv")
    df2 = pd.read_csv("Rulletest1/målinger_20250707_233312.csv")

    df1_433 = df1[df1["Frequency"] == 433]
    #df1_2450 = df1[df1["Frequency"] == 2450]

    df2_433 = df2[df2["Frequency"] == 433]

    d = df1_433["Distance"].to_numpy()
    rssi = df1_433["RSSI"].to_numpy()

    popt, pcov = curve_fit(model, d, rssi)
    c_fit = popt[0]

    print(f"Best fit c: {c_fit:.2f}")


    #df2_2450 = df2[df2["Frequency"] == 2450]
    d = np.linspace(1, 630, 200)
    rssi_est = model(d, c_fit)

    ## plotting
    fig, ax = plt.subplots(figsize=(18, 6))

    #ax.plot(
    #    df1_2450["Distance"],df1_2450["RSSI"],
    #    linewidth=0.2,
    #    label="2.4GHz RSSI",
    #    color="blue",
    #)
    ax.plot(
        df1_433["Distance"],df1_433["RSSI"],
        linewidth=0.2,
        label="433MHz RSSI",
        color="blue",
    )
    #ax.plot(
    #    df2_2450["Distance"],df2_2450["RSSI"],
    #    linewidth=0.3,
    #    label="2.4GHz RSSI",
    #    color="blue",
    #)
    ax.plot(
        df2_433["Distance"],df2_433["RSSI"],
        linewidth=0.3,
        label="433MHz RSSI",
        color="red",
    )

    ax.plot(
        d,rssi_est,
        linewidth=0.3,
        label="theoretical",
        color="gray",
    )

    ax.legend()
    plt.show()
