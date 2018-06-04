GAS LISTING  			page 1


   1              	 .file "ch_packets_reader.c"
   2              	 .text
   3              	.Ltext0:
   4              	 .section .rodata.str1.1,"aMS",@progbits,1
   5              	.LC0:
   6 0000 43617567 	 .string "Caught SIGINT, shutting down."
   6      68742053 
   6      4947494E 
   6      542C2073 
   6      68757474 
   7              	 .section .rodata.str1.8,"aMS",@progbits,1
   8              	 .align 8
   9              	.LC1:
  10 0000 43617567 	 .string "Caught SIGTERM, shutting down."
  10      68742053 
  10      49475445 
  10      524D2C20 
  10      73687574 
  11              	 .section .rodata.str1.1
  12              	.LC2:
  13 001e 43617567 	 .string "Caught unexpected signal %d\n"
  13      68742075 
  13      6E657870 
  13      65637465 
  13      64207369 
  14              	 .text
  15              	 .p2align 4,,15
  16              	 .type handle_signals,@function
  17              	handle_signals:
  18              	.LFB96:
  19              	 .file 1 "ch_packets_reader.c"
   1:ch_packets_reader.c **** /*
   2:ch_packets_reader.c ****  *
   3:ch_packets_reader.c ****  *      Filename: ch_packets_reader.c
   4:ch_packets_reader.c ****  *
   5:ch_packets_reader.c ****  *        Author: jacks001314@163.com
   6:ch_packets_reader.c ****  *   Description: ---
   7:ch_packets_reader.c ****  *        Create: 2017-06-01 11:23:41
   8:ch_packets_reader.c ****  * Last Modified: 2017-06-01 12:13:22
   9:ch_packets_reader.c ****  */
  10:ch_packets_reader.c **** 
  11:ch_packets_reader.c **** 
  12:ch_packets_reader.c **** #include <stdio.h>
  13:ch_packets_reader.c **** #include <stdlib.h>
  14:ch_packets_reader.c **** #include <apr_pools.h>
  15:ch_packets_reader.c **** #include <unistd.h>
  16:ch_packets_reader.c **** #include <apr_signal.h>
  17:ch_packets_reader.c **** 
  18:ch_packets_reader.c **** #include "ch_log.h"
  19:ch_packets_reader.c **** #include "ch_mmap_packets_format.h"
  20:ch_packets_reader.c **** 
  21:ch_packets_reader.c **** static int is_exit = 0;
  22:ch_packets_reader.c **** 
  23:ch_packets_reader.c **** /**
  24:ch_packets_reader.c ****  * Handle signals.
  25:ch_packets_reader.c ****  */
  26:ch_packets_reader.c **** static void handle_signals(int signum)
GAS LISTING  			page 2


  27:ch_packets_reader.c **** {
  20              	 .loc 1 27 0
  21              	 .cfi_startproc
  22              	.LVL0:
  23 0000 4883EC08 	 subq $8,%rsp
  24              	 .cfi_def_cfa_offset 16
  28:ch_packets_reader.c ****     switch (signum) {
  25              	 .loc 1 28 0
  26 0004 83FF02   	 cmpl $2,%edi
  27 0007 7438     	 je .L3
  28 0009 83FF0F   	 cmpl $15,%edi
  29 000c 7511     	 jne .L8
  29:ch_packets_reader.c ****         case SIGINT:
  30:ch_packets_reader.c ****             printf("Caught SIGINT, shutting down.\n");
  31:ch_packets_reader.c ****             exit(0);
  32:ch_packets_reader.c ****         case SIGTERM:
  33:ch_packets_reader.c ****             printf("Caught SIGTERM, shutting down.\n");
  30              	 .loc 1 33 0
  31 000e BF000000 	 movl $.LC1,%edi
  31      00
  32              	.LVL1:
  33 0013 E8000000 	 call puts
  33      00
  34              	.LVL2:
  34:ch_packets_reader.c ****             exit(0);
  35              	 .loc 1 34 0
  36 0018 31FF     	 xorl %edi,%edi
  37 001a E8000000 	 call exit
  37      00
  38              	.LVL3:
  39              	.L8:
  35:ch_packets_reader.c ****         default:
  36:ch_packets_reader.c ****             break;
  37:ch_packets_reader.c ****     }
  38:ch_packets_reader.c **** 
  39:ch_packets_reader.c **** 	is_exit = 1;
  40:ch_packets_reader.c **** 
  41:ch_packets_reader.c ****     printf("Caught unexpected signal %d\n", signum);
  40              	 .loc 1 41 0
  41 001f 89FE     	 movl %edi,%esi
  42 0021 31C0     	 xorl %eax,%eax
  43 0023 BF000000 	 movl $.LC2,%edi
  43      00
  44              	.LVL4:
  39:ch_packets_reader.c **** 
  45              	 .loc 1 39 0
  46 0028 C7050000 	 movl $1,is_exit(%rip)
  46      00000100 
  46      0000
  47              	 .loc 1 41 0
  48 0032 E8000000 	 call printf
  48      00
  49              	.LVL5:
  42:ch_packets_reader.c ****     exit(1);
  50              	 .loc 1 42 0
  51 0037 BF010000 	 movl $1,%edi
  51      00
GAS LISTING  			page 3


  52 003c E8000000 	 call exit
  52      00
  53              	.LVL6:
  54              	.L3:
  30:ch_packets_reader.c ****             exit(0);
  55              	 .loc 1 30 0
  56 0041 BF000000 	 movl $.LC0,%edi
  56      00
  57              	.LVL7:
  58 0046 E8000000 	 call puts
  58      00
  59              	.LVL8:
  31:ch_packets_reader.c ****         case SIGTERM:
  60              	 .loc 1 31 0
  61 004b 31FF     	 xorl %edi,%edi
  62 004d E8000000 	 call exit
  62      00
  63              	.LVL9:
  64              	 .cfi_endproc
  65              	.LFE96:
  66              	 .size handle_signals,.-handle_signals
  67              	 .section .rodata.str1.8
  68 001f 00       	 .align 8
  69              	.LC3:
  70 0020 55736167 	 .string "Usage: appName <packets interface file name>!\n"
  70      653A2061 
  70      70704E61 
  70      6D65203C 
  70      7061636B 
  71 004f 00       	 .align 8
  72              	.LC4:
  73 0050 63726561 	 .string "create global memory pool failed!\n"
  73      74652067 
  73      6C6F6261 
  73      6C206D65 
  73      6D6F7279 
  74              	 .section .rodata.str1.1
  75              	.LC5:
  76 003b 696E6974 	 .string "init errno failed!\n"
  76      20657272 
  76      6E6F2066 
  76      61696C65 
  76      64210A00 
  77              	.LC6:
  78 004f 2F746D70 	 .string "/tmp/preader.log"
  78      2F707265 
  78      61646572 
  78      2E6C6F67 
  78      00
  79              	 .section .rodata.str1.8
  80 0073 00000000 	 .align 8
  80      00
  81              	.LC7:
  82 0078 43726561 	 .string "Create packets mmap file format instance failed!\n"
  82      74652070 
  82      61636B65 
  82      7473206D 
GAS LISTING  			page 4


  82      6D617020 
  83 00aa 00000000 	 .align 8
  83      0000
  84              	.LC8:
  85 00b0 25733A20 	 .string "%s: iter is null, sleep %d seconds."
  85      69746572 
  85      20697320 
  85      6E756C6C 
  85      2C20736C 
  86              	 .section .text.startup,"ax",@progbits
  87              	 .p2align 4,,15
  88              	 .globl main
  89              	 .type main,@function
  90              	main:
  91              	.LFB98:
  43:ch_packets_reader.c **** }
  44:ch_packets_reader.c **** 
  45:ch_packets_reader.c **** static void read_packets_interface(ch_mmap_packets_format_t *pfmt){
  46:ch_packets_reader.c **** 
  47:ch_packets_reader.c **** 	ch_mmap_packets_iterator_t *iter = NULL;
  48:ch_packets_reader.c **** 	ch_mmap_packets_entry_t *pe = NULL; 
  49:ch_packets_reader.c **** 
  50:ch_packets_reader.c **** 	while(!is_exit) {
  51:ch_packets_reader.c **** 		
  52:ch_packets_reader.c **** 		iter = ch_mmap_packets_format_iterator_prefare(pfmt);
  53:ch_packets_reader.c **** 		
  54:ch_packets_reader.c **** 		if (iter == NULL) {
  55:ch_packets_reader.c **** 			ch_log(CH_LOG_INFO,"%s: iter is null, sleep %d seconds.", __func__, 1);
  56:ch_packets_reader.c **** 			
  57:ch_packets_reader.c **** 			usleep(1000000);
  58:ch_packets_reader.c **** 
  59:ch_packets_reader.c **** 			continue;
  60:ch_packets_reader.c **** 		}
  61:ch_packets_reader.c **** 
  62:ch_packets_reader.c **** 		while((pe = iter->next(iter))){
  63:ch_packets_reader.c **** 
  64:ch_packets_reader.c **** 			ch_mmap_packets_entry_dump(pe,stdout);
  65:ch_packets_reader.c **** 		}
  66:ch_packets_reader.c **** 
  67:ch_packets_reader.c ****         ch_mmap_packets_format_iterator_commit(pfmt, iter);
  68:ch_packets_reader.c **** 	}
  69:ch_packets_reader.c **** 
  70:ch_packets_reader.c **** 	/* exit */
  71:ch_packets_reader.c **** 	is_exit = 0;
  72:ch_packets_reader.c **** 
  73:ch_packets_reader.c **** 	if (pfmt && iter)
  74:ch_packets_reader.c **** 		ch_mmap_packets_format_iterator_commit(pfmt, iter);
  75:ch_packets_reader.c **** 
  76:ch_packets_reader.c **** 	return 0;
  77:ch_packets_reader.c **** 
  78:ch_packets_reader.c **** }
  79:ch_packets_reader.c **** 
  80:ch_packets_reader.c **** int main(int argc,char ** argv){
  92              	 .loc 1 80 0
  93              	 .cfi_startproc
  94              	.LVL10:
  95 0000 55       	 pushq %rbp
GAS LISTING  			page 5


  96              	 .cfi_def_cfa_offset 16
  97              	 .cfi_offset 6,-16
  98 0001 53       	 pushq %rbx
  99              	 .cfi_def_cfa_offset 24
 100              	 .cfi_offset 3,-24
 101 0002 4883EC28 	 subq $40,%rsp
 102              	 .cfi_def_cfa_offset 64
  81:ch_packets_reader.c **** 
  82:ch_packets_reader.c **** 	const char *fname;
  83:ch_packets_reader.c **** 	apr_pool_t *mp;
  84:ch_packets_reader.c **** 	int rc;
  85:ch_packets_reader.c **** 	ch_mmap_packets_format_t *pfmt;
  86:ch_packets_reader.c **** 
  87:ch_packets_reader.c **** 	if(argc<2){
 103              	 .loc 1 87 0
 104 0006 83FF01   	 cmpl $1,%edi
  80:ch_packets_reader.c **** 
 105              	 .loc 1 80 0
 106 0009 897C240C 	 movl %edi,12(%rsp)
 107 000d 48893424 	 movq %rsi,(%rsp)
 108              	 .loc 1 87 0
 109 0011 0F8E6001 	 jle .L24
 109      0000
  88:ch_packets_reader.c **** 	
  89:ch_packets_reader.c **** 		fprintf(stderr,"Usage: appName <packets interface file name>!\n");
  90:ch_packets_reader.c **** 		return -1;
  91:ch_packets_reader.c **** 	}
  92:ch_packets_reader.c **** 
  93:ch_packets_reader.c **** 	fname = argv[1];
 110              	 .loc 1 93 0
 111 0017 488B0424 	 movq (%rsp),%rax
  94:ch_packets_reader.c **** 
  95:ch_packets_reader.c ****     /*init apr app runtine*/
  96:ch_packets_reader.c ****     apr_app_initialize(&argc,&argv,NULL);
 112              	 .loc 1 96 0
 113 001b 488D7C24 	 leaq 12(%rsp),%rdi
 113      0C
 114              	.LVL11:
 115 0020 31D2     	 xorl %edx,%edx
 116 0022 4889E6   	 movq %rsp,%rsi
 117              	.LVL12:
  93:ch_packets_reader.c **** 
 118              	 .loc 1 93 0
 119 0025 488B5808 	 movq 8(%rax),%rbx
 120              	.LVL13:
 121              	 .loc 1 96 0
 122 0029 E8000000 	 call apr_app_initialize
 122      00
 123              	.LVL14:
  97:ch_packets_reader.c **** 
  98:ch_packets_reader.c ****     /*create global apr memory pool*/
  99:ch_packets_reader.c ****     apr_pool_create(&mp,NULL);
 124              	 .loc 1 99 0
 125 002e 488D7C24 	 leaq 24(%rsp),%rdi
 125      18
 126 0033 31C9     	 xorl %ecx,%ecx
 127 0035 31D2     	 xorl %edx,%edx
GAS LISTING  			page 6


 128 0037 31F6     	 xorl %esi,%esi
 129 0039 E8000000 	 call apr_pool_create_ex
 129      00
 130              	.LVL15:
 100:ch_packets_reader.c **** 
 101:ch_packets_reader.c ****     if(mp == NULL){
 131              	 .loc 1 101 0
 132 003e 48837C24 	 cmpq $0,24(%rsp)
 132      1800
 133 0044 0F847001 	 je .L25
 133      0000
 102:ch_packets_reader.c **** 
 103:ch_packets_reader.c ****         fprintf(stderr,"create global memory pool failed!\n");
 104:ch_packets_reader.c ****         return -1;
 105:ch_packets_reader.c ****     }
 106:ch_packets_reader.c **** 
 107:ch_packets_reader.c ****     /*init errno*/
 108:ch_packets_reader.c ****     rc = ch_strerror_init();
 134              	 .loc 1 108 0
 135 004a E8000000 	 call ch_strerror_init
 135      00
 136              	.LVL16:
 109:ch_packets_reader.c ****     if(rc!=0){
 137              	 .loc 1 109 0
 138 004f 85C0     	 testl %eax,%eax
 139 0051 0F850001 	 jne .L26
 139      0000
 110:ch_packets_reader.c ****         fprintf(stderr,"init errno failed!\n");
 111:ch_packets_reader.c ****         return -1;
 112:ch_packets_reader.c ****     }
 113:ch_packets_reader.c ****     
 114:ch_packets_reader.c ****     /*init log*/
 115:ch_packets_reader.c ****     ch_log_init(mp,"/tmp/preader.log",CH_LOG_DEBUG);
 140              	 .loc 1 115 0
 141 0057 488B7C24 	 movq 24(%rsp),%rdi
 141      18
 142 005c BA080000 	 movl $8,%edx
 142      00
 143 0061 BE000000 	 movl $.LC6,%esi
 143      00
 144 0066 E8000000 	 call ch_log_init
 144      00
 145              	.LVL17:
 116:ch_packets_reader.c **** 
 117:ch_packets_reader.c **** 	apr_signal(SIGINT, handle_signals);
 146              	 .loc 1 117 0
 147 006b BE000000 	 movl $handle_signals,%esi
 147      00
 148 0070 BF020000 	 movl $2,%edi
 148      00
 149 0075 E8000000 	 call apr_signal
 149      00
 150              	.LVL18:
 118:ch_packets_reader.c **** 	apr_signal(SIGTERM, handle_signals);
 151              	 .loc 1 118 0
 152 007a BE000000 	 movl $handle_signals,%esi
 152      00
GAS LISTING  			page 7


 153 007f BF0F0000 	 movl $15,%edi
 153      00
 154 0084 E8000000 	 call apr_signal
 154      00
 155              	.LVL19:
 119:ch_packets_reader.c **** 
 120:ch_packets_reader.c **** 	pfmt = ch_mmap_packets_format_create(mp,fname,0,0,0);
 156              	 .loc 1 120 0
 157 0089 488B7C24 	 movq 24(%rsp),%rdi
 157      18
 158 008e 4531C0   	 xorl %r8d,%r8d
 159 0091 31C9     	 xorl %ecx,%ecx
 160 0093 31D2     	 xorl %edx,%edx
 161 0095 4889DE   	 movq %rbx,%rsi
 162 0098 E8000000 	 call ch_mmap_packets_format_create
 162      00
 163              	.LVL20:
 121:ch_packets_reader.c **** 	if(pfmt == NULL){
 164              	 .loc 1 121 0
 165 009d 4885C0   	 testq %rax,%rax
 120:ch_packets_reader.c **** 	if(pfmt == NULL){
 166              	 .loc 1 120 0
 167 00a0 4889C5   	 movq %rax,%rbp
 168              	.LVL21:
 169              	 .loc 1 121 0
 170 00a3 0F84EE00 	 je .L14
 170      0000
 171 00a9 31DB     	 xorl %ebx,%ebx
 172              	.LVL22:
 173              	.L15:
 174              	.LBB4:
 175              	.LBB5:
  50:ch_packets_reader.c **** 		
 176              	 .loc 1 50 0
 177 00ab 8B050000 	 movl is_exit(%rip),%eax
 177      0000
 178 00b1 85C0     	 testl %eax,%eax
 179 00b3 754A     	 jne .L27
 180              	 .p2align 4,,10
 181 00b5 0F1F00   	 .p2align 3
 182              	.L20:
  52:ch_packets_reader.c **** 		
 183              	 .loc 1 52 0
 184 00b8 4889EF   	 movq %rbp,%rdi
 185 00bb E8000000 	 call ch_mmap_packets_format_iterator_prefare
 185      00
 186              	.LVL23:
  54:ch_packets_reader.c **** 			ch_log(CH_LOG_INFO,"%s: iter is null, sleep %d seconds.", __func__, 1);
 187              	 .loc 1 54 0
 188 00c0 4885C0   	 testq %rax,%rax
  52:ch_packets_reader.c **** 		
 189              	 .loc 1 52 0
 190 00c3 4889C3   	 movq %rax,%rbx
 191              	.LVL24:
  54:ch_packets_reader.c **** 			ch_log(CH_LOG_INFO,"%s: iter is null, sleep %d seconds.", __func__, 1);
 192              	 .loc 1 54 0
 193 00c6 7517     	 jne .L17
GAS LISTING  			page 8


 194 00c8 EB60     	 jmp .L29
 195              	.LVL25:
 196 00ca 660F1F44 	 .p2align 4,,10
 196      0000
 197              	 .p2align 3
 198              	.L19:
  64:ch_packets_reader.c **** 		}
 199              	 .loc 1 64 0
 200 00d0 488B3500 	 movq stdout(%rip),%rsi
 200      000000
 201 00d7 4889C7   	 movq %rax,%rdi
 202 00da E8000000 	 call ch_mmap_packets_entry_dump
 202      00
 203              	.LVL26:
 204              	.L17:
  62:ch_packets_reader.c **** 
 205              	 .loc 1 62 0
 206 00df 4889DF   	 movq %rbx,%rdi
 207 00e2 FF5308   	 call *8(%rbx)
 208              	.LVL27:
 209 00e5 4885C0   	 testq %rax,%rax
 210 00e8 75E6     	 jne .L19
  67:ch_packets_reader.c **** 	}
 211              	 .loc 1 67 0
 212 00ea 4889DE   	 movq %rbx,%rsi
 213 00ed 4889EF   	 movq %rbp,%rdi
 214 00f0 E8000000 	 call ch_mmap_packets_format_iterator_commit
 214      00
 215              	.LVL28:
  50:ch_packets_reader.c **** 		
 216              	 .loc 1 50 0
 217 00f5 8B050000 	 movl is_exit(%rip),%eax
 217      0000
 218 00fb 85C0     	 testl %eax,%eax
 219 00fd 74B9     	 je .L20
 220              	.L27:
 221              	.LBE5:
 222              	.LBE4:
 122:ch_packets_reader.c **** 
 123:ch_packets_reader.c **** 		fprintf(stderr,"Create packets mmap file format instance failed!\n");
 124:ch_packets_reader.c **** 		return -1;
 125:ch_packets_reader.c **** 	}
 126:ch_packets_reader.c **** 
 127:ch_packets_reader.c **** 	read_packets_interface(pfmt);
 128:ch_packets_reader.c **** 
 129:ch_packets_reader.c **** 	return 0;
 223              	 .loc 1 129 0
 224 00ff 31C0     	 xorl %eax,%eax
 225              	.LBB8:
 226              	.LBB6:
  73:ch_packets_reader.c **** 		ch_mmap_packets_format_iterator_commit(pfmt, iter);
 227              	 .loc 1 73 0
 228 0101 4885DB   	 testq %rbx,%rbx
  71:ch_packets_reader.c **** 
 229              	 .loc 1 71 0
 230 0104 C7050000 	 movl $0,is_exit(%rip)
 230      00000000 
GAS LISTING  			page 9


 230      0000
  73:ch_packets_reader.c **** 		ch_mmap_packets_format_iterator_commit(pfmt, iter);
 231              	 .loc 1 73 0
 232 010e 7413     	 je .L11
  74:ch_packets_reader.c **** 
 233              	 .loc 1 74 0
 234 0110 4889DE   	 movq %rbx,%rsi
 235 0113 4889EF   	 movq %rbp,%rdi
 236 0116 89442408 	 movl %eax,8(%rsp)
 237 011a E8000000 	 call ch_mmap_packets_format_iterator_commit
 237      00
 238              	.LVL29:
 239 011f 8B442408 	 movl 8(%rsp),%eax
 240              	.LVL30:
 241              	.L11:
 242              	.LBE6:
 243              	.LBE8:
 130:ch_packets_reader.c **** }
 244              	 .loc 1 130 0
 245 0123 4883C428 	 addq $40,%rsp
 246              	 .cfi_remember_state
 247              	 .cfi_def_cfa_offset 24
 248              	.LVL31:
 249 0127 5B       	 popq %rbx
 250              	 .cfi_def_cfa_offset 16
 251 0128 5D       	 popq %rbp
 252              	 .cfi_def_cfa_offset 8
 253 0129 C3       	 ret
 254              	.LVL32:
 255              	.L29:
 256              	 .cfi_restore_state
 257              	.LBB9:
 258              	.LBB7:
  55:ch_packets_reader.c **** 			
 259              	 .loc 1 55 0
 260 012a 41B80100 	 movl $1,%r8d
 260      0000
 261 0130 B9000000 	 movl $__func__.5664,%ecx
 261      00
 262 0135 BA000000 	 movl $.LC8,%edx
 262      00
 263 013a 31F6     	 xorl %esi,%esi
 264 013c BF070000 	 movl $7,%edi
 264      00
 265 0141 31C0     	 xorl %eax,%eax
 266              	.LVL33:
 267 0143 E8000000 	 call ch_log_error_core
 267      00
 268              	.LVL34:
  57:ch_packets_reader.c **** 
 269              	 .loc 1 57 0
 270 0148 BF40420F 	 movl $1000000,%edi
 270      00
 271 014d E8000000 	 call usleep
 271      00
 272              	.LVL35:
 273 0152 E954FFFF 	 jmp .L15
