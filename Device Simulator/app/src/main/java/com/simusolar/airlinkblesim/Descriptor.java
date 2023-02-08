package com.simusolar.airlinkblesim;

import android.bluetooth.BluetoothGattDescriptor;

import java.nio.charset.StandardCharsets;
import java.util.UUID;

public class Descriptor {

    public static final UUID CHARACTERISTIC_USER_DESCRIPTION_UUID = UUID.fromString("00002901-0000-1000-8000-00805f9b34fb");
    public static final UUID CLIENT_CHARACTERISTIC_CONFIGURATION_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    private static final String PC_DESCRIPTOR_VALUE = "pc";
    private static final String BATT_DESCRIPTOR_VALUE = "batt";
    private static final String TEMP_DESCRIPTOR_VALUE = "temp";
    private static final String CCFG_DESCRIPTOR_VALUE = "ccfg";
    private static final String DCFG_DESCRIPTOR_VALUE = "dcfg";
    private static final String DFU_DESCRIPTOR_VALUE = "dfu";

    private BluetoothGattDescriptor pcDescriptor;
    private BluetoothGattDescriptor battDescriptor;
    private BluetoothGattDescriptor tempDescriptor;
    private BluetoothGattDescriptor ccfgDescriptor;
    private BluetoothGattDescriptor dcfgDescriptor;
    private BluetoothGattDescriptor dfuClientDescriptor;
    private BluetoothGattDescriptor dfuUserDescriptor;

    public BluetoothGattDescriptor getBattDescriptor() {
        return battDescriptor;
    }

    public BluetoothGattDescriptor getTempDescriptor() {
        return tempDescriptor;
    }

    public BluetoothGattDescriptor getCcfgDescriptor() {
        return ccfgDescriptor;
    }

    public BluetoothGattDescriptor getDcfgDescriptor() {
        return dcfgDescriptor;
    }

    public BluetoothGattDescriptor getDfuClientDescriptor() {
        return dfuClientDescriptor;
    }

    public BluetoothGattDescriptor getDfuUserDescriptor() {
        return dfuUserDescriptor;
    }

    public BluetoothGattDescriptor getPcDescriptor() {
        return pcDescriptor;
    }

    public Descriptor() {
        init();
    }

    private void init() {

        //initializing descriptor for the `pc` characteristic
        pcDescriptor = new BluetoothGattDescriptor(CHARACTERISTIC_USER_DESCRIPTION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        pcDescriptor.setValue(PC_DESCRIPTOR_VALUE.getBytes(StandardCharsets.UTF_8));

        //initializing descriptor for the `batt` characteristic
        battDescriptor = new BluetoothGattDescriptor(CHARACTERISTIC_USER_DESCRIPTION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        battDescriptor.setValue(BATT_DESCRIPTOR_VALUE.getBytes(StandardCharsets.UTF_8));

        //initializing descriptor for the `temp` characteristic
        tempDescriptor = new BluetoothGattDescriptor(CHARACTERISTIC_USER_DESCRIPTION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        tempDescriptor.setValue(TEMP_DESCRIPTOR_VALUE.getBytes(StandardCharsets.UTF_8));

        //initializing descriptor for the `ccfg` characteristic
        ccfgDescriptor = new BluetoothGattDescriptor(CHARACTERISTIC_USER_DESCRIPTION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        ccfgDescriptor.setValue(CCFG_DESCRIPTOR_VALUE.getBytes(StandardCharsets.UTF_8));

        //initializing descriptor for the `dcfg` characteristic
        dcfgDescriptor = new BluetoothGattDescriptor(CHARACTERISTIC_USER_DESCRIPTION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        dcfgDescriptor.setValue(DCFG_DESCRIPTOR_VALUE.getBytes(StandardCharsets.UTF_8));

        //initializing user descriptor for the `dfu` characteristic
        dfuUserDescriptor = new BluetoothGattDescriptor(CHARACTERISTIC_USER_DESCRIPTION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        dfuUserDescriptor.setValue(DFU_DESCRIPTOR_VALUE.getBytes(StandardCharsets.UTF_8));

        //initializing client descriptor for the `dfu` characteristic
        dfuClientDescriptor = new BluetoothGattDescriptor(CLIENT_CHARACTERISTIC_CONFIGURATION_UUID, BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        dfuClientDescriptor.setValue(new byte[]{0, 0});

    }
}
