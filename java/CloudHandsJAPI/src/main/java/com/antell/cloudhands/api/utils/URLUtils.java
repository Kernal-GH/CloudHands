package com.antell.cloudhands.api.utils;

/**
 * Created by dell on 2017/4/19.
 */
public final class URLUtils {

    private static final int T_SLASH = 0x01;            /* '/' */
    private static final int T_QUESTION = 0x02;         /* '?' */
    private static final int T_HASH = 0x04;             /* '#' */
    private static final int T_ALPHA = 0x08;        /* 'A' ... 'Z', 'a' ... 'z' */
    private static final int T_SCHEME = 0x10;      /* '0' ... '9', '-', '+', '.'*/

    private static final int T_NUL = 0x80;        /* '\0' */

    private static final int NOTEND_PATH = (T_QUESTION | T_HASH | T_NUL);

    /* Delimiter table for the ASCII character set */
    private static final char uriDelims[] = {
            T_NUL,                      /* 0x00     */
            0,                          /* 0x01     */
            0,                          /* 0x02     */
            0,                          /* 0x03     */
            0,                          /* 0x04     */
            0,                          /* 0x05     */
            0,                          /* 0x06     */
            0,                          /* 0x07     */
            0,                          /* 0x08     */
            0,                          /* 0x09     */
            0,                          /* 0x0a     */
            0,                          /* 0x0b     */
            0,                          /* 0x0c     */
            0,                          /* 0x0d     */
            0,                          /* 0x0e     */
            0,                          /* 0x0f     */
            0,                          /* 0x10     */
            0,                          /* 0x11     */
            0,                          /* 0x12     */
            0,                          /* 0x13     */
            0,                          /* 0x14     */
            0,                          /* 0x15     */
            0,                          /* 0x16     */
            0,                          /* 0x17     */
            0,                          /* 0x18     */
            0,                          /* 0x19     */
            0,                          /* 0x1a     */
            0,                          /* 0x1b     */
            0,                          /* 0x1c     */
            0,                          /* 0x1d     */
            0,                          /* 0x1e     */
            0,                          /* 0x1f     */
            0,                          /* 0x20 ' ' */
            0,                          /* 0x21 '!' */
            0,                          /* 0x22 '"' */
            T_HASH,                     /* 0x23 '#' */
            0,                          /* 0x24 '$' */
            0,                          /* 0x25 '%' */
            0,                          /* 0x26 '&' */
            0,                          /* 0x27 ''' */
            0,                          /* 0x28 '(' */
            0,                          /* 0x29 ')' */
            0,                          /* 0x2a '*' */
            T_SCHEME,                   /* 0x2b '+' */
            0,                          /* 0x2c ',' */
            T_SCHEME,                   /* 0x2d '-' */
            T_SCHEME,                   /* 0x2e '.' */
            T_SLASH,                    /* 0x2f '/' */
            T_SCHEME,                   /* 0x30 '0' */
            T_SCHEME,                   /* 0x31 '1' */
            T_SCHEME,                   /* 0x32 '2' */
            T_SCHEME,                   /* 0x33 '3' */
            T_SCHEME,                   /* 0x34 '4' */
            T_SCHEME,                   /* 0x35 '5' */
            T_SCHEME,                   /* 0x36 '6' */
            T_SCHEME,                   /* 0x37 '7' */
            T_SCHEME,                   /* 0x38 '8' */
            T_SCHEME,                   /* 0x39 '9' */
            0,                          /* 0x3a ':' */
            0,                          /* 0x3b ';' */
            0,                          /* 0x3c '<' */
            0,                          /* 0x3d '=' */
            0,                          /* 0x3e '>' */
            T_QUESTION,                 /* 0x3f '?' */
            0,                          /* 0x40 '@' */
            T_ALPHA,                    /* 0x41 'A' */
            T_ALPHA,                    /* 0x42 'B' */
            T_ALPHA,                    /* 0x43 'C' */
            T_ALPHA,                    /* 0x44 'D' */
            T_ALPHA,                    /* 0x45 'E' */
            T_ALPHA,                    /* 0x46 'F' */
            T_ALPHA,                    /* 0x47 'G' */
            T_ALPHA,                    /* 0x48 'H' */
            T_ALPHA,                    /* 0x49 'I' */
            T_ALPHA,                    /* 0x4a 'J' */
            T_ALPHA,                    /* 0x4b 'K' */
            T_ALPHA,                    /* 0x4c 'L' */
            T_ALPHA,                    /* 0x4d 'M' */
            T_ALPHA,                    /* 0x4e 'N' */
            T_ALPHA,                    /* 0x4f 'O' */
            T_ALPHA,                    /* 0x50 'P' */
            T_ALPHA,                    /* 0x51 'Q' */
            T_ALPHA,                    /* 0x52 'R' */
            T_ALPHA,                    /* 0x53 'S' */
            T_ALPHA,                    /* 0x54 'T' */
            T_ALPHA,                    /* 0x55 'U' */
            T_ALPHA,                    /* 0x56 'V' */
            T_ALPHA,                    /* 0x57 'W' */
            T_ALPHA,                    /* 0x58 'X' */
            T_ALPHA,                    /* 0x59 'Y' */
            T_ALPHA,                    /* 0x5a 'Z' */
            0,                          /* 0x5b '[' */
            0,                          /* 0x5c '\' */
            0,                          /* 0x5d ']' */
            0,                          /* 0x5e '^' */
            0,                          /* 0x5f '_' */
            0,                          /* 0x60 '`' */
            T_ALPHA,                    /* 0x61 'a' */
            T_ALPHA,                    /* 0x62 'b' */
            T_ALPHA,                    /* 0x63 'c' */
            T_ALPHA,                    /* 0x64 'd' */
            T_ALPHA,                    /* 0x65 'e' */
            T_ALPHA,                    /* 0x66 'f' */
            T_ALPHA,                    /* 0x67 'g' */
            T_ALPHA,                    /* 0x68 'h' */
            T_ALPHA,                    /* 0x69 'i' */
            T_ALPHA,                    /* 0x6a 'j' */
            T_ALPHA,                    /* 0x6b 'k' */
            T_ALPHA,                    /* 0x6c 'l' */
            T_ALPHA,                    /* 0x6d 'm' */
            T_ALPHA,                    /* 0x6e 'n' */
            T_ALPHA,                    /* 0x6f 'o' */
            T_ALPHA,                    /* 0x70 'p' */
            T_ALPHA,                    /* 0x71 'q' */
            T_ALPHA,                    /* 0x72 'r' */
            T_ALPHA,                    /* 0x73 's' */
            T_ALPHA,                    /* 0x74 't' */
            T_ALPHA,                    /* 0x75 'u' */
            T_ALPHA,                    /* 0x76 'v' */
            T_ALPHA,                    /* 0x77 'w' */
            T_ALPHA,                    /* 0x78 'x' */
            T_ALPHA,                    /* 0x79 'y' */
            T_ALPHA,                    /* 0x7a 'z' */
            0,                          /* 0x7b '{' */
            0,                          /* 0x7c '|' */
            0,                          /* 0x7d '}' */
            0,                          /* 0x7e '~' */
            0,                          /* 0x7f     */
            0,                          /* 0x80     */
            0,                          /* 0x81     */
            0,                          /* 0x82     */
            0,                          /* 0x83     */
            0,                          /* 0x84     */
            0,                          /* 0x85     */
            0,                          /* 0x86     */
            0,                          /* 0x87     */
            0,                          /* 0x88     */
            0,                          /* 0x89     */
            0,                          /* 0x8a     */
            0,                          /* 0x8b     */
            0,                          /* 0x8c     */
            0,                          /* 0x8d     */
            0,                          /* 0x8e     */
            0,                          /* 0x8f     */
            0,                          /* 0x90     */
            0,                          /* 0x91     */
            0,                          /* 0x92     */
            0,                          /* 0x93     */
            0,                          /* 0x94     */
            0,                          /* 0x95     */
            0,                          /* 0x96     */
            0,                          /* 0x97     */
            0,                          /* 0x98     */
            0,                          /* 0x99     */
            0,                          /* 0x9a     */
            0,                          /* 0x9b     */
            0,                          /* 0x9c     */
            0,                          /* 0x9d     */
            0,                          /* 0x9e     */
            0,                          /* 0x9f     */
            0,                          /* 0xa0     */
            0,                          /* 0xa1     */
            0,                          /* 0xa2     */
            0,                          /* 0xa3     */
            0,                          /* 0xa4     */
            0,                          /* 0xa5     */
            0,                          /* 0xa6     */
            0,                          /* 0xa7     */
            0,                          /* 0xa8     */
            0,                          /* 0xa9     */
            0,                          /* 0xaa     */
            0,                          /* 0xab     */
            0,                          /* 0xac     */
            0,                          /* 0xad     */
            0,                          /* 0xae     */
            0,                          /* 0xaf     */
            0,                          /* 0xb0     */
            0,                          /* 0xb1     */
            0,                          /* 0xb2     */
            0,                          /* 0xb3     */
            0,                          /* 0xb4     */
            0,                          /* 0xb5     */
            0,                          /* 0xb6     */
            0,                          /* 0xb7     */
            0,                          /* 0xb8     */
            0,                          /* 0xb9     */
            0,                          /* 0xba     */
            0,                          /* 0xbb     */
            0,                          /* 0xbc     */
            0,                          /* 0xbd     */
            0,                          /* 0xbe     */
            0,                          /* 0xbf     */
            0,                          /* 0xc0     */
            0,                          /* 0xc1     */
            0,                          /* 0xc2     */
            0,                          /* 0xc3     */
            0,                          /* 0xc4     */
            0,                          /* 0xc5     */
            0,                          /* 0xc6     */
            0,                          /* 0xc7     */
            0,                          /* 0xc8     */
            0,                          /* 0xc9     */
            0,                          /* 0xca     */
            0,                          /* 0xcb     */
            0,                          /* 0xcc     */
            0,                          /* 0xcd     */
            0,                          /* 0xce     */
            0,                          /* 0xcf     */
            0,                          /* 0xd0     */
            0,                          /* 0xd1     */
            0,                          /* 0xd2     */
            0,                          /* 0xd3     */
            0,                          /* 0xd4     */
            0,                          /* 0xd5     */
            0,                          /* 0xd6     */
            0,                          /* 0xd7     */
            0,                          /* 0xd8     */
            0,                          /* 0xd9     */
            0,                          /* 0xda     */
            0,                          /* 0xdb     */
            0,                          /* 0xdc     */
            0,                          /* 0xdd     */
            0,                          /* 0xde     */
            0,                          /* 0xdf     */
            0,                          /* 0xe0     */
            0,                          /* 0xe1     */
            0,                          /* 0xe2     */
            0,                          /* 0xe3     */
            0,                          /* 0xe4     */
            0,                          /* 0xe5     */
            0,                          /* 0xe6     */
            0,                          /* 0xe7     */
            0,                          /* 0xe8     */
            0,                          /* 0xe9     */
            0,                          /* 0xea     */
            0,                          /* 0xeb     */
            0,                          /* 0xec     */
            0,                          /* 0xed     */
            0,                          /* 0xee     */
            0,                          /* 0xef     */
            0,                          /* 0xf0     */
            0,                          /* 0xf1     */
            0,                          /* 0xf2     */
            0,                          /* 0xf3     */
            0,                          /* 0xf4     */
            0,                          /* 0xf5     */
            0,                          /* 0xf6     */
            0,                          /* 0xf7     */
            0,                          /* 0xf8     */
            0,                          /* 0xf9     */
            0,                          /* 0xfa     */
            0,                          /* 0xfb     */
            0,                          /* 0xfc     */
            0,                          /* 0xfd     */
            0,                          /* 0xfe     */
            0                           /* 0xff     */
    };

