package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.Arrays;

/**
 * DNS extension options, as described in RFC 2671.  The rdata of an OPT record
 * is defined as a list of options; this represents a single option.
 * 
 */
public abstract class EDNSOption {

	private byte[] data;
	private final int code;

	public static class Code {

		private Code() {}


		/** Name Server Identifier, RFC 5001 */

		public final static int NSID = 3;


		/** Client Subnet, defined in draft-vandergaast-edns-client-subnet-00 */

		public final static int CLIENT_SUBNET = 20730;


		private static Mnemonic codes = new Mnemonic("EDNS Option Codes",
						     Mnemonic.CASE_UPPER);


		static {

			codes.setMaximum(0xFFFF);

			codes.setPrefix("CODE");

			codes.setNumericAllowed(true);


			codes.add(NSID, "NSID");

			codes.add(CLIENT_SUBNET, "CLIENT_SUBNET");

		}


		/**

		 * Converts an EDNS Option Code into its textual representation

		 */

		public static String string(int code) {
		return codes.getText(code);
	}



		/**
		 * Converts a textual representation of an EDNS Option Code into its
		 * numeric value.
		 * @param s The textual representation of the option code
	 	* @return The option code, or -1 on error.
	 	*/
		public static int
		value(String s) {
		return codes.getValue(s);
	}
	}



	/**
 	*
 	* Creates an option with the given option code and data.
 	*/
	public EDNSOption(int code) {
	this.code = Record.checkU16("code", code);
}

	public String toString() {
			StringBuffer sb = new StringBuffer();

			sb.append("{");
			sb.append(EDNSOption.Code.string(code));
			sb.append(": ");
			sb.append(optionToString());
			sb.append("}");

			return sb.toString();
	}

/**
 * Returns the EDNS Option's code.
 *
 * @return the option code
 */
	public int getCode() {
	return code;
}

	byte [] getData() {
		return data;
	}

	/**
 	* Converts the wire format of an EDNS Option (the option data only) into the
 	* type-specific format.
 	* @param in The input Stream.
 	*/

	abstract void optionRead(DNSDataInput in) throws IOException;

	/**
 	* Converts the wire format of an EDNS Option (including code and length) into
 	* the type-specific format.
 	*/
	public static EDNSOption build(DataInput in) throws IOException {
		int code, length;

		code = in.readUnsignedShort();

		byte[] data = Text.readBytes(in,2);
		length = data.length;

		DNSDataInput din = new BasicDNSDataInput(data);

		if (din.remaining() < length)
			throw new ParseException("truncated option");
		int save = din.saveActive();
		din.setActive(length);
		EDNSOption option;
		switch (code) {
		case Code.NSID:
			option = new NSIDOption();
			break;
		case Code.CLIENT_SUBNET:
			option = new ClientSubnetOption();
			break;
		default:
			option = new GenericEDNSOption(code);
			break;
	}

		option.setData(data);
		option.optionRead(din);
		din.restoreActive(save);
		return option;
	}

	public static EDNSOption build(MessageUnpacker unpacker) throws IOException {
		int code, length;

		MessagePackUtil.parseMapHeader(unpacker,false);
		code =MessagePackUtil.parseInt(unpacker) ;

		byte[] data = MessagePackUtil.parseBin(unpacker);
		length = data.length;

		DNSDataInput din = new BasicDNSDataInput(data);

		if (din.remaining() < length)
			throw new ParseException("truncated option");
		int save = din.saveActive();
		din.setActive(length);
		EDNSOption option;
		switch (code) {
			case Code.NSID:
				option = new NSIDOption();
				break;
			case Code.CLIENT_SUBNET:
				option = new ClientSubnetOption();
				break;
			default:
				option = new GenericEDNSOption(code);
				break;
		}

		option.setData(data);
		option.optionRead(din);
		din.restoreActive(save);
		return option;
	}

	public void setData(byte[] data) {
		this.data = data;
	}

	/**

	 * Determines if two EDNS Options are identical.

	 * @param arg The option to compare to

	 * @return true if the options are equal, false otherwise.

	 */

	public boolean equals(Object arg) {

		if (arg == null || !(arg instanceof EDNSOption))
			return false;


		EDNSOption opt = (EDNSOption) arg;


		if (code != opt.code)

			return false;


		return Arrays.equals(getData(), opt.getData());
	}


	/**
 	* Generates a hash code based on the EDNS Option's data.
 	*/

	public int hashCode() {
		byte [] array = getData();
		int hashval = 0;
		for (int i = 0; i < array.length; i++)
			hashval += ((hashval << 3) + (array[i] & 0xFF));
		return hashval;

	}

	abstract String optionToString();

}