GAS LISTING  			page 10


 273      FF
 274              	.LVL36:
 275              	.L26:
 276              	.LBE7:
 277              	.LBE9:
 110:ch_packets_reader.c ****         return -1;
 278              	 .loc 1 110 0
 279 0157 488B0D00 	 movq stderr(%rip),%rcx
 279      000000
 280 015e BA130000 	 movl $19,%edx
 280      00
 281 0163 BE010000 	 movl $1,%esi
 281      00
 282 0168 BF000000 	 movl $.LC5,%edi
 282      00
 283 016d E8000000 	 call fwrite
 283      00
 284              	.LVL37:
 111:ch_packets_reader.c ****     }
 285              	 .loc 1 111 0
 286 0172 83C8FF   	 orl $-1,%eax
 287 0175 EBAC     	 jmp .L11
 288              	.LVL38:
 289              	.L24:
  89:ch_packets_reader.c **** 		return -1;
 290              	 .loc 1 89 0
 291 0177 488B0D00 	 movq stderr(%rip),%rcx
 291      000000
 292 017e BA2E0000 	 movl $46,%edx
 292      00
 293 0183 BE010000 	 movl $1,%esi
 293      00
 294              	.LVL39:
 295 0188 BF000000 	 movl $.LC3,%edi
 295      00
 296              	.LVL40:
 297 018d E8000000 	 call fwrite
 297      00
 298              	.LVL41:
  90:ch_packets_reader.c **** 	}
 299              	 .loc 1 90 0
 300 0192 83C8FF   	 orl $-1,%eax
 301 0195 EB8C     	 jmp .L11
 302              	.LVL42:
 303              	.L14:
 123:ch_packets_reader.c **** 		return -1;
 304              	 .loc 1 123 0
 305 0197 488B0D00 	 movq stderr(%rip),%rcx
 305      000000
 306 019e BA310000 	 movl $49,%edx
 306      00
 307 01a3 BE010000 	 movl $1,%esi
 307      00
 308 01a8 BF000000 	 movl $.LC7,%edi
 308      00
 309 01ad E8000000 	 call fwrite
 309      00
GAS LISTING  			page 11


 310              	.LVL43:
 124:ch_packets_reader.c **** 	}
 311              	 .loc 1 124 0
 312 01b2 83C8FF   	 orl $-1,%eax
 313 01b5 E969FFFF 	 jmp .L11
 313      FF
 314              	.LVL44:
 315              	.L25:
 103:ch_packets_reader.c ****         return -1;
 316              	 .loc 1 103 0
 317 01ba 488B0D00 	 movq stderr(%rip),%rcx
 317      000000
 318 01c1 BA220000 	 movl $34,%edx
 318      00
 319 01c6 BE010000 	 movl $1,%esi
 319      00
 320 01cb BF000000 	 movl $.LC4,%edi
 320      00
 321 01d0 E8000000 	 call fwrite
 321      00
 322              	.LVL45:
 104:ch_packets_reader.c ****     }
 323              	 .loc 1 104 0
 324 01d5 83C8FF   	 orl $-1,%eax
 325 01d8 E946FFFF 	 jmp .L11
 325      FF
 326              	 .cfi_endproc
 327              	.LFE98:
 328              	 .size main,.-main
 329              	 .section .rodata
 330              	 .align 16
 331              	 .type __func__.5664,@object
 332              	 .size __func__.5664,23
 333              	__func__.5664:
 334 0000 72656164 	 .string "read_packets_interface"
 334      5F706163 
 334      6B657473 
 334      5F696E74 
 334      65726661 
 335              	 .local is_exit
 336              	 .comm is_exit,4,4
 337              	 .text
 338              	.Letext0:
 339              	 .file 2 "/usr/lib/gcc/x86_64-redhat-linux/4.8.5/include/stddef.h"
 340              	 .file 3 "/usr/include/bits/types.h"
 341              	 .file 4 "/usr/include/stdio.h"
 342              	 .file 5 "/usr/include/libio.h"
 343              	 .file 6 "/usr/include/stdint.h"
 344              	 .file 7 "/usr/local/apr/include/apr-1/apr_errno.h"
 345              	 .file 8 "/usr/local/apr/include/apr-1/apr_pools.h"
 346              	 .file 9 "/usr/local/apr/include/apr-1/apr_allocator.h"
 347              	 .file 10 "/usr/local/apr/include/apr-1/apr_signal.h"
 348              	 .file 11 "../util/ch_errno.h"
 349              	 .file 12 "../util/ch_mmap_packets_format.h"
 350              	 .file 13 "../util/ch_mmap.h"
 351              	 .file 14 "../util/ch_bin_format.h"
 352              	 .file 15 "/usr/include/stdlib.h"
GAS LISTING  			page 12


 353              	 .file 16 "../util/ch_log.h"
 354              	 .file 17 "/usr/include/unistd.h"
 355              	 .file 18 "/usr/local/apr/include/apr-1/apr_general.h"
 356              	 .file 19 "<built-in>"
 357              	 .section .debug_info,"",@progbits
 358              	.Ldebug_info0:
 359 0000 D10C0000 	 .long 0xcd1
 360 0004 0400     	 .value 0x4
 361 0006 00000000 	 .long .Ldebug_abbrev0
 362 000a 08       	 .byte 0x8
 363 000b 01       	 .uleb128 0x1
 364 000c 00000000 	 .long .LASF128
 365 0010 01       	 .byte 0x1
 366 0011 00000000 	 .long .LASF129
 367 0015 00000000 	 .long .LASF130
 368 0019 00000000 	 .long .Ldebug_ranges0+0x40
 369 001d 00000000 	 .quad 0
 369      00000000 
 370 0025 00000000 	 .long .Ldebug_line0
 371 0029 02       	 .uleb128 0x2
 372 002a 00000000 	 .long .LASF7
 373 002e 02       	 .byte 0x2
 374 002f D4       	 .byte 0xd4
 375 0030 34000000 	 .long 0x34
 376 0034 03       	 .uleb128 0x3
 377 0035 08       	 .byte 0x8
 378 0036 07       	 .byte 0x7
 379 0037 00000000 	 .long .LASF0
 380 003b 03       	 .uleb128 0x3
 381 003c 01       	 .byte 0x1
 382 003d 08       	 .byte 0x8
 383 003e 00000000 	 .long .LASF1
 384 0042 03       	 .uleb128 0x3
 385 0043 02       	 .byte 0x2
 386 0044 07       	 .byte 0x7
 387 0045 00000000 	 .long .LASF2
 388 0049 03       	 .uleb128 0x3
 389 004a 04       	 .byte 0x4
 390 004b 07       	 .byte 0x7
 391 004c 00000000 	 .long .LASF3
 392 0050 03       	 .uleb128 0x3
 393 0051 01       	 .byte 0x1
 394 0052 06       	 .byte 0x6
 395 0053 00000000 	 .long .LASF4
 396 0057 03       	 .uleb128 0x3
 397 0058 02       	 .byte 0x2
 398 0059 05       	 .byte 0x5
 399 005a 00000000 	 .long .LASF5
 400 005e 04       	 .uleb128 0x4
 401 005f 04       	 .byte 0x4
 402 0060 05       	 .byte 0x5
 403 0061 696E7400 	 .string "int"
 404 0065 03       	 .uleb128 0x3
 405 0066 08       	 .byte 0x8
 406 0067 05       	 .byte 0x5
 407 0068 00000000 	 .long .LASF6
 408 006c 02       	 .uleb128 0x2
GAS LISTING  			page 13


 409 006d 00000000 	 .long .LASF8
 410 0071 03       	 .byte 0x3
 411 0072 8C       	 .byte 0x8c
 412 0073 65000000 	 .long 0x65
 413 0077 02       	 .uleb128 0x2
 414 0078 00000000 	 .long .LASF9
 415 007c 03       	 .byte 0x3
 416 007d 8D       	 .byte 0x8d
 417 007e 65000000 	 .long 0x65
 418 0082 03       	 .uleb128 0x3
 419 0083 08       	 .byte 0x8
 420 0084 07       	 .byte 0x7
 421 0085 00000000 	 .long .LASF10
 422 0089 02       	 .uleb128 0x2
 423 008a 00000000 	 .long .LASF11
 424 008e 03       	 .byte 0x3
 425 008f 95       	 .byte 0x95
 426 0090 49000000 	 .long 0x49
 427 0094 05       	 .uleb128 0x5
 428 0095 08       	 .byte 0x8
 429 0096 06       	 .uleb128 0x6
 430 0097 08       	 .byte 0x8
 431 0098 9C000000 	 .long 0x9c
 432 009c 03       	 .uleb128 0x3
 433 009d 01       	 .byte 0x1
 434 009e 06       	 .byte 0x6
 435 009f 00000000 	 .long .LASF12
 436 00a3 02       	 .uleb128 0x2
 437 00a4 00000000 	 .long .LASF13
 438 00a8 04       	 .byte 0x4
 439 00a9 30       	 .byte 0x30
 440 00aa AE000000 	 .long 0xae
 441 00ae 07       	 .uleb128 0x7
 442 00af 00000000 	 .long .LASF43
 443 00b3 D8       	 .byte 0xd8
 444 00b4 05       	 .byte 0x5
 445 00b5 F6       	 .byte 0xf6
 446 00b6 2F020000 	 .long 0x22f
 447 00ba 08       	 .uleb128 0x8
 448 00bb 00000000 	 .long .LASF14
 449 00bf 05       	 .byte 0x5
 450 00c0 F7       	 .byte 0xf7
 451 00c1 5E000000 	 .long 0x5e
 452 00c5 00       	 .byte 0
 453 00c6 08       	 .uleb128 0x8
 454 00c7 00000000 	 .long .LASF15
 455 00cb 05       	 .byte 0x5
 456 00cc FC       	 .byte 0xfc
 457 00cd 96000000 	 .long 0x96
 458 00d1 08       	 .byte 0x8
 459 00d2 08       	 .uleb128 0x8
 460 00d3 00000000 	 .long .LASF16
 461 00d7 05       	 .byte 0x5
 462 00d8 FD       	 .byte 0xfd
 463 00d9 96000000 	 .long 0x96
 464 00dd 10       	 .byte 0x10
 465 00de 08       	 .uleb128 0x8
GAS LISTING  			page 14


 466 00df 00000000 	 .long .LASF17
 467 00e3 05       	 .byte 0x5
 468 00e4 FE       	 .byte 0xfe
 469 00e5 96000000 	 .long 0x96
 470 00e9 18       	 .byte 0x18
 471 00ea 08       	 .uleb128 0x8
 472 00eb 00000000 	 .long .LASF18
 473 00ef 05       	 .byte 0x5
 474 00f0 FF       	 .byte 0xff
 475 00f1 96000000 	 .long 0x96
 476 00f5 20       	 .byte 0x20
 477 00f6 09       	 .uleb128 0x9
 478 00f7 00000000 	 .long .LASF19
 479 00fb 05       	 .byte 0x5
 480 00fc 0001     	 .value 0x100
 481 00fe 96000000 	 .long 0x96
 482 0102 28       	 .byte 0x28
 483 0103 09       	 .uleb128 0x9
 484 0104 00000000 	 .long .LASF20
 485 0108 05       	 .byte 0x5
 486 0109 0101     	 .value 0x101
 487 010b 96000000 	 .long 0x96
 488 010f 30       	 .byte 0x30
 489 0110 09       	 .uleb128 0x9
 490 0111 00000000 	 .long .LASF21
 491 0115 05       	 .byte 0x5
 492 0116 0201     	 .value 0x102
 493 0118 96000000 	 .long 0x96
 494 011c 38       	 .byte 0x38
 495 011d 09       	 .uleb128 0x9
 496 011e 00000000 	 .long .LASF22
 497 0122 05       	 .byte 0x5
 498 0123 0301     	 .value 0x103
 499 0125 96000000 	 .long 0x96
 500 0129 40       	 .byte 0x40
 501 012a 09       	 .uleb128 0x9
 502 012b 00000000 	 .long .LASF23
 503 012f 05       	 .byte 0x5
 504 0130 0501     	 .value 0x105
 505 0132 96000000 	 .long 0x96
 506 0136 48       	 .byte 0x48
 507 0137 09       	 .uleb128 0x9
 508 0138 00000000 	 .long .LASF24
 509 013c 05       	 .byte 0x5
 510 013d 0601     	 .value 0x106
 511 013f 96000000 	 .long 0x96
 512 0143 50       	 .byte 0x50
 513 0144 09       	 .uleb128 0x9
 514 0145 00000000 	 .long .LASF25
 515 0149 05       	 .byte 0x5
 516 014a 0701     	 .value 0x107
 517 014c 96000000 	 .long 0x96
 518 0150 58       	 .byte 0x58
 519 0151 09       	 .uleb128 0x9
 520 0152 00000000 	 .long .LASF26
 521 0156 05       	 .byte 0x5
 522 0157 0901     	 .value 0x109
GAS LISTING  			page 15


 523 0159 67020000 	 .long 0x267
 524 015d 60       	 .byte 0x60
 525 015e 09       	 .uleb128 0x9
 526 015f 00000000 	 .long .LASF27
 527 0163 05       	 .byte 0x5
 528 0164 0B01     	 .value 0x10b
 529 0166 6D020000 	 .long 0x26d
 530 016a 68       	 .byte 0x68
 531 016b 09       	 .uleb128 0x9
 532 016c 00000000 	 .long .LASF28
 533 0170 05       	 .byte 0x5
 534 0171 0D01     	 .value 0x10d
 535 0173 5E000000 	 .long 0x5e
 536 0177 70       	 .byte 0x70
 537 0178 09       	 .uleb128 0x9
 538 0179 00000000 	 .long .LASF29
 539 017d 05       	 .byte 0x5
 540 017e 1101     	 .value 0x111
 541 0180 5E000000 	 .long 0x5e
 542 0184 74       	 .byte 0x74
 543 0185 09       	 .uleb128 0x9
 544 0186 00000000 	 .long .LASF30
 545 018a 05       	 .byte 0x5
 546 018b 1301     	 .value 0x113
 547 018d 6C000000 	 .long 0x6c
 548 0191 78       	 .byte 0x78
 549 0192 09       	 .uleb128 0x9
 550 0193 00000000 	 .long .LASF31
 551 0197 05       	 .byte 0x5
 552 0198 1701     	 .value 0x117
 553 019a 42000000 	 .long 0x42
 554 019e 80       	 .byte 0x80
 555 019f 09       	 .uleb128 0x9
 556 01a0 00000000 	 .long .LASF32
 557 01a4 05       	 .byte 0x5
 558 01a5 1801     	 .value 0x118
 559 01a7 50000000 	 .long 0x50
 560 01ab 82       	 .byte 0x82
 561 01ac 09       	 .uleb128 0x9
 562 01ad 00000000 	 .long .LASF33
 563 01b1 05       	 .byte 0x5
 564 01b2 1901     	 .value 0x119
 565 01b4 73020000 	 .long 0x273
 566 01b8 83       	 .byte 0x83
 567 01b9 09       	 .uleb128 0x9
 568 01ba 00000000 	 .long .LASF34
 569 01be 05       	 .byte 0x5
 570 01bf 1D01     	 .value 0x11d
 571 01c1 83020000 	 .long 0x283
 572 01c5 88       	 .byte 0x88
 573 01c6 09       	 .uleb128 0x9
 574 01c7 00000000 	 .long .LASF35
 575 01cb 05       	 .byte 0x5
 576 01cc 2601     	 .value 0x126
 577 01ce 77000000 	 .long 0x77
 578 01d2 90       	 .byte 0x90
 579 01d3 09       	 .uleb128 0x9
GAS LISTING  			page 16


 580 01d4 00000000 	 .long .LASF36
 581 01d8 05       	 .byte 0x5
 582 01d9 2F01     	 .value 0x12f
 583 01db 94000000 	 .long 0x94
 584 01df 98       	 .byte 0x98
 585 01e0 09       	 .uleb128 0x9
 586 01e1 00000000 	 .long .LASF37
 587 01e5 05       	 .byte 0x5
 588 01e6 3001     	 .value 0x130
 589 01e8 94000000 	 .long 0x94
 590 01ec A0       	 .byte 0xa0
 591 01ed 09       	 .uleb128 0x9
 592 01ee 00000000 	 .long .LASF38
 593 01f2 05       	 .byte 0x5
 594 01f3 3101     	 .value 0x131
 595 01f5 94000000 	 .long 0x94
 596 01f9 A8       	 .byte 0xa8
 597 01fa 09       	 .uleb128 0x9
 598 01fb 00000000 	 .long .LASF39
 599 01ff 05       	 .byte 0x5
 600 0200 3201     	 .value 0x132
 601 0202 94000000 	 .long 0x94
 602 0206 B0       	 .byte 0xb0
 603 0207 09       	 .uleb128 0x9
 604 0208 00000000 	 .long .LASF40
 605 020c 05       	 .byte 0x5
 606 020d 3301     	 .value 0x133
 607 020f 29000000 	 .long 0x29
 608 0213 B8       	 .byte 0xb8
 609 0214 09       	 .uleb128 0x9
 610 0215 00000000 	 .long .LASF41
 611 0219 05       	 .byte 0x5
 612 021a 3501     	 .value 0x135
 613 021c 5E000000 	 .long 0x5e
 614 0220 C0       	 .byte 0xc0
 615 0221 09       	 .uleb128 0x9
 616 0222 00000000 	 .long .LASF42
 617 0226 05       	 .byte 0x5
 618 0227 3701     	 .value 0x137
 619 0229 89020000 	 .long 0x289
 620 022d C4       	 .byte 0xc4
 621 022e 00       	 .byte 0
 622 022f 0A       	 .uleb128 0xa
 623 0230 00000000 	 .long .LASF131
 624 0234 05       	 .byte 0x5
 625 0235 9B       	 .byte 0x9b
 626 0236 07       	 .uleb128 0x7
 627 0237 00000000 	 .long .LASF44
 628 023b 18       	 .byte 0x18
 629 023c 05       	 .byte 0x5
 630 023d A1       	 .byte 0xa1
 631 023e 67020000 	 .long 0x267
 632 0242 08       	 .uleb128 0x8
 633 0243 00000000 	 .long .LASF45
 634 0247 05       	 .byte 0x5
 635 0248 A2       	 .byte 0xa2
 636 0249 67020000 	 .long 0x267
GAS LISTING  			page 17


 637 024d 00       	 .byte 0
 638 024e 08       	 .uleb128 0x8
 639 024f 00000000 	 .long .LASF46
 640 0253 05       	 .byte 0x5
 641 0254 A3       	 .byte 0xa3
 642 0255 6D020000 	 .long 0x26d
 643 0259 08       	 .byte 0x8
 644 025a 08       	 .uleb128 0x8
 645 025b 00000000 	 .long .LASF47
 646 025f 05       	 .byte 0x5
 647 0260 A7       	 .byte 0xa7
 648 0261 5E000000 	 .long 0x5e
 649 0265 10       	 .byte 0x10
 650 0266 00       	 .byte 0
 651 0267 06       	 .uleb128 0x6
 652 0268 08       	 .byte 0x8
 653 0269 36020000 	 .long 0x236
 654 026d 06       	 .uleb128 0x6
 655 026e 08       	 .byte 0x8
 656 026f AE000000 	 .long 0xae
 657 0273 0B       	 .uleb128 0xb
 658 0274 9C000000 	 .long 0x9c
 659 0278 83020000 	 .long 0x283
 660 027c 0C       	 .uleb128 0xc
 661 027d 82000000 	 .long 0x82
 662 0281 00       	 .byte 0
 663 0282 00       	 .byte 0
 664 0283 06       	 .uleb128 0x6
 665 0284 08       	 .byte 0x8
 666 0285 2F020000 	 .long 0x22f
 667 0289 0B       	 .uleb128 0xb
 668 028a 9C000000 	 .long 0x9c
 669 028e 99020000 	 .long 0x299
 670 0292 0C       	 .uleb128 0xc
 671 0293 82000000 	 .long 0x82
 672 0297 13       	 .byte 0x13
 673 0298 00       	 .byte 0
 674 0299 06       	 .uleb128 0x6
 675 029a 08       	 .byte 0x8
 676 029b 9F020000 	 .long 0x29f
 677 029f 0D       	 .uleb128 0xd
 678 02a0 9C000000 	 .long 0x9c
 679 02a4 06       	 .uleb128 0x6
 680 02a5 08       	 .byte 0x8
 681 02a6 5E000000 	 .long 0x5e
 682 02aa 03       	 .uleb128 0x3
 683 02ab 08       	 .byte 0x8
 684 02ac 05       	 .byte 0x5
 685 02ad 00000000 	 .long .LASF48
 686 02b1 03       	 .uleb128 0x3
 687 02b2 08       	 .byte 0x8
 688 02b3 07       	 .byte 0x7
 689 02b4 00000000 	 .long .LASF49
 690 02b8 06       	 .uleb128 0x6
 691 02b9 08       	 .byte 0x8
 692 02ba BE020000 	 .long 0x2be
 693 02be 0E       	 .uleb128 0xe
