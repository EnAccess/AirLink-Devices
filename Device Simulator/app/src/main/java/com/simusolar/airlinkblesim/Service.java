package com.simusolar.airlinkblesim;

import android.bluetooth.BluetoothGattService;

import java.util.UUID;

public class Service {
    private static final UUID DEVICE_DISCOVERY_SERVICE_UUID = UUID.fromString("9df106fc-8dc8-11ed-a1eb-0242ac120002");
    private static final UUID DEVICE_CONFIG_SERVICE_UUID = UUID.fromString("9df109cc-8dc8-11ed-a1eb-0242ac120002");

    private BluetoothGattService deviceDiscoveryGattService;
    private BluetoothGattService deviceConfigGattService;

    Service() {
        init();
    }

    public BluetoothGattService getDeviceDiscoveryGattService() {
        return deviceDiscoveryGattService;
    }

    public BluetoothGattService getDeviceConfigService() {
        return deviceConfigGattService;
    }

    private void init() {
        //create an object of class Characteristic
        Characteristic characteristic = new Characteristic();

        //setting up the device discovery device
        deviceDiscoveryGattService = new BluetoothGattService(DEVICE_DISCOVERY_SERVICE_UUID, BluetoothGattService.SERVICE_TYPE_PRIMARY);
        deviceDiscoveryGattService.addCharacteristic(characteristic.getPcGattCharacteristic());
        deviceDiscoveryGattService.addCharacteristic(characteristic.getBattGattCharacteristic());
        deviceDiscoveryGattService.addCharacteristic(characteristic.getTempGattCharacteristic());

        //setting up the device configuration service
        deviceConfigGattService = new BluetoothGattService(DEVICE_CONFIG_SERVICE_UUID, BluetoothGattService.SERVICE_TYPE_PRIMARY);
        deviceConfigGattService.addCharacteristic(characteristic.getCcfgGattCharacteristic());
        deviceConfigGattService.addCharacteristic(characteristic.getDcfgGattCharacteristic());
        deviceConfigGattService.addCharacteristic(characteristic.getDfuGattCharacteristic());
    }

}
