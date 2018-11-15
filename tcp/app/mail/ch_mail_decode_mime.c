#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ch_log.h"
#include "ch_string.h"
#include "ch_mail_common.h"
#include "ch_mail_decode_mime.h"

/* Character constants */
#ifndef CR
#define CR  13
#define LF  10
#endif

#define CRLF             "\r\n"
#define COLON             58
#define DASH              45
#define PRINTABLE_START   33
#define PRINTABLE_END    126
#define UC_START          65
#define UC_END            90
#define LC_START          97
#define LC_END           122
#define UC_LC_DIFF        32
#define EOL_LEN            2

/* Base-64 constants */
#define BASE64_STR        "Base64"

/* Mime Constants */
#define MAX_LINE_LEN       998 /* Def in RFC 2045, excluding CRLF sequence */
#define MAX_ENC_LINE_LEN    76 /* Def in RFC 2045, excluding CRLF sequence */
#define MAX_HEADER_NAME     75 /* 75 + ":" = 76 */
#define MAX_HEADER_VALUE  2000 /* Default - arbitrary limit */
#define BOUNDARY_BUF       256
#define CTNT_TYPE_STR     "content-type"
#define CTNT_DISP_STR     "content-disposition"
#define CTNT_TRAN_STR     "content-transfer-encoding"
#define MSG_ID_STR        "message-id"
#define BND_START_STR     "boundary=\""
#define TOK_END_STR       "\""
#define MSG_STR           "message/"
#define MULTIPART_STR     "multipart/"
#define QP_STR            "quoted-printable"
#define TXT_STR           "text/plain"
#define HTML_STR          "text/html"
#define URL_STR           "http://"

/* Memory Usage Constants */
#define STACK_FREE_NODES  10

/* Other Constants */
#define MAX_IP4_CHARS  15
#define MAX_IP6_CHARS  39

/* Globally hold configuration data */
static MimeDecConfig mime_dec_config = { 1, 1, 0, MAX_HEADER_VALUE };

/* Mime Parser String translation */
static const char *StateFlags[] = { "NONE",
        "HEADER_READY",
        "HEADER_STARTED",
        "HEADER_DONE",
        "BODY_STARTED",
        "BODY_DONE",
        "BODY_END_BOUND",
        "PARSE_DONE",
        "PARSE_ERROR",
        NULL };

static int
MimeDecodeBase64(uint8_t *dest, const uint8_t *src, uint32_t len)
{
	ch_str_t d;
	ch_str_t s;
	unsigned char buf[DATA_CHUNK_SIZE] = {0};

	s.data = (unsigned char *)src;
	s.len = len;

	d.data = buf;
	d.len = 0;
	
	if (ch_decode_base64(&d, &s) != 0) {
		CH_ERR_LOG("%s: decode base64 error.\n", __func__);
		return 0;
	}

	memcpy(dest, d.data, d.len);

	return d.len;
}

/**
 * \brief Set global config policy
 *
 * \param config Config policy to set
 * \return none
 */
void MimeDecSetConfig(MimeDecConfig *config)
{
    if (config != NULL) {
        mime_dec_config = *config;

        /* Set to default */
        if (mime_dec_config.header_value_depth == 0) {
            mime_dec_config.header_value_depth = MAX_HEADER_VALUE;
        }
    } else {
        CH_DEBUG_LOG("Invalid null configuration parameters");
    }
}

/**
 * \brief Get global config policy
 *
 * \return config data structure
 */
MimeDecConfig * MimeDecGetConfig(void)
{
    return &mime_dec_config;
}

/**
 * \brief Follow the 'next' pointers to the leaf
 *
 * \param node The root entity
 *
 * \return Pointer to leaf on 'next' side
 *
 */
static MimeDecEntity *findLastSibling(MimeDecEntity *node)
{
    if (node == NULL)
        return NULL;
    while(node->next != NULL)
        node = node->next;
    return node;
}

/**
 * \brief Frees a mime entity tree
 *
 * \param entity The root entity
 *
 * \return none
 *
 */
void MimeDecFreeEntity (MimeDecEntity *entity)
{
    if (entity == NULL)
        return;

    MimeDecEntity *lastSibling = findLastSibling(entity);

    while (entity != NULL) {
        /**
         * Move child to next
         * Transform tree into list
         */ 
        if (entity->child != NULL) {
            lastSibling->next = entity->child;
            lastSibling = findLastSibling(lastSibling);
        }

        /**
         * Move to next element
         */
        MimeDecEntity *old = entity;
        entity = entity->next;

		if (old->ctnt_tran) {
			SCFree(old->ctnt_tran->data);
			SCFree(old->ctnt_tran);
		}

		if (old->ctnt_disp) {
			SCFree(old->ctnt_disp->data);
			SCFree(old->ctnt_disp);
		}

		if (old->ctnt_type) {
			SCFree(old->ctnt_type->data);
			SCFree(old->ctnt_type);
		}

		if (old->msg_id) {
			SCFree(old->msg_id->data);
			SCFree(old->msg_id);
		}

		SCFree(old->filename);

        SCFree(old);
    }
}

/**
 * \brief Creates and adds a child entity to the specified parent entity
 *
 * \param parent The parent entity
 *
 * \return The child entity, or NULL if the operation fails
 *
 */
MimeDecEntity * MimeDecAddEntity(MimeDecEntity *parent)
{
    MimeDecEntity *curr, *node = SCMalloc(sizeof(MimeDecEntity));
    if (node == NULL) {
        CH_DEBUG_LOG("memory allocation failed");
        return NULL;
    }
    memset(node, 0x00, sizeof(MimeDecEntity));

    /* If parent is NULL then just return the new pointer */
    if (parent != NULL) {
        if (parent->child == NULL) {
            parent->child = node;
        } else {
            curr = parent->child;
            while (curr->next != NULL) {
                curr = curr->next;
            }
            curr->next = node;
        }
    }

    return node;
}

static ch_str_t *mime_create_new_filed(const uint8_t *value, uint32_t vlen)
{
	ch_str_t *n = NULL;

	n = (ch_str_t *)malloc(sizeof(ch_str_t));
	if (n == NULL) {
		CH_ERR_LOG("%s, malloc failed.\n", __func__);
		return NULL;
	}

    if (vlen > 0) {
		n->data = (unsigned char *) malloc(vlen);
		if (n->data == NULL) {
			goto fail;
		}
		memcpy(n->data, value, vlen);
        n->len = vlen;
    }

	return n;

fail:
	free(n);
	return NULL;
}

/**
 * \brief Creates a mime header field and fills in its values and adds it to the
 * specified entity
 *
 * \param entity Entity in which to add the field
 * \param name String containing the name
 * \param nlen Length of the name
 * \param value String containing the value
 * \param vlen Length of the value
 *
 * \return The field or NULL if the operation fails
 */
static ch_str_t * MimeDecFillField(MimeDecEntity *entity, uint8_t *name,
        uint32_t nlen, const uint8_t *value, uint32_t vlen)
{
	ch_str_t *n = NULL;
	uint32_t u;

    if (nlen == 0 && vlen == 0)
        return NULL;

	for (u = 0; u < nlen; u++)
		name[u] = tolower(name[u]);

	if (strlen(CTNT_TYPE_STR) == nlen) {
		if (memcmp(name, CTNT_TYPE_STR, nlen) == 0) {
			n = mime_create_new_filed(value, vlen);
			entity->ctnt_type = n;
		}
	} else if (strlen(CTNT_DISP_STR) == nlen) {
		if (memcmp(name, CTNT_DISP_STR, nlen) == 0) {
			n = mime_create_new_filed(value, vlen);
			entity->ctnt_disp = n;
		}
	} else if (strlen(CTNT_TRAN_STR) == nlen) {
		if (memcmp(name, CTNT_TRAN_STR, nlen) == 0) {
			n = mime_create_new_filed(value, vlen);
			entity->ctnt_tran = n;
		}
	} else if (strlen(MSG_ID_STR) == nlen) {
		if (memcmp(name, MSG_ID_STR, nlen) == 0) {
			n = mime_create_new_filed(value, vlen);
			entity->msg_id = n;
		}
	} 

	return n;
}