GAS LISTING  			page 18


 694 02bf 02       	 .uleb128 0x2
 695 02c0 00000000 	 .long .LASF50
 696 02c4 06       	 .byte 0x6
 697 02c5 30       	 .byte 0x30
 698 02c6 3B000000 	 .long 0x3b
 699 02ca 02       	 .uleb128 0x2
 700 02cb 00000000 	 .long .LASF51
 701 02cf 06       	 .byte 0x6
 702 02d0 31       	 .byte 0x31
 703 02d1 42000000 	 .long 0x42
 704 02d5 02       	 .uleb128 0x2
 705 02d6 00000000 	 .long .LASF52
 706 02da 06       	 .byte 0x6
 707 02db 33       	 .byte 0x33
 708 02dc 49000000 	 .long 0x49
 709 02e0 02       	 .uleb128 0x2
 710 02e1 00000000 	 .long .LASF53
 711 02e5 06       	 .byte 0x6
 712 02e6 37       	 .byte 0x37
 713 02e7 34000000 	 .long 0x34
 714 02eb 0F       	 .uleb128 0xf
 715 02ec F6020000 	 .long 0x2f6
 716 02f0 10       	 .uleb128 0x10
 717 02f1 5E000000 	 .long 0x5e
 718 02f5 00       	 .byte 0
 719 02f6 02       	 .uleb128 0x2
 720 02f7 00000000 	 .long .LASF54
 721 02fb 07       	 .byte 0x7
 722 02fc 2C       	 .byte 0x2c
 723 02fd 5E000000 	 .long 0x5e
 724 0301 02       	 .uleb128 0x2
 725 0302 00000000 	 .long .LASF55
 726 0306 08       	 .byte 0x8
 727 0307 3C       	 .byte 0x3c
 728 0308 0C030000 	 .long 0x30c
 729 030c 11       	 .uleb128 0x11
 730 030d 00000000 	 .long .LASF55
 731 0311 02       	 .uleb128 0x2
 732 0312 00000000 	 .long .LASF56
 733 0316 08       	 .byte 0x8
 734 0317 94       	 .byte 0x94
 735 0318 1C030000 	 .long 0x31c
 736 031c 06       	 .uleb128 0x6
 737 031d 08       	 .byte 0x8
 738 031e 22030000 	 .long 0x322
 739 0322 12       	 .uleb128 0x12
 740 0323 5E000000 	 .long 0x5e
 741 0327 31030000 	 .long 0x331
 742 032b 10       	 .uleb128 0x10
 743 032c 5E000000 	 .long 0x5e
 744 0330 00       	 .byte 0
 745 0331 02       	 .uleb128 0x2
 746 0332 00000000 	 .long .LASF57
 747 0336 09       	 .byte 0x9
 748 0337 29       	 .byte 0x29
 749 0338 3C030000 	 .long 0x33c
 750 033c 11       	 .uleb128 0x11
GAS LISTING  			page 19


 751 033d 00000000 	 .long .LASF57
 752 0341 02       	 .uleb128 0x2
 753 0342 00000000 	 .long .LASF58
 754 0346 0A       	 .byte 0xa
 755 0347 39       	 .byte 0x39
 756 0348 EB020000 	 .long 0x2eb
 757 034c 06       	 .uleb128 0x6
 758 034d 08       	 .byte 0x8
 759 034e 01030000 	 .long 0x301
 760 0352 02       	 .uleb128 0x2
 761 0353 00000000 	 .long .LASF59
 762 0357 0B       	 .byte 0xb
 763 0358 19       	 .byte 0x19
 764 0359 5E000000 	 .long 0x5e
 765 035d 02       	 .uleb128 0x2
 766 035e 00000000 	 .long .LASF60
 767 0362 0C       	 .byte 0xc
 768 0363 0E       	 .byte 0xe
 769 0364 68030000 	 .long 0x368
 770 0368 07       	 .uleb128 0x7
 771 0369 00000000 	 .long .LASF60
 772 036d 78       	 .byte 0x78
 773 036e 0C       	 .byte 0xc
 774 036f 2A       	 .byte 0x2a
 775 0370 AF030000 	 .long 0x3af
 776 0374 08       	 .uleb128 0x8
 777 0375 00000000 	 .long .LASF61
 778 0379 0C       	 .byte 0xc
 779 037a 2B       	 .byte 0x2b
 780 037b AF030000 	 .long 0x3af
 781 037f 00       	 .byte 0
 782 0380 13       	 .uleb128 0x13
 783 0381 6D7000   	 .string "mp"
 784 0384 0C       	 .byte 0xc
 785 0385 2C       	 .byte 0x2c
 786 0386 4C030000 	 .long 0x34c
 787 038a 58       	 .byte 0x58
 788 038b 13       	 .uleb128 0x13
 789 038c 6D6D00   	 .string "mm"
 790 038f 0C       	 .byte 0xc
 791 0390 2D       	 .byte 0x2d
 792 0391 49060000 	 .long 0x649
 793 0395 60       	 .byte 0x60
 794 0396 08       	 .uleb128 0x8
 795 0397 00000000 	 .long .LASF62
 796 039b 0C       	 .byte 0xc
 797 039c 2E       	 .byte 0x2e
 798 039d 5E000000 	 .long 0x5e
 799 03a1 68       	 .byte 0x68
 800 03a2 08       	 .uleb128 0x8
 801 03a3 00000000 	 .long .LASF63
 802 03a7 0C       	 .byte 0xc
 803 03a8 2F       	 .byte 0x2f
 804 03a9 E0020000 	 .long 0x2e0
 805 03ad 70       	 .byte 0x70
 806 03ae 00       	 .byte 0
 807 03af 02       	 .uleb128 0x2
GAS LISTING  			page 20


 808 03b0 00000000 	 .long .LASF64
 809 03b4 0C       	 .byte 0xc
 810 03b5 0F       	 .byte 0xf
 811 03b6 BA030000 	 .long 0x3ba
 812 03ba 07       	 .uleb128 0x7
 813 03bb 00000000 	 .long .LASF64
 814 03bf 58       	 .byte 0x58
 815 03c0 0C       	 .byte 0xc
 816 03c1 1F       	 .byte 0x1f
 817 03c2 1B040000 	 .long 0x41b
 818 03c6 13       	 .uleb128 0x13
 819 03c7 6D6D6200 	 .string "mmb"
 820 03cb 0C       	 .byte 0xc
 821 03cc 20       	 .byte 0x20
 822 03cd F0050000 	 .long 0x5f0
 823 03d1 00       	 .byte 0
 824 03d2 08       	 .uleb128 0x8
 825 03d3 00000000 	 .long .LASF65
 826 03d7 0C       	 .byte 0xc
 827 03d8 21       	 .byte 0x21
 828 03d9 4F060000 	 .long 0x64f
 829 03dd 28       	 .byte 0x28
 830 03de 08       	 .uleb128 0x8
 831 03df 00000000 	 .long .LASF66
 832 03e3 0C       	 .byte 0xc
 833 03e4 22       	 .byte 0x22
 834 03e5 D5020000 	 .long 0x2d5
 835 03e9 40       	 .byte 0x40
 836 03ea 08       	 .uleb128 0x8
 837 03eb 00000000 	 .long .LASF67
 838 03ef 0C       	 .byte 0xc
 839 03f0 23       	 .byte 0x23
 840 03f1 D5020000 	 .long 0x2d5
 841 03f5 44       	 .byte 0x44
 842 03f6 08       	 .uleb128 0x8
 843 03f7 00000000 	 .long .LASF68
 844 03fb 0C       	 .byte 0xc
 845 03fc 24       	 .byte 0x24
 846 03fd D5020000 	 .long 0x2d5
 847 0401 48       	 .byte 0x48
 848 0402 08       	 .uleb128 0x8
 849 0403 00000000 	 .long .LASF69
 850 0407 0C       	 .byte 0xc
 851 0408 25       	 .byte 0x25
 852 0409 D5020000 	 .long 0x2d5
 853 040d 4C       	 .byte 0x4c
 854 040e 08       	 .uleb128 0x8
 855 040f 00000000 	 .long .LASF70
 856 0413 0C       	 .byte 0xc
 857 0414 26       	 .byte 0x26
 858 0415 D5020000 	 .long 0x2d5
 859 0419 50       	 .byte 0x50
 860 041a 00       	 .byte 0
 861 041b 02       	 .uleb128 0x2
 862 041c 00000000 	 .long .LASF71
 863 0420 0C       	 .byte 0xc
 864 0421 10       	 .byte 0x10
GAS LISTING  			page 21


 865 0422 26040000 	 .long 0x426
 866 0426 07       	 .uleb128 0x7
 867 0427 00000000 	 .long .LASF71
 868 042b 40       	 .byte 0x40
 869 042c 0C       	 .byte 0xc
 870 042d 32       	 .byte 0x32
 871 042e DB040000 	 .long 0x4db
 872 0432 08       	 .uleb128 0x8
 873 0433 00000000 	 .long .LASF66
 874 0437 0C       	 .byte 0xc
 875 0438 33       	 .byte 0x33
 876 0439 D5020000 	 .long 0x2d5
 877 043d 00       	 .byte 0
 878 043e 08       	 .uleb128 0x8
 879 043f 00000000 	 .long .LASF63
 880 0443 0C       	 .byte 0xc
 881 0444 34       	 .byte 0x34
 882 0445 D5020000 	 .long 0x2d5
 883 0449 04       	 .byte 0x4
 884 044a 08       	 .uleb128 0x8
 885 044b 00000000 	 .long .LASF72
 886 044f 0C       	 .byte 0xc
 887 0450 37       	 .byte 0x37
 888 0451 BF020000 	 .long 0x2bf
 889 0455 08       	 .byte 0x8
 890 0456 08       	 .uleb128 0x8
 891 0457 00000000 	 .long .LASF73
 892 045b 0C       	 .byte 0xc
 893 045c 38       	 .byte 0x38
 894 045d BF020000 	 .long 0x2bf
 895 0461 09       	 .byte 0x9
 896 0462 08       	 .uleb128 0x8
 897 0463 00000000 	 .long .LASF74
 898 0467 0C       	 .byte 0xc
 899 0468 39       	 .byte 0x39
 900 0469 D5020000 	 .long 0x2d5
 901 046d 0C       	 .byte 0xc
 902 046e 08       	 .uleb128 0x8
 903 046f 00000000 	 .long .LASF75
 904 0473 0C       	 .byte 0xc
 905 0474 3A       	 .byte 0x3a
 906 0475 D5020000 	 .long 0x2d5
 907 0479 10       	 .byte 0x10
 908 047a 08       	 .uleb128 0x8
 909 047b 00000000 	 .long .LASF76
 910 047f 0C       	 .byte 0xc
 911 0480 3C       	 .byte 0x3c
 912 0481 E0020000 	 .long 0x2e0
 913 0485 18       	 .byte 0x18
 914 0486 08       	 .uleb128 0x8
 915 0487 00000000 	 .long .LASF77
 916 048b 0C       	 .byte 0xc
 917 048c 3E       	 .byte 0x3e
 918 048d D5020000 	 .long 0x2d5
 919 0491 20       	 .byte 0x20
 920 0492 08       	 .uleb128 0x8
 921 0493 00000000 	 .long .LASF78
GAS LISTING  			page 22


 922 0497 0C       	 .byte 0xc
 923 0498 3F       	 .byte 0x3f
 924 0499 D5020000 	 .long 0x2d5
 925 049d 24       	 .byte 0x24
 926 049e 08       	 .uleb128 0x8
 927 049f 00000000 	 .long .LASF79
 928 04a3 0C       	 .byte 0xc
 929 04a4 40       	 .byte 0x40
 930 04a5 CA020000 	 .long 0x2ca
 931 04a9 28       	 .byte 0x28
 932 04aa 08       	 .uleb128 0x8
 933 04ab 00000000 	 .long .LASF80
 934 04af 0C       	 .byte 0xc
 935 04b0 41       	 .byte 0x41
 936 04b1 CA020000 	 .long 0x2ca
 937 04b5 2A       	 .byte 0x2a
 938 04b6 08       	 .uleb128 0x8
 939 04b7 00000000 	 .long .LASF81
 940 04bb 0C       	 .byte 0xc
 941 04bc 43       	 .byte 0x43
 942 04bd D5020000 	 .long 0x2d5
 943 04c1 2C       	 .byte 0x2c
 944 04c2 08       	 .uleb128 0x8
 945 04c3 00000000 	 .long .LASF82
 946 04c7 0C       	 .byte 0xc
 947 04c8 44       	 .byte 0x44
 948 04c9 29000000 	 .long 0x29
 949 04cd 30       	 .byte 0x30
 950 04ce 08       	 .uleb128 0x8
 951 04cf 00000000 	 .long .LASF83
 952 04d3 0C       	 .byte 0xc
 953 04d4 45       	 .byte 0x45
 954 04d5 94000000 	 .long 0x94
 955 04d9 38       	 .byte 0x38
 956 04da 00       	 .byte 0
 957 04db 02       	 .uleb128 0x2
 958 04dc 00000000 	 .long .LASF84
 959 04e0 0C       	 .byte 0xc
 960 04e1 11       	 .byte 0x11
 961 04e2 E6040000 	 .long 0x4e6
 962 04e6 07       	 .uleb128 0x7
 963 04e7 00000000 	 .long .LASF84
 964 04eb 18       	 .byte 0x18
 965 04ec 0C       	 .byte 0xc
 966 04ed 4F       	 .byte 0x4f
 967 04ee 17050000 	 .long 0x517
 968 04f2 08       	 .uleb128 0x8
 969 04f3 00000000 	 .long .LASF85
 970 04f7 0C       	 .byte 0xc
 971 04f8 51       	 .byte 0x51
 972 04f9 8B060000 	 .long 0x68b
 973 04fd 00       	 .byte 0
 974 04fe 08       	 .uleb128 0x8
 975 04ff 00000000 	 .long .LASF86
 976 0503 0C       	 .byte 0xc
 977 0504 52       	 .byte 0x52
 978 0505 AC060000 	 .long 0x6ac
GAS LISTING  			page 23


 979 0509 08       	 .byte 0x8
 980 050a 08       	 .uleb128 0x8
 981 050b 00000000 	 .long .LASF87
 982 050f 0C       	 .byte 0xc
 983 0510 53       	 .byte 0x53
 984 0511 D5020000 	 .long 0x2d5
 985 0515 10       	 .byte 0x10
 986 0516 00       	 .byte 0
 987 0517 02       	 .uleb128 0x2
 988 0518 00000000 	 .long .LASF88
 989 051c 0D       	 .byte 0xd
 990 051d 0C       	 .byte 0xc
 991 051e 22050000 	 .long 0x522
 992 0522 07       	 .uleb128 0x7
 993 0523 00000000 	 .long .LASF88
 994 0527 38       	 .byte 0x38
 995 0528 0D       	 .byte 0xd
 996 0529 12       	 .byte 0x12
 997 052a 84050000 	 .long 0x584
 998 052e 08       	 .uleb128 0x8
 999 052f 00000000 	 .long .LASF89
 1000 0533 0D       	 .byte 0xd
 1001 0534 14       	 .byte 0x14
 1002 0535 E0020000 	 .long 0x2e0
 1003 0539 00       	 .byte 0
 1004 053a 13       	 .uleb128 0x13
 1005 053b 6D7000   	 .string "mp"
 1006 053e 0D       	 .byte 0xd
 1007 053f 16       	 .byte 0x16
 1008 0540 4C030000 	 .long 0x34c
 1009 0544 08       	 .byte 0x8
 1010 0545 08       	 .uleb128 0x8
 1011 0546 00000000 	 .long .LASF90
 1012 054a 0D       	 .byte 0xd
 1013 054b 19       	 .byte 0x19
 1014 054c 99020000 	 .long 0x299
 1015 0550 10       	 .byte 0x10
 1016 0551 13       	 .uleb128 0x13
 1017 0552 666400   	 .string "fd"
 1018 0555 0D       	 .byte 0xd
 1019 0556 1C       	 .byte 0x1c
 1020 0557 5E000000 	 .long 0x5e
 1021 055b 18       	 .byte 0x18
 1022 055c 08       	 .uleb128 0x8
 1023 055d 00000000 	 .long .LASF91
 1024 0561 0D       	 .byte 0xd
 1025 0562 1F       	 .byte 0x1f
 1026 0563 E0020000 	 .long 0x2e0
 1027 0567 20       	 .byte 0x20
 1028 0568 14       	 .uleb128 0x14
 1029 0569 00000000 	 .long .LASF132
 1030 056d 0D       	 .byte 0xd
 1031 056e 22       	 .byte 0x22
 1032 056f 49000000 	 .long 0x49
 1033 0573 04       	 .byte 0x4
 1034 0574 01       	 .byte 0x1
 1035 0575 1F       	 .byte 0x1f
GAS LISTING  			page 24


 1036 0576 28       	 .byte 0x28
 1037 0577 08       	 .uleb128 0x8
 1038 0578 00000000 	 .long .LASF92
 1039 057c 0D       	 .byte 0xd
 1040 057d 25       	 .byte 0x25
 1041 057e 43060000 	 .long 0x643
 1042 0582 30       	 .byte 0x30
 1043 0583 00       	 .byte 0
 1044 0584 02       	 .uleb128 0x2
 1045 0585 00000000 	 .long .LASF93
 1046 0589 0D       	 .byte 0xd
 1047 058a 0D       	 .byte 0xd
 1048 058b 8F050000 	 .long 0x58f
 1049 058f 07       	 .uleb128 0x7
 1050 0590 00000000 	 .long .LASF93
 1051 0594 38       	 .byte 0x38
 1052 0595 0D       	 .byte 0xd
 1053 0596 29       	 .byte 0x29
 1054 0597 F0050000 	 .long 0x5f0
 1055 059b 08       	 .uleb128 0x8
 1056 059c 00000000 	 .long .LASF94
 1057 05a0 0D       	 .byte 0xd
 1058 05a1 2A       	 .byte 0x2a
 1059 05a2 E0020000 	 .long 0x2e0
 1060 05a6 00       	 .byte 0
 1061 05a7 08       	 .uleb128 0x8
 1062 05a8 00000000 	 .long .LASF95
 1063 05ac 0D       	 .byte 0xd
 1064 05ad 2B       	 .byte 0x2b
 1065 05ae E0020000 	 .long 0x2e0
 1066 05b2 08       	 .byte 0x8
 1067 05b3 08       	 .uleb128 0x8
 1068 05b4 00000000 	 .long .LASF96
 1069 05b8 0D       	 .byte 0xd
 1070 05b9 2C       	 .byte 0x2c
 1071 05ba E0020000 	 .long 0x2e0
 1072 05be 10       	 .byte 0x10
 1073 05bf 08       	 .uleb128 0x8
 1074 05c0 00000000 	 .long .LASF97
 1075 05c4 0D       	 .byte 0xd
 1076 05c5 2D       	 .byte 0x2d
 1077 05c6 E0020000 	 .long 0x2e0
 1078 05ca 18       	 .byte 0x18
 1079 05cb 08       	 .uleb128 0x8
 1080 05cc 00000000 	 .long .LASF98
 1081 05d0 0D       	 .byte 0xd
 1082 05d1 2E       	 .byte 0x2e
 1083 05d2 E0020000 	 .long 0x2e0
 1084 05d6 20       	 .byte 0x20
 1085 05d7 08       	 .uleb128 0x8
 1086 05d8 00000000 	 .long .LASF99
 1087 05dc 0D       	 .byte 0xd
 1088 05dd 2F       	 .byte 0x2f
 1089 05de E0020000 	 .long 0x2e0
 1090 05e2 28       	 .byte 0x28
 1091 05e3 08       	 .uleb128 0x8
 1092 05e4 00000000 	 .long .LASF100
GAS LISTING  			page 25


 1093 05e8 0D       	 .byte 0xd
 1094 05e9 30       	 .byte 0x30
 1095 05ea E0020000 	 .long 0x2e0
 1096 05ee 30       	 .byte 0x30
 1097 05ef 00       	 .byte 0
 1098 05f0 02       	 .uleb128 0x2
 1099 05f1 00000000 	 .long .LASF101
 1100 05f5 0D       	 .byte 0xd
 1101 05f6 0E       	 .byte 0xe
 1102 05f7 FB050000 	 .long 0x5fb
 1103 05fb 07       	 .uleb128 0x7
 1104 05fc 00000000 	 .long .LASF101
 1105 0600 28       	 .byte 0x28
 1106 0601 0D       	 .byte 0xd
 1107 0602 35       	 .byte 0x35
 1108 0603 43060000 	 .long 0x643
 1109 0607 13       	 .uleb128 0x13
 1110 0608 6D6D00   	 .string "mm"
 1111 060b 0D       	 .byte 0xd
 1112 060c 36       	 .byte 0x36
 1113 060d 49060000 	 .long 0x649
 1114 0611 00       	 .byte 0
 1115 0612 08       	 .uleb128 0x8
 1116 0613 00000000 	 .long .LASF102
 1117 0617 0D       	 .byte 0xd
 1118 0618 37       	 .byte 0x37
 1119 0619 E0020000 	 .long 0x2e0
 1120 061d 08       	 .byte 0x8
 1121 061e 08       	 .uleb128 0x8
 1122 061f 00000000 	 .long .LASF103
 1123 0623 0D       	 .byte 0xd
 1124 0624 38       	 .byte 0x38
 1125 0625 94000000 	 .long 0x94
 1126 0629 10       	 .byte 0x10
 1127 062a 13       	 .uleb128 0x13
 1128 062b 706F7300 	 .string "pos"
 1129 062f 0D       	 .byte 0xd
 1130 0630 39       	 .byte 0x39
 1131 0631 94000000 	 .long 0x94
 1132 0635 18       	 .byte 0x18
 1133 0636 13       	 .uleb128 0x13
 1134 0637 656E6400 	 .string "end"
 1135 063b 0D       	 .byte 0xd
 1136 063c 3A       	 .byte 0x3a
 1137 063d 94000000 	 .long 0x94
 1138 0641 20       	 .byte 0x20
 1139 0642 00       	 .byte 0
 1140 0643 06       	 .uleb128 0x6
 1141 0644 08       	 .byte 0x8
 1142 0645 84050000 	 .long 0x584
 1143 0649 06       	 .uleb128 0x6
 1144 064a 08       	 .byte 0x8
 1145 064b 17050000 	 .long 0x517
 1146 064f 02       	 .uleb128 0x2
 1147 0650 00000000 	 .long .LASF104
 1148 0654 0E       	 .byte 0xe
 1149 0655 0B       	 .byte 0xb
