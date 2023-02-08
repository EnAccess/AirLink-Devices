package com.simusolar.airlinkblesim;

import android.util.Log;

import com.upokecenter.cbor.CBORObject;

public class Helper {
    public static byte[] hexStringToByteArray(String s) {
        String newString = s.replace(" ", "");
        int len = newString.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    private static final char[] HEX_ARRAY = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        if(bytes == null) {
            return "null";
        }
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = HEX_ARRAY[v >>> 4];
            hexChars[j * 2 + 1] = HEX_ARRAY[v & 0x0F];
        }
        return new String(hexChars);
    }

    public static String cborToString(byte[] bytes) {
        try {
            CBORObject message = CBORObject.DecodeFromBytes(bytes);
            return message.toString();
        } catch (Exception e) {
            Log.e("CBOR Error", e.toString());
            return "Error decoding CBOR Object";
        }

    }
}