    private final static int getPathEndPos(String uri) {
        int pos = 0;
        int len = uri.length();
        while ((pos<len)&&((uriDelims[uri.charAt(pos)] & NOTEND_PATH) == 0)) {
            pos++;
        }

        return pos;
    }

    public final static String getPath(String uri) {

        if(uri == null || uri.length() == 0)
            return "";

        int end = getPathEndPos(uri);
        if (end == 0||end>uri.length())
            return "";

        return uri.substring(0, end);
    }

    public final static String getQuery(String uri) {

        int pos = getPathEndPos(uri);
        if (pos >= uri.length())
            return  null;

        if (uri.charAt(pos) == '?') {
            int end = uri.indexOf('#');
            if (end != -1) {
                return uri.substring(pos, end);
            } else {
                return uri.substring(pos + 1);
            }
        }
        return null;
    }

    public final static String urlDecode(String uri) {

        boolean needToChange = false;
        int numChars = uri.length();
        StringBuffer sb = new StringBuffer(numChars > 500 ? numChars / 2 : numChars);
        int i = 0;

        char c;
        byte[] bytes = null;
        while (i < numChars) {
            c = uri.charAt(i);
            switch (c) {
                case '+':
                    sb.append(' ');
                    i++;
                    needToChange = true;
                    break;
                case '%':
                /*
                 * Starting with this instance of %, process all
                 * consecutive substrings of the form %xy. Each
                 * substring %xy will yield a byte. Convert all
                 * consecutive  bytes obtained this way to whatever
                 * character(s) they represent in the provided
                 * encoding.
                 */


                    // (numChars-i)/3 is an upper bound for the number
                    // of remaining bytes

                    while (((i + 2) < numChars) &&
                            (c == '%')) {

                        try {
                            int v = Integer.parseInt(uri.substring(i + 1, i + 3), 16);
                            if (v < 0){
                                /*error no decode*/
                                sb.append(uri.charAt(i));
                                sb.append(uri.charAt(i+1));
                                sb.append(uri.charAt(i+2));
                            }else{
                                sb.append((char)v);
                            }

                        }catch (Exception e){
                            sb.append(uri.charAt(i));
                            sb.append(uri.charAt(i+1));
                            sb.append(uri.charAt(i+2));
                        }

                        i += 3;
                        if (i < numChars)
                            c = uri.charAt(i);
                    }

                    // A trailing, incomplete byte encoding such as
                    // "%x" will copy them into buffer

                    if ((i < numChars) && (c=='%'))
                    {
                        while(i<numChars){
                            sb.append(uri.charAt(i++));
                        }
                    }
                    needToChange = true;
                    break;
                default:
                    sb.append(c);
                    i++;
                    break;
            }
        }

        return (needToChange ? sb.toString() : uri);
    }

}