GAS LISTING  			page 26


 1150 0656 5A060000 	 .long 0x65a
 1151 065a 07       	 .uleb128 0x7
 1152 065b 00000000 	 .long .LASF104
 1153 065f 18       	 .byte 0x18
 1154 0660 0E       	 .byte 0xe
 1155 0661 11       	 .byte 0x11
 1156 0662 8B060000 	 .long 0x68b
 1157 0666 08       	 .uleb128 0x8
 1158 0667 00000000 	 .long .LASF103
 1159 066b 0E       	 .byte 0xe
 1160 066c 13       	 .byte 0x13
 1161 066d 94000000 	 .long 0x94
 1162 0671 00       	 .byte 0
 1163 0672 13       	 .uleb128 0x13
 1164 0673 706F7300 	 .string "pos"
 1165 0677 0E       	 .byte 0xe
 1166 0678 14       	 .byte 0x14
 1167 0679 94000000 	 .long 0x94
 1168 067d 08       	 .byte 0x8
 1169 067e 13       	 .uleb128 0x13
 1170 067f 656E6400 	 .string "end"
 1171 0683 0E       	 .byte 0xe
 1172 0684 15       	 .byte 0x15
 1173 0685 94000000 	 .long 0x94
 1174 0689 10       	 .byte 0x10
 1175 068a 00       	 .byte 0
 1176 068b 06       	 .uleb128 0x6
 1177 068c 08       	 .byte 0x8
 1178 068d AF030000 	 .long 0x3af
 1179 0691 12       	 .uleb128 0x12
 1180 0692 A0060000 	 .long 0x6a0
 1181 0696 A0060000 	 .long 0x6a0
 1182 069a 10       	 .uleb128 0x10
 1183 069b A6060000 	 .long 0x6a6
 1184 069f 00       	 .byte 0
 1185 06a0 06       	 .uleb128 0x6
 1186 06a1 08       	 .byte 0x8
 1187 06a2 1B040000 	 .long 0x41b
 1188 06a6 06       	 .uleb128 0x6
 1189 06a7 08       	 .byte 0x8
 1190 06a8 DB040000 	 .long 0x4db
 1191 06ac 06       	 .uleb128 0x6
 1192 06ad 08       	 .byte 0x8
 1193 06ae 91060000 	 .long 0x691
 1194 06b2 15       	 .uleb128 0x15
 1195 06b3 00000000 	 .long .LASF133
 1196 06b7 01       	 .byte 0x1
 1197 06b8 1A       	 .byte 0x1a
 1198 06b9 00000000 	 .quad .LFB96
 1198      00000000 
 1199 06c1 52000000 	 .quad .LFE96-.LFB96
 1199      00000000 
 1200 06c9 01       	 .uleb128 0x1
 1201 06ca 9C       	 .byte 0x9c
 1202 06cb 84070000 	 .long 0x784
 1203 06cf 16       	 .uleb128 0x16
 1204 06d0 00000000 	 .long .LASF107
GAS LISTING  			page 27


 1205 06d4 01       	 .byte 0x1
 1206 06d5 1A       	 .byte 0x1a
 1207 06d6 5E000000 	 .long 0x5e
 1208 06da 00000000 	 .long .LLST0
 1209 06de 17       	 .uleb128 0x17
 1210 06df 00000000 	 .quad .LVL2
 1210      00000000 
 1211 06e7 260B0000 	 .long 0xb26
 1212 06eb FD060000 	 .long 0x6fd
 1213 06ef 18       	 .uleb128 0x18
 1214 06f0 01       	 .uleb128 0x1
 1215 06f1 55       	 .byte 0x55
 1216 06f2 09       	 .uleb128 0x9
 1217 06f3 03       	 .byte 0x3
 1218 06f4 00000000 	 .quad .LC1
 1218      00000000 
 1219 06fc 00       	 .byte 0
 1220 06fd 17       	 .uleb128 0x17
 1221 06fe 00000000 	 .quad .LVL3
 1221      00000000 
 1222 0706 3F0B0000 	 .long 0xb3f
 1223 070a 14070000 	 .long 0x714
 1224 070e 18       	 .uleb128 0x18
 1225 070f 01       	 .uleb128 0x1
 1226 0710 55       	 .byte 0x55
 1227 0711 01       	 .uleb128 0x1
 1228 0712 30       	 .byte 0x30
 1229 0713 00       	 .byte 0
 1230 0714 17       	 .uleb128 0x17
 1231 0715 00000000 	 .quad .LVL5
 1231      00000000 
 1232 071d 510B0000 	 .long 0xb51
 1233 0721 3A070000 	 .long 0x73a
 1234 0725 18       	 .uleb128 0x18
 1235 0726 01       	 .uleb128 0x1
 1236 0727 55       	 .byte 0x55
 1237 0728 09       	 .uleb128 0x9
 1238 0729 03       	 .byte 0x3
 1239 072a 00000000 	 .quad .LC2
 1239      00000000 
 1240 0732 18       	 .uleb128 0x18
 1241 0733 01       	 .uleb128 0x1
 1242 0734 54       	 .byte 0x54
 1243 0735 03       	 .uleb128 0x3
 1244 0736 F3       	 .byte 0xf3
 1245 0737 01       	 .uleb128 0x1
 1246 0738 55       	 .byte 0x55
 1247 0739 00       	 .byte 0
 1248 073a 17       	 .uleb128 0x17
 1249 073b 00000000 	 .quad .LVL6
 1249      00000000 
 1250 0743 3F0B0000 	 .long 0xb3f
 1251 0747 51070000 	 .long 0x751
 1252 074b 18       	 .uleb128 0x18
 1253 074c 01       	 .uleb128 0x1
 1254 074d 55       	 .byte 0x55
 1255 074e 01       	 .uleb128 0x1
GAS LISTING  			page 28


 1256 074f 31       	 .byte 0x31
 1257 0750 00       	 .byte 0
 1258 0751 17       	 .uleb128 0x17
 1259 0752 00000000 	 .quad .LVL8
 1259      00000000 
 1260 075a 260B0000 	 .long 0xb26
 1261 075e 70070000 	 .long 0x770
 1262 0762 18       	 .uleb128 0x18
 1263 0763 01       	 .uleb128 0x1
 1264 0764 55       	 .byte 0x55
 1265 0765 09       	 .uleb128 0x9
 1266 0766 03       	 .byte 0x3
 1267 0767 00000000 	 .quad .LC0
 1267      00000000 
 1268 076f 00       	 .byte 0
 1269 0770 19       	 .uleb128 0x19
 1270 0771 00000000 	 .quad .LVL9
 1270      00000000 
 1271 0779 3F0B0000 	 .long 0xb3f
 1272 077d 18       	 .uleb128 0x18
 1273 077e 01       	 .uleb128 0x1
 1274 077f 55       	 .byte 0x55
 1275 0780 01       	 .uleb128 0x1
 1276 0781 30       	 .byte 0x30
 1277 0782 00       	 .byte 0
 1278 0783 00       	 .byte 0
 1279 0784 1A       	 .uleb128 0x1a
 1280 0785 00000000 	 .long .LASF134
 1281 0789 01       	 .byte 0x1
 1282 078a 2D       	 .byte 0x2d
 1283 078b 01       	 .byte 0x1
 1284 078c BE070000 	 .long 0x7be
 1285 0790 1B       	 .uleb128 0x1b
 1286 0791 00000000 	 .long .LASF110
 1287 0795 01       	 .byte 0x1
 1288 0796 2D       	 .byte 0x2d
 1289 0797 BE070000 	 .long 0x7be
 1290 079b 1C       	 .uleb128 0x1c
 1291 079c 00000000 	 .long .LASF105
 1292 07a0 01       	 .byte 0x1
 1293 07a1 2F       	 .byte 0x2f
 1294 07a2 A6060000 	 .long 0x6a6
 1295 07a6 1D       	 .uleb128 0x1d
 1296 07a7 706500   	 .string "pe"
 1297 07aa 01       	 .byte 0x1
 1298 07ab 30       	 .byte 0x30
 1299 07ac A0060000 	 .long 0x6a0
 1300 07b0 1E       	 .uleb128 0x1e
 1301 07b1 00000000 	 .long .LASF106
 1302 07b5 D4070000 	 .long 0x7d4
 1303 07b9 00000000 	 .long .LASF134
 1304 07bd 00       	 .byte 0
 1305 07be 06       	 .uleb128 0x6
 1306 07bf 08       	 .byte 0x8
 1307 07c0 5D030000 	 .long 0x35d
 1308 07c4 0B       	 .uleb128 0xb
 1309 07c5 9C000000 	 .long 0x9c
GAS LISTING  			page 29


 1310 07c9 D4070000 	 .long 0x7d4
 1311 07cd 0C       	 .uleb128 0xc
 1312 07ce 82000000 	 .long 0x82
 1313 07d2 16       	 .byte 0x16
 1314 07d3 00       	 .byte 0
 1315 07d4 0D       	 .uleb128 0xd
 1316 07d5 C4070000 	 .long 0x7c4
 1317 07d9 1F       	 .uleb128 0x1f
 1318 07da 00000000 	 .long .LASF135
 1319 07de 01       	 .byte 0x1
 1320 07df 50       	 .byte 0x50
 1321 07e0 5E000000 	 .long 0x5e
 1322 07e4 00000000 	 .quad .LFB98
 1322      00000000 
 1323 07ec DD010000 	 .quad .LFE98-.LFB98
 1323      00000000 
 1324 07f4 01       	 .uleb128 0x1
 1325 07f5 9C       	 .byte 0x9c
 1326 07f6 EA0A0000 	 .long 0xaea
 1327 07fa 16       	 .uleb128 0x16
 1328 07fb 00000000 	 .long .LASF108
 1329 07ff 01       	 .byte 0x1
 1330 0800 50       	 .byte 0x50
 1331 0801 5E000000 	 .long 0x5e
 1332 0805 00000000 	 .long .LLST1
 1333 0809 16       	 .uleb128 0x16
 1334 080a 00000000 	 .long .LASF109
 1335 080e 01       	 .byte 0x1
 1336 080f 50       	 .byte 0x50
 1337 0810 EA0A0000 	 .long 0xaea
 1338 0814 00000000 	 .long .LLST2
 1339 0818 20       	 .uleb128 0x20
 1340 0819 00000000 	 .long .LASF90
 1341 081d 01       	 .byte 0x1
 1342 081e 52       	 .byte 0x52
 1343 081f 99020000 	 .long 0x299
 1344 0823 00000000 	 .long .LLST3
 1345 0827 21       	 .uleb128 0x21
 1346 0828 6D7000   	 .string "mp"
 1347 082b 01       	 .byte 0x1
 1348 082c 53       	 .byte 0x53
 1349 082d 4C030000 	 .long 0x34c
 1350 0831 02       	 .uleb128 0x2
 1351 0832 91       	 .byte 0x91
 1352 0833 58       	 .sleb128 -40
 1353 0834 22       	 .uleb128 0x22
 1354 0835 726300   	 .string "rc"
 1355 0838 01       	 .byte 0x1
 1356 0839 54       	 .byte 0x54
 1357 083a 5E000000 	 .long 0x5e
 1358 083e 00000000 	 .long .LLST4
 1359 0842 20       	 .uleb128 0x20
 1360 0843 00000000 	 .long .LASF110
 1361 0847 01       	 .byte 0x1
 1362 0848 55       	 .byte 0x55
 1363 0849 BE070000 	 .long 0x7be
 1364 084d 00000000 	 .long .LLST5
GAS LISTING  			page 30


 1365 0851 23       	 .uleb128 0x23
 1366 0852 84070000 	 .long 0x784
 1367 0856 00000000 	 .quad .LBB4
 1367      00000000 
 1368 085e 00000000 	 .long .Ldebug_ranges0+0
 1369 0862 01       	 .byte 0x1
 1370 0863 7F       	 .byte 0x7f
 1371 0864 5C090000 	 .long 0x95c
 1372 0868 24       	 .uleb128 0x24
 1373 0869 90070000 	 .long 0x790
 1374 086d 25       	 .uleb128 0x25
 1375 086e 00000000 	 .long .Ldebug_ranges0+0
 1376 0872 26       	 .uleb128 0x26
 1377 0873 9B070000 	 .long 0x79b
 1378 0877 00000000 	 .long .LLST6
 1379 087b 26       	 .uleb128 0x26
 1380 087c A6070000 	 .long 0x7a6
 1381 0880 00000000 	 .long .LLST7
 1382 0884 27       	 .uleb128 0x27
 1383 0885 B0070000 	 .long 0x7b0
 1384 0889 09       	 .uleb128 0x9
 1385 088a 03       	 .byte 0x3
 1386 088b 00000000 	 .quad __func__.5664
 1386      00000000 
 1387 0893 17       	 .uleb128 0x17
 1388 0894 00000000 	 .quad .LVL23
 1388      00000000 
 1389 089c 680B0000 	 .long 0xb68
 1390 08a0 AB080000 	 .long 0x8ab
 1391 08a4 18       	 .uleb128 0x18
 1392 08a5 01       	 .uleb128 0x1
 1393 08a6 55       	 .byte 0x55
 1394 08a7 02       	 .uleb128 0x2
 1395 08a8 76       	 .byte 0x76
 1396 08a9 00       	 .sleb128 0
 1397 08aa 00       	 .byte 0
 1398 08ab 28       	 .uleb128 0x28
 1399 08ac 00000000 	 .quad .LVL26
 1399      00000000 
 1400 08b4 7D0B0000 	 .long 0xb7d
 1401 08b8 29       	 .uleb128 0x29
 1402 08b9 00000000 	 .quad .LVL27
 1402      00000000 
 1403 08c1 CC080000 	 .long 0x8cc
 1404 08c5 18       	 .uleb128 0x18
 1405 08c6 01       	 .uleb128 0x1
 1406 08c7 55       	 .byte 0x55
 1407 08c8 02       	 .uleb128 0x2
 1408 08c9 73       	 .byte 0x73
 1409 08ca 00       	 .sleb128 0
 1410 08cb 00       	 .byte 0
 1411 08cc 17       	 .uleb128 0x17
 1412 08cd 00000000 	 .quad .LVL28
 1412      00000000 
 1413 08d5 990B0000 	 .long 0xb99
 1414 08d9 EA080000 	 .long 0x8ea
 1415 08dd 18       	 .uleb128 0x18
GAS LISTING  			page 31


 1416 08de 01       	 .uleb128 0x1
 1417 08df 55       	 .byte 0x55
 1418 08e0 02       	 .uleb128 0x2
 1419 08e1 76       	 .byte 0x76
 1420 08e2 00       	 .sleb128 0
 1421 08e3 18       	 .uleb128 0x18
 1422 08e4 01       	 .uleb128 0x1
 1423 08e5 54       	 .byte 0x54
 1424 08e6 02       	 .uleb128 0x2
 1425 08e7 73       	 .byte 0x73
 1426 08e8 00       	 .sleb128 0
 1427 08e9 00       	 .byte 0
 1428 08ea 17       	 .uleb128 0x17
 1429 08eb 00000000 	 .quad .LVL29
 1429      00000000 
 1430 08f3 990B0000 	 .long 0xb99
 1431 08f7 08090000 	 .long 0x908
 1432 08fb 18       	 .uleb128 0x18
 1433 08fc 01       	 .uleb128 0x1
 1434 08fd 55       	 .byte 0x55
 1435 08fe 02       	 .uleb128 0x2
 1436 08ff 76       	 .byte 0x76
 1437 0900 00       	 .sleb128 0
 1438 0901 18       	 .uleb128 0x18
 1439 0902 01       	 .uleb128 0x1
 1440 0903 54       	 .byte 0x54
 1441 0904 02       	 .uleb128 0x2
 1442 0905 73       	 .byte 0x73
 1443 0906 00       	 .sleb128 0
 1444 0907 00       	 .byte 0
 1445 0908 17       	 .uleb128 0x17
 1446 0909 00000000 	 .quad .LVL34
 1446      00000000 
 1447 0911 AF0B0000 	 .long 0xbaf
 1448 0915 43090000 	 .long 0x943
 1449 0919 18       	 .uleb128 0x18
 1450 091a 01       	 .uleb128 0x1
 1451 091b 55       	 .byte 0x55
 1452 091c 01       	 .uleb128 0x1
 1453 091d 37       	 .byte 0x37
 1454 091e 18       	 .uleb128 0x18
 1455 091f 01       	 .uleb128 0x1
 1456 0920 54       	 .byte 0x54
 1457 0921 01       	 .uleb128 0x1
 1458 0922 30       	 .byte 0x30
 1459 0923 18       	 .uleb128 0x18
 1460 0924 01       	 .uleb128 0x1
 1461 0925 51       	 .byte 0x51
 1462 0926 09       	 .uleb128 0x9
 1463 0927 03       	 .byte 0x3
 1464 0928 00000000 	 .quad .LC8
 1464      00000000 
 1465 0930 18       	 .uleb128 0x18
 1466 0931 01       	 .uleb128 0x1
 1467 0932 52       	 .byte 0x52
 1468 0933 09       	 .uleb128 0x9
 1469 0934 03       	 .byte 0x3
GAS LISTING  			page 32


 1470 0935 00000000 	 .quad __func__.5664
 1470      00000000 
 1471 093d 18       	 .uleb128 0x18
 1472 093e 01       	 .uleb128 0x1
 1473 093f 58       	 .byte 0x58
 1474 0940 01       	 .uleb128 0x1
 1475 0941 31       	 .byte 0x31
 1476 0942 00       	 .byte 0
 1477 0943 19       	 .uleb128 0x19
 1478 0944 00000000 	 .quad .LVL35
 1478      00000000 
 1479 094c CB0B0000 	 .long 0xbcb
 1480 0950 18       	 .uleb128 0x18
 1481 0951 01       	 .uleb128 0x1
 1482 0952 55       	 .byte 0x55
 1483 0953 05       	 .uleb128 0x5
 1484 0954 0C       	 .byte 0xc
 1485 0955 40420F00 	 .long 0xf4240
 1486 0959 00       	 .byte 0
 1487 095a 00       	 .byte 0
 1488 095b 00       	 .byte 0
 1489 095c 17       	 .uleb128 0x17
 1490 095d 00000000 	 .quad .LVL14
 1490      00000000 
 1491 0965 E10B0000 	 .long 0xbe1
 1492 0969 7F090000 	 .long 0x97f
 1493 096d 18       	 .uleb128 0x18
 1494 096e 01       	 .uleb128 0x1
 1495 096f 55       	 .byte 0x55
 1496 0970 02       	 .uleb128 0x2
 1497 0971 91       	 .byte 0x91
 1498 0972 4C       	 .sleb128 -52
 1499 0973 18       	 .uleb128 0x18
 1500 0974 01       	 .uleb128 0x1
 1501 0975 54       	 .byte 0x54
 1502 0976 02       	 .uleb128 0x2
 1503 0977 77       	 .byte 0x77
 1504 0978 00       	 .sleb128 0
 1505 0979 18       	 .uleb128 0x18
 1506 097a 01       	 .uleb128 0x1
 1507 097b 51       	 .byte 0x51
 1508 097c 01       	 .uleb128 0x1
 1509 097d 30       	 .byte 0x30
 1510 097e 00       	 .byte 0
 1511 097f 17       	 .uleb128 0x17
 1512 0980 00000000 	 .quad .LVL15
 1512      00000000 
 1513 0988 110C0000 	 .long 0xc11
 1514 098c A6090000 	 .long 0x9a6
 1515 0990 18       	 .uleb128 0x18
 1516 0991 01       	 .uleb128 0x1
 1517 0992 55       	 .byte 0x55
 1518 0993 02       	 .uleb128 0x2
 1519 0994 91       	 .byte 0x91
 1520 0995 58       	 .sleb128 -40
 1521 0996 18       	 .uleb128 0x18
 1522 0997 01       	 .uleb128 0x1
GAS LISTING  			page 33


 1523 0998 54       	 .byte 0x54
 1524 0999 01       	 .uleb128 0x1
 1525 099a 30       	 .byte 0x30
 1526 099b 18       	 .uleb128 0x18
 1527 099c 01       	 .uleb128 0x1
 1528 099d 51       	 .byte 0x51
 1529 099e 01       	 .uleb128 0x1
 1530 099f 30       	 .byte 0x30
 1531 09a0 18       	 .uleb128 0x18
 1532 09a1 01       	 .uleb128 0x1
 1533 09a2 52       	 .byte 0x52
 1534 09a3 01       	 .uleb128 0x1
 1535 09a4 30       	 .byte 0x30
 1536 09a5 00       	 .byte 0
 1537 09a6 28       	 .uleb128 0x28
 1538 09a7 00000000 	 .quad .LVL16
 1538      00000000 
 1539 09af 410C0000 	 .long 0xc41
 1540 09b3 17       	 .uleb128 0x17
 1541 09b4 00000000 	 .quad .LVL17
 1541      00000000 
 1542 09bc 4C0C0000 	 .long 0xc4c
 1543 09c0 D7090000 	 .long 0x9d7
 1544 09c4 18       	 .uleb128 0x18
 1545 09c5 01       	 .uleb128 0x1
 1546 09c6 54       	 .byte 0x54
 1547 09c7 09       	 .uleb128 0x9
 1548 09c8 03       	 .byte 0x3
 1549 09c9 00000000 	 .quad .LC6
 1549      00000000 
 1550 09d1 18       	 .uleb128 0x18
 1551 09d2 01       	 .uleb128 0x1
 1552 09d3 51       	 .byte 0x51
 1553 09d4 01       	 .uleb128 0x1
 1554 09d5 38       	 .byte 0x38
 1555 09d6 00       	 .byte 0
 1556 09d7 17       	 .uleb128 0x17
 1557 09d8 00000000 	 .quad .LVL18
 1557      00000000 
 1558 09e0 670C0000 	 .long 0xc67
 1559 09e4 FB090000 	 .long 0x9fb
 1560 09e8 18       	 .uleb128 0x18
 1561 09e9 01       	 .uleb128 0x1
 1562 09ea 55       	 .byte 0x55
 1563 09eb 01       	 .uleb128 0x1
 1564 09ec 32       	 .byte 0x32
 1565 09ed 18       	 .uleb128 0x18
 1566 09ee 01       	 .uleb128 0x1
 1567 09ef 54       	 .byte 0x54
 1568 09f0 09       	 .uleb128 0x9
 1569 09f1 03       	 .byte 0x3
 1570 09f2 00000000 	 .quad handle_signals
 1570      00000000 
 1571 09fa 00       	 .byte 0
 1572 09fb 17       	 .uleb128 0x17
 1573 09fc 00000000 	 .quad .LVL19
 1573      00000000 
