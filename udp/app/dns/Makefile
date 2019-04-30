##########################################################
#Copyright(C) 2012 WAF PROJECT TEAM
#Author(A) shajianfeng
##########################################################

include ../../../make.include
DPDK=/usr/local/dpdk/dpdk_src
DPDK_FLAGS=$(shell cat ${DPDK}/include/cflags.txt)
CFLAGS  = ${BUILD_CFLAGS}  -O0 -rdynamic -D_GNU_SOURCE -DUSE_DPDK ${DPDK_FLAGS}
CFLAGS += -I../../../lib/common -I../../../packet -I../../../lib/ptables -I../../../lib/shm -I../../../lib/thread -I../../../lib/redis 
CFLAGS += -I../../../MProcess -I../../../SessionMonitor -I../../../WBList
CFLAGS += -I../ -I../../ -I${DPDK}/include -include ${DPDK}/include/rte_config.h
CFLAGS +=  -fPIC

ch_dns_SOURCES = ch_dns_app.c \
				 ch_dns_app_context.c \
				 ch_dns_session.c \
				 ch_dns_header.c \
				 ch_dns_request.c \
				 ch_dns_response.c \
				 ch_dns_name.c \
				 ch_dns_rdata_pool.c \
				 ch_dns_rdata_cname.c \
				 ch_dns_rdata_afsdb.c \
				 ch_dns_rdata_hinfo.c \
				 ch_dns_rdata_ipv4.c \
				 ch_dns_rdata_ipv6.c \
				 ch_dns_rdata_mb.c \
				 ch_dns_rdata_md.c \
				 ch_dns_rdata_mf.c \
				 ch_dns_rdata_mg.c \
				 ch_dns_rdata_minfo.c \
				 ch_dns_rdata_mr.c \
				 ch_dns_rdata_mx.c \
				 ch_dns_rdata_ns.c \
				 ch_dns_rdata_null.c \
				 ch_dns_rdata_pool.c \
				 ch_dns_rdata_ptr.c \
				 ch_dns_rdata_soa.c \
				 ch_dns_rdata_txt.c \
				 ch_dns_rdata_wks.c \
				 ch_dns_rdata_caa.c \
				 ch_dns_rdata_cert.c \
				 ch_dns_rdata_dhcid.c \
				 ch_dns_rdata_dlv.c \
				 ch_dns_rdata_apl.c \
				 ch_dns_rdata_dname.c \
				 ch_dns_sec_key.c \
				 ch_dns_sec_sig.c \
				 ch_dns_rdata_dnskey.c \
				 ch_dns_rdata_ds.c \
				 ch_dns_rdata_gpos.c \
				 ch_dns_rdata_ipseckey.c \
				 ch_dns_rdata_isdn.c \
				 ch_dns_rdata_key.c \
				 ch_dns_rdata_kx.c \
				 ch_dns_rdata_loc.c \
				 ch_dns_rdata_naptr.c \
				 ch_dns_rdata_nsap_ptr.c \
				 ch_dns_rdata_nsap.c \
				 ch_dns_rdata_nsec3param.c \
				 ch_dns_rdata_nsec3.c \
				 ch_dns_rdata_nsec.c \
				 ch_dns_rdata_nxt.c \
				 ch_dns_rdata_openpgpkey.c \
				 ch_dns_rdata_opt.c \
				 ch_dns_rdata_px.c \
				 ch_dns_rdata_rp.c \
				 ch_dns_rdata_rrsig.c \
				 ch_dns_rdata_rt.c \
				 ch_dns_rdata_sig.c \
				 ch_dns_rdata_smimea.c  \
				 ch_dns_txt_base.c \
				 ch_dns_rdata_spf.c \
				 ch_dns_rdata_srv.c \
				 ch_dns_rdata_sshfp.c \
				 ch_dns_rdata_tkey.c \
				 ch_dns_rdata_tlsa.c \
				 ch_dns_rdata_tsig.c \
				 ch_dns_rdata_x25.c \
				 ch_dns_rdata_uri.c

ch_dns_OBJECTS = $(patsubst %.c,%.o,$(ch_dns_SOURCES))
ch_dns_DEPENDS = $(patsubst %.c,%.d,$(ch_dns_SOURCES))
ch_dns_ASMFILE = $(patsubst %.c,%.s,$(ch_dns_SOURCES))

.PHONY: all clean

all: $(ch_dns_OBJECTS)

clean:
	@rm -fr $(ch_dns_OBJECTS) $(ch_dns_DEPENDS) $(ch_dns_ASMFILE) $(ch_dns_package)
	@rm -fr *.d *.o *.s 

