//--------------------------------------------------------------------------------
//    © Copyright Deepak Prabhakara. 2004. All Rights Reserved.
//--------------------------------------------------------------------------------
//
//  AppSecureApplet.java
//
//
//  This card applet allows the user to store code section of an executable
//  in order to securely protect it and prevent illegal copies from being
//  distributed.
//
//  This card applet is built on the Skeleton.java file.
//
//package com.slb.javacard.AppSecureApplet;
package AppSecureApplet;

import javacard.framework.*;
import javacard.security.*;

// Class Definition

public class AppSecureApplet extends javacard.framework.Applet {

	byte TheBuffer[];	//  This buffer contains the code section on the card

	//  This applet is designed to respond to the following
	//  class of instructions.

	final static byte AppSecureApplet_CLA = (byte)0x85;

	boolean payperuse;

	//  Instruction set for AppSecureApplet

	final static byte SET = (byte)0x10;
	final static byte GET = (byte)0x20;
	final static byte SELECT = (byte) 0xA4;

	// codes of INS byte in the command APDU header
	 final static byte LOADLICENSE = (byte) 0x30;  
	 final static byte Validate = (byte) 0x40;
	 final static byte Encrypt = (byte) 0x50;
	 final static byte PinChange = (byte) 0x60;

	// maximum number of incorrect tries before the PIN is blocked
	 final static byte PinTryLimit =(byte)0x03;

	// maximum size PIN
	 final static byte MaxPinSize =(byte)0x04;

	// status word (SW1-SW2) to signal that the balance becomes negative;
	 final static short SW_LICENSE_EXPIRED = (short) 0x6910;
	 final static short SW_WRONG_PIN = (short) 0x6300;

	/* instance variables declaration */ 
	OwnerPIN pin;
	static byte base_count = (byte) 0x20;
	byte license_count;
	DESKey      key;
	Signature   signature;

	//  Declare your instance variables here
	//  The constructor. 

	private AppSecureApplet(byte buffer[],short offset,byte length) {

		TheBuffer = new byte[100];

		pin = new OwnerPIN(PinTryLimit, MaxPinSize);
		byte [] newpin={(byte)0x00,(byte)0x01,(byte)0x09,(byte)0x05};
		pin.update(newpin,(short)0,(byte)4);
		license_count = (byte) 0;
		key = (DESKey)KeyBuilder.buildKey(KeyBuilder.TYPE_DES_TRANSIENT_RESET,KeyBuilder.LENGTH_DES,false);
		signature = Signature.getInstance(Signature.ALG_DES_MAC8_ISO9797_M1,false);
		signature.init(key,Signature.MODE_SIGN);

		if (buffer[offset] == (byte)0) {
			register();
		}
		else {
			register(buffer, (short)(offset+1) ,(byte)(buffer[offset]));
		}
		
	}

	//  Every applet running JavaCard 2.0 must implement the following
	//  three functions.


	//  You create the one instance of your applet here.

	public static void install(byte buffer[],short offset,byte length) {

		new AppSecureApplet(buffer, offset, length);

	}

	//  This function is called when your applet is selected.

	public boolean select() {

		// reset validation flag in the PIN object to false 
		pin.reset(); 
		
		return true;
	}

	//  The process method dispatches messages to your class methods
	//  depending on the instruction type.

	public void process(APDU apdu) throws ISOException{

		byte buffer[] = apdu.getBuffer();

        // Implement a select handler 
        if (selectingApplet()) {
            ISOException.throwIt(ISO7816.SW_NO_ERROR);
		}
		
		if (buffer[ISO7816.OFFSET_CLA] != AppSecureApplet_CLA) 
				ISOException.throwIt(ISO7816.SW_CLA_NOT_SUPPORTED);
    
        byte ins = buffer[ISO7816.OFFSET_INS];
        
		switch (ins) {

		case SET:
			StoreCodeSection(apdu);
			break;
		case GET:
			RetrieveCodeSection(apdu);
			break;
		case LOADLICENSE:
			loadlicense(apdu);
			break;
		case Validate:
			validate(apdu);
			break;
		case Encrypt:
			encrypt(apdu);
			break;
		case PinChange:
			PinChange(apdu);
			break;			
		default:
			ISOException.throwIt(ISO7816.SW_INS_NOT_SUPPORTED);
		}

	}