GAS LISTING  			page 34


 1574 0a04 670C0000 	 .long 0xc67
 1575 0a08 1F0A0000 	 .long 0xa1f
 1576 0a0c 18       	 .uleb128 0x18
 1577 0a0d 01       	 .uleb128 0x1
 1578 0a0e 55       	 .byte 0x55
 1579 0a0f 01       	 .uleb128 0x1
 1580 0a10 3F       	 .byte 0x3f
 1581 0a11 18       	 .uleb128 0x18
 1582 0a12 01       	 .uleb128 0x1
 1583 0a13 54       	 .byte 0x54
 1584 0a14 09       	 .uleb128 0x9
 1585 0a15 03       	 .byte 0x3
 1586 0a16 00000000 	 .quad handle_signals
 1586      00000000 
 1587 0a1e 00       	 .byte 0
 1588 0a1f 17       	 .uleb128 0x17
 1589 0a20 00000000 	 .quad .LVL20
 1589      00000000 
 1590 0a28 870C0000 	 .long 0xc87
 1591 0a2c 460A0000 	 .long 0xa46
 1592 0a30 18       	 .uleb128 0x18
 1593 0a31 01       	 .uleb128 0x1
 1594 0a32 54       	 .byte 0x54
 1595 0a33 02       	 .uleb128 0x2
 1596 0a34 73       	 .byte 0x73
 1597 0a35 00       	 .sleb128 0
 1598 0a36 18       	 .uleb128 0x18
 1599 0a37 01       	 .uleb128 0x1
 1600 0a38 51       	 .byte 0x51
 1601 0a39 01       	 .uleb128 0x1
 1602 0a3a 30       	 .byte 0x30
 1603 0a3b 18       	 .uleb128 0x18
 1604 0a3c 01       	 .uleb128 0x1
 1605 0a3d 52       	 .byte 0x52
 1606 0a3e 01       	 .uleb128 0x1
 1607 0a3f 30       	 .byte 0x30
 1608 0a40 18       	 .uleb128 0x18
 1609 0a41 01       	 .uleb128 0x1
 1610 0a42 58       	 .byte 0x58
 1611 0a43 01       	 .uleb128 0x1
 1612 0a44 30       	 .byte 0x30
 1613 0a45 00       	 .byte 0
 1614 0a46 17       	 .uleb128 0x17
 1615 0a47 00000000 	 .quad .LVL37
 1615      00000000 
 1616 0a4f B00C0000 	 .long 0xcb0
 1617 0a53 6F0A0000 	 .long 0xa6f
 1618 0a57 18       	 .uleb128 0x18
 1619 0a58 01       	 .uleb128 0x1
 1620 0a59 55       	 .byte 0x55
 1621 0a5a 09       	 .uleb128 0x9
 1622 0a5b 03       	 .byte 0x3
 1623 0a5c 00000000 	 .quad .LC5
 1623      00000000 
 1624 0a64 18       	 .uleb128 0x18
 1625 0a65 01       	 .uleb128 0x1
 1626 0a66 54       	 .byte 0x54
GAS LISTING  			page 35


 1627 0a67 01       	 .uleb128 0x1
 1628 0a68 31       	 .byte 0x31
 1629 0a69 18       	 .uleb128 0x18
 1630 0a6a 01       	 .uleb128 0x1
 1631 0a6b 51       	 .byte 0x51
 1632 0a6c 01       	 .uleb128 0x1
 1633 0a6d 43       	 .byte 0x43
 1634 0a6e 00       	 .byte 0
 1635 0a6f 17       	 .uleb128 0x17
 1636 0a70 00000000 	 .quad .LVL41
 1636      00000000 
 1637 0a78 B00C0000 	 .long 0xcb0
 1638 0a7c 990A0000 	 .long 0xa99
 1639 0a80 18       	 .uleb128 0x18
 1640 0a81 01       	 .uleb128 0x1
 1641 0a82 55       	 .byte 0x55
 1642 0a83 09       	 .uleb128 0x9
 1643 0a84 03       	 .byte 0x3
 1644 0a85 00000000 	 .quad .LC3
 1644      00000000 
 1645 0a8d 18       	 .uleb128 0x18
 1646 0a8e 01       	 .uleb128 0x1
 1647 0a8f 54       	 .byte 0x54
 1648 0a90 01       	 .uleb128 0x1
 1649 0a91 31       	 .byte 0x31
 1650 0a92 18       	 .uleb128 0x18
 1651 0a93 01       	 .uleb128 0x1
 1652 0a94 51       	 .byte 0x51
 1653 0a95 02       	 .uleb128 0x2
 1654 0a96 08       	 .byte 0x8
 1655 0a97 2E       	 .byte 0x2e
 1656 0a98 00       	 .byte 0
 1657 0a99 17       	 .uleb128 0x17
 1658 0a9a 00000000 	 .quad .LVL43
 1658      00000000 
 1659 0aa2 B00C0000 	 .long 0xcb0
 1660 0aa6 C30A0000 	 .long 0xac3
 1661 0aaa 18       	 .uleb128 0x18
 1662 0aab 01       	 .uleb128 0x1
 1663 0aac 55       	 .byte 0x55
 1664 0aad 09       	 .uleb128 0x9
 1665 0aae 03       	 .byte 0x3
 1666 0aaf 00000000 	 .quad .LC7
 1666      00000000 
 1667 0ab7 18       	 .uleb128 0x18
 1668 0ab8 01       	 .uleb128 0x1
 1669 0ab9 54       	 .byte 0x54
 1670 0aba 01       	 .uleb128 0x1
 1671 0abb 31       	 .byte 0x31
 1672 0abc 18       	 .uleb128 0x18
 1673 0abd 01       	 .uleb128 0x1
 1674 0abe 51       	 .byte 0x51
 1675 0abf 02       	 .uleb128 0x2
 1676 0ac0 08       	 .byte 0x8
 1677 0ac1 31       	 .byte 0x31
 1678 0ac2 00       	 .byte 0
 1679 0ac3 19       	 .uleb128 0x19
GAS LISTING  			page 36


 1680 0ac4 00000000 	 .quad .LVL45
 1680      00000000 
 1681 0acc B00C0000 	 .long 0xcb0
 1682 0ad0 18       	 .uleb128 0x18
 1683 0ad1 01       	 .uleb128 0x1
 1684 0ad2 55       	 .byte 0x55
 1685 0ad3 09       	 .uleb128 0x9
 1686 0ad4 03       	 .byte 0x3
 1687 0ad5 00000000 	 .quad .LC4
 1687      00000000 
 1688 0add 18       	 .uleb128 0x18
 1689 0ade 01       	 .uleb128 0x1
 1690 0adf 54       	 .byte 0x54
 1691 0ae0 01       	 .uleb128 0x1
 1692 0ae1 31       	 .byte 0x31
 1693 0ae2 18       	 .uleb128 0x18
 1694 0ae3 01       	 .uleb128 0x1
 1695 0ae4 51       	 .byte 0x51
 1696 0ae5 02       	 .uleb128 0x2
 1697 0ae6 08       	 .byte 0x8
 1698 0ae7 22       	 .byte 0x22
 1699 0ae8 00       	 .byte 0
 1700 0ae9 00       	 .byte 0
 1701 0aea 06       	 .uleb128 0x6
 1702 0aeb 08       	 .byte 0x8
 1703 0aec 96000000 	 .long 0x96
 1704 0af0 2A       	 .uleb128 0x2a
 1705 0af1 00000000 	 .long .LASF111
 1706 0af5 01       	 .byte 0x1
 1707 0af6 15       	 .byte 0x15
 1708 0af7 5E000000 	 .long 0x5e
 1709 0afb 09       	 .uleb128 0x9
 1710 0afc 03       	 .byte 0x3
 1711 0afd 00000000 	 .quad is_exit
 1711      00000000 
 1712 0b05 2B       	 .uleb128 0x2b
 1713 0b06 00000000 	 .long .LASF112
 1714 0b0a 04       	 .byte 0x4
 1715 0b0b A8       	 .byte 0xa8
 1716 0b0c 6D020000 	 .long 0x26d
 1717 0b10 2B       	 .uleb128 0x2b
 1718 0b11 00000000 	 .long .LASF113
 1719 0b15 04       	 .byte 0x4
 1720 0b16 A9       	 .byte 0xa9
 1721 0b17 6D020000 	 .long 0x26d
 1722 0b1b 2B       	 .uleb128 0x2b
 1723 0b1c 00000000 	 .long .LASF114
 1724 0b20 04       	 .byte 0x4
 1725 0b21 AA       	 .byte 0xaa
 1726 0b22 6D020000 	 .long 0x26d
 1727 0b26 2C       	 .uleb128 0x2c
 1728 0b27 00000000 	 .long .LASF136
 1729 0b2b 13       	 .byte 0x13
 1730 0b2c 00       	 .byte 0
 1731 0b2d 00000000 	 .long .LASF137
 1732 0b31 5E000000 	 .long 0x5e
 1733 0b35 3F0B0000 	 .long 0xb3f
GAS LISTING  			page 37


 1734 0b39 10       	 .uleb128 0x10
 1735 0b3a 99020000 	 .long 0x299
 1736 0b3e 00       	 .byte 0
 1737 0b3f 2D       	 .uleb128 0x2d
 1738 0b40 00000000 	 .long .LASF117
 1739 0b44 0F       	 .byte 0xf
 1740 0b45 1E02     	 .value 0x21e
 1741 0b47 510B0000 	 .long 0xb51
 1742 0b4b 10       	 .uleb128 0x10
 1743 0b4c 5E000000 	 .long 0x5e
 1744 0b50 00       	 .byte 0
 1745 0b51 2E       	 .uleb128 0x2e
 1746 0b52 00000000 	 .long .LASF115
 1747 0b56 04       	 .byte 0x4
 1748 0b57 6A01     	 .value 0x16a
 1749 0b59 5E000000 	 .long 0x5e
 1750 0b5d 680B0000 	 .long 0xb68
 1751 0b61 10       	 .uleb128 0x10
 1752 0b62 99020000 	 .long 0x299
 1753 0b66 2F       	 .uleb128 0x2f
 1754 0b67 00       	 .byte 0
 1755 0b68 30       	 .uleb128 0x30
 1756 0b69 00000000 	 .long .LASF116
 1757 0b6d 0C       	 .byte 0xc
 1758 0b6e 5F       	 .byte 0x5f
 1759 0b6f A6060000 	 .long 0x6a6
 1760 0b73 7D0B0000 	 .long 0xb7d
 1761 0b77 10       	 .uleb128 0x10
 1762 0b78 BE070000 	 .long 0x7be
 1763 0b7c 00       	 .byte 0
 1764 0b7d 31       	 .uleb128 0x31
 1765 0b7e 00000000 	 .long .LASF118
 1766 0b82 0C       	 .byte 0xc
 1767 0b83 63       	 .byte 0x63
 1768 0b84 930B0000 	 .long 0xb93
 1769 0b88 10       	 .uleb128 0x10
 1770 0b89 A0060000 	 .long 0x6a0
 1771 0b8d 10       	 .uleb128 0x10
 1772 0b8e 930B0000 	 .long 0xb93
 1773 0b92 00       	 .byte 0
 1774 0b93 06       	 .uleb128 0x6
 1775 0b94 08       	 .byte 0x8
 1776 0b95 A3000000 	 .long 0xa3
 1777 0b99 31       	 .uleb128 0x31
 1778 0b9a 00000000 	 .long .LASF119
 1779 0b9e 0C       	 .byte 0xc
 1780 0b9f 61       	 .byte 0x61
 1781 0ba0 AF0B0000 	 .long 0xbaf
 1782 0ba4 10       	 .uleb128 0x10
 1783 0ba5 BE070000 	 .long 0x7be
 1784 0ba9 10       	 .uleb128 0x10
 1785 0baa A6060000 	 .long 0x6a6
 1786 0bae 00       	 .byte 0
 1787 0baf 31       	 .uleb128 0x31
 1788 0bb0 00000000 	 .long .LASF120
 1789 0bb4 10       	 .byte 0x10
 1790 0bb5 3F       	 .byte 0x3f
GAS LISTING  			page 38


 1791 0bb6 CB0B0000 	 .long 0xbcb
 1792 0bba 10       	 .uleb128 0x10
 1793 0bbb 5E000000 	 .long 0x5e
 1794 0bbf 10       	 .uleb128 0x10
 1795 0bc0 52030000 	 .long 0x352
 1796 0bc4 10       	 .uleb128 0x10
 1797 0bc5 99020000 	 .long 0x299
 1798 0bc9 2F       	 .uleb128 0x2f
 1799 0bca 00       	 .byte 0
 1800 0bcb 2E       	 .uleb128 0x2e
 1801 0bcc 00000000 	 .long .LASF121
 1802 0bd0 11       	 .byte 0x11
 1803 0bd1 CC01     	 .value 0x1cc
 1804 0bd3 5E000000 	 .long 0x5e
 1805 0bd7 E10B0000 	 .long 0xbe1
 1806 0bdb 10       	 .uleb128 0x10
 1807 0bdc 89000000 	 .long 0x89
 1808 0be0 00       	 .byte 0
 1809 0be1 30       	 .uleb128 0x30
 1810 0be2 00000000 	 .long .LASF122
 1811 0be6 12       	 .byte 0x12
 1812 0be7 BF       	 .byte 0xbf
 1813 0be8 F6020000 	 .long 0x2f6
 1814 0bec 000C0000 	 .long 0xc00
 1815 0bf0 10       	 .uleb128 0x10
 1816 0bf1 A4020000 	 .long 0x2a4
 1817 0bf5 10       	 .uleb128 0x10
 1818 0bf6 000C0000 	 .long 0xc00
 1819 0bfa 10       	 .uleb128 0x10
 1820 0bfb 000C0000 	 .long 0xc00
 1821 0bff 00       	 .byte 0
 1822 0c00 06       	 .uleb128 0x6
 1823 0c01 08       	 .byte 0x8
 1824 0c02 060C0000 	 .long 0xc06
 1825 0c06 06       	 .uleb128 0x6
 1826 0c07 08       	 .byte 0x8
 1827 0c08 0C0C0000 	 .long 0xc0c
 1828 0c0c 0D       	 .uleb128 0xd
 1829 0c0d 99020000 	 .long 0x299
 1830 0c11 30       	 .uleb128 0x30
 1831 0c12 00000000 	 .long .LASF123
 1832 0c16 08       	 .byte 0x8
 1833 0c17 C4       	 .byte 0xc4
 1834 0c18 F6020000 	 .long 0x2f6
 1835 0c1c 350C0000 	 .long 0xc35
 1836 0c20 10       	 .uleb128 0x10
 1837 0c21 350C0000 	 .long 0xc35
 1838 0c25 10       	 .uleb128 0x10
 1839 0c26 4C030000 	 .long 0x34c
 1840 0c2a 10       	 .uleb128 0x10
 1841 0c2b 11030000 	 .long 0x311
 1842 0c2f 10       	 .uleb128 0x10
 1843 0c30 3B0C0000 	 .long 0xc3b
 1844 0c34 00       	 .byte 0
 1845 0c35 06       	 .uleb128 0x6
 1846 0c36 08       	 .byte 0x8
 1847 0c37 4C030000 	 .long 0x34c
GAS LISTING  			page 39


 1848 0c3b 06       	 .uleb128 0x6
 1849 0c3c 08       	 .byte 0x8
 1850 0c3d 31030000 	 .long 0x331
 1851 0c41 32       	 .uleb128 0x32
 1852 0c42 00000000 	 .long .LASF138
 1853 0c46 0B       	 .byte 0xb
 1854 0c47 44       	 .byte 0x44
 1855 0c48 5E000000 	 .long 0x5e
 1856 0c4c 31       	 .uleb128 0x31
 1857 0c4d 00000000 	 .long .LASF124
 1858 0c51 10       	 .byte 0x10
 1859 0c52 43       	 .byte 0x43
 1860 0c53 670C0000 	 .long 0xc67
 1861 0c57 10       	 .uleb128 0x10
 1862 0c58 4C030000 	 .long 0x34c
 1863 0c5c 10       	 .uleb128 0x10
 1864 0c5d 99020000 	 .long 0x299
 1865 0c61 10       	 .uleb128 0x10
 1866 0c62 5E000000 	 .long 0x5e
 1867 0c66 00       	 .byte 0
 1868 0c67 30       	 .uleb128 0x30
 1869 0c68 00000000 	 .long .LASF125
 1870 0c6c 0A       	 .byte 0xa
 1871 0c6d 40       	 .byte 0x40
 1872 0c6e 810C0000 	 .long 0xc81
 1873 0c72 810C0000 	 .long 0xc81
 1874 0c76 10       	 .uleb128 0x10
 1875 0c77 5E000000 	 .long 0x5e
 1876 0c7b 10       	 .uleb128 0x10
 1877 0c7c 810C0000 	 .long 0xc81
 1878 0c80 00       	 .byte 0
 1879 0c81 06       	 .uleb128 0x6
 1880 0c82 08       	 .byte 0x8
 1881 0c83 41030000 	 .long 0x341
 1882 0c87 30       	 .uleb128 0x30
 1883 0c88 00000000 	 .long .LASF126
 1884 0c8c 0C       	 .byte 0xc
 1885 0c8d 57       	 .byte 0x57
 1886 0c8e BE070000 	 .long 0x7be
 1887 0c92 B00C0000 	 .long 0xcb0
 1888 0c96 10       	 .uleb128 0x10
 1889 0c97 4C030000 	 .long 0x34c
 1890 0c9b 10       	 .uleb128 0x10
 1891 0c9c 99020000 	 .long 0x299
 1892 0ca0 10       	 .uleb128 0x10
 1893 0ca1 E0020000 	 .long 0x2e0
 1894 0ca5 10       	 .uleb128 0x10
 1895 0ca6 E0020000 	 .long 0x2e0
 1896 0caa 10       	 .uleb128 0x10
 1897 0cab 5E000000 	 .long 0x5e
 1898 0caf 00       	 .byte 0
 1899 0cb0 33       	 .uleb128 0x33
 1900 0cb1 00000000 	 .long .LASF127
 1901 0cb5 13       	 .byte 0x13
 1902 0cb6 00       	 .byte 0
 1903 0cb7 00000000 	 .long .LASF139
 1904 0cbb 34000000 	 .long 0x34
GAS LISTING  			page 40


 1905 0cbf 10       	 .uleb128 0x10
 1906 0cc0 B8020000 	 .long 0x2b8
 1907 0cc4 10       	 .uleb128 0x10
 1908 0cc5 34000000 	 .long 0x34
 1909 0cc9 10       	 .uleb128 0x10
 1910 0cca 34000000 	 .long 0x34
 1911 0cce 10       	 .uleb128 0x10
 1912 0ccf 94000000 	 .long 0x94
 1913 0cd3 00       	 .byte 0
 1914 0cd4 00       	 .byte 0
 1915              	 .section .debug_abbrev,"",@progbits
 1916              	.Ldebug_abbrev0:
 1917 0000 01       	 .uleb128 0x1
 1918 0001 11       	 .uleb128 0x11
 1919 0002 01       	 .byte 0x1
 1920 0003 25       	 .uleb128 0x25
 1921 0004 0E       	 .uleb128 0xe
 1922 0005 13       	 .uleb128 0x13
 1923 0006 0B       	 .uleb128 0xb
 1924 0007 03       	 .uleb128 0x3
 1925 0008 0E       	 .uleb128 0xe
 1926 0009 1B       	 .uleb128 0x1b
 1927 000a 0E       	 .uleb128 0xe
 1928 000b 55       	 .uleb128 0x55
 1929 000c 17       	 .uleb128 0x17
 1930 000d 11       	 .uleb128 0x11
 1931 000e 01       	 .uleb128 0x1
 1932 000f 10       	 .uleb128 0x10
 1933 0010 17       	 .uleb128 0x17
 1934 0011 00       	 .byte 0
 1935 0012 00       	 .byte 0
 1936 0013 02       	 .uleb128 0x2
 1937 0014 16       	 .uleb128 0x16
 1938 0015 00       	 .byte 0
 1939 0016 03       	 .uleb128 0x3
 1940 0017 0E       	 .uleb128 0xe
 1941 0018 3A       	 .uleb128 0x3a
 1942 0019 0B       	 .uleb128 0xb
 1943 001a 3B       	 .uleb128 0x3b
 1944 001b 0B       	 .uleb128 0xb
 1945 001c 49       	 .uleb128 0x49
 1946 001d 13       	 .uleb128 0x13
 1947 001e 00       	 .byte 0
 1948 001f 00       	 .byte 0
 1949 0020 03       	 .uleb128 0x3
 1950 0021 24       	 .uleb128 0x24
 1951 0022 00       	 .byte 0
 1952 0023 0B       	 .uleb128 0xb
 1953 0024 0B       	 .uleb128 0xb
 1954 0025 3E       	 .uleb128 0x3e
 1955 0026 0B       	 .uleb128 0xb
 1956 0027 03       	 .uleb128 0x3
 1957 0028 0E       	 .uleb128 0xe
 1958 0029 00       	 .byte 0
 1959 002a 00       	 .byte 0
 1960 002b 04       	 .uleb128 0x4
 1961 002c 24       	 .uleb128 0x24
