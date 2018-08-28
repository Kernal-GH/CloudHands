package com.antell.cloudhands.api.packet.security.clamav;

/**
 * Created by g on 5/23/17.
 */
public class ClamAVConstants {
    public static final int CL_DB_PHISHING = 0x2;
    public static final int CL_DB_PHISHING_URLS = 0x8;
    public static final int CL_DB_PUA = 0x10;
    public static final int CL_DB_CVDNOTMP = 0x20;   /* obsolete */
    public static final int CL_DB_OFFICIAL = 0x40;   /* internal */
    public static final int CL_DB_PUA_MODE = 0x80;
    public static final int CL_DB_PUA_INCLUDE = 0x100;
    public static final int CL_DB_PUA_EXCLUDE = 0x200;
    public static final int CL_DB_COMPILED = 0x400;  /* internal */
    public static final int CL_DB_DIRECTORY = 0x800;  /* internal */
    public static final int CL_DB_OFFICIAL_ONLY = 0x1000;
    public static final int CL_DB_BYTECODE = 0x2000;
    public static final int CL_DB_SIGNED = 0x4000;  /* internal */
    public static final int CL_DB_BYTECODE_UNSIGNED = 0x8000;
    public static final int CL_DB_UNSIGNED = 0x10000; /* internal */
    public static final int CL_DB_BYTECODE_STATS = 0x20000;
    public static final int CL_DB_ENHANCED = 0x40000;
    public static final int CL_DB_PCRE_STATS = 0x80000;
    public static final int CL_DB_YARA_EXCLUDE = 0x100000;
    public static final int CL_DB_YARA_ONLY = 0x200000;

    /* recommended db settings */
    public static final int CL_DB_STDOPT = (CL_DB_PHISHING | CL_DB_PHISHING_URLS | CL_DB_BYTECODE);

    /* scan options */
    public static final int CL_SCAN_RAW = 0x0;
    public static final int CL_SCAN_ARCHIVE = 0x1;
    public static final int CL_SCAN_MAIL = 0x2;
    public static final int CL_SCAN_OLE2 = 0x4;
    public static final int CL_SCAN_BLOCKENCRYPTED = 0x8;
    public static final int CL_SCAN_HTML = 0x10;
    public static final int CL_SCAN_PE = 0x20;
    public static final int CL_SCAN_BLOCKBROKEN = 0x40;
    public static final int CL_SCAN_MAILURL = 0x80; /* ignored */
    public static final int CL_SCAN_BLOCKMAX = 0x100; /* ignored */
    public static final int CL_SCAN_ALGORITHMIC = 0x200;
    public static final int CL_SCAN_PHISHING_BLOCKSSL = 0x800; /* ssl mismatches, not ssl by itself*/
    public static final int CL_SCAN_PHISHING_BLOCKCLOAK = 0x1000;
    public static final int CL_SCAN_ELF = 0x2000;
    public static final int CL_SCAN_PDF = 0x4000;
    public static final int CL_SCAN_STRUCTURED = 0x8000;
    public static final int CL_SCAN_STRUCTURED_SSN_NORMAL = 0x10000;
    public static final int CL_SCAN_STRUCTURED_SSN_STRIPPED = 0x20000;
    public static final int CL_SCAN_PARTIAL_MESSAGE = 0x40000;
    public static final int CL_SCAN_HEURISTIC_PRECEDENCE = 0x80000;
    public static final int CL_SCAN_BLOCKMACROS = 0x100000;
    public static final int CL_SCAN_ALLMATCHES = 0x200000;
    public static final int CL_SCAN_SWF = 0x400000;
    public static final int CL_SCAN_PARTITION_INTXN = 0x800000;
    public static final int CL_SCAN_XMLDOCS = 0x1000000;
    public static final int CL_SCAN_HWP3 = 0x2000000;
    public static final int CL_SCAN_FILE_PROPERTIES = 0x10000000;
    //public static final int UNUSED                        0x20000000
    public static final int CL_SCAN_PERFORMANCE_INFO = 0x40000000; /* collect performance timings */
    public static final int CL_SCAN_INTERNAL_COLLECT_SHA = 0x80000000; /* Enables hash output in sha-collect builds - for internal use only */

    /* recommended scan settings */
    public static final int CL_SCAN_STDOPT = (CL_SCAN_ARCHIVE | CL_SCAN_MAIL | CL_SCAN_OLE2 | CL_SCAN_PDF | CL_SCAN_HTML | CL_SCAN_PE | CL_SCAN_ALGORITHMIC | CL_SCAN_ELF | CL_SCAN_SWF | CL_SCAN_XMLDOCS | CL_SCAN_HWP3);

}