/**
 * \brief Pushes a node onto a stack and returns the new node.
 *
 * \param stack The top of the stack
 *
 * \return pointer to a new node, otherwise NULL if it fails
 */
static MimeDecStackNode * PushStack(MimeDecStack *stack)
{
    /* Attempt to pull from free nodes list */
    MimeDecStackNode *node = stack->free_nodes;
    if (node == NULL) {
        node = SCMalloc(sizeof(MimeDecStackNode));
        if (node == NULL) {
            CH_DEBUG_LOG("memory allocation failed");
            return NULL;
        }
    } else {
        /* Move free nodes pointer over */
        stack->free_nodes = stack->free_nodes->next;
        stack->free_nodes_cnt--;
    }
    memset(node, 0x00, sizeof(MimeDecStackNode));

    /* Push to top of stack */
    node->next = stack->top;
    stack->top = node;

    /* Return a pointer to the top of the stack */
    return node;
}

/**
 * \brief Pops the top node from the stack and returns the next node.
 *
 * \param stack The top of the stack
 *
 * \return pointer to the next node, otherwise NULL if no nodes remain
 */
static MimeDecStackNode * PopStack(MimeDecStack *stack)
{
    /* Move stack pointer to next item */
    MimeDecStackNode *curr = stack->top;
    if (curr != NULL) {
        curr = curr->next;
    }

    /* Always free alloc'd memory */
    SCFree(stack->top->bdef);

    /* Now move head to free nodes list */
    if (stack->free_nodes_cnt < STACK_FREE_NODES) {
        stack->top->next = stack->free_nodes;
        stack->free_nodes = stack->top;
        stack->free_nodes_cnt++;
    } else {
        SCFree(stack->top);
    }
    stack->top = curr;

    /* Return a pointer to the top of the stack */
    return curr;
}

/**
 * \brief Frees the stack along with the free-nodes list
 *
 * \param stack The stack pointer
 *
 * \return none
 */
static void FreeMimeDecStack(MimeDecStack *stack)
{
    MimeDecStackNode *temp, *curr;

    if (stack != NULL) {
        /* Top of stack */
        curr = stack->top;
        while (curr != NULL) {
            temp = curr;
            curr = curr->next;

            /* Now free node */
            SCFree(temp->bdef);
            SCFree(temp);
        }

        /* Free nodes */
        curr = stack->free_nodes;
        while (curr != NULL) {
            temp = curr;
            curr = curr->next;

            /* Now free node */
            SCFree(temp);
        }

        SCFree(stack);
    }
}

/**
 * \brief Adds a data value to the data values linked list
 *
 * \param dv The head of the linked list (NULL if new list)
 *
 * \return pointer to a new node, otherwise NULL if it fails
 */
static DataValue * AddDataValue(DataValue *dv)
{
    DataValue *curr, *node = SCMalloc(sizeof(DataValue));
    if (node == NULL) {
        CH_DEBUG_LOG("memory allocation failed");
        return NULL;
    }
    memset(node, 0x00, sizeof(DataValue));

    if (dv != NULL) {
        curr = dv;
        while (curr->next != NULL) {
            curr = curr->next;
        }

        curr->next = node;
    }

    return node;
}

/**
 * \brief Frees a linked list of data values starting at the head
 *
 * \param dv The head of the linked list
 *
 * \return none
 */
static void FreeDataValue(DataValue *dv)
{
    DataValue *temp, *curr;

    if (dv != NULL) {
        curr = dv;
        while (curr != NULL) {
            temp = curr;
            curr = curr->next;

            /* Now free node */
            SCFree(temp->value);
            SCFree(temp);
        }
    }
}

/**
 * \brief Converts a list of data values into a single value (returns dynamically
 * allocated memory)
 *
 * \param dv The head of the linked list (NULL if new list)
 * \param len The output length of the single value
 *
 * \return pointer to a single value, otherwise NULL if it fails or is zero-length
 */
static uint8_t * GetFullValue(DataValue *dv, uint32_t *len)
{
    DataValue *curr;
    uint32_t offset = 0;
    uint8_t *val = NULL;

    /* First calculate total length */
    *len = 0;
    curr = dv;
    while (curr != NULL) {
        *len += curr->value_len;

#if 0
        /* Add CRLF except on last one */
        if (curr->next != NULL) {
            *len += 2;
        }
#endif
        curr = curr->next;
    }

    /* Must have at least one character in the value */
    if (*len > 0) {
        val = SCCalloc(1, *len);
        if (val == NULL) {
            CH_DEBUG_LOG("memory allocation failed");
            *len = 0;
            return NULL;
        }

        curr = dv;
        while (curr != NULL) {
            memcpy(val + offset, curr->value, curr->value_len);
            offset += curr->value_len;

#if 0       /* VJ unclear why this is needed ? */
            /* Add CRLF except on last one */
            if (curr->next != NULL) {
                memcpy(val + offset, CRLF, 2);
                offset += 2;
            }
#endif
            curr = curr->next;
        }
    }

    return val;
}

/**
 * \brief Find a string while searching up to N characters within a source
 *        buffer
 *
 * \param src The source string (not null-terminated)
 * \param len The length of the source string
 * \param find The string to find (null-terminated)
 * \param find_len length of the 'find' string
 *
 * \return Pointer to the position it was found, otherwise NULL if not found
 */
static inline uint8_t * 
FindBuffer(const uint8_t *src, uint32_t len, const uint8_t *find, uint32_t find_len)
{
    /* Use utility search function */
	return (uint8_t *)ch_strlcasestrn((char *)src, (char *)src + len, (char *)find, find_len - 1);
}

/**
 * \brief Get token from buffer and return pointer to it
 *
 * \param buf The input buffer (not null-terminated)
 * \param blen The length of the input buffer
 * \param delims Character delimiters (null-terminated)
 * \param remainPtr Pointer to remaining after tokenizing iteration
 * \param tokLen Output token length (if non-null line)
 *
 * \return Pointer to token, or NULL if not found
 */
static uint8_t * GetToken(uint8_t *buf, uint32_t blen, const char *delims,
        uint8_t **remainPtr, uint32_t *tokenLen)
{
    uint32_t i, j, delimFound = 0;
    uint8_t *tok = NULL;

    /* So that it can be used just like strtok_r */
    if (buf == NULL) {
        buf = *remainPtr;
    } else {
        *remainPtr = buf;
    }
    if (buf == NULL)
        return NULL;

    /* Must specify length */
    for (i = 0; i < blen && buf[i] != 0; i++) {

        /* Look for delimiters */
        for (j = 0; delims[j] != 0; j++) {
            if (buf[i] == delims[j]) {
                /* Data must be found before delimiter matters */
                if (tok != NULL) {
                    (*remainPtr) += (i + 1);
                }
                delimFound = 1;
                break;
            }
        }

        /* If at least one non-delimiter found, then a token is found */
        if (tok == NULL && !delimFound) {
            tok = buf + i;
        } else {
            /* Reset delimiter */
            delimFound = 0;
        }

        /* If delimiter found, then break out of loop */
        if (buf != *remainPtr) {
            break;
        }
    }

    /* Make sure remaining points to end of buffer if delimiters not found */
    if (tok != NULL) {
        if (buf == *remainPtr) {
            (*remainPtr) += i;
        }

        /* Calculate token length */
        *tokenLen = (buf + i) - tok;
    }

    return tok;
}

