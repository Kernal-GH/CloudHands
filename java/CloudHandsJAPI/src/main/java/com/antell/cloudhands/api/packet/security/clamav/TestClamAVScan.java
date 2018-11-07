package com.antell.cloudhands.api.packet.security.clamav;

public class TestClamAVScan {


    public static void main(String[] args){

        ClamAVScan scan = new ClamAVScan();

        if(args.length == 0){

            System.out.println("Please give files checked!");
            System.exit(-1);
        }

        if(scan.init("") == -1){
            System.out.println("Cannot init ClamAV engine!");
            System.exit(-1);
        }

        for(String fname:args){

            String vname = scan.scanFile(fname,ClamAVConstants.CL_SCAN_STDOPT);

            System.out.println("check result:{fname:"+fname+",virusName:"+vname+"}");
        }
    }

}