	//  Class methods

	//  StoreCodeSection stores the code section on the card.

	private void StoreCodeSection(APDU apdu) {

		byte buffer[] = apdu.getBuffer();

		byte size = (byte)(apdu.setIncomingAndReceive());

		byte index;

		for (index = 0; index < 100; index++) 
			TheBuffer[(byte)(index)] = buffer[(byte)(ISO7816.OFFSET_CDATA + index)];

		return;

	}

	//  RetrieveCodeSection retrieves the code section from the card.
	//

	private void RetrieveCodeSection(APDU apdu) {

		byte buffer[] = apdu.getBuffer(); 

		// access authentication
		if ( ! pin.isValidated() )
		ISOException.throwIt(ISO7816.SW_SECURITY_STATUS_NOT_SATISFIED); 

if(payperuse==true)
{
		if ( (byte)( (byte) license_count - (byte) 1) < (byte) -127 ) 
		ISOException.throwIt(SW_LICENSE_EXPIRED); 

		license_count = (byte) (license_count - (byte) 1);
}

		byte numBytes = buffer[ISO7816.OFFSET_LC];

		apdu.setOutgoing();
		apdu.setOutgoingLength((short)100);
		

		byte index;

		for (index = 0; index < 100; index++) 
			buffer[index] = TheBuffer[(byte)(index)];

		apdu.sendBytes((short)0,(short)100);


		return;
	}

	private void loadlicense(APDU apdu) { 

		byte buffer[] = apdu.getBuffer(); 

		// access authentication
		if ( ! pin.isValidated() )
		ISOException.throwIt (ISO7816.SW_SECURITY_STATUS_NOT_SATISFIED); 

		// indicate that this APDU has incoming data and
		// receive data starting from the offset 
		// ISO7816.OFFSET_CDATA
		byte byteRead = (byte)(apdu.setIncomingAndReceive()); 

		// it is an error if the number of data bytes read does not 
		// match the number in Lc byte
		if (byteRead != (byte) 1) 
		ISOException.throwIt(ISO7816.SW_WRONG_LENGTH); 

		// increase the license_count by the amount specified in the 
		// data field of the command APDU.
		license_count = (byte)(buffer[ISO7816.OFFSET_CDATA]);
		if(license_count==(byte)-127) payperuse=false;
		else payperuse=true; 

		// return successfully
		return; 

	} // end of loadlicense method

	private void validate(APDU apdu) { 

		byte buffer[] = apdu.getBuffer(); 

		// retrieve the PIN data which requires to be valid ated
		// the user interface data is stored in the data field of the APDU
		byte byteRead = (byte)(apdu.setIncomingAndReceive()); 

		// validate user interface and set the validation flag in the user interface
		// object to be true if the validation succeeds.
		// if user interface validation fails, PinException would be 
		// thrown from pin.check() method.
		if (!pin.check(buffer, ISO7816.OFFSET_CDATA, byteRead)) 
			ISOException.throwIt(SW_WRONG_PIN); 

		key.setKey(buffer,(short)0);
	} // end of validate method 
	
	private void  encrypt(APDU apdu) {

		byte buffer[] = apdu.getBuffer(); 

		// get 4 bytes of data and return the encrypted result.
		// access authentication
		if ( ! pin.isValidated() )
		ISOException.throwIt(ISO7816.SW_SECURITY_STATUS_NOT_SATISFIED); 
		
		short byteRead = apdu.setIncomingAndReceive();
		
		signature.sign(buffer,(short)5,byteRead,buffer,(short)0);
		apdu.setOutgoingAndSend((short)0,(short)8);
	}
	
	private void  PinChange(APDU apdu) {
		
		byte buffer[] = apdu.getBuffer(); 

		byte newPIN[] = {(byte)buffer[ISO7816.OFFSET_P1],(byte)buffer[ISO7816.OFFSET_P1],(byte)buffer[ISO7816.OFFSET_P1],(byte)buffer[ISO7816.OFFSET_P1]};
		pin.update(newPIN,(short)0, (byte)4);  
	}
	

}