/**
 * \brief Stores the final MIME header value into the current entity on the
 * stack.
 *
 * \param state The parser state
 *
 * \return MIME_DEC_OK if stored, otherwise a negative number indicating error
 */
static int StoreMimeHeader(MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint8_t *val;
    uint32_t vlen;

    /* Lets save the most recent header */
    if (state->hname != NULL || state->hvalue != NULL) {
        CH_DEBUG_LOG("Storing last header");
        val = GetFullValue(state->hvalue, &vlen);
        if (val != NULL) {
            if (state->hname == NULL) {
                CH_ERR_LOG("Error: Invalid parser state - header value without"
                        " name");
                ret = MIME_DEC_ERR_PARSE;
            } else if (state->stack->top != NULL) {
                /* Store each header name and value */
                
				MimeDecFillField(state->stack->top->data, state->hname,
                            state->hlen, val, vlen);

				/* added by goodian@20160706 */
				if (state->HeaderProcessorFunc)
					state->HeaderProcessorFunc(state->hname, 
												state->hlen, 
												val, 
												vlen,
												state);
            } else {
                CH_ERR_LOG("Error: Stack pointer missing");
                ret = MIME_DEC_ERR_DATA;
            }
        } else if (state->hvalue != NULL) {
            /* Memory allocation must have failed since val is NULL */
            CH_DEBUG_LOG("GetFullValue() function failed");
            ret = MIME_DEC_ERR_MEM;
        }

        /* Do cleanup here */
		SCFree(state->hname);
		SCFree(val);

		state->hname = NULL;
        FreeDataValue(state->hvalue);
        state->hvalue = NULL;
        state->hvlen = 0;
    }

    return ret;
}

/**
 * \brief This function is a pre-processor for handling decoded data chunks that
 * then invokes the caller's callback function for further processing
 *
 * \param chunk The decoded chunk
 * \param len The decoded chunk length (varies)
 * \param state The current parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessDecodedDataChunk(const uint8_t *chunk, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;

    if ((state->stack != NULL) && (state->stack->top != NULL) &&
        (state->stack->top->data != NULL)) {
        /* Now invoke callback */
        if (state->DataChunkProcessorFunc != NULL) {
            ret = state->DataChunkProcessorFunc(chunk, len, state);
            if (ret != MIME_DEC_OK) {
                CH_ERR_LOG("Error: state->dataChunkProcessor() callback function"
                            " failed");
            }
        }
    } else {
        CH_ERR_LOG("Error: Stack pointer missing");
        ret = MIME_DEC_ERR_DATA;
    }

    /* Reset data chunk buffer */
    state->data_chunk_len = 0;

    /* Mark body / file as no longer at beginning */
    state->body_begin = 0;

    return ret;
}

/**
 * \brief Processes a remainder (line % 4 = remainder) from the previous line
 * such that all base64 decoding attempts are divisible by 4
 *
 * \param buf The current line
 * \param len The length of the line
 * \param state The current parser state
 * \param force Flag indicating whether decoding should always occur
 *
 * \return Number of bytes pulled from the current buffer
 */
static uint8_t ProcessBase64Remainder(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state, int force)
{
    uint32_t ret;
    uint8_t remainder = 0, remdec = 0;

    CH_DEBUG_LOG("Base64 line remainder found: %u", state->bvr_len);

    /* Fill in block with first few bytes of current line */
    remainder = B64_BLOCK - state->bvr_len;
    remainder = remainder < len ? remainder : len;
    memcpy(state->bvremain + state->bvr_len, buf, remainder);
    state->bvr_len += remainder;

    /* If data chunk buffer will be full, then clear it now */
    if (DATA_CHUNK_SIZE - state->data_chunk_len < ASCII_BLOCK) {

        /* Invoke pre-processor and callback */
        ret = ProcessDecodedDataChunk(state->data_chunk, state->data_chunk_len,
                state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessDecodedDataChunk() function failed");
        }
    }

    /* Only decode if divisible by 4 */
    if (state->bvr_len == B64_BLOCK || force) {
		remdec = MimeDecodeBase64(state->data_chunk + state->data_chunk_len,
								state->bvremain, state->bvr_len);
        if (remdec > 0) {

            /* Track decoded length */
            state->stack->top->data->decoded_body_len += remdec;

            /* Update length */
            state->data_chunk_len += remdec;

            /* If data chunk buffer is now full, then clear */
            if (DATA_CHUNK_SIZE - state->data_chunk_len < ASCII_BLOCK) {

                /* Invoke pre-processor and callback */
                ret = ProcessDecodedDataChunk(state->data_chunk,
                        state->data_chunk_len, state);
                if (ret != MIME_DEC_OK) {
                    CH_ERR_LOG("Error: ProcessDecodedDataChunk() function "
                            "failed");
                }
            }
        } else {
            /* Track failed base64 */
            state->stack->top->data->anomaly_flags |= ANOM_INVALID_BASE64;
            state->msg->anomaly_flags |= ANOM_INVALID_BASE64;
            CH_ERR_LOG("Error: MimeDecodeBase64 function failed");
            CH_ERR_LOG("Base64 failed string: %.*s", state->bvr_len, state->bvremain);
        }

        /* Reset remaining */
        state->bvr_len = 0;
    }

    return remainder;
}

/**
 * \brief Processes a body line by base64-decoding and passing to the data chunk
 * processing callback function when the buffer is read
 *
 * \param buf The current line
 * \param len The length of the line
 * \param state The current parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessBase64BodyLine(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint8_t rem1 = 0, rem2 = 0;
    uint32_t numDecoded, remaining, offset, avail, tobuf;

    /* Track long line */
    if (len > MAX_ENC_LINE_LEN) {
        state->stack->top->data->anomaly_flags |= ANOM_LONG_ENC_LINE;
        state->msg->anomaly_flags |= ANOM_LONG_ENC_LINE;
        CH_ERR_LOG("Error: Max encoded input line length exceeded %u > %u",
                len, MAX_ENC_LINE_LEN);
    }

    /* First process remaining from previous line */
    if (state->bvr_len > 0) {

        CH_DEBUG_LOG("Base64 line remainder found: %u", state->bvr_len);

        /* Process remainder and return number of bytes pulled from current buffer */
        rem1 = ProcessBase64Remainder(buf, len, state, 0);
    }

    /* No error and at least some more data needs to be decoded */
    if ((int) (len - rem1) > 0) {

        /* Determine whether we need to save a remainder if not divisible by 4 */
        rem2 = (len - rem1) % B64_BLOCK;
        if (rem2 > 0) {

            CH_DEBUG_LOG("Base64 saving remainder: %u", rem2);

            memcpy(state->bvremain, buf + (len - rem2), rem2);
            state->bvr_len = rem2;
        }

        /* Process remaining in loop in case buffer fills up */
        remaining = len - rem1 - rem2;
        offset = rem1;
        while (remaining > 0) {
            /* Determine amount to add to buffer */
            avail = (DATA_CHUNK_SIZE - state->data_chunk_len) * B64_BLOCK / ASCII_BLOCK;
            tobuf = avail > remaining ? remaining : avail;
            while (tobuf % 4 != 0) {
                tobuf--;
            }

            if (tobuf < B64_BLOCK) {
                CH_ERR_LOG("Error: Invalid state for decoding base-64 block");
                return MIME_DEC_ERR_PARSE;
            }

            CH_DEBUG_LOG("Decoding: %u", len - rem1 - rem2);

			numDecoded = MimeDecodeBase64(state->data_chunk + state->data_chunk_len,
								buf + offset, tobuf);
            if (numDecoded > 0) {
                /* Track decoded length */
                state->stack->top->data->decoded_body_len += numDecoded;

                /* Update length */
                state->data_chunk_len += numDecoded;

                if ((int) (DATA_CHUNK_SIZE - state->data_chunk_len) < 0) {
                    CH_ERR_LOG("Error: Invalid Chunk length: %u",
                            state->data_chunk_len);
                    ret = MIME_DEC_ERR_PARSE;
                    break;
                }

                /* If buffer full, then invoke callback */
                if (DATA_CHUNK_SIZE - state->data_chunk_len < ASCII_BLOCK) {

                    /* Invoke pre-processor and callback */
                    ret = ProcessDecodedDataChunk(state->data_chunk,
                            state->data_chunk_len, state);
                    if (ret != MIME_DEC_OK) {
                        CH_ERR_LOG("Error: ProcessDecodedDataChunk() "
                                "function failed");
                    }
                }
            } else {
                /* Track failed base64 */
                state->stack->top->data->anomaly_flags |= ANOM_INVALID_BASE64;
                state->msg->anomaly_flags |= ANOM_INVALID_BASE64;
                CH_ERR_LOG("Error: MimeDecodeBase64 function failed");
				CH_ERR_LOG("Base64 failed string: %.*s", tobuf, buf + offset);
            }

            /* Update counts */
            remaining -= tobuf;
            offset += tobuf;
        }
    }

    return ret;
}