GAS LISTING  			page 41


 1962 002d 00       	 .byte 0
 1963 002e 0B       	 .uleb128 0xb
 1964 002f 0B       	 .uleb128 0xb
 1965 0030 3E       	 .uleb128 0x3e
 1966 0031 0B       	 .uleb128 0xb
 1967 0032 03       	 .uleb128 0x3
 1968 0033 08       	 .uleb128 0x8
 1969 0034 00       	 .byte 0
 1970 0035 00       	 .byte 0
 1971 0036 05       	 .uleb128 0x5
 1972 0037 0F       	 .uleb128 0xf
 1973 0038 00       	 .byte 0
 1974 0039 0B       	 .uleb128 0xb
 1975 003a 0B       	 .uleb128 0xb
 1976 003b 00       	 .byte 0
 1977 003c 00       	 .byte 0
 1978 003d 06       	 .uleb128 0x6
 1979 003e 0F       	 .uleb128 0xf
 1980 003f 00       	 .byte 0
 1981 0040 0B       	 .uleb128 0xb
 1982 0041 0B       	 .uleb128 0xb
 1983 0042 49       	 .uleb128 0x49
 1984 0043 13       	 .uleb128 0x13
 1985 0044 00       	 .byte 0
 1986 0045 00       	 .byte 0
 1987 0046 07       	 .uleb128 0x7
 1988 0047 13       	 .uleb128 0x13
 1989 0048 01       	 .byte 0x1
 1990 0049 03       	 .uleb128 0x3
 1991 004a 0E       	 .uleb128 0xe
 1992 004b 0B       	 .uleb128 0xb
 1993 004c 0B       	 .uleb128 0xb
 1994 004d 3A       	 .uleb128 0x3a
 1995 004e 0B       	 .uleb128 0xb
 1996 004f 3B       	 .uleb128 0x3b
 1997 0050 0B       	 .uleb128 0xb
 1998 0051 01       	 .uleb128 0x1
 1999 0052 13       	 .uleb128 0x13
 2000 0053 00       	 .byte 0
 2001 0054 00       	 .byte 0
 2002 0055 08       	 .uleb128 0x8
 2003 0056 0D       	 .uleb128 0xd
 2004 0057 00       	 .byte 0
 2005 0058 03       	 .uleb128 0x3
 2006 0059 0E       	 .uleb128 0xe
 2007 005a 3A       	 .uleb128 0x3a
 2008 005b 0B       	 .uleb128 0xb
 2009 005c 3B       	 .uleb128 0x3b
 2010 005d 0B       	 .uleb128 0xb
 2011 005e 49       	 .uleb128 0x49
 2012 005f 13       	 .uleb128 0x13
 2013 0060 38       	 .uleb128 0x38
 2014 0061 0B       	 .uleb128 0xb
 2015 0062 00       	 .byte 0
 2016 0063 00       	 .byte 0
 2017 0064 09       	 .uleb128 0x9
 2018 0065 0D       	 .uleb128 0xd
GAS LISTING  			page 42


 2019 0066 00       	 .byte 0
 2020 0067 03       	 .uleb128 0x3
 2021 0068 0E       	 .uleb128 0xe
 2022 0069 3A       	 .uleb128 0x3a
 2023 006a 0B       	 .uleb128 0xb
 2024 006b 3B       	 .uleb128 0x3b
 2025 006c 05       	 .uleb128 0x5
 2026 006d 49       	 .uleb128 0x49
 2027 006e 13       	 .uleb128 0x13
 2028 006f 38       	 .uleb128 0x38
 2029 0070 0B       	 .uleb128 0xb
 2030 0071 00       	 .byte 0
 2031 0072 00       	 .byte 0
 2032 0073 0A       	 .uleb128 0xa
 2033 0074 16       	 .uleb128 0x16
 2034 0075 00       	 .byte 0
 2035 0076 03       	 .uleb128 0x3
 2036 0077 0E       	 .uleb128 0xe
 2037 0078 3A       	 .uleb128 0x3a
 2038 0079 0B       	 .uleb128 0xb
 2039 007a 3B       	 .uleb128 0x3b
 2040 007b 0B       	 .uleb128 0xb
 2041 007c 00       	 .byte 0
 2042 007d 00       	 .byte 0
 2043 007e 0B       	 .uleb128 0xb
 2044 007f 01       	 .uleb128 0x1
 2045 0080 01       	 .byte 0x1
 2046 0081 49       	 .uleb128 0x49
 2047 0082 13       	 .uleb128 0x13
 2048 0083 01       	 .uleb128 0x1
 2049 0084 13       	 .uleb128 0x13
 2050 0085 00       	 .byte 0
 2051 0086 00       	 .byte 0
 2052 0087 0C       	 .uleb128 0xc
 2053 0088 21       	 .uleb128 0x21
 2054 0089 00       	 .byte 0
 2055 008a 49       	 .uleb128 0x49
 2056 008b 13       	 .uleb128 0x13
 2057 008c 2F       	 .uleb128 0x2f
 2058 008d 0B       	 .uleb128 0xb
 2059 008e 00       	 .byte 0
 2060 008f 00       	 .byte 0
 2061 0090 0D       	 .uleb128 0xd
 2062 0091 26       	 .uleb128 0x26
 2063 0092 00       	 .byte 0
 2064 0093 49       	 .uleb128 0x49
 2065 0094 13       	 .uleb128 0x13
 2066 0095 00       	 .byte 0
 2067 0096 00       	 .byte 0
 2068 0097 0E       	 .uleb128 0xe
 2069 0098 26       	 .uleb128 0x26
 2070 0099 00       	 .byte 0
 2071 009a 00       	 .byte 0
 2072 009b 00       	 .byte 0
 2073 009c 0F       	 .uleb128 0xf
 2074 009d 15       	 .uleb128 0x15
 2075 009e 01       	 .byte 0x1
GAS LISTING  			page 43


 2076 009f 27       	 .uleb128 0x27
 2077 00a0 19       	 .uleb128 0x19
 2078 00a1 01       	 .uleb128 0x1
 2079 00a2 13       	 .uleb128 0x13
 2080 00a3 00       	 .byte 0
 2081 00a4 00       	 .byte 0
 2082 00a5 10       	 .uleb128 0x10
 2083 00a6 05       	 .uleb128 0x5
 2084 00a7 00       	 .byte 0
 2085 00a8 49       	 .uleb128 0x49
 2086 00a9 13       	 .uleb128 0x13
 2087 00aa 00       	 .byte 0
 2088 00ab 00       	 .byte 0
 2089 00ac 11       	 .uleb128 0x11
 2090 00ad 13       	 .uleb128 0x13
 2091 00ae 00       	 .byte 0
 2092 00af 03       	 .uleb128 0x3
 2093 00b0 0E       	 .uleb128 0xe
 2094 00b1 3C       	 .uleb128 0x3c
 2095 00b2 19       	 .uleb128 0x19
 2096 00b3 00       	 .byte 0
 2097 00b4 00       	 .byte 0
 2098 00b5 12       	 .uleb128 0x12
 2099 00b6 15       	 .uleb128 0x15
 2100 00b7 01       	 .byte 0x1
 2101 00b8 27       	 .uleb128 0x27
 2102 00b9 19       	 .uleb128 0x19
 2103 00ba 49       	 .uleb128 0x49
 2104 00bb 13       	 .uleb128 0x13
 2105 00bc 01       	 .uleb128 0x1
 2106 00bd 13       	 .uleb128 0x13
 2107 00be 00       	 .byte 0
 2108 00bf 00       	 .byte 0
 2109 00c0 13       	 .uleb128 0x13
 2110 00c1 0D       	 .uleb128 0xd
 2111 00c2 00       	 .byte 0
 2112 00c3 03       	 .uleb128 0x3
 2113 00c4 08       	 .uleb128 0x8
 2114 00c5 3A       	 .uleb128 0x3a
 2115 00c6 0B       	 .uleb128 0xb
 2116 00c7 3B       	 .uleb128 0x3b
 2117 00c8 0B       	 .uleb128 0xb
 2118 00c9 49       	 .uleb128 0x49
 2119 00ca 13       	 .uleb128 0x13
 2120 00cb 38       	 .uleb128 0x38
 2121 00cc 0B       	 .uleb128 0xb
 2122 00cd 00       	 .byte 0
 2123 00ce 00       	 .byte 0
 2124 00cf 14       	 .uleb128 0x14
 2125 00d0 0D       	 .uleb128 0xd
 2126 00d1 00       	 .byte 0
 2127 00d2 03       	 .uleb128 0x3
 2128 00d3 0E       	 .uleb128 0xe
 2129 00d4 3A       	 .uleb128 0x3a
 2130 00d5 0B       	 .uleb128 0xb
 2131 00d6 3B       	 .uleb128 0x3b
 2132 00d7 0B       	 .uleb128 0xb
GAS LISTING  			page 44


 2133 00d8 49       	 .uleb128 0x49
 2134 00d9 13       	 .uleb128 0x13
 2135 00da 0B       	 .uleb128 0xb
 2136 00db 0B       	 .uleb128 0xb
 2137 00dc 0D       	 .uleb128 0xd
 2138 00dd 0B       	 .uleb128 0xb
 2139 00de 0C       	 .uleb128 0xc
 2140 00df 0B       	 .uleb128 0xb
 2141 00e0 38       	 .uleb128 0x38
 2142 00e1 0B       	 .uleb128 0xb
 2143 00e2 00       	 .byte 0
 2144 00e3 00       	 .byte 0
 2145 00e4 15       	 .uleb128 0x15
 2146 00e5 2E       	 .uleb128 0x2e
 2147 00e6 01       	 .byte 0x1
 2148 00e7 03       	 .uleb128 0x3
 2149 00e8 0E       	 .uleb128 0xe
 2150 00e9 3A       	 .uleb128 0x3a
 2151 00ea 0B       	 .uleb128 0xb
 2152 00eb 3B       	 .uleb128 0x3b
 2153 00ec 0B       	 .uleb128 0xb
 2154 00ed 27       	 .uleb128 0x27
 2155 00ee 19       	 .uleb128 0x19
 2156 00ef 11       	 .uleb128 0x11
 2157 00f0 01       	 .uleb128 0x1
 2158 00f1 12       	 .uleb128 0x12
 2159 00f2 07       	 .uleb128 0x7
 2160 00f3 40       	 .uleb128 0x40
 2161 00f4 18       	 .uleb128 0x18
 2162 00f5 9742     	 .uleb128 0x2117
 2163 00f7 19       	 .uleb128 0x19
 2164 00f8 01       	 .uleb128 0x1
 2165 00f9 13       	 .uleb128 0x13
 2166 00fa 00       	 .byte 0
 2167 00fb 00       	 .byte 0
 2168 00fc 16       	 .uleb128 0x16
 2169 00fd 05       	 .uleb128 0x5
 2170 00fe 00       	 .byte 0
 2171 00ff 03       	 .uleb128 0x3
 2172 0100 0E       	 .uleb128 0xe
 2173 0101 3A       	 .uleb128 0x3a
 2174 0102 0B       	 .uleb128 0xb
 2175 0103 3B       	 .uleb128 0x3b
 2176 0104 0B       	 .uleb128 0xb
 2177 0105 49       	 .uleb128 0x49
 2178 0106 13       	 .uleb128 0x13
 2179 0107 02       	 .uleb128 0x2
 2180 0108 17       	 .uleb128 0x17
 2181 0109 00       	 .byte 0
 2182 010a 00       	 .byte 0
 2183 010b 17       	 .uleb128 0x17
 2184 010c 898201   	 .uleb128 0x4109
 2185 010f 01       	 .byte 0x1
 2186 0110 11       	 .uleb128 0x11
 2187 0111 01       	 .uleb128 0x1
 2188 0112 31       	 .uleb128 0x31
 2189 0113 13       	 .uleb128 0x13
GAS LISTING  			page 45


 2190 0114 01       	 .uleb128 0x1
 2191 0115 13       	 .uleb128 0x13
 2192 0116 00       	 .byte 0
 2193 0117 00       	 .byte 0
 2194 0118 18       	 .uleb128 0x18
 2195 0119 8A8201   	 .uleb128 0x410a
 2196 011c 00       	 .byte 0
 2197 011d 02       	 .uleb128 0x2
 2198 011e 18       	 .uleb128 0x18
 2199 011f 9142     	 .uleb128 0x2111
 2200 0121 18       	 .uleb128 0x18
 2201 0122 00       	 .byte 0
 2202 0123 00       	 .byte 0
 2203 0124 19       	 .uleb128 0x19
 2204 0125 898201   	 .uleb128 0x4109
 2205 0128 01       	 .byte 0x1
 2206 0129 11       	 .uleb128 0x11
 2207 012a 01       	 .uleb128 0x1
 2208 012b 31       	 .uleb128 0x31
 2209 012c 13       	 .uleb128 0x13
 2210 012d 00       	 .byte 0
 2211 012e 00       	 .byte 0
 2212 012f 1A       	 .uleb128 0x1a
 2213 0130 2E       	 .uleb128 0x2e
 2214 0131 01       	 .byte 0x1
 2215 0132 03       	 .uleb128 0x3
 2216 0133 0E       	 .uleb128 0xe
 2217 0134 3A       	 .uleb128 0x3a
 2218 0135 0B       	 .uleb128 0xb
 2219 0136 3B       	 .uleb128 0x3b
 2220 0137 0B       	 .uleb128 0xb
 2221 0138 27       	 .uleb128 0x27
 2222 0139 19       	 .uleb128 0x19
 2223 013a 20       	 .uleb128 0x20
 2224 013b 0B       	 .uleb128 0xb
 2225 013c 01       	 .uleb128 0x1
 2226 013d 13       	 .uleb128 0x13
 2227 013e 00       	 .byte 0
 2228 013f 00       	 .byte 0
 2229 0140 1B       	 .uleb128 0x1b
 2230 0141 05       	 .uleb128 0x5
 2231 0142 00       	 .byte 0
 2232 0143 03       	 .uleb128 0x3
 2233 0144 0E       	 .uleb128 0xe
 2234 0145 3A       	 .uleb128 0x3a
 2235 0146 0B       	 .uleb128 0xb
 2236 0147 3B       	 .uleb128 0x3b
 2237 0148 0B       	 .uleb128 0xb
 2238 0149 49       	 .uleb128 0x49
 2239 014a 13       	 .uleb128 0x13
 2240 014b 00       	 .byte 0
 2241 014c 00       	 .byte 0
 2242 014d 1C       	 .uleb128 0x1c
 2243 014e 34       	 .uleb128 0x34
 2244 014f 00       	 .byte 0
 2245 0150 03       	 .uleb128 0x3
 2246 0151 0E       	 .uleb128 0xe
GAS LISTING  			page 46


 2247 0152 3A       	 .uleb128 0x3a
 2248 0153 0B       	 .uleb128 0xb
 2249 0154 3B       	 .uleb128 0x3b
 2250 0155 0B       	 .uleb128 0xb
 2251 0156 49       	 .uleb128 0x49
 2252 0157 13       	 .uleb128 0x13
 2253 0158 00       	 .byte 0
 2254 0159 00       	 .byte 0
 2255 015a 1D       	 .uleb128 0x1d
 2256 015b 34       	 .uleb128 0x34
 2257 015c 00       	 .byte 0
 2258 015d 03       	 .uleb128 0x3
 2259 015e 08       	 .uleb128 0x8
 2260 015f 3A       	 .uleb128 0x3a
 2261 0160 0B       	 .uleb128 0xb
 2262 0161 3B       	 .uleb128 0x3b
 2263 0162 0B       	 .uleb128 0xb
 2264 0163 49       	 .uleb128 0x49
 2265 0164 13       	 .uleb128 0x13
 2266 0165 00       	 .byte 0
 2267 0166 00       	 .byte 0
 2268 0167 1E       	 .uleb128 0x1e
 2269 0168 34       	 .uleb128 0x34
 2270 0169 00       	 .byte 0
 2271 016a 03       	 .uleb128 0x3
 2272 016b 0E       	 .uleb128 0xe
 2273 016c 49       	 .uleb128 0x49
 2274 016d 13       	 .uleb128 0x13
 2275 016e 34       	 .uleb128 0x34
 2276 016f 19       	 .uleb128 0x19
 2277 0170 1C       	 .uleb128 0x1c
 2278 0171 0E       	 .uleb128 0xe
 2279 0172 00       	 .byte 0
 2280 0173 00       	 .byte 0
 2281 0174 1F       	 .uleb128 0x1f
 2282 0175 2E       	 .uleb128 0x2e
 2283 0176 01       	 .byte 0x1
 2284 0177 3F       	 .uleb128 0x3f
 2285 0178 19       	 .uleb128 0x19
 2286 0179 03       	 .uleb128 0x3
 2287 017a 0E       	 .uleb128 0xe
 2288 017b 3A       	 .uleb128 0x3a
 2289 017c 0B       	 .uleb128 0xb
 2290 017d 3B       	 .uleb128 0x3b
 2291 017e 0B       	 .uleb128 0xb
 2292 017f 27       	 .uleb128 0x27
 2293 0180 19       	 .uleb128 0x19
 2294 0181 49       	 .uleb128 0x49
 2295 0182 13       	 .uleb128 0x13
 2296 0183 11       	 .uleb128 0x11
 2297 0184 01       	 .uleb128 0x1
 2298 0185 12       	 .uleb128 0x12
 2299 0186 07       	 .uleb128 0x7
 2300 0187 40       	 .uleb128 0x40
 2301 0188 18       	 .uleb128 0x18
 2302 0189 9742     	 .uleb128 0x2117
 2303 018b 19       	 .uleb128 0x19
GAS LISTING  			page 47


 2304 018c 01       	 .uleb128 0x1
 2305 018d 13       	 .uleb128 0x13
 2306 018e 00       	 .byte 0
 2307 018f 00       	 .byte 0
 2308 0190 20       	 .uleb128 0x20
 2309 0191 34       	 .uleb128 0x34
 2310 0192 00       	 .byte 0
 2311 0193 03       	 .uleb128 0x3
 2312 0194 0E       	 .uleb128 0xe
 2313 0195 3A       	 .uleb128 0x3a
 2314 0196 0B       	 .uleb128 0xb
 2315 0197 3B       	 .uleb128 0x3b
 2316 0198 0B       	 .uleb128 0xb
 2317 0199 49       	 .uleb128 0x49
 2318 019a 13       	 .uleb128 0x13
 2319 019b 02       	 .uleb128 0x2
 2320 019c 17       	 .uleb128 0x17
 2321 019d 00       	 .byte 0
 2322 019e 00       	 .byte 0
 2323 019f 21       	 .uleb128 0x21
 2324 01a0 34       	 .uleb128 0x34
 2325 01a1 00       	 .byte 0
 2326 01a2 03       	 .uleb128 0x3
 2327 01a3 08       	 .uleb128 0x8
 2328 01a4 3A       	 .uleb128 0x3a
 2329 01a5 0B       	 .uleb128 0xb
 2330 01a6 3B       	 .uleb128 0x3b
 2331 01a7 0B       	 .uleb128 0xb
 2332 01a8 49       	 .uleb128 0x49
 2333 01a9 13       	 .uleb128 0x13
 2334 01aa 02       	 .uleb128 0x2
 2335 01ab 18       	 .uleb128 0x18
 2336 01ac 00       	 .byte 0
 2337 01ad 00       	 .byte 0
 2338 01ae 22       	 .uleb128 0x22
 2339 01af 34       	 .uleb128 0x34
 2340 01b0 00       	 .byte 0
 2341 01b1 03       	 .uleb128 0x3
 2342 01b2 08       	 .uleb128 0x8
 2343 01b3 3A       	 .uleb128 0x3a
 2344 01b4 0B       	 .uleb128 0xb
 2345 01b5 3B       	 .uleb128 0x3b
 2346 01b6 0B       	 .uleb128 0xb
 2347 01b7 49       	 .uleb128 0x49
 2348 01b8 13       	 .uleb128 0x13
 2349 01b9 02       	 .uleb128 0x2
 2350 01ba 17       	 .uleb128 0x17
 2351 01bb 00       	 .byte 0
 2352 01bc 00       	 .byte 0
 2353 01bd 23       	 .uleb128 0x23
 2354 01be 1D       	 .uleb128 0x1d
 2355 01bf 01       	 .byte 0x1
 2356 01c0 31       	 .uleb128 0x31
 2357 01c1 13       	 .uleb128 0x13
 2358 01c2 52       	 .uleb128 0x52
 2359 01c3 01       	 .uleb128 0x1
 2360 01c4 55       	 .uleb128 0x55
GAS LISTING  			page 48


 2361 01c5 17       	 .uleb128 0x17
 2362 01c6 58       	 .uleb128 0x58
 2363 01c7 0B       	 .uleb128 0xb
 2364 01c8 59       	 .uleb128 0x59
 2365 01c9 0B       	 .uleb128 0xb
 2366 01ca 01       	 .uleb128 0x1
 2367 01cb 13       	 .uleb128 0x13
 2368 01cc 00       	 .byte 0
 2369 01cd 00       	 .byte 0
 2370 01ce 24       	 .uleb128 0x24
 2371 01cf 05       	 .uleb128 0x5
 2372 01d0 00       	 .byte 0
 2373 01d1 31       	 .uleb128 0x31
 2374 01d2 13       	 .uleb128 0x13
 2375 01d3 00       	 .byte 0
 2376 01d4 00       	 .byte 0
 2377 01d5 25       	 .uleb128 0x25
 2378 01d6 0B       	 .uleb128 0xb
 2379 01d7 01       	 .byte 0x1
 2380 01d8 55       	 .uleb128 0x55
 2381 01d9 17       	 .uleb128 0x17
 2382 01da 00       	 .byte 0
 2383 01db 00       	 .byte 0
 2384 01dc 26       	 .uleb128 0x26
 2385 01dd 34       	 .uleb128 0x34
 2386 01de 00       	 .byte 0
 2387 01df 31       	 .uleb128 0x31
 2388 01e0 13       	 .uleb128 0x13
 2389 01e1 02       	 .uleb128 0x2
 2390 01e2 17       	 .uleb128 0x17
 2391 01e3 00       	 .byte 0
 2392 01e4 00       	 .byte 0
 2393 01e5 27       	 .uleb128 0x27
 2394 01e6 34       	 .uleb128 0x34
 2395 01e7 00       	 .byte 0
 2396 01e8 31       	 .uleb128 0x31
 2397 01e9 13       	 .uleb128 0x13
 2398 01ea 02       	 .uleb128 0x2
 2399 01eb 18       	 .uleb128 0x18
 2400 01ec 00       	 .byte 0
 2401 01ed 00       	 .byte 0
 2402 01ee 28       	 .uleb128 0x28
 2403 01ef 898201   	 .uleb128 0x4109
 2404 01f2 00       	 .byte 0
 2405 01f3 11       	 .uleb128 0x11
 2406 01f4 01       	 .uleb128 0x1
 2407 01f5 31       	 .uleb128 0x31
 2408 01f6 13       	 .uleb128 0x13
 2409 01f7 00       	 .byte 0
 2410 01f8 00       	 .byte 0
 2411 01f9 29       	 .uleb128 0x29
 2412 01fa 898201   	 .uleb128 0x4109
 2413 01fd 01       	 .byte 0x1
 2414 01fe 11       	 .uleb128 0x11
 2415 01ff 01       	 .uleb128 0x1
 2416 0200 01       	 .uleb128 0x1
 2417 0201 13       	 .uleb128 0x13
