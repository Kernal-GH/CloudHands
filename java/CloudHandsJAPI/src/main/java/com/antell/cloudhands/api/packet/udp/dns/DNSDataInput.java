package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Created by dell on 2018/6/14.
 */
public interface DNSDataInput {

    /*Return the current position*/
    int current();

    /*Return the number of bytes that can be read from this stream before
    * reaching the end*/
    int remaining();

    /*Marks the following bytes in the stream as active
    * @param len the Number of bytes in the active region
    * @throw IllegalArgumentException The number of bytes in the active region
    * is longer than the remaining of the input*/
    void setActive(int len);

    /** Clears the active region of the string.  Further operations are not
     * restricted to part of the input.
     */
    void clearActive();

    /**
     * Returns the position of the end of the current active region.
     */
    int saveActive();

    /**
     * Restores the previously set active region.  This differs from setActive() in
     * that restoreActive() takes an absolute position, and setActive takes an
     * offset from the current location.
     * @param pos The end of the active region.
     */
    void restoreActive(int pos);

    /**
     * Resets the current position of the input stream to the specified index,
     * and clears the active region.
     * @param index The position to continue parsing at.
     * @throws IllegalArgumentException The index is not within the input.
     */
    void jump(int index);

    /**
     * Saves the current state of the input stream.  Both the current position and
     * the end of the active region are saved.
     * @throws IllegalArgumentException The index is not within the input.
     */
    void save();

    /**
     * Restores the input stream to its state before the call to {@link #save}.
     */
    void restore();

    /**
     * Reads an unsigned 8 bit value from the stream, as an int.
     * @return An unsigned 8 bit value.
     * @throws ParseException The end of the stream was reached.
     */
    int readU8() throws ParseException;

    /**
     * Reads an unsigned 16 bit value from the stream, as an int.
     * @return An unsigned 16 bit value.
     * @throws ParseException The end of the stream was reached.
     */
    int readU16() throws ParseException;

    /**
     * Reads an unsigned 32 bit value from the stream, as a long.
     * @return An unsigned 32 bit value.
     * @throws ParseException The end of the stream was reached.
     */
    long readU32() throws ParseException;

    /**
     * Reads a byte array of a specified length from the stream into an existing
     * array.
     * @param b The array to read into.
     * @param off The offset of the array to start copying data into.
     * @param len The number of bytes to copy.
     * @throws ParseException The end of the stream was reached.
     */
    void readByteArray(byte[] b, int off, int len) throws ParseException;

    /**
     * Reads a byte array of a specified length from the stream.
     * @return The byte array.
     * @throws ParseException The end of the stream was reached.
     */
    byte [] readByteArray(int len) throws ParseException;

    /**
     * Reads a byte array consisting of the remainder of the stream (or the
     * active region, if one is set.
     * @return The byte array.
     */
    byte [] readByteArray();

    /**
     * Reads a counted string from the stream.  A counted string is a one byte
     * value indicating string length, followed by bytes of data.
     * @return A byte array containing the string.
     * @throws ParseException The end of the stream was reached.
     */
    byte [] readCountedString() throws ParseException;

}