/**
 * \brief Decoded a hex character into its equivalent byte value for
 * quoted-printable decoding
 *
 * \param h The hex char
 *
 * \return byte value on success, -1 if failed
 **/
static int16_t DecodeQPChar(char h)
{
    uint16_t res = 0;

    /* 0-9 */
    if (h >= 48 && h <= 57) {
        res = h - 48;
    } else if (h >= 65 && h <= 70) {
        /* A-F */
        res = h - 55;
    } else {
        /* Invalid */
        res = -1;
    }

    return res;

}

/**
 * \brief Processes a quoted-printable encoded body line by decoding and passing
 * to the data chunk processing callback function when the buffer is read
 *
 * \param buf The current line
 * \param len The length of the line
 * \param state The current parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessQuotedPrintableBodyLine(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint32_t remaining, offset;
    MimeDecEntity *entity = (MimeDecEntity *) state->stack->top->data;
    uint8_t c, h1, h2, val;
    int16_t res;

    /* Track long line */
    if (len > MAX_ENC_LINE_LEN) {
        state->stack->top->data->anomaly_flags |= ANOM_LONG_ENC_LINE;
        state->msg->anomaly_flags |= ANOM_LONG_ENC_LINE;
        CH_ERR_LOG("Error: Max encoded input line length exceeded %u > %u",
                len, MAX_ENC_LINE_LEN);
    }

    remaining = len;
    offset = 0;
    while (remaining > 0) {

        c = *(buf + offset);

        /* Copy over normal character */
        if (c != '=') {
            state->data_chunk[state->data_chunk_len] = c;
            state->data_chunk_len++;
            entity->decoded_body_len += 1;

            /* Add CRLF sequence if end of line */
            if (remaining == 1) {
                memcpy(state->data_chunk + state->data_chunk_len, CRLF, EOL_LEN);
                state->data_chunk_len += EOL_LEN;
                entity->decoded_body_len += EOL_LEN;
            }
        } else if (remaining > 1) {
            /* If last character handle as soft line break by ignoring,
                       otherwise process as escaped '=' character */

            /* Not enough characters */
            if (remaining < 3) {
                entity->anomaly_flags |= ANOM_INVALID_QP;
                state->msg->anomaly_flags |= ANOM_INVALID_QP;
                CH_ERR_LOG("Error: Quoted-printable decoding failed");
            } else {
                h1 = *(buf + offset + 1);
                res = DecodeQPChar(h1);
                if (res < 0) {
                    entity->anomaly_flags |= ANOM_INVALID_QP;
                    state->msg->anomaly_flags |= ANOM_INVALID_QP;
                    CH_ERR_LOG("Error: Quoted-printable decoding failed");
                } else {
                    val = (res << 4); /* Shift result left */
                    h2 = *(buf + offset + 2);
                    res = DecodeQPChar(h2);
                    if (res < 0) {
                        entity->anomaly_flags |= ANOM_INVALID_QP;
                        state->msg->anomaly_flags |= ANOM_INVALID_QP;
                        CH_ERR_LOG("Error: Quoted-printable decoding failed");
                    } else {
                        /* Decoding sequence succeeded */
                        val += res;

                        state->data_chunk[state->data_chunk_len] = val;
                        state->data_chunk_len++;
                        entity->decoded_body_len++;

                        /* Add CRLF sequence if end of line */
                        if (remaining == 3) {
                            memcpy(state->data_chunk + state->data_chunk_len,
                                    CRLF, EOL_LEN);
                            state->data_chunk_len += EOL_LEN;
                            entity->decoded_body_len += EOL_LEN;
                        }

                        /* Account for extra 2 characters in 3-characted QP
                         * sequence */
                        remaining -= 2;
                        offset += 2;
                    }
                }
            }
        }

        /* Change by 1 */
        remaining--;
        offset++;

        /* If buffer full, then invoke callback */
        if (DATA_CHUNK_SIZE - state->data_chunk_len < EOL_LEN + 1) {

            /* Invoke pre-processor and callback */
            ret = ProcessDecodedDataChunk(state->data_chunk, state->data_chunk_len,
                    state);
            if (ret != MIME_DEC_OK) {
                CH_ERR_LOG("Error: ProcessDecodedDataChunk() function "
                        "failed");
            }
        }
    }

    return ret;
}

/**
 * \brief Processes a body line by base64-decoding (if applicable) and passing to
 * the data chunk processing callback function
 *
 * \param buf The current line
 * \param len The length of the line
 * \param state The current parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessBodyLine(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint32_t remaining, offset, avail, tobuf;
    MimeDecEntity *entity = (MimeDecEntity *) state->stack->top->data;

    CH_DEBUG_LOG("Processing body line");

    /* Track length */
    entity->body_len += len + 2; /* With CRLF */

    /* Process base-64 content if enabled */
    MimeDecConfig *mdcfg = MimeDecGetConfig();
    if (mdcfg != NULL && mdcfg->decode_base64 &&
            (entity->ctnt_flags & CTNT_IS_BASE64)) {

        ret = ProcessBase64BodyLine(buf, len, state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessBase64BodyLine() function failed");
        }
    } else if (mdcfg != NULL && mdcfg->decode_quoted_printable &&
            (entity->ctnt_flags & CTNT_IS_QP)) {
        /* Process quoted-printable content if enabled */
        ret = ProcessQuotedPrintableBodyLine(buf, len, state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessQuotedPrintableBodyLine() function "
                    "failed");
        }
    } else {
        /* Process non-decoded content */
        remaining = len;
        offset = 0;
        while (remaining > 0) {

            /* Plan to add CRLF to the end of each line */
            avail = DATA_CHUNK_SIZE - state->data_chunk_len;
            tobuf = avail > remaining + EOL_LEN ? remaining : avail - EOL_LEN;

            /* Copy over to buffer */
            memcpy(state->data_chunk + state->data_chunk_len, buf + offset, tobuf);
            state->data_chunk_len += tobuf;

            /* Now always add a CRLF to the end */
            if (tobuf == remaining) {
                memcpy(state->data_chunk + state->data_chunk_len, CRLF, EOL_LEN);
                state->data_chunk_len += EOL_LEN;
            }

            if ((int) (DATA_CHUNK_SIZE - state->data_chunk_len) < 0) {
                CH_ERR_LOG("Error: Invalid Chunk length: %u",
                        state->data_chunk_len);
                ret = MIME_DEC_ERR_PARSE;
                break;
            }

            /* If buffer full, then invoke callback */
            if (DATA_CHUNK_SIZE - state->data_chunk_len < EOL_LEN + 1) {

                /* Invoke pre-processor and callback */
                ret = ProcessDecodedDataChunk(state->data_chunk,
                        state->data_chunk_len, state);
                if (ret != MIME_DEC_OK) {
                    CH_ERR_LOG("Error: ProcessDecodedDataChunk() function "
                            "failed");
                }
            }

            remaining -= tobuf;
            offset += tobuf;
        }
    }

    return ret;
}