GAS LISTING  			page 49


 2418 0202 00       	 .byte 0
 2419 0203 00       	 .byte 0
 2420 0204 2A       	 .uleb128 0x2a
 2421 0205 34       	 .uleb128 0x34
 2422 0206 00       	 .byte 0
 2423 0207 03       	 .uleb128 0x3
 2424 0208 0E       	 .uleb128 0xe
 2425 0209 3A       	 .uleb128 0x3a
 2426 020a 0B       	 .uleb128 0xb
 2427 020b 3B       	 .uleb128 0x3b
 2428 020c 0B       	 .uleb128 0xb
 2429 020d 49       	 .uleb128 0x49
 2430 020e 13       	 .uleb128 0x13
 2431 020f 02       	 .uleb128 0x2
 2432 0210 18       	 .uleb128 0x18
 2433 0211 00       	 .byte 0
 2434 0212 00       	 .byte 0
 2435 0213 2B       	 .uleb128 0x2b
 2436 0214 34       	 .uleb128 0x34
 2437 0215 00       	 .byte 0
 2438 0216 03       	 .uleb128 0x3
 2439 0217 0E       	 .uleb128 0xe
 2440 0218 3A       	 .uleb128 0x3a
 2441 0219 0B       	 .uleb128 0xb
 2442 021a 3B       	 .uleb128 0x3b
 2443 021b 0B       	 .uleb128 0xb
 2444 021c 49       	 .uleb128 0x49
 2445 021d 13       	 .uleb128 0x13
 2446 021e 3F       	 .uleb128 0x3f
 2447 021f 19       	 .uleb128 0x19
 2448 0220 3C       	 .uleb128 0x3c
 2449 0221 19       	 .uleb128 0x19
 2450 0222 00       	 .byte 0
 2451 0223 00       	 .byte 0
 2452 0224 2C       	 .uleb128 0x2c
 2453 0225 2E       	 .uleb128 0x2e
 2454 0226 01       	 .byte 0x1
 2455 0227 3F       	 .uleb128 0x3f
 2456 0228 19       	 .uleb128 0x19
 2457 0229 03       	 .uleb128 0x3
 2458 022a 0E       	 .uleb128 0xe
 2459 022b 3A       	 .uleb128 0x3a
 2460 022c 0B       	 .uleb128 0xb
 2461 022d 3B       	 .uleb128 0x3b
 2462 022e 0B       	 .uleb128 0xb
 2463 022f 6E       	 .uleb128 0x6e
 2464 0230 0E       	 .uleb128 0xe
 2465 0231 27       	 .uleb128 0x27
 2466 0232 19       	 .uleb128 0x19
 2467 0233 49       	 .uleb128 0x49
 2468 0234 13       	 .uleb128 0x13
 2469 0235 3C       	 .uleb128 0x3c
 2470 0236 19       	 .uleb128 0x19
 2471 0237 01       	 .uleb128 0x1
 2472 0238 13       	 .uleb128 0x13
 2473 0239 00       	 .byte 0
 2474 023a 00       	 .byte 0
GAS LISTING  			page 50


 2475 023b 2D       	 .uleb128 0x2d
 2476 023c 2E       	 .uleb128 0x2e
 2477 023d 01       	 .byte 0x1
 2478 023e 3F       	 .uleb128 0x3f
 2479 023f 19       	 .uleb128 0x19
 2480 0240 03       	 .uleb128 0x3
 2481 0241 0E       	 .uleb128 0xe
 2482 0242 3A       	 .uleb128 0x3a
 2483 0243 0B       	 .uleb128 0xb
 2484 0244 3B       	 .uleb128 0x3b
 2485 0245 05       	 .uleb128 0x5
 2486 0246 27       	 .uleb128 0x27
 2487 0247 19       	 .uleb128 0x19
 2488 0248 3C       	 .uleb128 0x3c
 2489 0249 19       	 .uleb128 0x19
 2490 024a 01       	 .uleb128 0x1
 2491 024b 13       	 .uleb128 0x13
 2492 024c 00       	 .byte 0
 2493 024d 00       	 .byte 0
 2494 024e 2E       	 .uleb128 0x2e
 2495 024f 2E       	 .uleb128 0x2e
 2496 0250 01       	 .byte 0x1
 2497 0251 3F       	 .uleb128 0x3f
 2498 0252 19       	 .uleb128 0x19
 2499 0253 03       	 .uleb128 0x3
 2500 0254 0E       	 .uleb128 0xe
 2501 0255 3A       	 .uleb128 0x3a
 2502 0256 0B       	 .uleb128 0xb
 2503 0257 3B       	 .uleb128 0x3b
 2504 0258 05       	 .uleb128 0x5
 2505 0259 27       	 .uleb128 0x27
 2506 025a 19       	 .uleb128 0x19
 2507 025b 49       	 .uleb128 0x49
 2508 025c 13       	 .uleb128 0x13
 2509 025d 3C       	 .uleb128 0x3c
 2510 025e 19       	 .uleb128 0x19
 2511 025f 01       	 .uleb128 0x1
 2512 0260 13       	 .uleb128 0x13
 2513 0261 00       	 .byte 0
 2514 0262 00       	 .byte 0
 2515 0263 2F       	 .uleb128 0x2f
 2516 0264 18       	 .uleb128 0x18
 2517 0265 00       	 .byte 0
 2518 0266 00       	 .byte 0
 2519 0267 00       	 .byte 0
 2520 0268 30       	 .uleb128 0x30
 2521 0269 2E       	 .uleb128 0x2e
 2522 026a 01       	 .byte 0x1
 2523 026b 3F       	 .uleb128 0x3f
 2524 026c 19       	 .uleb128 0x19
 2525 026d 03       	 .uleb128 0x3
 2526 026e 0E       	 .uleb128 0xe
 2527 026f 3A       	 .uleb128 0x3a
 2528 0270 0B       	 .uleb128 0xb
 2529 0271 3B       	 .uleb128 0x3b
 2530 0272 0B       	 .uleb128 0xb
 2531 0273 27       	 .uleb128 0x27
GAS LISTING  			page 51


 2532 0274 19       	 .uleb128 0x19
 2533 0275 49       	 .uleb128 0x49
 2534 0276 13       	 .uleb128 0x13
 2535 0277 3C       	 .uleb128 0x3c
 2536 0278 19       	 .uleb128 0x19
 2537 0279 01       	 .uleb128 0x1
 2538 027a 13       	 .uleb128 0x13
 2539 027b 00       	 .byte 0
 2540 027c 00       	 .byte 0
 2541 027d 31       	 .uleb128 0x31
 2542 027e 2E       	 .uleb128 0x2e
 2543 027f 01       	 .byte 0x1
 2544 0280 3F       	 .uleb128 0x3f
 2545 0281 19       	 .uleb128 0x19
 2546 0282 03       	 .uleb128 0x3
 2547 0283 0E       	 .uleb128 0xe
 2548 0284 3A       	 .uleb128 0x3a
 2549 0285 0B       	 .uleb128 0xb
 2550 0286 3B       	 .uleb128 0x3b
 2551 0287 0B       	 .uleb128 0xb
 2552 0288 27       	 .uleb128 0x27
 2553 0289 19       	 .uleb128 0x19
 2554 028a 3C       	 .uleb128 0x3c
 2555 028b 19       	 .uleb128 0x19
 2556 028c 01       	 .uleb128 0x1
 2557 028d 13       	 .uleb128 0x13
 2558 028e 00       	 .byte 0
 2559 028f 00       	 .byte 0
 2560 0290 32       	 .uleb128 0x32
 2561 0291 2E       	 .uleb128 0x2e
 2562 0292 00       	 .byte 0
 2563 0293 3F       	 .uleb128 0x3f
 2564 0294 19       	 .uleb128 0x19
 2565 0295 03       	 .uleb128 0x3
 2566 0296 0E       	 .uleb128 0xe
 2567 0297 3A       	 .uleb128 0x3a
 2568 0298 0B       	 .uleb128 0xb
 2569 0299 3B       	 .uleb128 0x3b
 2570 029a 0B       	 .uleb128 0xb
 2571 029b 27       	 .uleb128 0x27
 2572 029c 19       	 .uleb128 0x19
 2573 029d 49       	 .uleb128 0x49
 2574 029e 13       	 .uleb128 0x13
 2575 029f 3C       	 .uleb128 0x3c
 2576 02a0 19       	 .uleb128 0x19
 2577 02a1 00       	 .byte 0
 2578 02a2 00       	 .byte 0
 2579 02a3 33       	 .uleb128 0x33
 2580 02a4 2E       	 .uleb128 0x2e
 2581 02a5 01       	 .byte 0x1
 2582 02a6 3F       	 .uleb128 0x3f
 2583 02a7 19       	 .uleb128 0x19
 2584 02a8 03       	 .uleb128 0x3
 2585 02a9 0E       	 .uleb128 0xe
 2586 02aa 3A       	 .uleb128 0x3a
 2587 02ab 0B       	 .uleb128 0xb
 2588 02ac 3B       	 .uleb128 0x3b
GAS LISTING  			page 52


 2589 02ad 0B       	 .uleb128 0xb
 2590 02ae 6E       	 .uleb128 0x6e
 2591 02af 0E       	 .uleb128 0xe
 2592 02b0 27       	 .uleb128 0x27
 2593 02b1 19       	 .uleb128 0x19
 2594 02b2 49       	 .uleb128 0x49
 2595 02b3 13       	 .uleb128 0x13
 2596 02b4 3C       	 .uleb128 0x3c
 2597 02b5 19       	 .uleb128 0x19
 2598 02b6 00       	 .byte 0
 2599 02b7 00       	 .byte 0
 2600 02b8 00       	 .byte 0
 2601              	 .section .debug_loc,"",@progbits
 2602              	.Ldebug_loc0:
 2603              	.LLST0:
 2604 0000 00000000 	 .quad .LVL0
 2604      00000000 
 2605 0008 00000000 	 .quad .LVL1
 2605      00000000 
 2606 0010 0100     	 .value 0x1
 2607 0012 55       	 .byte 0x55
 2608 0013 00000000 	 .quad .LVL1
 2608      00000000 
 2609 001b 00000000 	 .quad .LVL3
 2609      00000000 
 2610 0023 0400     	 .value 0x4
 2611 0025 F3       	 .byte 0xf3
 2612 0026 01       	 .uleb128 0x1
 2613 0027 55       	 .byte 0x55
 2614 0028 9F       	 .byte 0x9f
 2615 0029 00000000 	 .quad .LVL3
 2615      00000000 
 2616 0031 00000000 	 .quad .LVL4
 2616      00000000 
 2617 0039 0100     	 .value 0x1
 2618 003b 55       	 .byte 0x55
 2619 003c 00000000 	 .quad .LVL4
 2619      00000000 
 2620 0044 00000000 	 .quad .LVL5-1
 2620      00000000 
 2621 004c 0100     	 .value 0x1
 2622 004e 54       	 .byte 0x54
 2623 004f 00000000 	 .quad .LVL5-1
 2623      00000000 
 2624 0057 00000000 	 .quad .LVL6
 2624      00000000 
 2625 005f 0400     	 .value 0x4
 2626 0061 F3       	 .byte 0xf3
 2627 0062 01       	 .uleb128 0x1
 2628 0063 55       	 .byte 0x55
 2629 0064 9F       	 .byte 0x9f
 2630 0065 00000000 	 .quad .LVL6
 2630      00000000 
 2631 006d 00000000 	 .quad .LVL7
 2631      00000000 
 2632 0075 0100     	 .value 0x1
 2633 0077 55       	 .byte 0x55
GAS LISTING  			page 53


 2634 0078 00000000 	 .quad .LVL7
 2634      00000000 
 2635 0080 00000000 	 .quad .LFE96
 2635      00000000 
 2636 0088 0400     	 .value 0x4
 2637 008a F3       	 .byte 0xf3
 2638 008b 01       	 .uleb128 0x1
 2639 008c 55       	 .byte 0x55
 2640 008d 9F       	 .byte 0x9f
 2641 008e 00000000 	 .quad 0
 2641      00000000 
 2642 0096 00000000 	 .quad 0
 2642      00000000 
 2643              	.LLST1:
 2644 009e 00000000 	 .quad .LVL10
 2644      00000000 
 2645 00a6 00000000 	 .quad .LVL11
 2645      00000000 
 2646 00ae 0100     	 .value 0x1
 2647 00b0 55       	 .byte 0x55
 2648 00b1 00000000 	 .quad .LVL11
 2648      00000000 
 2649 00b9 00000000 	 .quad .LVL14-1
 2649      00000000 
 2650 00c1 0200     	 .value 0x2
 2651 00c3 75       	 .byte 0x75
 2652 00c4 00       	 .sleb128 0
 2653 00c5 00000000 	 .quad .LVL14-1
 2653      00000000 
 2654 00cd 00000000 	 .quad .LVL38
 2654      00000000 
 2655 00d5 0200     	 .value 0x2
 2656 00d7 91       	 .byte 0x91
 2657 00d8 4C       	 .sleb128 -52
 2658 00d9 00000000 	 .quad .LVL38
 2658      00000000 
 2659 00e1 00000000 	 .quad .LVL40
 2659      00000000 
 2660 00e9 0100     	 .value 0x1
 2661 00eb 55       	 .byte 0x55
 2662 00ec 00000000 	 .quad .LVL40
 2662      00000000 
 2663 00f4 00000000 	 .quad .LFE98
 2663      00000000 
 2664 00fc 0200     	 .value 0x2
 2665 00fe 91       	 .byte 0x91
 2666 00ff 4C       	 .sleb128 -52
 2667 0100 00000000 	 .quad 0
 2667      00000000 
 2668 0108 00000000 	 .quad 0
 2668      00000000 
 2669              	.LLST2:
 2670 0110 00000000 	 .quad .LVL10
 2670      00000000 
 2671 0118 00000000 	 .quad .LVL12
 2671      00000000 
 2672 0120 0100     	 .value 0x1
GAS LISTING  			page 54


 2673 0122 54       	 .byte 0x54
 2674 0123 00000000 	 .quad .LVL12
 2674      00000000 
 2675 012b 00000000 	 .quad .LVL14-1
 2675      00000000 
 2676 0133 0100     	 .value 0x1
 2677 0135 50       	 .byte 0x50
 2678 0136 00000000 	 .quad .LVL14-1
 2678      00000000 
 2679 013e 00000000 	 .quad .LVL31
 2679      00000000 
 2680 0146 0200     	 .value 0x2
 2681 0148 77       	 .byte 0x77
 2682 0149 00       	 .sleb128 0
 2683 014a 00000000 	 .quad .LVL31
 2683      00000000 
 2684 0152 00000000 	 .quad .LVL32
 2684      00000000 
 2685 015a 0200     	 .value 0x2
 2686 015c 91       	 .byte 0x91
 2687 015d 40       	 .sleb128 -64
 2688 015e 00000000 	 .quad .LVL32
 2688      00000000 
 2689 0166 00000000 	 .quad .LVL38
 2689      00000000 
 2690 016e 0200     	 .value 0x2
 2691 0170 77       	 .byte 0x77
 2692 0171 00       	 .sleb128 0
 2693 0172 00000000 	 .quad .LVL38
 2693      00000000 
 2694 017a 00000000 	 .quad .LVL39
 2694      00000000 
 2695 0182 0100     	 .value 0x1
 2696 0184 54       	 .byte 0x54
 2697 0185 00000000 	 .quad .LVL39
 2697      00000000 
 2698 018d 00000000 	 .quad .LFE98
 2698      00000000 
 2699 0195 0200     	 .value 0x2
 2700 0197 77       	 .byte 0x77
 2701 0198 00       	 .sleb128 0
 2702 0199 00000000 	 .quad 0
 2702      00000000 
 2703 01a1 00000000 	 .quad 0
 2703      00000000 
 2704              	.LLST3:
 2705 01a9 00000000 	 .quad .LVL13
 2705      00000000 
 2706 01b1 00000000 	 .quad .LVL22
 2706      00000000 
 2707 01b9 0100     	 .value 0x1
 2708 01bb 53       	 .byte 0x53
 2709 01bc 00000000 	 .quad .LVL36
 2709      00000000 
 2710 01c4 00000000 	 .quad .LVL38
 2710      00000000 
 2711 01cc 0100     	 .value 0x1
GAS LISTING  			page 55


 2712 01ce 53       	 .byte 0x53
 2713 01cf 00000000 	 .quad .LVL42
 2713      00000000 
 2714 01d7 00000000 	 .quad .LFE98
 2714      00000000 
 2715 01df 0100     	 .value 0x1
 2716 01e1 53       	 .byte 0x53
 2717 01e2 00000000 	 .quad 0
 2717      00000000 
 2718 01ea 00000000 	 .quad 0
 2718      00000000 
 2719              	.LLST4:
 2720 01f2 00000000 	 .quad .LVL16
 2720      00000000 
 2721 01fa 00000000 	 .quad .LVL17-1
 2721      00000000 
 2722 0202 0100     	 .value 0x1
 2723 0204 50       	 .byte 0x50
 2724 0205 00000000 	 .quad .LVL36
 2724      00000000 
 2725 020d 00000000 	 .quad .LVL37-1
 2725      00000000 
 2726 0215 0100     	 .value 0x1
 2727 0217 50       	 .byte 0x50
 2728 0218 00000000 	 .quad 0
 2728      00000000 
 2729 0220 00000000 	 .quad 0
 2729      00000000 
 2730              	.LLST5:
 2731 0228 00000000 	 .quad .LVL21
 2731      00000000 
 2732 0230 00000000 	 .quad .LVL22
 2732      00000000 
 2733 0238 0100     	 .value 0x1
 2734 023a 50       	 .byte 0x50
 2735 023b 00000000 	 .quad .LVL22
 2735      00000000 
 2736 0243 00000000 	 .quad .LVL30
 2736      00000000 
 2737 024b 0100     	 .value 0x1
 2738 024d 56       	 .byte 0x56
 2739 024e 00000000 	 .quad .LVL32
 2739      00000000 
 2740 0256 00000000 	 .quad .LVL36
 2740      00000000 
 2741 025e 0100     	 .value 0x1
 2742 0260 56       	 .byte 0x56
 2743 0261 00000000 	 .quad .LVL42
 2743      00000000 
 2744 0269 00000000 	 .quad .LVL43-1
 2744      00000000 
 2745 0271 0100     	 .value 0x1
 2746 0273 50       	 .byte 0x50
 2747 0274 00000000 	 .quad .LVL43-1
 2747      00000000 
 2748 027c 00000000 	 .quad .LVL44
 2748      00000000 
GAS LISTING  			page 56


 2749 0284 0100     	 .value 0x1
 2750 0286 56       	 .byte 0x56
 2751 0287 00000000 	 .quad 0
 2751      00000000 
 2752 028f 00000000 	 .quad 0
 2752      00000000 
 2753              	.LLST6:
 2754 0297 00000000 	 .quad .LVL22
 2754      00000000 
 2755 029f 00000000 	 .quad .LVL24
 2755      00000000 
 2756 02a7 0100     	 .value 0x1
 2757 02a9 53       	 .byte 0x53
 2758 02aa 00000000 	 .quad .LVL24
 2758      00000000 
 2759 02b2 00000000 	 .quad .LVL25
 2759      00000000 
 2760 02ba 0100     	 .value 0x1
 2761 02bc 50       	 .byte 0x50
 2762 02bd 00000000 	 .quad .LVL25
 2762      00000000 
 2763 02c5 00000000 	 .quad .LVL30
 2763      00000000 
 2764 02cd 0100     	 .value 0x1
 2765 02cf 53       	 .byte 0x53
 2766 02d0 00000000 	 .quad .LVL32
 2766      00000000 
 2767 02d8 00000000 	 .quad .LVL33
 2767      00000000 
 2768 02e0 0100     	 .value 0x1
 2769 02e2 50       	 .byte 0x50
 2770 02e3 00000000 	 .quad .LVL33
 2770      00000000 
 2771 02eb 00000000 	 .quad .LVL36
 2771      00000000 
 2772 02f3 0100     	 .value 0x1
 2773 02f5 53       	 .byte 0x53
 2774 02f6 00000000 	 .quad 0
 2774      00000000 
 2775 02fe 00000000 	 .quad 0
 2775      00000000 
 2776              	.LLST7:
 2777 0306 00000000 	 .quad .LVL25
 2777      00000000 
 2778 030e 00000000 	 .quad .LVL26-1
 2778      00000000 
 2779 0316 0100     	 .value 0x1
 2780 0318 50       	 .byte 0x50
 2781 0319 00000000 	 .quad .LVL27
 2781      00000000 
 2782 0321 00000000 	 .quad .LVL28-1
 2782      00000000 
 2783 0329 0100     	 .value 0x1
 2784 032b 50       	 .byte 0x50
 2785 032c 00000000 	 .quad 0
 2785      00000000 
 2786 0334 00000000 	 .quad 0
GAS LISTING  			page 57


 2786      00000000 
 2787              	 .section .debug_aranges,"",@progbits
 2788 0000 3C000000 	 .long 0x3c
 2789 0004 0200     	 .value 0x2
 2790 0006 00000000 	 .long .Ldebug_info0
 2791 000a 08       	 .byte 0x8
 2792 000b 00       	 .byte 0
 2793 000c 0000     	 .value 0
 2794 000e 0000     	 .value 0
 2795 0010 00000000 	 .quad .Ltext0
 2795      00000000 
 2796 0018 52000000 	 .quad .Letext0-.Ltext0
 2796      00000000 
 2797 0020 00000000 	 .quad .LFB98
 2797      00000000 
 2798 0028 DD010000 	 .quad .LFE98-.LFB98
 2798      00000000 
 2799 0030 00000000 	 .quad 0
 2799      00000000 
 2800 0038 00000000 	 .quad 0
 2800      00000000 
 2801              	 .section .debug_ranges,"",@progbits
 2802              	.Ldebug_ranges0:
 2803 0000 00000000 	 .quad .LBB4
 2803      00000000 
 2804 0008 00000000 	 .quad .LBE4
 2804      00000000 
 2805 0010 00000000 	 .quad .LBB8
 2805      00000000 
 2806 0018 00000000 	 .quad .LBE8
 2806      00000000 
 2807 0020 00000000 	 .quad .LBB9
 2807      00000000 
 2808 0028 00000000 	 .quad .LBE9
 2808      00000000 
 2809 0030 00000000 	 .quad 0
 2809      00000000 
 2810 0038 00000000 	 .quad 0
 2810      00000000 
 2811 0040 00000000 	 .quad .Ltext0
 2811      00000000 
 2812 0048 00000000 	 .quad .Letext0
 2812      00000000 
 2813 0050 00000000 	 .quad .LFB98
 2813      00000000 
 2814 0058 00000000 	 .quad .LFE98
 2814      00000000 
 2815 0060 00000000 	 .quad 0
 2815      00000000 
 2816 0068 00000000 	 .quad 0
 2816      00000000 
 2817              	 .section .debug_line,"",@progbits
 2818              	.Ldebug_line0:
 2819 0000 2A020000 	 .section .debug_str,"MS",@progbits,1
 2819      0200A401 
 2819      00000101 
 2819      FB0E0D00 
