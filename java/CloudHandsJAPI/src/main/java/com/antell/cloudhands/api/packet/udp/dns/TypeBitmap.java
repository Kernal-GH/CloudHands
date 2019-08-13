package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Routines for deal with the lists of types found in NSEC/NSEC3 records.
 */
import com.antell.cloudhands.api.utils.Text;

import java.io.DataInput;
import java.io.IOException;
import java.util.Iterator;
import java.util.TreeSet;

public final class TypeBitmap  {
    private TreeSet types;

    private TypeBitmap() {
        types = new TreeSet();
    }

    public TypeBitmap(int[] array) {
        this();
        for (int i = 0; i < array.length; i++) {
            Type.check(array[i]);
            types.add(new Integer(array[i]));
        }
    }

    public TypeBitmap(byte[] data) throws ParseException {
        this();
        DNSDataInput din = new BasicDNSDataInput(data);

        int lastbase = -1;
        while (din.remaining() > 0) {
            if (din.remaining() < 2)
                throw new ParseException
                        ("invalid bitmap descriptor");
            int mapbase = din.readU8();
            if (mapbase < lastbase)
                throw new ParseException("invalid ordering");
            int maplength = din.readU8();
            if (maplength > din.remaining())
                throw new ParseException("invalid bitmap");
            for (int i = 0; i < maplength; i++) {
                int current = din.readU8();
                if (current == 0)
                    continue;
                for (int j = 0; j < 8; j++) {
                    if ((current & (1 << (7 - j))) == 0)
                        continue;
                    int typecode = mapbase * 256 + +i * 8 + j;
                    types.add(Mnemonic.toInteger(typecode));
                }
            }
        }

    }


    public int[] toArray() {
        int[] array = new int[types.size()];
        int n = 0;
        for (Iterator it = types.iterator(); it.hasNext(); )
            array[n++] = ((Integer) it.next()).intValue();
        return array;
    }

    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();
        for (Iterator it = types.iterator(); it.hasNext(); ) {
            int t = ((Integer) it.next()).intValue();
            sb.append(Type.string(t));
            if (it.hasNext())
                sb.append(' ');
        }
        return sb.toString();
    }



    public boolean empty() {
        return types.isEmpty();
    }

    public boolean contains(int typecode) {
        return types.contains(Mnemonic.toInteger(typecode));
    }

}