/**
 * \brief Find the start of a header name on the current line
 *
 * \param buf The input line (not null-terminated)
 * \param blen The length of the input line
 * \param glen The output length of the header name
 *
 * \return Pointer to header name, or NULL if not found
 */
static uint8_t * FindMimeHeaderStart(const uint8_t *buf, uint32_t blen, uint32_t *hlen)
{
    uint32_t i, valid = 0;
    uint8_t *hname = NULL;

    /* Init */
    *hlen = 0;

    /* Look for sequence of printable characters followed by ':', or
       CRLF then printable characters followed by ':' */
    for (i = 0; i < blen && buf[i] != 0; i++) {

        /* If ready for printable characters and found one, then increment */
        if (buf[i] != COLON && buf[i] >= PRINTABLE_START &&
                buf[i] <= PRINTABLE_END) {
            valid++;
        } else if (valid > 0 && buf[i] == COLON) {
            /* If ready for printable characters, found some, and found colon
             * delimiter, then a match is found */
            hname = (uint8_t *) buf + i - valid;
            *hlen = valid;
            break;
        } else {
            /* Otherwise reset and quit */
            break;
        }
    }

    return hname;
}

/**
 * \brief Find full header name and value on the current line based on the
 * current state
 *
 * \param buf The current line (no CRLF)
 * \param blen The length of the current line
 * \param state The current state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int FindMimeHeader(const uint8_t *buf, uint32_t blen,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint8_t *hname, *hval = NULL;
    DataValue *dv;
    uint32_t hlen, vlen;
    int finish_header = 0, new_header = 0;
    MimeDecConfig *mdcfg = MimeDecGetConfig();

    /* Find first header */
    hname = FindMimeHeaderStart(buf, blen, &hlen);
    if (hname != NULL) {

        /* Warn and track but don't do anything yet */
        if (hlen > MAX_HEADER_NAME) {
            state->stack->top->data->anomaly_flags |= ANOM_LONG_HEADER_NAME;
            state->msg->anomaly_flags |= ANOM_LONG_HEADER_NAME;
            CH_ERR_LOG("Error: Header name exceeds limit (%u > %u)",
                    hlen, MAX_HEADER_NAME);
        }

        /* Value starts after 'header:' (normalize spaces) */
        hval = hname + hlen + 1;
        if (hval - buf >= (int)blen) {
            CH_DEBUG_LOG("No Header value found");
            hval = NULL;
        } else {
            while (hval[0] == ' ') {

                /* If last character before end of bounds, set to NULL */
                if (hval - buf >= (int)blen - 1) {
                    CH_DEBUG_LOG("No Header value found");
                    hval = NULL;
                    break;
                }

                hval++;
            }
        }

        /* If new header found, then previous header is finished */
        if (state->state_flag == HEADER_STARTED) {
            finish_header = 1;
        }

        /* Now process new header */
        new_header = 1;

        /* Must wait for next line to determine if finished */
        state->state_flag = HEADER_STARTED;
    } else if (blen == 0) {
        /* Found body */
        /* No more headers */
        state->state_flag = HEADER_DONE;

        finish_header = 1;

        CH_DEBUG_LOG("All Header processing finished");
    } else if (state->state_flag == HEADER_STARTED) {
        /* Found multi-line value (ie. Received header) */
        /* If max header value exceeded, flag it */
        vlen = blen;
        if ((mdcfg != NULL) && (state->hvlen + vlen > mdcfg->header_value_depth)) {
            CH_ERR_LOG("Error: Header value of length (%u) is too long",
                    state->hvlen + vlen);
            vlen = mdcfg->header_value_depth - state->hvlen;
            state->stack->top->data->anomaly_flags |= ANOM_LONG_HEADER_VALUE;
            state->msg->anomaly_flags |= ANOM_LONG_HEADER_VALUE;
        }
        if (vlen > 0) {
            dv = AddDataValue(state->hvalue);
            if (dv == NULL) {
                CH_DEBUG_LOG("AddDataValue() function failed");
                return MIME_DEC_ERR_MEM;
            }
            if (state->hvalue == NULL) {
                state->hvalue = dv;
            }

            dv->value = SCMalloc(vlen);
            if (dv->value == NULL) {
                CH_DEBUG_LOG("Memory allocation failed");
                return MIME_DEC_ERR_MEM;
            }
            memcpy(dv->value, buf, vlen);
            dv->value_len = vlen;
            state->hvlen += vlen;
        }
    } else {
        /* Likely a body without headers */
        CH_DEBUG_LOG("No headers found");

        state->state_flag = BODY_STARTED;

        /* Flag beginning of body */
        state->body_begin = 1;
        state->body_end = 0;

        ret = ProcessBodyLine(buf, blen, state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessBodyLine() function failed");
            return ret;
        }
    }

    /* If we need to finish a header, then do so below and then cleanup */
    if (finish_header) {
        /* Store the header value */
        ret = StoreMimeHeader(state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: StoreMimeHeader() function failed");
            return ret;
        }
    }

    /* When next header is found, we always create a new one */
    if (new_header) {
        /* Copy name and value to state */
        state->hname = SCMalloc(hlen);
        if (state->hname == NULL) {
            CH_DEBUG_LOG("Memory allocation failed");
            return MIME_DEC_ERR_MEM;
        }
        memcpy(state->hname, hname, hlen);
        state->hlen = hlen;

        if (state->hvalue != NULL) {
            CH_ERR_LOG("Error: Parser failed due to unexpected header "
                    "value");
            return MIME_DEC_ERR_DATA;
        }

        if (hval != NULL) {
            /* If max header value exceeded, flag it */
            vlen = blen - (hval - buf);
            if ((mdcfg != NULL) && (state->hvlen + vlen > mdcfg->header_value_depth)) {
                CH_ERR_LOG("Error: Header value of length (%u) is too long",
                        state->hvlen + vlen);
                vlen = mdcfg->header_value_depth - state->hvlen;
                state->stack->top->data->anomaly_flags |= ANOM_LONG_HEADER_VALUE;
                state->msg->anomaly_flags |= ANOM_LONG_HEADER_VALUE;
            }

            if (vlen > 0) {
                state->hvalue = AddDataValue(NULL);
                if (state->hvalue == NULL) {
                    CH_DEBUG_LOG("AddDataValue() function failed");
                    return MIME_DEC_ERR_MEM;
                }
                state->hvalue->value = SCMalloc(vlen);
                if (state->hvalue->value == NULL) {
                    CH_DEBUG_LOG("Memory allocation failed");
                    return MIME_DEC_ERR_MEM;
                }
                memcpy(state->hvalue->value, hval, vlen);
                state->hvalue->value_len = vlen;
                state->hvlen += vlen;
            }
        }
    }

    return ret;
}