GAS LISTING  			page 58


 2819      01010101 
 2820              	.LASF115:
 2821 0000 7072696E 	 .string "printf"
 2821      746600
 2822              	.LASF8:
 2823 0007 5F5F6F66 	 .string "__off_t"
 2823      665F7400 
 2824              	.LASF128:
 2825 000f 474E5520 	 .string "GNU C 4.8.5 20150623 (Red Hat 4.8.5-16) -mtune=generic -march=x86-64 -g -g -O2 -fno-strict-aliasing -fno-common"
 2825      4320342E 
 2825      382E3520 
 2825      32303135 
 2825      30363233 
 2826              	.LASF27:
 2827 007f 5F636861 	 .string "_chain"
 2827      696E00
 2828              	.LASF57:
 2829 0086 6170725F 	 .string "apr_allocator_t"
 2829      616C6C6F 
 2829      6361746F 
 2829      725F7400 
 2830              	.LASF7:
 2831 0096 73697A65 	 .string "size_t"
 2831      5F7400
 2832              	.LASF89:
 2833 009d 70616765 	 .string "page_size"
 2833      5F73697A 
 2833      6500
 2834              	.LASF122:
 2835 00a7 6170725F 	 .string "apr_app_initialize"
 2835      6170705F 
 2835      696E6974 
 2835      69616C69 
 2835      7A6500
 2836              	.LASF33:
 2837 00ba 5F73686F 	 .string "_shortbuf"
 2837      72746275 
 2837      6600
 2838              	.LASF102:
 2839 00c4 656E7472 	 .string "entry_index"
 2839      795F696E 
 2839      64657800 
 2840              	.LASF63:
 2841 00d0 656E7472 	 .string "entry_size"
 2841      795F7369 
 2841      7A6500
 2842              	.LASF21:
 2843 00db 5F494F5F 	 .string "_IO_buf_base"
 2843      6275665F 
 2843      62617365 
 2843      00
 2844              	.LASF119:
 2845 00e8 63685F6D 	 .string "ch_mmap_packets_format_iterator_commit"
 2845      6D61705F 
 2845      7061636B 
 2845      6574735F 
 2845      666F726D 
GAS LISTING  			page 59


 2846              	.LASF84:
 2847 010f 63685F6D 	 .string "ch_mmap_packets_iterator_t"
 2847      6D61705F 
 2847      7061636B 
 2847      6574735F 
 2847      69746572 
 2848              	.LASF49:
 2849 012a 6C6F6E67 	 .string "long long unsigned int"
 2849      206C6F6E 
 2849      6720756E 
 2849      7369676E 
 2849      65642069 
 2850              	.LASF22:
 2851 0141 5F494F5F 	 .string "_IO_buf_end"
 2851      6275665F 
 2851      656E6400 
 2852              	.LASF86:
 2853 014d 6E657874 	 .string "next"
 2853      00
 2854              	.LASF54:
 2855 0152 6170725F 	 .string "apr_status_t"
 2855      73746174 
 2855      75735F74 
 2855      00
 2856              	.LASF48:
 2857 015f 6C6F6E67 	 .string "long long int"
 2857      206C6F6E 
 2857      6720696E 
 2857      7400
 2858              	.LASF4:
 2859 016d 7369676E 	 .string "signed char"
 2859      65642063 
 2859      68617200 
 2860              	.LASF127:
 2861 0179 5F5F6275 	 .string "__builtin_fwrite"
 2861      696C7469 
 2861      6E5F6677 
 2861      72697465 
 2861      00
 2862              	.LASF138:
 2863 018a 63685F73 	 .string "ch_strerror_init"
 2863      74726572 
 2863      726F725F 
 2863      696E6974 
 2863      00
 2864              	.LASF91:
 2865 019b 6673697A 	 .string "fsize"
 2865      6500
 2866              	.LASF94:
 2867 01a1 6D6D6170 	 .string "mmap_entries_start"
 2867      5F656E74 
 2867      72696573 
 2867      5F737461 
 2867      727400
 2868              	.LASF74:
 2869 01b4 73656E74 	 .string "sent_seq"
 2869      5F736571 
GAS LISTING  			page 60


 2869      00
 2870              	.LASF16:
 2871 01bd 5F494F5F 	 .string "_IO_read_end"
 2871      72656164 
 2871      5F656E64 
 2871      00
 2872              	.LASF121:
 2873 01ca 75736C65 	 .string "usleep"
 2873      657000
 2874              	.LASF28:
 2875 01d1 5F66696C 	 .string "_fileno"
 2875      656E6F00 
 2876              	.LASF6:
 2877 01d9 6C6F6E67 	 .string "long int"
 2877      20696E74 
 2877      00
 2878              	.LASF66:
 2879 01e2 6D616769 	 .string "magic"
 2879      6300
 2880              	.LASF14:
 2881 01e8 5F666C61 	 .string "_flags"
 2881      677300
 2882              	.LASF136:
 2883 01ef 5F5F6275 	 .string "__builtin_puts"
 2883      696C7469 
 2883      6E5F7075 
 2883      747300
 2884              	.LASF31:
 2885 01fe 5F637572 	 .string "_cur_column"
 2885      5F636F6C 
 2885      756D6E00 
 2886              	.LASF92:
 2887 020a 6D6D6170 	 .string "mmap_header"
 2887      5F686561 
 2887      64657200 
 2888              	.LASF71:
 2889 0216 63685F6D 	 .string "ch_mmap_packets_entry_t"
 2889      6D61705F 
 2889      7061636B 
 2889      6574735F 
 2889      656E7472 
 2890              	.LASF51:
 2891 022e 75696E74 	 .string "uint16_t"
 2891      31365F74 
 2891      00
 2892              	.LASF56:
 2893 0237 6170725F 	 .string "apr_abortfunc_t"
 2893      61626F72 
 2893      7466756E 
 2893      635F7400 
 2894              	.LASF30:
 2895 0247 5F6F6C64 	 .string "_old_offset"
 2895      5F6F6666 
 2895      73657400 
 2896              	.LASF35:
 2897 0253 5F6F6666 	 .string "_offset"
 2897      73657400 
GAS LISTING  			page 61


 2898              	.LASF110:
 2899 025b 70666D74 	 .string "pfmt"
 2899      00
 2900              	.LASF45:
 2901 0260 5F6E6578 	 .string "_next"
 2901      7400
 2902              	.LASF101:
 2903 0266 63685F6D 	 .string "ch_mmap_buf_t"
 2903      6D61705F 
 2903      6275665F 
 2903      7400
 2904              	.LASF44:
 2905 0274 5F494F5F 	 .string "_IO_marker"
 2905      6D61726B 
 2905      657200
 2906              	.LASF80:
 2907 027f 6473745F 	 .string "dst_port"
 2907      706F7274 
 2907      00
 2908              	.LASF3:
 2909 0288 756E7369 	 .string "unsigned int"
 2909      676E6564 
 2909      20696E74 
 2909      00
 2910              	.LASF60:
 2911 0295 63685F6D 	 .string "ch_mmap_packets_format_t"
 2911      6D61705F 
 2911      7061636B 
 2911      6574735F 
 2911      666F726D 
 2912              	.LASF0:
 2913 02ae 6C6F6E67 	 .string "long unsigned int"
 2913      20756E73 
 2913      69676E65 
 2913      6420696E 
 2913      7400
 2914              	.LASF19:
 2915 02c0 5F494F5F 	 .string "_IO_write_ptr"
 2915      77726974 
 2915      655F7074 
 2915      7200
 2916              	.LASF69:
 2917 02ce 656E7472 	 .string "entries_number"
 2917      6965735F 
 2917      6E756D62 
 2917      657200
 2918              	.LASF107:
 2919 02dd 7369676E 	 .string "signum"
 2919      756D00
 2920              	.LASF46:
 2921 02e4 5F736275 	 .string "_sbuf"
 2921      6600
 2922              	.LASF83:
 2923 02ea 64617461 	 .string "data"
 2923      00
 2924              	.LASF2:
 2925 02ef 73686F72 	 .string "short unsigned int"
GAS LISTING  			page 62


 2925      7420756E 
 2925      7369676E 
 2925      65642069 
 2925      6E7400
 2926              	.LASF116:
 2927 0302 63685F6D 	 .string "ch_mmap_packets_format_iterator_prefare"
 2927      6D61705F 
 2927      7061636B 
 2927      6574735F 
 2927      666F726D 
 2928              	.LASF23:
 2929 032a 5F494F5F 	 .string "_IO_save_base"
 2929      73617665 
 2929      5F626173 
 2929      6500
 2930              	.LASF112:
 2931 0338 73746469 	 .string "stdin"
 2931      6E00
 2932              	.LASF34:
 2933 033e 5F6C6F63 	 .string "_lock"
 2933      6B00
 2934              	.LASF41:
 2935 0344 5F6D6F64 	 .string "_mode"
 2935      6500
 2936              	.LASF113:
 2937 034a 7374646F 	 .string "stdout"
 2937      757400
 2938              	.LASF105:
 2939 0351 69746572 	 .string "iter"
 2939      00
 2940              	.LASF75:
 2941 0356 72656376 	 .string "recv_ack"
 2941      5F61636B 
 2941      00
 2942              	.LASF85:
 2943 035f 70686561 	 .string "pheader"
 2943      64657200 
 2944              	.LASF132:
 2945 0367 69735F77 	 .string "is_write"
 2945      72697465 
 2945      00
 2946              	.LASF137:
 2947 0370 70757473 	 .string "puts"
 2947      00
 2948              	.LASF111:
 2949 0375 69735F65 	 .string "is_exit"
 2949      78697400 
 2950              	.LASF67:
 2951 037d 6D616A6F 	 .string "major_version"
 2951      725F7665 
 2951      7273696F 
 2951      6E00
 2952              	.LASF10:
 2953 038b 73697A65 	 .string "sizetype"
 2953      74797065 
 2953      00
 2954              	.LASF95:
GAS LISTING  			page 63


 2955 0394 6D6D6170 	 .string "mmap_entries_count"
 2955      5F656E74 
 2955      72696573 
 2955      5F636F75 
 2955      6E7400
 2956              	.LASF15:
 2957 03a7 5F494F5F 	 .string "_IO_read_ptr"
 2957      72656164 
 2957      5F707472 
 2957      00
 2958              	.LASF53:
 2959 03b4 75696E74 	 .string "uint64_t"
 2959      36345F74 
 2959      00
 2960              	.LASF72:
 2961 03bd 70726F74 	 .string "proto_id"
 2961      6F5F6964 
 2961      00
 2962              	.LASF131:
 2963 03c6 5F494F5F 	 .string "_IO_lock_t"
 2963      6C6F636B 
 2963      5F7400
 2964              	.LASF43:
 2965 03d1 5F494F5F 	 .string "_IO_FILE"
 2965      46494C45 
 2965      00
 2966              	.LASF76:
 2967 03da 74696D65 	 .string "time"
 2967      00
 2968              	.LASF58:
 2969 03df 6170725F 	 .string "apr_sigfunc_t"
 2969      73696766 
 2969      756E635F 
 2969      7400
 2970              	.LASF62:
 2971 03ed 6861735F 	 .string "has_pheader"
 2971      70686561 
 2971      64657200 
 2972              	.LASF118:
 2973 03f9 63685F6D 	 .string "ch_mmap_packets_entry_dump"
 2973      6D61705F 
 2973      7061636B 
 2973      6574735F 
 2973      656E7472 
 2974              	.LASF126:
 2975 0414 63685F6D 	 .string "ch_mmap_packets_format_create"
 2975      6D61705F 
 2975      7061636B 
 2975      6574735F 
 2975      666F726D 
 2976              	.LASF47:
 2977 0432 5F706F73 	 .string "_pos"
 2977      00
 2978              	.LASF55:
 2979 0437 6170725F 	 .string "apr_pool_t"
 2979      706F6F6C 
 2979      5F7400
GAS LISTING  			page 64


 2980              	.LASF77:
 2981 0442 7372635F 	 .string "src_ip"
 2981      697000
 2982              	.LASF78:
 2983 0449 6473745F 	 .string "dst_ip"
 2983      697000
 2984              	.LASF104:
 2985 0450 63685F62 	 .string "ch_bin_format_t"
 2985      696E5F66 
 2985      6F726D61 
 2985      745F7400 
 2986              	.LASF120:
 2987 0460 63685F6C 	 .string "ch_log_error_core"
 2987      6F675F65 
 2987      72726F72 
 2987      5F636F72 
 2987      6500
 2988              	.LASF1:
 2989 0472 756E7369 	 .string "unsigned char"
 2989      676E6564 
 2989      20636861 
 2989      7200
 2990              	.LASF81:
 2991 0480 705F7369 	 .string "p_size"
 2991      7A6500
 2992              	.LASF90:
 2993 0487 666E616D 	 .string "fname"
 2993      6500
 2994              	.LASF5:
 2995 048d 73686F72 	 .string "short int"
 2995      7420696E 
 2995      7400
 2996              	.LASF96:
 2997 0497 6D6D6170 	 .string "mmap_entry_size"
 2997      5F656E74 
 2997      72795F73 
 2997      697A6500 
 2998              	.LASF130:
 2999 04a7 2F686F6D 	 .string "/home/shajf/dev/antell/CloudHand/build/tool"
 2999      652F7368 
 2999      616A662F 
 2999      6465762F 
 2999      616E7465 
 3000              	.LASF29:
 3001 04d3 5F666C61 	 .string "_flags2"
 3001      67733200 
 3002              	.LASF61:
 3003 04db 6375725F 	 .string "cur_pheader"
 3003      70686561 
 3003      64657200 
 3004              	.LASF106:
 3005 04e7 5F5F6675 	 .string "__func__"
 3005      6E635F5F 
 3005      00
 3006              	.LASF82:
 3007 04f0 64617461 	 .string "data_len"
 3007      5F6C656E 
GAS LISTING  			page 65


 3007      00
 3008              	.LASF13:
 3009 04f9 46494C45 	 .string "FILE"
 3009      00
 3010              	.LASF134:
 3011 04fe 72656164 	 .string "read_packets_interface"
 3011      5F706163 
 3011      6B657473 
 3011      5F696E74 
 3011      65726661 
 3012              	.LASF117:
 3013 0515 65786974 	 .string "exit"
 3013      00
 3014              	.LASF26:
 3015 051a 5F6D6172 	 .string "_markers"
 3015      6B657273 
 3015      00
 3016              	.LASF88:
 3017 0523 63685F6D 	 .string "ch_mmap_t"
 3017      6D61705F 
 3017      7400
 3018              	.LASF52:
 3019 052d 75696E74 	 .string "uint32_t"
 3019      33325F74 
 3019      00
 3020              	.LASF12:
 3021 0536 63686172 	 .string "char"
 3021      00
 3022              	.LASF97:
 3023 053b 6D6D6170 	 .string "mmap_entries_count_cur"
 3023      5F656E74 
 3023      72696573 
 3023      5F636F75 
 3023      6E745F63 
 3024              	.LASF59:
 3025 0552 63685F65 	 .string "ch_err_t"
 3025      72725F74 
 3025      00
 3026              	.LASF98:
 3027 055b 6D6D6170 	 .string "mmap_write_entry_pos"
 3027      5F777269 
 3027      74655F65 
 3027      6E747279 
 3027      5F706F73 
 3028              	.LASF9:
 3029 0570 5F5F6F66 	 .string "__off64_t"
 3029      6636345F 
 3029      7400
 3030              	.LASF65:
 3031 057a 62666D74 	 .string "bfmt"
 3031      00
 3032              	.LASF17:
 3033 057f 5F494F5F 	 .string "_IO_read_base"
 3033      72656164 
 3033      5F626173 
 3033      6500
 3034              	.LASF25:
GAS LISTING  			page 66


 3035 058d 5F494F5F 	 .string "_IO_save_end"
 3035      73617665 
 3035      5F656E64 
 3035      00
 3036              	.LASF93:
 3037 059a 63685F6D 	 .string "ch_mmap_header_t"
 3037      6D61705F 
 3037      68656164 
 3037      65725F74 
 3037      00
 3038              	.LASF87:
 3039 05ab 656E7472 	 .string "entries_count"
 3039      6965735F 
 3039      636F756E 
 3039      7400
 3040              	.LASF133:
 3041 05b9 68616E64 	 .string "handle_signals"
 3041      6C655F73 
 3041      69676E61 
 3041      6C7300
 3042              	.LASF36:
 3043 05c8 5F5F7061 	 .string "__pad1"
 3043      643100
 3044              	.LASF37:
 3045 05cf 5F5F7061 	 .string "__pad2"
 3045      643200
 3046              	.LASF38:
 3047 05d6 5F5F7061 	 .string "__pad3"
 3047      643300
 3048              	.LASF39:
 3049 05dd 5F5F7061 	 .string "__pad4"
 3049      643400
 3050              	.LASF40:
 3051 05e4 5F5F7061 	 .string "__pad5"
 3051      643500
 3052              	.LASF20:
 3053 05eb 5F494F5F 	 .string "_IO_write_end"
 3053      77726974 
 3053      655F656E 
 3053      6400
 3054              	.LASF11:
 3055 05f9 5F5F7573 	 .string "__useconds_t"
 3055      65636F6E 
 3055      64735F74 
 3055      00
 3056              	.LASF42:
 3057 0606 5F756E75 	 .string "_unused2"
 3057      73656432 
 3057      00
 3058              	.LASF114:
 3059 060f 73746465 	 .string "stderr"
 3059      727200
 3060              	.LASF109:
 3061 0616 61726776 	 .string "argv"
 3061      00
 3062              	.LASF139:
 3063 061b 66777269 	 .string "fwrite"
GAS LISTING  			page 67


 3063      746500
 3064              	.LASF99:
 3065 0622 6D6D6170 	 .string "mmap_read_entry_pos"
 3065      5F726561 
 3065      645F656E 
 3065      7472795F 
 3065      706F7300 
 3066              	.LASF123:
 3067 0636 6170725F 	 .string "apr_pool_create_ex"
 3067      706F6F6C 
 3067      5F637265 
 3067      6174655F 
 3067      657800
 3068              	.LASF50:
 3069 0649 75696E74 	 .string "uint8_t"
 3069      385F7400 
 3070              	.LASF24:
 3071 0651 5F494F5F 	 .string "_IO_backup_base"
 3071      6261636B 
 3071      75705F62 
 3071      61736500 
 3072              	.LASF124:
 3073 0661 63685F6C 	 .string "ch_log_init"
 3073      6F675F69 
 3073      6E697400 
 3074              	.LASF73:
 3075 066d 666C6167 	 .string "flags"
 3075      7300
 3076              	.LASF32:
 3077 0673 5F767461 	 .string "_vtable_offset"
 3077      626C655F 
 3077      6F666673 
 3077      657400
 3078              	.LASF108:
 3079 0682 61726763 	 .string "argc"
 3079      00
 3080              	.LASF103:
 3081 0687 73746172 	 .string "start"
 3081      7400
 3082              	.LASF125:
 3083 068d 6170725F 	 .string "apr_signal"
 3083      7369676E 
 3083      616C00
 3084              	.LASF100:
 3085 0698 6D6D6170 	 .string "mmap_priv_data_size"
 3085      5F707269 
 3085      765F6461 
 3085      74615F73 
 3085      697A6500 
 3086              	.LASF129:
 3087 06ac 63685F70 	 .string "ch_packets_reader.c"
 3087      61636B65 
 3087      74735F72 
 3087      65616465 
 3087      722E6300 
 3088              	.LASF135:
 3089 06c0 6D61696E 	 .string "main"
GAS LISTING  			page 68


 3089      00
 3090              	.LASF18:
 3091 06c5 5F494F5F 	 .string "_IO_write_base"
 3091      77726974 
 3091      655F6261 
 3091      736500
 3092              	.LASF64:
 3093 06d4 63685F6D 	 .string "ch_mmap_packets_header_t"
 3093      6D61705F 
 3093      7061636B 
 3093      6574735F 
 3093      68656164 
 3094              	.LASF79:
 3095 06ed 7372635F 	 .string "src_port"
 3095      706F7274 
 3095      00
 3096              	.LASF70:
 3097 06f6 656E7472 	 .string "entries_size"
 3097      6965735F 
 3097      73697A65 
 3097      00
 3098              	.LASF68:
 3099 0703 6D696E6F 	 .string "minor_version"
 3099      725F7665 
 3099      7273696F 
 3099      6E00
 3100              	 .ident "GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-16)"
 3101              	 .section .note.GNU-stack,"",@progbits
GAS LISTING  			page 69


DEFINED SYMBOLS
                            *ABS*:0000000000000000 ch_packets_reader.c
    {standard input}:17     .text:0000000000000000 handle_signals
    {standard input}:336    .bss:0000000000000000 is_exit
    {standard input}:90     .text.startup:0000000000000000 main
    {standard input}:333    .rodata:0000000000000000 __func__.5664

UNDEFINED SYMBOLS
puts
exit
printf
apr_app_initialize
apr_pool_create_ex
ch_strerror_init
ch_log_init
apr_signal
ch_mmap_packets_format_create
ch_mmap_packets_format_iterator_prefare
stdout
ch_mmap_packets_entry_dump
ch_mmap_packets_format_iterator_commit
ch_log_error_core
usleep
stderr
fwrite
