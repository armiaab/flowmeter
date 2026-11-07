# Flowmeter

A flow meter that sends its readings over LoRa to a second board, which logs them to Google Sheets. A small dashboard then plots them.

## How it works

1. **`flowmeter/`** is the sender. An ESP32 counts pulses from a flow sensor and works out the flow rate every 50 ms. Every 10 seconds it sends the average and standard deviation in a 12-byte LoRa packet at 433 MHz.
2. **`reciever/`** is the receiver. A second ESP32 picks up the packets and posts them to a Google Apps Script web app (`reciever/google_apps_script.js`), which adds them to a spreadsheet.
3. **`Flowmeter web.py`** is a Streamlit dashboard. It plots the average and standard deviation, filters by date and exports CSV.

## Setup

Both firmware folders are PlatformIO projects. Before building the receiver, set your Wi-Fi name, Wi-Fi password and Apps Script URL at the top of `reciever/src/main.cpp`.

The Google Sheets steps are in [`reciever/README.md`](reciever/README.md). Its data format section is older: the sender now uses the binary packet described above.

Point `DATA_URL` in `Flowmeter web.py` at your own Apps Script URL, then run:

```sh
streamlit run "Flowmeter web.py"
```
