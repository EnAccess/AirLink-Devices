# AirLink Device BLE Simulator

## About
The AirLink Device BLE Simulator app is a useful tool for developers who want to test their BLE applications following AirLink standards. The App provides various functionalities for test purposes with the [AirLink Gateway Mobile App](https://airlinkdocs.enaccess.org/AirLink%20Manual%2083f2c2ce8fb0463a9078d63aad81a0c7/AirLink%20App%20ab448f056ad94eb097874d756342f361.html) such as:
- Device authorization
- Device provisioning/registration
- Data sync with the AirLink server (inc. timeseries)
- Emulating entry of PayG tokens

## How to use
Using this App is easy. Clone/download this repo on your favorite IDE and generate an APK.

Once the App starts, it will try to advertise on its own and available to connect. If advertisement fails, see below for the list of possible issues and troubleshooting.

| Overview |
| ------------- | 
| <img src="https://github.com/kkhahima/airlink-ble-simulator/blob/master/app/src/main/res/screenshots/overview.jpg"  width="400"> | 

 - **Advertise** - Tries to (stop advertising if already started and) advertise
 - **Stop advertising** - Stops the app from advertising
 - **Reset** - Stops the app from advertising, sets default initial data, and attempts to advertise again

## Known issues
1. **Does not advertise for SDK > 28 (Above Android 9)** - The app does not work as a BLE advertiser on devices running Android 9 or above (SDK version 28 and above).

2. **Device ID (MAC Address) keeps changin ** - Every time the app advertises, the Device ID (MAC Address) changes, making it difficult to keep track of the same device over time.

3. **Duplicate services added** - Occasionally, the app adds redundant services (should normally be two), causing the [AirLink Gateway Mobile App](https://airlinkdocs.enaccess.org/AirLink%20Manual%2083f2c2ce8fb0463a9078d63aad81a0c7/AirLink%20App%20ab448f056ad94eb097874d756342f361.html) to be unable to read descriptors. In these cases, resetting the solves the issue.

You might want to use the [nRF Connect App](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en&gl=US) for further debugging such as looking for duplicate services

4. **Advertisement packet limit** - If the advertisement packet (manufacturer data) exceeds 31 bytes, the device may fail to advertise. This can be caused by writing more than 5 characters while provisioning the device.


## License
This repository is available under the [MIT License](https://opensource.org/licenses/MIT).
