/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "asn1/ISO-TS-19091-addgrp-C-2018.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#ifndef	_IntersectionAccessPoint_H_
#define	_IntersectionAccessPoint_H_


#include "asn_application.h"

/* Including external dependencies */
#include "LaneID.h"
#include "ApproachID.h"
#include "LaneConnectionID.h"
#include "constr_CHOICE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum IntersectionAccessPoint_PR {
	IntersectionAccessPoint_PR_NOTHING,	/* No components present */
	IntersectionAccessPoint_PR_lane,
	IntersectionAccessPoint_PR_approach,
	IntersectionAccessPoint_PR_connection
	/* Extensions may appear below */
	
} IntersectionAccessPoint_PR;

/* IntersectionAccessPoint */
typedef struct IntersectionAccessPoint {
	IntersectionAccessPoint_PR present;
	union IntersectionAccessPoint_u {
		LaneID_t	 lane;
		ApproachID_t	 approach;
		LaneConnectionID_t	 connection;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} IntersectionAccessPoint_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_IntersectionAccessPoint;
extern asn_CHOICE_specifics_t asn_SPC_IntersectionAccessPoint_specs_1;
extern asn_TYPE_member_t asn_MBR_IntersectionAccessPoint_1[3];
extern asn_per_constraints_t asn_PER_type_IntersectionAccessPoint_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _IntersectionAccessPoint_H_ */
#include "asn_internal.h"
