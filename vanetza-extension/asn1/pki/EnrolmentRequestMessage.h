/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EtsiTs102941MessagesCa"
 * 	found in "asn1/TS102941v131-MessagesCa.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_EnrolmentRequestMessage_H_
#define	_EnrolmentRequestMessage_H_


#include "asn_application.h"

/* Including external dependencies */
#include "EtsiTs103097Data-SignedAndEncrypted-Unicast.h"

#ifdef __cplusplus
extern "C" {
#endif

/* EnrolmentRequestMessage */
typedef EtsiTs103097Data_SignedAndEncrypted_Unicast_64P0_t	 EnrolmentRequestMessage_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EnrolmentRequestMessage;
asn_struct_free_f EnrolmentRequestMessage_free;
asn_struct_print_f EnrolmentRequestMessage_print;
asn_constr_check_f EnrolmentRequestMessage_constraint;
ber_type_decoder_f EnrolmentRequestMessage_decode_ber;
der_type_encoder_f EnrolmentRequestMessage_encode_der;
xer_type_decoder_f EnrolmentRequestMessage_decode_xer;
xer_type_encoder_f EnrolmentRequestMessage_encode_xer;
oer_type_decoder_f EnrolmentRequestMessage_decode_oer;
oer_type_encoder_f EnrolmentRequestMessage_encode_oer;
per_type_decoder_f EnrolmentRequestMessage_decode_uper;
per_type_encoder_f EnrolmentRequestMessage_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _EnrolmentRequestMessage_H_ */
#include "asn_internal.h"
