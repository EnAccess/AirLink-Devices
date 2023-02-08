package com.simusolar.airlinkblesim;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.BatteryManager;

import com.upokecenter.cbor.CBORObject;

import java.util.Random;

public class AirLinkData {

    private static CBORObject advertData;
    private static CBORObject pcMap;
    private static CBORObject battMap;
    private static CBORObject tempMap;
    private static CBORObject ccfgMap;
    private static CBORObject dcfgMap;
    private static CBORObject dfuMap;

    public CBORObject getAdvertData() {
        return advertData;
    }

    public CBORObject getPcMap() {
        return pcMap;
    }

    public CBORObject getBattMap() {
        return battMap;
    }

    public CBORObject getTempMap() {
        return tempMap;
    }

    public CBORObject getCcfgMap() {
        return ccfgMap;
    }

    public CBORObject getDcfgMap() {
        return dcfgMap;
    }

    public CBORObject getDfuMap() {
        return dfuMap;
    }

    //public AirLinkData() {
    //    setData();
    //}

    public static void setData(Context context, SharedPreferences prefs) {
        System.out.println("setData() called");

        //get battery percentage
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent batteryStatus = context.registerReceiver(null, ifilter);

        int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

        float batteryPct = level * 100 / (float)scale;

        //get battery health
        int batteryHealth = batteryStatus.getIntExtra(BatteryManager.EXTRA_HEALTH, -1);
        int chargingStatus = 0;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            chargingStatus = batteryStatus.getIntExtra(String.valueOf(BatteryManager.BATTERY_PROPERTY_STATUS), -1);
        }

        //get current time in epoch format
        long currentTime = System.currentTimeMillis() / 1000;


        pcMap = CBORObject.NewMap()
                .Add("rtr", prefs.getInt("pc_rtr", 65001))
                .Add("rv", prefs.getInt("pc_rv", 1))
                .Add("re", prefs.getInt("glb_re", 0))
                .Add("mo", prefs.getInt("pc_mo", 1))
                .Add("lcr", prefs.getInt("pc_lcr", 7200))
                .Add("pts", prefs.getInt("pc_pts", 0))
                .Add("lts", prefs.getInt("pc_lts", 0))
                .Add("thi", prefs.getInt("glb_thi", 0))
                .Add("tkn", prefs.getString("pc_tkn", "*12345666#"))
                .Add("ts", currentTime);

        battMap = CBORObject.NewMap()
                .Add("rtr", prefs.getInt("batt_rtr", 65009))
                .Add("vb", prefs.getInt("batt_vb", 0))
                .Add("cp", batteryPct)
                .Add("cs", chargingStatus)
                .Add("th",prefs.getInt("batt_th", 10) )
                .Add("lb", prefs.getInt("batt_lb", 0))
                .Add("cmin", prefs.getInt("batt_cmin", 0))
                .Add("cmax", prefs.getInt("batt_cmax", 0))
                .Add("tc", prefs.getInt("batt_tc", 0))
                .Add("qc", prefs.getInt("batt_qc", 0))
                .Add("bh", batteryHealth)
                .Add("thi", prefs.getInt("glb_thi", 0))
                .Add("ts", currentTime);

        tempMap = CBORObject.NewMap()
                .Add("rtr", prefs.getInt("temp_rtr", 65011))
                .Add("temp", prefs.getInt("temp_temp", 0))
                .Add("tmax", prefs.getInt("temp_tmax", 0))
                .Add("tlim", prefs.getInt("temp_tlim", 0))
                .Add("thi", prefs.getInt("glb_thi", 0))
                .Add("ts", currentTime);

        dcfgMap = CBORObject.NewMap()
                .Add("rtr", prefs.getInt("dcfg_rtr", 65003))
                .Add("rv", prefs.getInt("dcfg_rv", 1))
                .Add("did", prefs.getInt("glb_did", 0))
                .Add("pul", prefs.getString("dcfg_pul", "s"))
                .Add("tdf", prefs.getInt("dcfg_tdf", 0))
                .Add("pst", prefs.getInt("glb_pst", 1))
                .Add("df", prefs.getInt("dcfg_df", 0))
                .Add("cut", prefs.getString("dcfg_cut", String.valueOf(currentTime)))
                .Add("opmax", prefs.getInt("dcfg_opmax", 1000));

        ccfgMap = CBORObject.NewMap()
                .Add("rtr", prefs.getInt("ccfg_rtr", 65002))
                .Add("rv", prefs.getInt("ccfg_rtr", 1))
                .Add("cn", prefs.getString("ccfg_cn", "John Doe"))
                .Add("cp", prefs.getString("ccfg_rtr", "123-456-7890"))
                .Add("rid", prefs.getInt("ccfg_rtr", 0))
                .Add("pst", prefs.getInt("glb_pst", 1));

        dfuMap = CBORObject.NewMap()
                .Add("rtr", prefs.getInt("dfu_rtr", 65004))
                .Add("rv", prefs.getInt("dfu_rv", 1))
                .Add("fv", prefs.getInt("dfu_fv", 257))
                .Add("fs", prefs.getInt("dfu_fs", 100000))
                .Add("hv", prefs.getInt("dfu_hv", 1));

        advertData = CBORObject.NewArray()
                .Add(prefs.getInt("adv_rv", 0))
                .Add(prefs.getInt("adv_ft", 0))
                .Add(prefs.getInt("glb_did", 0))
                .Add(prefs.getInt("adv_gts", 0))
                .Add(prefs.getInt("glb_pst", 1))
                .Add(prefs.getInt("adv_fv", 0))
                .Add(prefs.getInt("adv_cr", 7050))
                .Add(prefs.getString("adv_pu", "m"));

    }


}
