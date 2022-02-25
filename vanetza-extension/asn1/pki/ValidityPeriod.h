/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "IEEE1609dot2BaseTypes"
 * 	found in "asn1/IEEE1609dot2BaseTypes.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_ValidityPeriod_H_
#define	_ValidityPeriod_H_


#include "asn_application.h"

/* Including external dependencies */
#include "Time32.h"
#include "Duration.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ValidityPeriod */
typedef struct ValidityPeriod {
	Time32_t	 start;
	Duration_t	 duration;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ValidityPeriod_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ValidityPeriod;
extern asn_SEQUENCE_specifics_t asn_SPC_ValidityPeriod_specs_1;
extern asn_TYPE_member_t asn_MBR_ValidityPeriod_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _ValidityPeriod_H_ */
#include "asn_internal.h"
