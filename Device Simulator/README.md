# AirLink Device BLE Simulator

## About
The AirLink Device BLE Simulator app is a useful tool for developers who want to test their BLE applications following AirLink standards. The App provides various functionalities for test purposes with the [AirLink Gateway Mobile App](https://airlinkdocs.enaccess.org/AirLink%20Manual%2083f2c2ce8fb0463a9078d63aad81a0c7/AirLink%20App%20ab448f056ad94eb097874d756342f361.html) such as:
- Device authorization
- Device provisioning/registration
- Data sync with the AirLink server (inc. timeseries)
- Emulating entry of PayG tokens

## How to use
Using this App is easy. You can [download the APK file](https://github.com/EnAccess/AirLink-Devices/releases/download/v1.0.0/AirLinkBLESimulator.apk) or  clone/download this repo on your favorite IDE and generate an APK.

Once the App starts, it will try to advertise on its own and available to connect. If advertisement fails, see below for the list of possible issues and troubleshooting.

| Overview |
| ------------- | 
| <img src="https://github.com/EnAccess/AirLink-Devices/blob/main/Device%20Simulator/assets/screenshots/overview.jpg"  width="400"> | 

 - **Advertise** - Tries to (stop advertising if already started and) advertise
 - **Stop advertising** - Stops the app from advertising
 - **Reset** - Stops the app from advertising, sets default initial data, and attempts to advertise again

## Known issues

1. **Device ID (MAC Address) keeps changing** - Every time the app advertises, the Device ID (MAC Address) changes, making it difficult to keep track of the same device over time.

2. **Duplicate services added** - Occasionally, the app adds redundant services (should normally be two), causing the [AirLink Gateway Mobile App](https://airlinkdocs.enaccess.org/AirLink%20Manual%2083f2c2ce8fb0463a9078d63aad81a0c7/AirLink%20App%20ab448f056ad94eb097874d756342f361.html) to be unable to read descriptors. In these cases, resetting the app solves the issue.

You might want to use the [nRF Connect App](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en&gl=US) for further debugging such as looking for duplicate services

4. **Advertisement packet limit** - If the advertisement packet (manufacturer data) exceeds 31 bytes, the device may fail to advertise. This can be caused by writing more than 6 characters while provisioning the device.

5. **Updating advertisement packet** - In order to update the advertisement data within the application, it is necessary to initiate a restart. This can be accomplished simply by removing the application from the list of "recent" apps and subsequently relaunching it.


## License
This repository is available under the [MIT License](https://opensource.org/licenses/MIT).
