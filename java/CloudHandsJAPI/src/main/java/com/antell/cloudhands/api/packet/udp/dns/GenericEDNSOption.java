package com.antell.cloudhands.api.packet.udp.dns;



import com.antell.cloudhands.api.utils.Base16;

import java.io.IOException;

/**
 * An EDNSOption with no internal structure.
 * 
 */
public class GenericEDNSOption extends EDNSOption {


	public GenericEDNSOption(int code) {
	super(code);
}



	@Override
	public void optionRead(DNSDataInput in) throws IOException {

	}

	String optionToString() {
		return "<" + Base16.toString(getData()) + ">";

	}

}