/**
 * \brief Finds a mime header token within the specified field
 *
 * \param field The current field
 * \param search_start The start of the search (ie. boundary=\")
 * \param search_end The end of the search (ie. \")
 * \param tlen The output length of the token (if found)
 *
 * \return A pointer to the token if found, otherwise NULL if not found
 */
static uint8_t * FindMimeHeaderToken(ch_str_t *field, char *search_start,
        char *search_end, uint32_t *tlen)
{
    uint8_t *fptr, *tptr = NULL, *tok = NULL;

    CH_DEBUG_LOG("Looking for token: %s", search_start);

    /* Check for token definition */
    fptr = FindBuffer((const uint8_t *)field->data, field->len, 
		(const uint8_t *)search_start, strlen(search_start));
    if (fptr != NULL) {
        fptr += strlen(search_start); /* Start at end of start string */
        tok = GetToken(fptr, field->len - ((unsigned char *)fptr - field->data), search_end,
                &tptr, tlen);
        if (tok != NULL) {
            CH_DEBUG_LOG("Found mime token");
        }
    }

    return tok;
}

/**
 * \brief Processes the current line for mime headers and also does post-processing
 * when all headers found
 *
 * \param buf The current line
 * \param len The length of the line
 * \param state The current parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessMimeHeaders(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    ch_str_t *field;
	uint8_t *tmp;
	uint32_t tmp_len;
    uint8_t *bptr = NULL, *rptr = NULL;
    uint32_t blen = 0;
    MimeDecEntity *entity = (MimeDecEntity *) state->stack->top->data;

    /* Look for mime header in current line */
    ret = FindMimeHeader(buf, len, state);
    if (ret != MIME_DEC_OK) {
        CH_ERR_LOG("Error: FindMimeHeader() function failed: %d", ret);
        return ret;
    }

    /* Post-processing after all headers done */
    if (state->state_flag == HEADER_DONE) {
        /* First determine encoding by looking at Content-Transfer-Encoding */
		field = entity->ctnt_tran;
        if (field != NULL) {
            /* Look for base64 */
            if (FindBuffer((const uint8_t *)field->data, field->len, 
					(const uint8_t *)BASE64_STR, strlen(BASE64_STR))) {
                CH_DEBUG_LOG("Base64 encoding found");
                entity->ctnt_flags |= CTNT_IS_BASE64;
            } else if (FindBuffer((const uint8_t *)field->data, field->len, 
					(const uint8_t *)QP_STR, strlen(QP_STR))) {
                /* Look for quoted-printable */
                CH_DEBUG_LOG("quoted-printable encoding found");
                entity->ctnt_flags |= CTNT_IS_QP;
            }
        }

        /* Check for file attachment in content disposition */
        field = entity->ctnt_disp;
        if (field != NULL) {
            bptr = FindMimeHeaderToken(field, "filename=\"", TOK_END_STR, &blen);
            if (bptr != NULL) {
                CH_DEBUG_LOG("File attachment found in disposition");
                entity->ctnt_flags |= CTNT_IS_ATTACHMENT;

                /* Copy over using dynamic memory */
                entity->filename = SCMalloc(blen);
                if (entity->filename == NULL) {
                    CH_DEBUG_LOG("memory allocation failed");
                    return MIME_DEC_ERR_MEM;
                }
                memcpy(entity->filename, bptr, blen);
                entity->filename_len = blen;
            }
        }

        /* Check for boundary, encapsulated message, and file name in Content-Type */
        field = entity->ctnt_type;
        if (field != NULL) {
            /* Check if child entity boundary definition found */
            bptr = FindMimeHeaderToken(field, BND_START_STR, TOK_END_STR, &blen);
            if (bptr != NULL) {
                state->found_child = 1;
                entity->ctnt_flags |= CTNT_IS_MULTIPART;

                if (blen > (BOUNDARY_BUF - 2)) {
                    state->stack->top->data->anomaly_flags |= ANOM_LONG_BOUNDARY;
                    return MIME_DEC_ERR_PARSE;
                }

                /* Store boundary in parent node */
                state->stack->top->bdef = SCMalloc(blen);
                if (state->stack->top->bdef == NULL) {
                    CH_DEBUG_LOG("Memory allocation failed");
                    return MIME_DEC_ERR_MEM;
                }
                memcpy(state->stack->top->bdef, bptr, blen);
                state->stack->top->bdef_len = blen;
            }

            /* Look for file name (if not already found) */
            if (!(entity->ctnt_flags & CTNT_IS_ATTACHMENT)) {
                bptr = FindMimeHeaderToken(field, "name=\"", TOK_END_STR, &blen);
                if (bptr != NULL) {
                    CH_DEBUG_LOG("File attachment found");
                    entity->ctnt_flags |= CTNT_IS_ATTACHMENT;

                    /* Copy over using dynamic memory */
                    entity->filename = SCMalloc(blen);
                    if (entity->filename == NULL) {
                        CH_DEBUG_LOG("memory allocation failed");
                        return MIME_DEC_ERR_MEM;
                    }
                    memcpy(entity->filename, bptr, blen);
                    entity->filename_len = blen;
                }
            }

            /* Pull out short-hand content type */
            tmp = GetToken((uint8_t *)field->data, field->len, " \r\n;",
                    &rptr, &tmp_len);
            if (tmp != NULL) {
                /* Check for encapsulated message */
                if (FindBuffer(tmp, tmp_len,
                            (const uint8_t *)MSG_STR, strlen(MSG_STR)))
                {
                    CH_DEBUG_LOG("Found encapsulated message entity");

                    entity->ctnt_flags |= CTNT_IS_ENV;

                    /* Create and push child to stack */
                    MimeDecEntity *child = MimeDecAddEntity(entity);
                    if (child == NULL)
                        return MIME_DEC_ERR_MEM;
                    child->ctnt_flags |= (CTNT_IS_ENCAP | CTNT_IS_MSG);
                    PushStack(state->stack);
                    state->stack->top->data = child;

                    /* Mark as encapsulated child */
                    state->stack->top->is_encap = 1;

                    /* Ready to parse headers */
                    state->state_flag = HEADER_READY;
                } else if (FindBuffer(tmp, tmp_len,
                        (const uint8_t *)MULTIPART_STR, strlen(MULTIPART_STR)))
                {
                    /* Check for multipart */
                    CH_DEBUG_LOG("Found multipart entity");
                    entity->ctnt_flags |= CTNT_IS_MULTIPART;
                } else if (FindBuffer(tmp, tmp_len,
                        (const uint8_t *)TXT_STR, strlen(TXT_STR)))
                {
                    /* Check for plain text */
                    CH_DEBUG_LOG("Found plain text entity");
                    entity->ctnt_flags |= CTNT_IS_TEXT;
                } else if (FindBuffer(tmp, tmp_len,
                        (const uint8_t *)HTML_STR, strlen(HTML_STR)))
                {
                    /* Check for html */
                    CH_DEBUG_LOG("Found html entity");
                    entity->ctnt_flags |= CTNT_IS_HTML;
                }
            }
        }

        /* Flag beginning of body */
        state->body_begin = 1;
        state->body_end = 0;
    }

    return ret;
}

/**
 * \brief Indicates to the parser that the body of an entity has completed
 * processing on the previous line
 *
 * \param state The current parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */

static int ProcessBodyComplete(MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;

    CH_DEBUG_LOG("Process body complete called");

    /* Mark the file as hitting the end */
    state->body_end = 1;

    if (state->bvr_len > 0) {
        CH_DEBUG_LOG("Found (%u) remaining base64 bytes not processed",
                state->bvr_len);

        /* Process the remainder */
        ret = ProcessBase64Remainder(NULL, 0, state, 1);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessBase64BodyLine() function failed");
        }
    }

