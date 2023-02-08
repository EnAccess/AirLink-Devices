package com.simusolar.airlinkblesim;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.content.Context;
import android.graphics.Typeface;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.TextView;

import java.util.HashMap;
import java.util.List;

public class CustomExpandableListAdapter extends BaseExpandableListAdapter {

    private final Context context;
    private final List<BluetoothGattCharacteristic> expandableListTitle;
    private final HashMap<BluetoothGattCharacteristic, List<BluetoothGattDescriptor>> expandableListDetail;

    public CustomExpandableListAdapter(Context context, List<BluetoothGattCharacteristic> expandableListTitle, HashMap<BluetoothGattCharacteristic, List<BluetoothGattDescriptor>> expandableListDetail) {
        this.context = context;
        this.expandableListTitle = expandableListTitle;
        this.expandableListDetail = expandableListDetail;
    }

    @Override
    public Object getChild(int listPosition, int expandedListPosition) {
        return this.expandableListDetail.get(this.expandableListTitle.get(listPosition)).get(expandedListPosition);
    }

    @Override
    public long getChildId(int listPosition, int expandedListPosition) {
        return expandedListPosition;
    }

    @Override
    public View getChildView(int listPosition, final int expandedListPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        BluetoothGattCharacteristic characteristic = (BluetoothGattCharacteristic) getGroup(listPosition);
        if (convertView == null) {
            LayoutInflater layoutInflater = (LayoutInflater) this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = layoutInflater.inflate(R.layout.list_item, null);
        }
        TextView uuidTextView = convertView.findViewById(R.id.uuidListItem);
        uuidTextView.setText("UUID: " + characteristic.getUuid().toString());

        TextView hexTextView = convertView.findViewById(R.id.hexListItem);
        TextView jsonTextView = convertView.findViewById(R.id.jsonListItem);

        AirLinkData airLinkData = new AirLinkData();
        String dataInHex;
        String dataInJson;

        switch (characteristic.getUuid().toString()) {
            case Characteristic.PC_CHARACTERISTIC_UUID:
                dataInHex = Helper.bytesToHex(airLinkData.getPcMap().EncodeToBytes());
                dataInJson = Helper.cborToString(airLinkData.getPcMap().EncodeToBytes());
                break;
            case Characteristic.BATT_CHARACTERISTIC_UUID:
                dataInHex = Helper.bytesToHex(airLinkData.getBattMap().EncodeToBytes());
                dataInJson = Helper.cborToString(airLinkData.getBattMap().EncodeToBytes());
                break;
            case Characteristic.TEMP_CHARACTERISTIC_UUID:
                dataInHex = Helper.bytesToHex(airLinkData.getTempMap().EncodeToBytes());
                dataInJson = Helper.cborToString(airLinkData.getTempMap().EncodeToBytes());
                break;
            case Characteristic.CCFG_CHARACTERISTIC_UUID:
                dataInHex = Helper.bytesToHex(airLinkData.getCcfgMap().EncodeToBytes());
                dataInJson = Helper.cborToString(airLinkData.getCcfgMap().EncodeToBytes());
                break;
            case Characteristic.DCFG_CHARACTERISTIC_UUID:
                dataInHex = Helper.bytesToHex(airLinkData.getDcfgMap().EncodeToBytes());
                dataInJson = Helper.cborToString(airLinkData.getDcfgMap().EncodeToBytes());
                break;
            case Characteristic.DFU_CHARACTERISTIC_UUID:
                dataInHex = Helper.bytesToHex(airLinkData.getDfuMap().EncodeToBytes());
                dataInJson = Helper.cborToString(airLinkData.getDfuMap().EncodeToBytes());
                break;
            default:
                Log.e("ERROR", "Unknown characteristic: " + characteristic.getUuid());
                dataInHex = "Error decoding data";
                dataInJson = dataInHex;
                break;

        }

        hexTextView.setText("Data (Hex): " + dataInHex);
        jsonTextView.setText("Data (JSON): " + dataInJson);


        return convertView;
    }

    @Override
    public int getChildrenCount(int listPosition) {
        return this.expandableListDetail.get(this.expandableListTitle.get(listPosition)).size();
    }

    @Override
    public Object getGroup(int listPosition) {
        return this.expandableListTitle.get(listPosition);
    }

    @Override
    public int getGroupCount() {
        return this.expandableListTitle.size();
    }

    @Override
    public long getGroupId(int listPosition) {
        return listPosition;
    }

    @Override
    public View getGroupView(int listPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        BluetoothGattCharacteristic characteristic = (BluetoothGattCharacteristic) getGroup(listPosition);
        if (convertView == null) {
            LayoutInflater layoutInflater = (LayoutInflater) this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = layoutInflater.inflate(R.layout.list_group, null);
        }
        TextView listTitleTextView = convertView.findViewById(R.id.listTitle);
        listTitleTextView.setTypeface(null, Typeface.BOLD);

        String title;
        if(characteristic.getUuid().toString().equals(Characteristic.DFU_CHARACTERISTIC_UUID)) {
            title = new String(characteristic.getDescriptors().get(1).getValue()).toUpperCase();
        }
        else {
            title = new String(characteristic.getDescriptors().get(0).getValue()).toUpperCase();
        }

        listTitleTextView.setText(title);

        return convertView;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public boolean isChildSelectable(int listPosition, int expandedListPosition) {
        return true;
    }
}
