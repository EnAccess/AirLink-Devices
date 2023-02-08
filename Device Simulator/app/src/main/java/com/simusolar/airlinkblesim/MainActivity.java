package com.simusolar.airlinkblesim;

import android.Manifest;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.Button;
import android.widget.ExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Random;
import java.util.UUID;


public class MainActivity extends AppCompatActivity {

    private static final String DEVICE_NAME = "AIRLINK";
    private static final UUID PC_CHARACTERISTIC_UUID = UUID.fromString("d40a2489-f277-4600-b583-dd255530a759");
    private static final UUID CLIENT_CHARACTERISTIC_CONFIGURATION_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    private static final String TAG = MainActivity.class.getCanonicalName();
    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_BLUETOOTH_CONNECT_PERMISSION = 2;
    private static final int REQUEST_BLUETOOTH_ADVERTISE_PERMISSION = 3;
    private TextView bleSupportValue;
    private TextView connectedDevices;
    private TextView advtStatus;
    private TextView deviceName;
    private TextView deviceSerialNo;
    private Button rstButton;
    private Button advtButton;
    private Button stopAdvtButton;

    private byte[] manufacturerSpecificData;

    Boolean isAuthorized = false;

    ExpandableListView expandableListView;
    ExpandableListAdapter expandableListAdapter;
    List<BluetoothGattCharacteristic> expandableListTitle;
    HashMap<BluetoothGattCharacteristic, List<BluetoothGattDescriptor>> expandableListDetail;
    BluetoothGattServer bluetoothGattServer;
    AdvertiseSettings settings;
    AdvertiseData advertiseData;
    AdvertiseData advertiseDataScanResponse;
    BluetoothLeAdvertiser bluetoothLeAdvertiser;
    BluetoothManager bluetoothManager;
    BluetoothAdapter bluetoothAdapter;
    Service service;
    Boolean isAdvertising = false;
    SharedPreferences prefs;
    SharedPreferences.OnSharedPreferenceChangeListener listener;

    private final AdvertiseCallback advertiseCallback = new AdvertiseCallback() {
        @Override
        public void onStartSuccess(AdvertiseSettings settingsInEffect) {
            super.onStartSuccess(settingsInEffect);
            advtStatus.setText(R.string.status_advertising);
            isAdvertising = true;
            setDeviceValues();
            getData();
        }

        @Override
        public void onStartFailure(int errorCode) {
            super.onStartFailure(errorCode);
            switch (errorCode) {
                case ADVERTISE_FAILED_ALREADY_STARTED:
                    isAdvertising = true;
                    advtStatus.setText(R.string.status_advertising);
                    //getData();
                    Log.e(TAG, "Advert already started");
                    //show toast on ui thread
                    runOnUiThread(() -> Toast.makeText(MainActivity.this, R.string.status_advAlreadyStarted, Toast.LENGTH_LONG).show());
                    break;
                case ADVERTISE_FAILED_DATA_TOO_LARGE:
                    isAdvertising = false;
                    Log.e(TAG, "Advert data too large");
                    //show toast on ui thread
                    runOnUiThread(() -> Toast.makeText(MainActivity.this, R.string.status_advDataTooLarge, Toast.LENGTH_LONG).show());
                    break;
                case ADVERTISE_FAILED_FEATURE_UNSUPPORTED:
                    isAdvertising = false;
                    Log.e(TAG, "Feature not supported on platform");
                    //show toast on ui thread
                    runOnUiThread(() -> Toast.makeText(MainActivity.this, R.string.status_featureNotSupportedOnPlatform, Toast.LENGTH_LONG).show());
                    break;
                case ADVERTISE_FAILED_INTERNAL_ERROR:
                    isAdvertising = false;
                    Log.e(TAG, String.valueOf(R.string.status_advInternalError));
                    //show toast on ui thread
                    runOnUiThread(() -> Toast.makeText(MainActivity.this, R.string.status_advInternalError, Toast.LENGTH_LONG).show());
                    break;
                case ADVERTISE_FAILED_TOO_MANY_ADVERTISERS:
                    isAdvertising = false;
                    Log.e(TAG, "Too many advertisers");
                    //show toast on ui thread
                    runOnUiThread(() -> Toast.makeText(MainActivity.this, R.string.status_advTooManyAdvertisers, Toast.LENGTH_LONG).show());
                    break;
                default:
                    isAdvertising = false;
                    Log.wtf(TAG, "Unhandled error: " + errorCode);
            }
        }
    };