#ifdef HAVE_NSS
    if (state->md5_ctx) {
        unsigned int len = 0;
        HASH_End(state->md5_ctx, state->md5, &len, sizeof(state->md5));
    }
#endif

    /* Invoke pre-processor and callback with remaining data */
    ret = ProcessDecodedDataChunk(state->data_chunk, state->data_chunk_len, state);
    if (ret != MIME_DEC_OK) {
        CH_ERR_LOG("Error: ProcessDecodedDataChunk() function failed");
    }

    /* Now reset */
    state->body_begin = 0;
    state->body_end = 0;

    return ret;
}

/**
 * \brief When a mime boundary is found, look for end boundary and also do stack
 * management
 *
 * \param buf The current line
 * \param len The length of the line
 * \param bdef_len The length of the current boundary
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessMimeBoundary(const uint8_t *buf, uint32_t len, uint32_t bdef_len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint8_t *rptr;
    MimeDecEntity *child;

    CH_DEBUG_LOG("PROCESSING BOUNDARY - START: %d",
            state->state_flag);

    /* If previous line was not an end boundary, then we process the body as
     * completed */
    if (state->state_flag != BODY_END_BOUND) {

        /* First lets complete the body */
        ret = ProcessBodyComplete(state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessBodyComplete() function failed");
            return ret;
        }
    } else {
        /* If last line was an end boundary, then now we are ready to parse
         * headers again */
        state->state_flag = HEADER_READY;
    }

    /* Update remaining buffer */
    rptr = (uint8_t *) buf + bdef_len + 2;

    /* If entity is encapsulated and current and parent didn't define the boundary,
     * then pop out */
    if (state->stack->top->is_encap && state->stack->top->bdef_len == 0) {

        if (state->stack->top->next == NULL) {
            CH_ERR_LOG("Error: Missing parent entity from stack");
            return MIME_DEC_ERR_DATA;
        }

        if (state->stack->top->next->bdef_len == 0) {

            CH_DEBUG_LOG("POPPED ENCAPSULATED CHILD FROM STACK: %p=%p",
                    state->stack->top, state->stack->top->data);

            /* If end of boundary found, pop the child off the stack */
            PopStack(state->stack);
            if (state->stack->top == NULL) {
                CH_ERR_LOG("Error: Message is malformed");
                return MIME_DEC_ERR_DATA;
            }
        }
    }

    /* Now check for end of nested boundary */
    if (len - (rptr - buf) > 1 && rptr[0] == DASH && rptr[1] == DASH) {
        CH_DEBUG_LOG("FOUND END BOUNDARY, POPPING: %p=%p",
                state->stack->top, state->stack->top->data);

        /* If end of boundary found, pop the child off the stack */
        PopStack(state->stack);
        if (state->stack->top == NULL) {
            CH_ERR_LOG("Error: Message is malformed");
            return MIME_DEC_ERR_DATA;
        }

        /* If current is an encapsulated message with a boundary definition,
         * then pop him as well */
        if (state->stack->top->is_encap && state->stack->top->bdef_len != 0) {
            CH_DEBUG_LOG("FOUND END BOUNDARY AND ENCAP, POPPING: %p=%p",
                    state->stack->top, state->stack->top->data);

            PopStack(state->stack);
            if (state->stack->top == NULL) {
                CH_ERR_LOG("Error: Message is malformed");
                return MIME_DEC_ERR_DATA;
            }
        }

        state->state_flag = BODY_END_BOUND;
    } else if (state->found_child) {
        /* Otherwise process new child */
        CH_DEBUG_LOG("Child entity created");

        /* Create and push child to stack */
        child = MimeDecAddEntity(state->stack->top->data);
        if (child == NULL)
            return MIME_DEC_ERR_MEM;
        child->ctnt_flags |= CTNT_IS_BODYPART;
        PushStack(state->stack);
        state->stack->top->data = child;

        /* Reset flag */
        state->found_child = 0;
    } else {
        /* Otherwise process sibling */
        if (state->stack->top->next == NULL) {
            CH_ERR_LOG("Error: Missing parent entity from stack");
            return MIME_DEC_ERR_DATA;
        }

        CH_DEBUG_LOG("SIBLING CREATED, POPPING PARENT: %p=%p",
                state->stack->top, state->stack->top->data);

        /* First pop current to get access to parent */
        PopStack(state->stack);
        if (state->stack->top == NULL) {
            CH_ERR_LOG("Error: Message is malformed");
            return MIME_DEC_ERR_DATA;
        }

        /* Create and push child to stack */
        child = MimeDecAddEntity(state->stack->top->data);
        if (child == NULL)
            return MIME_DEC_ERR_MEM;
        child->ctnt_flags |= CTNT_IS_BODYPART;
        PushStack(state->stack);
        state->stack->top->data = child;
    }

    /* After boundary look for headers */
    if (state->state_flag != BODY_END_BOUND) {
        state->state_flag = HEADER_READY;
    }

    CH_DEBUG_LOG("PROCESSING BOUNDARY - END: %d", state->state_flag);
    return ret;
}

/**
 * \brief Processes the MIME Entity body based on the input line and current
 * state of the parser
 *
 * \param buf The current line
 * \param len The length of the line
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessMimeBody(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;
    uint8_t temp[BOUNDARY_BUF];
    uint8_t *bstart;
    int body_found = 0;
    uint32_t tlen;

#ifdef HAVE_NSS
    if (MimeDecGetConfig()->body_md5) {
        if (state->body_begin == 1) {
            if (state->md5_ctx == NULL) {
                state->md5_ctx = HASH_Create(HASH_AlgMD5);
                HASH_Begin(state->md5_ctx);
            }
        }
        HASH_Update(state->md5_ctx, buf, len + state->current_line_delimiter_len);
    }
#endif

    /* Ignore empty lines */
    if (len == 0) {
        return ret;
    }

    /* First look for boundary */
    MimeDecStackNode *node = state->stack->top;
    if (node == NULL) {
        CH_ERR_LOG("Error: Invalid stack state");
        return MIME_DEC_ERR_PARSE;
    }

    /* Traverse through stack to find a boundary definition */
    if (state->state_flag == BODY_END_BOUND || node->bdef == NULL) {

        /* If not found, then use parent's boundary */
        node = node->next;
        while (node != NULL && node->bdef == NULL) {
            CH_DEBUG_LOG("Traversing through stack for node with boundary");
            node = node->next;
        }
    }

    /* This means no boundary / parent w/boundary was found so we are in the body */
    if (node == NULL) {
        body_found = 1;
    } else {

        /* Now look for start of boundary */
        if (len > 1 && buf[0] == '-' && buf[1] == '-') {

            tlen = node->bdef_len + 2;
            if (tlen > BOUNDARY_BUF) {
                if (state->stack->top->data)
                    state->stack->top->data->anomaly_flags |= ANOM_LONG_BOUNDARY;
                return MIME_DEC_ERR_PARSE;
            }

            memcpy(temp, "--", 2);
            memcpy(temp + 2, node->bdef, node->bdef_len);

            /* Find either next boundary or end boundary */
            bstart = FindBuffer((const uint8_t *)buf, len, temp, tlen);
            if (bstart != NULL) {
                ret = ProcessMimeBoundary(buf, len, node->bdef_len, state);
                if (ret != MIME_DEC_OK) {
                    CH_ERR_LOG("Error: ProcessMimeBoundary() function "
                            "failed");
                    return ret;
                }
            } else {
                /* Otherwise add value to body */
                body_found = 1;
            }
        } else {
            /* Otherwise add value to body */
            body_found = 1;
        }
    }

    /* Process body line */
    if (body_found) {
        state->state_flag = BODY_STARTED;

        ret = ProcessBodyLine(buf, len, state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessBodyLine() function failed");
            return ret;
        }
    }

    return ret;
}

