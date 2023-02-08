package com.simusolar.airlinkblesim;

import android.bluetooth.BluetoothGattCharacteristic;

import java.util.UUID;

public class Characteristic {

    public static final String PC_CHARACTERISTIC_UUID = "b2ffc2ae-8dc8-11ed-a1eb-0242ac120002";
    public static final String BATT_CHARACTERISTIC_UUID = "b2ffc57e-8dc8-11ed-a1eb-0242ac120002";
    public static final String TEMP_CHARACTERISTIC_UUID = "b2ffc6d2-8dc8-11ed-a1eb-0242ac120002";
    public static final String CCFG_CHARACTERISTIC_UUID = "b2ffc7fe-8dc8-11ed-a1eb-0242ac120002";
    public static final String DCFG_CHARACTERISTIC_UUID = "b2ffcbb4-8dc8-11ed-a1eb-0242ac120002";
    public static final String DFU_CHARACTERISTIC_UUID = "b2ffccea-8dc8-11ed-a1eb-0242ac120002";

    private BluetoothGattCharacteristic pcGattCharacteristic;
    private BluetoothGattCharacteristic battGattCharacteristic;
    private BluetoothGattCharacteristic tempGattCharacteristic;
    private BluetoothGattCharacteristic ccfgGattCharacteristic;
    private BluetoothGattCharacteristic dcfgGattCharacteristic;
    private BluetoothGattCharacteristic dfuGattCharacteristic;

    public BluetoothGattCharacteristic getBattGattCharacteristic() {
        return battGattCharacteristic;
    }

    public BluetoothGattCharacteristic getTempGattCharacteristic() {
        return tempGattCharacteristic;
    }

    public BluetoothGattCharacteristic getCcfgGattCharacteristic() {
        return ccfgGattCharacteristic;
    }

    public BluetoothGattCharacteristic getDcfgGattCharacteristic() {
        return dcfgGattCharacteristic;
    }

    public BluetoothGattCharacteristic getDfuGattCharacteristic() {
        return dfuGattCharacteristic;
    }

    public BluetoothGattCharacteristic getPcGattCharacteristic() {
        return pcGattCharacteristic;
    }

    public Characteristic() {
        init();
    }

    private void init() {

        System.out.println("characteristic.init() called");

        Descriptor descriptor = new Descriptor();
        AirLinkData airLinkData = new AirLinkData();

        //initializing the `pc` characteristic
        pcGattCharacteristic = new BluetoothGattCharacteristic(UUID.fromString(PC_CHARACTERISTIC_UUID),
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);

        //pcGattCharacteristic.setValue(airLinkData.getPcMap().EncodeToBytes());
        //System.out.println("pcGattCharacteristic.Value1: " + Helper.cborToString(pcGattCharacteristic.getValue()));
        //System.out.println("pcGattCharacteristic.Value2: " + airLinkData.getPcMap().ToJSONString());
        pcGattCharacteristic.addDescriptor(descriptor.getPcDescriptor());

        //initializing the `batt` characteristic
        battGattCharacteristic = new BluetoothGattCharacteristic(UUID.fromString(BATT_CHARACTERISTIC_UUID),
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);

        //battGattCharacteristic.setValue(airLinkData.getBattMap().EncodeToBytes());
        battGattCharacteristic.addDescriptor(descriptor.getBattDescriptor());

        //initializing the `temp` characteristic
        tempGattCharacteristic = new BluetoothGattCharacteristic(UUID.fromString(TEMP_CHARACTERISTIC_UUID),
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);

        //tempGattCharacteristic.setValue(airLinkData.getTempMap().EncodeToBytes());
        tempGattCharacteristic.addDescriptor(descriptor.getTempDescriptor());

        //initializing the `ccfg` characteristic
        ccfgGattCharacteristic = new BluetoothGattCharacteristic(UUID.fromString(CCFG_CHARACTERISTIC_UUID),
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);

        //ccfgGattCharacteristic.setValue(airLinkData.getCcfgMap().EncodeToBytes());
        ccfgGattCharacteristic.addDescriptor(descriptor.getCcfgDescriptor());

        //initializing the `dcfg` characteristic
        dcfgGattCharacteristic = new BluetoothGattCharacteristic(UUID.fromString(DCFG_CHARACTERISTIC_UUID),
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);

        //dcfgGattCharacteristic.setValue(airLinkData.getDcfgMap().EncodeToBytes());
        dcfgGattCharacteristic.addDescriptor(descriptor.getDcfgDescriptor());

        //initializing the `dfu` characteristic
        dfuGattCharacteristic = new BluetoothGattCharacteristic(UUID.fromString(DFU_CHARACTERISTIC_UUID),
                BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE | BluetoothGattCharacteristic.PROPERTY_INDICATE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);

        //dfuGattCharacteristic.setValue(airLinkData.getDfuMap().EncodeToBytes());
        dfuGattCharacteristic.addDescriptor(descriptor.getDfuClientDescriptor());
        dfuGattCharacteristic.addDescriptor(descriptor.getDfuUserDescriptor());

    }


}