    private final BluetoothGattServerCallback bluetoothGattServerCallback = new BluetoothGattServerCallback() {
        @Override
        public void onConnectionStateChange(BluetoothDevice device, int status, int newState) {
            super.onConnectionStateChange(device, status, newState);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (newState == BluetoothGatt.STATE_CONNECTED) {
                    updateConnectedDevicesStatus();
                    Log.v(TAG, "Connected to device: " + device.getAddress());

                } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                    Log.v(TAG, "Disconnected from device");
                    updateConnectedDevicesStatus();
                    isAuthorized = false;
                }
            } else {
                Log.e(TAG, "Error when connecting: " + status);
            }
        }

        @Override
        public void onServiceAdded(int status, BluetoothGattService service) {
            super.onServiceAdded(status, service);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.v(TAG, "Service added");
            } else {
                Log.e(TAG, "Error adding service");
            }
        }

        @SuppressLint("MissingPermission")
        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicReadRequest(device, requestId, offset, characteristic);

            Log.d(TAG, "Device tried to read characteristic: " + characteristic.getUuid());
            //set characteristic value to null if not authorized

            byte[] nullValue = {};

            AirLinkData airLinkData = new AirLinkData();

            switch (characteristic.getUuid().toString()) {
                case Characteristic.PC_CHARACTERISTIC_UUID:
                    if (isAuthorized) {
                        characteristic.setValue(airLinkData.getPcMap().EncodeToBytes());
                        decrementThi();
                    } else {
                        characteristic.setValue(nullValue);
                    }
                    break;
                case Characteristic.BATT_CHARACTERISTIC_UUID:
                    if (isAuthorized) {
                        characteristic.setValue(airLinkData.getBattMap().EncodeToBytes());
                        decrementThi();
                    } else {
                        characteristic.setValue(nullValue);
                    }
                    break;
                case Characteristic.TEMP_CHARACTERISTIC_UUID:
                    if (isAuthorized) {
                        characteristic.setValue(airLinkData.getTempMap().EncodeToBytes());
                        decrementThi();
                    } else {
                        characteristic.setValue(nullValue);
                    }
                    break;
                case Characteristic.CCFG_CHARACTERISTIC_UUID:
                    characteristic.setValue(isAuthorized ? airLinkData.getCcfgMap().EncodeToBytes() : nullValue);
                    break;
                case Characteristic.DCFG_CHARACTERISTIC_UUID:
                    characteristic.setValue(isAuthorized ? airLinkData.getDcfgMap().EncodeToBytes() : nullValue);
                    break;
                case Characteristic.DFU_CHARACTERISTIC_UUID:
                    characteristic.setValue(isAuthorized ? airLinkData.getDfuMap().EncodeToBytes() : nullValue);
                    break;
                default:
                    Log.e(TAG, "Unknown characteristic: " + characteristic.getUuid());
                    bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_FAILURE, offset, characteristic.getValue());
                    return;

            }

            if (offset != 0) {
                System.out.println("Invalid offset: " + offset);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_INVALID_OFFSET, offset, characteristic.getValue());
                return;
            }

            bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, characteristic.getValue());
        }

        @SuppressLint("MissingPermission")
        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId, BluetoothGattCharacteristic characteristic, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
            super.onCharacteristicWriteRequest(device, requestId, characteristic, preparedWrite, responseNeeded, offset, value);
            Log.v(TAG, "Characteristic Write request: " + characteristic.getUuid().toString());
            try {
                JSONObject jsonObject = new JSONObject(Helper.cborToString(value));
                //todo: check for descriptor, characteristic and value.. update the appropriate map
                writeToCharacteristic(characteristic, jsonObject);
            } catch (JSONException err) {
                Log.d("Error", err.toString());
            }
            int status = writeCharacteristic(characteristic, offset, value);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                bluetoothGattServer.sendResponse(device, requestId, status, 0, null);
            }

            if (responseNeeded) {
                bluetoothGattServer.sendResponse(device, requestId, status, 0, null);
            }
        }

        @SuppressLint("MissingPermission")
        @Override
        public void onDescriptorReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
            super.onDescriptorReadRequest(device, requestId, offset, descriptor);

            Log.d(TAG, "Device tried to read descriptor: " + descriptor.getUuid());
            Log.d(TAG, "Value: " + Arrays.toString(descriptor.getValue()));
            if (offset != 0) {
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_INVALID_OFFSET, offset,
                        /* value (optional) */ null);
                return;
            }
            bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, descriptor.getValue());
        }

        @Override
        public void onDescriptorWriteRequest(BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
            super.onDescriptorWriteRequest(device, requestId, descriptor, preparedWrite, responseNeeded, offset, value);

            Log.v(TAG, "Descriptor Write Request " + descriptor.getUuid() + " " + Arrays.toString(value));
            int status;
            if (descriptor.getUuid() == CLIENT_CHARACTERISTIC_CONFIGURATION_UUID) {
                BluetoothGattCharacteristic characteristic = descriptor.getCharacteristic();
                boolean supportsNotifications = (characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0;
                boolean supportsIndications = (characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_INDICATE) != 0;

                if (!(supportsNotifications || supportsIndications)) {
                    status = BluetoothGatt.GATT_REQUEST_NOT_SUPPORTED;
                } else if (value.length != 2) {
                    status = BluetoothGatt.GATT_INVALID_ATTRIBUTE_LENGTH;
                } else if (Arrays.equals(value, BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE)) {
                    status = BluetoothGatt.GATT_SUCCESS;
                    notificationsDisabled(characteristic);
                    descriptor.setValue(value);
                } else if (supportsNotifications && Arrays.equals(value, BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE)) {
                    status = BluetoothGatt.GATT_SUCCESS;
                    notificationsEnabled(characteristic, false /* indicate */);
                    descriptor.setValue(value);
                } else if (supportsIndications && Arrays.equals(value, BluetoothGattDescriptor.ENABLE_INDICATION_VALUE)) {
                    status = BluetoothGatt.GATT_SUCCESS;
                    notificationsEnabled(characteristic, true /* indicate */);
                    descriptor.setValue(value);
                } else {
                    status = BluetoothGatt.GATT_REQUEST_NOT_SUPPORTED;
                }
            } else {
                status = BluetoothGatt.GATT_SUCCESS;
                descriptor.setValue(value);
            }
            if (responseNeeded) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                        ActivityCompat.requestPermissions(
                                MainActivity.this,
                                new String[]{Manifest.permission.BLUETOOTH_CONNECT},
                                REQUEST_BLUETOOTH_CONNECT_PERMISSION);
                    }
                }
                bluetoothGattServer.sendResponse(device, requestId, status,
                        /* No need to respond with offset */ 0,
                        /* No need to respond with a value */ null);
            }
        }

        @Override
        public void onExecuteWrite(BluetoothDevice device, int requestId, boolean execute) {
            super.onExecuteWrite(device, requestId, execute);
        }

        @Override
        public void onNotificationSent(BluetoothDevice device, int status) {
            super.onNotificationSent(device, status);

        }

        @Override
        public void onMtuChanged(BluetoothDevice device, int mtu) {
            super.onMtuChanged(device, mtu);
            // Show a toast on the UI thread
            Thread thread = new Thread() {
                public void run() {
                    runOnUiThread(() -> Toast.makeText(getApplicationContext(), "MTU changed to: " + mtu, Toast.LENGTH_LONG).show());
                }
            };
            thread.start();
        }
    };

    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                        BluetoothAdapter.ERROR);
                if (state == BluetoothAdapter.STATE_OFF) {
                    onStop();
                } else if (state == BluetoothAdapter.STATE_ON) {
                    //if bluetooth is on, try to advertise again
                    reAdvertise();

                }
            }
        }
    };

    private void writeToCharacteristic(BluetoothGattCharacteristic characteristic, JSONObject jsonObject) {

        String descriptorName;

        if (characteristic.getUuid().toString().equals(Characteristic.DFU_CHARACTERISTIC_UUID)) {
            descriptorName = new String(characteristic.getDescriptors().get(1).getValue()).toLowerCase();
        } else {
            descriptorName = new String(characteristic.getDescriptors().get(0).getValue()).toLowerCase();
        }

        for (Iterator<String> it = jsonObject.keys(); it.hasNext(); ) {
            String key = it.next();
            try {
                //update data on local storage (shared prefs)
                //todo: check for special keys i.e. those that authorizes device to be read, updating pst, re, etc.

                if (descriptorName.equals("dfu") && key.equals("sat")) {
                    Log.i(TAG, "Device auth request");
                    //todo: check if access token (sat) is valid
                    if (jsonObject.get(key).toString().equals(prefs.getString("glb_sat", ""))) {
                        //set device as authorized
                        isAuthorized = true;
                        //make toast on UI thread
                        runOnUiThread(() -> Toast.makeText(getApplicationContext(), "Device authorized", Toast.LENGTH_LONG).show());
                    } else {
                        //make toast on UI thread
                        runOnUiThread(() -> Toast.makeText(getApplicationContext(), "Device failed to authorize", Toast.LENGTH_LONG).show());
                    }
                }

                //if the user tries to enter a PayG token
                else if (descriptorName.equals("pc") && key.equals("tkn")) {
                    Log.i(TAG, "PayG token entry request");
                    if (!isAuthorized) {
                        //make toast on UI thread
                        runOnUiThread(() -> Toast.makeText(getApplicationContext(), "Device not authorized", Toast.LENGTH_LONG).show());
                        return;
                    }

                    //generate random number btn 1000 and 9999
                    Random rand = new Random();
                    int randNum = rand.nextInt(8999) + 1000;

                    //update PayG credit remaining (re)
                    prefs.edit().putInt("glb_re", randNum).apply();
                }
                //provisioning
                else if (descriptorName.equals("dcfg")) {
                    if (!isAuthorized) {
                        //make toast on UI thread
                        runOnUiThread(() -> Toast.makeText(getApplicationContext(), "Device not authorized", Toast.LENGTH_LONG).show());
                        return;
                    }

                    if (key.equals("did")) {
                        //update device id
                        prefs.edit().putInt("glb_did", (Integer) jsonObject.get(key)).apply();
                    }
                    if (key.equals("dsc")) {
                        //update device provisioning status
                        prefs.edit().putString("dcfg_dsc", jsonObject.get(key).toString()).apply();
                    }
                    if (key.equals("sat")) {
                        //update device access token
                        prefs.edit().putString("glb_sat", jsonObject.get(key).toString()).apply();
                    }

                    prefs.edit().putInt("glb_pst", 7).apply();
                }

                //update data on local storage (shared prefs)
                prefs.edit().putString(descriptorName + "_" + key, jsonObject.get(key).toString()).apply();

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }

    @SuppressLint("MissingPermission")
    private void updateConnectedDevicesStatus() {
        List<BluetoothDevice> devices = bluetoothManager.getConnectedDevices(BluetoothProfile.GATT_SERVER);
        System.out.println("connected devices: " + devices.size());
        //run on UI thread
        runOnUiThread(() -> connectedDevices.setText(String.valueOf(devices.size())));

    }

    public int writeCharacteristic(BluetoothGattCharacteristic characteristic, int offset, byte[] value) {
        if (offset != 0) {
            return BluetoothGatt.GATT_INVALID_OFFSET;
        }
        if (value.length != 1) {
            return BluetoothGatt.GATT_INVALID_ATTRIBUTE_LENGTH;
        }
        //characteristic.setValue(value);
        return BluetoothGatt.GATT_SUCCESS;
    }

    public void notificationsEnabled(BluetoothGattCharacteristic characteristic, boolean indicate) {
        //todo: change this UUID to the one that has notifications enabled
        //if (characteristic.getUuid() != PC_CHARACTERISTIC_UUID) {
        //    return;
        //}
        //if (indicate) {
        //    Log.d(TAG, "Indications enabled");
        //    runOnUiThread(() -> Toast.makeText(getApplicationContext(), "Indications enabled", Toast.LENGTH_LONG).show());
        //}
    }

    public void notificationsDisabled(BluetoothGattCharacteristic characteristic) {
        //todo: change this UUID to the one that has notifications enabled

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        switch (requestCode) {
            case REQUEST_BLUETOOTH_CONNECT_PERMISSION:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // Permission granted.
                    //make toast
                    Toast.makeText(this, "Bluetooth Connect Permission Granted", Toast.LENGTH_SHORT).show();
                } else {
                    // User refused to grant permission.
                    Toast.makeText(this, "Bluetooth Connect Permission Not Granted", Toast.LENGTH_SHORT).show();
                }
                break;
            case REQUEST_BLUETOOTH_ADVERTISE_PERMISSION:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // Permission granted.
                    //make toast
                    Toast.makeText(this, "Bluetooth Advertise Permission Granted", Toast.LENGTH_SHORT).show();
                } else {
                    // User refused to grant permission.
                    Toast.makeText(this, "Bluetooth Advertise Permission Not Granted", Toast.LENGTH_SHORT).show();
                }
                break;
        }

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == RESULT_OK) {
                if (!bluetoothAdapter.isMultipleAdvertisementSupported()) {
                    Toast.makeText(this, "Advertising not supported", Toast.LENGTH_LONG).show();
                    Log.e(TAG, "Advertising not supported");
                }
                onStart();

            } else {
                //TODO: UX for asking the user to activate bt
                Toast.makeText(this, "Bluetooth not enabled", Toast.LENGTH_LONG).show();
                Log.e(TAG, "Bluetooth not enabled");
                finish();
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        bleSupportValue = findViewById(R.id.bleSupportValueTextView);
        connectedDevices = findViewById(R.id.connectedDevicesTextView);
        advtButton = findViewById(R.id.advBtn);
        stopAdvtButton = findViewById(R.id.stopAdvBtn);
        rstButton = findViewById(R.id.rstBtn);
        advtStatus = findViewById(R.id.advtStatusTextView);
        deviceName = findViewById(R.id.deviceNameValueTextView);
        deviceSerialNo = findViewById(R.id.deviceNumberValueTextView);
        Toolbar myToolbar = findViewById(R.id.toolbar);
        setSupportActionBar(myToolbar);

        prefs = PreferenceManager.getDefaultSharedPreferences(this);

        listener = (prefs, key) -> {
            System.out.println("Preferences changed: " + key);
            AirLinkData.setData(this, prefs);
            setDeviceValues();
        };

        prefs.registerOnSharedPreferenceChangeListener(listener);

        AirLinkData.setData(this, prefs);

        // Register for broadcasts on BluetoothAdapter state change
        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(receiver, filter);

        bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();

        //get the service object instance
        service = new Service();

        //set manufacturer data
        manufacturerSpecificData = new AirLinkData().getAdvertData().EncodeToBytes();

        //Get Advertiser
        bluetoothLeAdvertiser = bluetoothAdapter.getBluetoothLeAdvertiser();

        //Configure advertiser
        settings = new AdvertiseSettings.Builder()
                .setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_BALANCED)
                .setTxPowerLevel(AdvertiseSettings.ADVERTISE_TX_POWER_MEDIUM)
                .setConnectable(true)
                .build();

        advertiseData = new AdvertiseData.Builder()
                .setIncludeTxPowerLevel(true)
                .addManufacturerData(0x0059, manufacturerSpecificData)
                .build();

        //configure advert packet
        advertiseDataScanResponse = new AdvertiseData
                .Builder()
                .setIncludeDeviceName(true)
                .build();

        //set default data only once
        if (!prefs.getBoolean("firstTime", false)) {
            // run your one time code
            setDefaultData();
            prefs.edit().putBoolean("firstTime", true).apply();
        }

        rstButton.setOnClickListener(v -> {
            setDefaultData();
        });

        advtButton.setOnClickListener(v -> {
            reAdvertise();
        });

        stopAdvtButton.setOnClickListener(v -> {
            onStop();
        });

    }

    @Override
    protected void onStart() {
        super.onStart();
        resetStatusViews();
        // If the user disabled Bluetooth when the app was in the background,
        // openGattServer() will return null.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(
                        this,
                        new String[]{Manifest.permission.BLUETOOTH_CONNECT},
                        REQUEST_BLUETOOTH_CONNECT_PERMISSION);
            }
        }
        bluetoothGattServer = bluetoothManager.openGattServer(this, bluetoothGattServerCallback);

        if (bluetoothGattServer == null) {
            Log.e(TAG, "Unable to create GATT server");
            ensureBleFeaturesAvailable();
            return;
        }
        Log.i(TAG, "Bluetooth GATT server opened");

        //clear all services
        bluetoothGattServer.clearServices();

        //add the first service
        bluetoothGattServer.addService(service.getDeviceDiscoveryGattService());

        //add delay to wait for onServiceCallback
        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        //add another service
        bluetoothGattServer.addService(service.getDeviceConfigService());

        //generate random number btn 1000 and 9999
        Random rand = new Random();
        int thi = rand.nextInt(5) + 2;

        //update PayG credit remaining (re)
        prefs.edit().putInt("glb_thi", thi).apply();

        if (bluetoothAdapter.isMultipleAdvertisementSupported()) {
            bleSupportValue.setText(R.string.supported);

            if(!bluetoothAdapter.isEnabled()) {
                ensureBleFeaturesAvailable();
            }
            bluetoothAdapter.setName(DEVICE_NAME);
            deviceName.setText(bluetoothAdapter.getName());
            bluetoothLeAdvertiser = bluetoothAdapter.getBluetoothLeAdvertiser();

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADVERTISE) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(
                            this,
                            new String[]{Manifest.permission.BLUETOOTH_ADVERTISE},
                            REQUEST_BLUETOOTH_ADVERTISE_PERMISSION);
                }
            }
            bluetoothLeAdvertiser.startAdvertising(settings, advertiseData, advertiseDataScanResponse, advertiseCallback);
            Log.i(TAG, "Advertising started");
        } else {
            bleSupportValue.setText(R.string.not_supported);
            Log.e(TAG, "BLE advertising not supported");
        }

    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop");
        if (bluetoothGattServer != null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADVERTISE) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(
                            this,
                            new String[]{Manifest.permission.BLUETOOTH_ADVERTISE},
                            REQUEST_BLUETOOTH_ADVERTISE_PERMISSION);
                }
            }
            bluetoothGattServer.close();
        }

        bluetoothGattServer = null;

        if (bluetoothLeAdvertiser != null) {
            bluetoothLeAdvertiser.stopAdvertising(advertiseCallback);
        }
        resetStatusViews();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Unregister broadcast listeners
        unregisterReceiver(receiver);
    }

    private void ensureBleFeaturesAvailable() {
        Log.i(TAG, "ensureBleFeaturesAvailable called");
        if (bluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth not supported", Toast.LENGTH_LONG).show();
            Log.e(TAG, "Bluetooth not supported");
            finish();
        } else if (!bluetoothAdapter.isEnabled()) {
            // Make sure bluetooth is enabled.
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(
                            this,
                            new String[]{Manifest.permission.BLUETOOTH_CONNECT},
                            REQUEST_BLUETOOTH_CONNECT_PERMISSION);
                }
            }
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    private void setDefaultData() {
        onStop();
        SharedPreferences.Editor edit = prefs.edit();
        //generate random number
        Random rand = new Random();
        int did = rand.nextInt(9999) + 1000;

        //global resources
        edit.putString("glb_sat", "92WwT5ZYXtKuUtZrJZAs"); //todo: this is the default auth token. consider changing and saving it to a secure storage
        edit.putInt("glb_pst", 1);
        edit.putInt("glb_re", 1);
        edit.putInt("glb_did", did);

        //advert resources
        edit.putInt("adv_rv", 0);
        edit.putInt("adv_ft", 0);
        edit.putInt("adv_gts", 1630585957);
        edit.putInt("adv_fv", 0);
        edit.putInt("adv_cr", 7050);
        edit.putString("adv_pu", "m");

        edit.apply();

        //add delay to allow data to be saved
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        manufacturerSpecificData = new AirLinkData().getAdvertData().EncodeToBytes();

        System.out.println("Default data set: " + Helper.cborToString(manufacturerSpecificData));

        onStart();

    }

    public void reAdvertise() {
        onStop();
        onStart();
    }

    private void getData() {

        HashMap<BluetoothGattCharacteristic, List<BluetoothGattDescriptor>> resources = new HashMap<>();

        List<BluetoothGattCharacteristic> characteristicList = new ArrayList<>();

        if (bluetoothGattServer == null) {
            Log.e(TAG, "bluetoothGattServer is null");
        }

        List<BluetoothGattService> services = bluetoothGattServer.getServices();
        for (BluetoothGattService service : services) {
            List<BluetoothGattCharacteristic> characteristics = service.getCharacteristics();

            characteristicList.addAll(characteristics);
        }

        for (BluetoothGattCharacteristic characteristic : characteristicList) {
            resources.put(characteristic, characteristic.getDescriptors());
        }

        expandableListView = findViewById(R.id.expandableListView);
        expandableListDetail = resources;
        expandableListTitle = new ArrayList<>(expandableListDetail.keySet());
        expandableListAdapter = new CustomExpandableListAdapter(this, expandableListTitle, expandableListDetail);
        expandableListView.setAdapter(expandableListAdapter);

        expandableListView.setOnChildClickListener((parent, v, groupPosition, childPosition, id) -> {

            //final BluetoothGattDescriptor descriptor = expandableListDetail.get(expandableListTitle.get(groupPosition)).get(childPosition);
            //final BluetoothGattCharacteristic characteristic = expandableListTitle.get(groupPosition);


            //Toast.makeText(getApplicationContext(), expandableListTitle.get(groupPosition) + " -> " + expandableListDetail.get(expandableListTitle.get(groupPosition)).get(childPosition), Toast.LENGTH_SHORT).show();
            return false;
        });

    }

    private void setDeviceValues() {
        deviceSerialNo.setText(String.valueOf(prefs.getInt("glb_did", 0)));

    }

    private void decrementThi() {
        //decrements the value of thi in shared prefs
        int thi = prefs.getInt("glb_thi", 0);
        int decrementedThi = 0;
        if (thi > 0) {
            decrementedThi = thi - 1;
        }
        prefs.edit().putInt("glb_thi", decrementedThi).apply();
    }

    private void disconnectFromDevices() {
        Log.d(TAG, "Disconnecting devices...");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(
                        this,
                        new String[]{Manifest.permission.BLUETOOTH_CONNECT},
                        REQUEST_BLUETOOTH_CONNECT_PERMISSION);
            }
        }
        for (BluetoothDevice device : bluetoothManager.getConnectedDevices(
                BluetoothGattServer.GATT)) {
            Log.d(TAG, "Devices: " + device.getAddress() + " " + device.getName());
            bluetoothGattServer.cancelConnection(device);
        }
    }

    private void resetStatusViews() {
        isAdvertising = false;
        advtStatus.setText(R.string.status_notAdvertising);
        Log.i(TAG, "resetStatusViews called");
        updateConnectedDevicesStatus();
    }




}