const char *MimeDecParseStateGetStatus(MimeDecParseState *state)
{
    return StateFlags[state->state_flag];
}

/**
 * \brief Processes the MIME Entity based on the input line and current state of
 * the parser
 *
 * \param buf The current line
 * \param len The length of the line
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
static int ProcessMimeEntity(const uint8_t *buf, uint32_t len,
        MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;

    CH_DEBUG_LOG("START FLAG: %s", StateFlags[state->state_flag]);

    /* Track long line */
    if (len > MAX_LINE_LEN) {
        state->stack->top->data->anomaly_flags |= ANOM_LONG_LINE;
        state->msg->anomaly_flags |= ANOM_LONG_LINE;
        CH_ERR_LOG("Error: Max input line length exceeded %u > %u", len,
                MAX_LINE_LEN);
    }

    /* Looking for headers */
    if (state->state_flag == HEADER_READY ||
            state->state_flag == HEADER_STARTED) {

        CH_DEBUG_LOG("Processing Headers");

        /* Process message headers */
        ret = ProcessMimeHeaders(buf, len, state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessMimeHeaders() function failed: %d",
                    ret);
            return ret;
        }
    } else {
        /* Processing body */
        CH_DEBUG_LOG("Processing Body of: %p", state->stack->top);

        ret = ProcessMimeBody(buf, len, state);
        if (ret != MIME_DEC_OK) {
            CH_ERR_LOG("Error: ProcessMimeBody() function failed: %d",
                    ret);
            return ret;
        }
    }

    CH_DEBUG_LOG("END FLAG: %s", StateFlags[state->state_flag]);

    return ret;
}

/**
 * \brief Init the parser by allocating memory for the state and top-level entity
 *
 * \param data A caller-specified pointer to data for access within the data chunk
 * processor callback function
 * \param dcpfunc The data chunk processor callback function
 *
 * \return A pointer to the state object, or NULL if the operation fails
 */
MimeDecParseState * MimeDecInitParser(void *data,
        int (*DataChunkProcessorFunc)(const uint8_t *chunk, uint32_t len,
                MimeDecParseState *state),
		int (*HeaderProcessorFunc)(const uint8_t *name, uint32_t nlen,
			const uint8_t *value, uint32_t vlen, MimeDecParseState *state))
{
    MimeDecParseState *state;
    MimeDecEntity *mimeMsg;

    state = SCMalloc(sizeof(MimeDecParseState));
    if (state == NULL) {
        CH_DEBUG_LOG("memory allocation failed");
        return NULL;
    }
    memset(state, 0x00, sizeof(MimeDecParseState));

    state->stack = SCMalloc(sizeof(MimeDecStack));
    if (state->stack == NULL) {
        CH_DEBUG_LOG("memory allocation failed");
        SCFree(state);
        return NULL;
    }
    memset(state->stack, 0x00, sizeof(MimeDecStack));

    mimeMsg = SCMalloc(sizeof(MimeDecEntity));
    if (mimeMsg == NULL) {
        CH_DEBUG_LOG("memory allocation failed");
        SCFree(state->stack);
        SCFree(state);
        return NULL;
    }
    memset(mimeMsg, 0x00, sizeof(MimeDecEntity));
    mimeMsg->ctnt_flags |= CTNT_IS_MSG;

    /* Init state */
    state->msg = mimeMsg;
    PushStack(state->stack);
    if (state->stack->top == NULL) {
        CH_DEBUG_LOG("memory allocation failed");
        SCFree(state->stack);
        SCFree(state);
        return NULL;
    }
    state->stack->top->data = mimeMsg;
    state->state_flag = HEADER_READY;
    state->data = data;
    state->DataChunkProcessorFunc = DataChunkProcessorFunc;
	state->HeaderProcessorFunc = HeaderProcessorFunc;

    return state;
}

/**
 * \brief De-Init parser by freeing up any residual memory
 *
 * \param state The parser state
 *
 * \return none
 */
void MimeDecDeInitParser(MimeDecParseState *state)
{
    uint32_t cnt = 0;

    while (state->stack->top != NULL) {
        CH_DEBUG_LOG("Remaining on stack: [%p]=>[%p]",
                state->stack->top, state->stack->top->data);

        PopStack(state->stack);
        cnt++;
    }

    if (cnt > 1) {
        state->msg->anomaly_flags |= ANOM_MALFORMED_MSG;
        CH_DEBUG_LOG("Warning: Stack is not empty upon completion of "
                "processing (%u items remaining)", cnt);
    }

    SCFree(state->hname);
    FreeDataValue(state->hvalue);
    FreeMimeDecStack(state->stack);
#ifdef HAVE_NSS
    if (state->md5_ctx)
        HASH_Destroy(state->md5_ctx);
#endif
    SCFree(state);
}

/**
 * \brief Called to indicate that the last message line has been processed and
 * the parsing operation is complete
 *
 * This function should be called directly by the caller.
 *
 * \param state The parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
int MimeDecParseComplete(MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;

    CH_DEBUG_LOG("Parsing flagged as completed");

    /* Store the header value */
    ret = StoreMimeHeader(state);
    if (ret != MIME_DEC_OK) {
        CH_ERR_LOG("Error: StoreMimeHeader() function failed");
        return ret;
    }

    /* Lets complete the body */
    ret = ProcessBodyComplete(state);
    if (ret != MIME_DEC_OK) {
        CH_ERR_LOG("Error: ProcessBodyComplete() function failed");
        return ret;
    }

    if (state->stack->top == NULL) {
        state->msg->anomaly_flags |= ANOM_MALFORMED_MSG;
        CH_ERR_LOG("Error: Message is malformed");
        return MIME_DEC_ERR_DATA;
    }

    /* If encapsulated, pop off the stack */
    if (state->stack->top->is_encap) {
        PopStack(state->stack);
        if (state->stack->top == NULL) {
            state->msg->anomaly_flags |= ANOM_MALFORMED_MSG;
            CH_ERR_LOG("Error: Message is malformed");
            return MIME_DEC_ERR_DATA;
        }
    }

    /* Look extra stack items remaining */
    if (state->stack->top->next != NULL) {
        state->msg->anomaly_flags |= ANOM_MALFORMED_MSG;
        CH_DEBUG_LOG("Warning: Message has unclosed message part boundary");
    }

    state->state_flag = MIME_PARSE_DONE;

    return ret;
}

/**
 * \brief Parse a line of a MIME message and update the parser state
 *
 * \param line A string representing the line (w/out CRLF)
 * \param len The length of the line
 * \param delim_len The length of the line end delimiter
 * \param state The parser state
 *
 * \return MIME_DEC_OK on success, otherwise < 0 on failure
 */
int MimeDecParseLine(const uint8_t *line, const uint32_t len,
        const uint8_t delim_len, MimeDecParseState *state)
{
    int ret = MIME_DEC_OK;

    /* For debugging purposes */
    if (len > 0) {
        CH_DEBUG_LOG("SMTP LINE: %.*s", len, line);
    } else {
        CH_DEBUG_LOG("SMTP LINE - EMPTY");
    }

    state->current_line_delimiter_len = delim_len;
    /* Process the entity */
    ret = ProcessMimeEntity(line, len, state);
    if (ret != MIME_DEC_OK) {
        state->state_flag = MIME_PARSE_ERROR;
        CH_ERR_LOG("Error: ProcessMimeEntity() function failed: %d", ret);
    }

    return ret;
}

