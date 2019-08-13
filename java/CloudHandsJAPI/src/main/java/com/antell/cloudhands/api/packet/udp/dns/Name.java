
package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import com.google.common.base.Preconditions;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * A representation of a domain name.  It may either be absolute (fully
 * qualified) or relative.
 */

public class Name implements Comparable {

    /* The name data */
    private String name;

    private int length;
    private int labels;

    /* Precomputed hashcode. */
    private int hashcode;


    public Name(DataInput in) throws IOException {

        labels = in.readUnsignedShort();
        name = Text.readString(in,2);
        length = name.length();

    }

    public Name(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==2,"Invalid msgpack packet of udp session name entry:"+n);

        labels = MessagePackUtil.parseInt(unpacker);
        name = MessagePackUtil.parseText(unpacker);
        length = name.length();
    }
    /**
     * Is this name a wildcard?
     */
    public boolean isWild() {
        if (labels == 0)
            return false;
        return (name.charAt(0) ==  (byte)1 && name.charAt(1) == (byte) '*');
    }

    /**
     * Is this name absolute?
     */
    public boolean isAbsolute() {
        if (labels == 0)
            return false;
        return name.charAt(labels - 1) == 0;
    }



    /**
     * Convert the nth label in a Name to a String
     *
     * @param n The label to be converted to a (printable) String.  The first
     *          label is 0.
     */
    public String getLabelString(int n) {

        if(n<0)
            return null;

        String[] splits = name.split(".");

        if(splits == null ||splits.length<=n)
            return null;

        return splits[n];
    }

    /**
     * Are these two Names equivalent?
     */
    @Override
    public boolean equals(Object arg) {
        if (arg == this)
            return true;
        if (arg == null || !(arg instanceof Name))
            return false;
        Name d = (Name) arg;
        if (d.hashcode == 0)
            d.hashCode();
        if (hashcode == 0)
            hashCode();

        if (d.hashcode != hashcode)
            return false;

        if (d.getLabels() != labels)
            return false;

        if(d.getLength()!=length)
            return false;

        String dname = d.getName();

        return name.equalsIgnoreCase(dname);
    }

    /**
     * Computes a hashcode based on the value
     */
    @Override
    public int hashCode() {
        if (hashcode != 0)
            return (hashcode);
        int code = 0;

        String lname = name.toLowerCase();

        for (int i = 0; i < lname.length(); i++)
            code += ((code << 3) + lname.charAt(i));

        hashcode = code;
        return hashcode;
    }

    /**
     * Compares this Name to another Object.
     *
     * @param o The Object to be compared.
     * @return The value 0 if the argument is a name equivalent to this name;
     * a value less than 0 if the argument is less than this name in the canonical
     * ordering, and a value greater than 0 if the argument is greater than this
     * name in the canonical ordering.
     * @throws ClassCastException if the argument is not a Name.
     */
    @Override
    public int compareTo(Object o) {
        Name arg = (Name) o;

        if (this == arg)
            return (0);

        return name.toLowerCase().compareTo(arg.getName().toLowerCase());
    }

    public int getLength() {
        return length;
    }

    public void setLength(int length) {
        this.length = length;
    }

    public int getLabels() {
        return labels;
    }

    public void setLabels(int labels) {
        this.labels = labels;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName(){

        return name;
    }

    public String toString(){

        return name;
    }
}
