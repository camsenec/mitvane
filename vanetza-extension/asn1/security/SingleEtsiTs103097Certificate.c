/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EtsiTs103097Module"
 * 	found in "asn1/TS103097v131.asn"
 * 	`asn1c -fcompound-names -fincludes-quoted -no-gen-example -R`
 */

#include "SingleEtsiTs103097Certificate.h"

static asn_TYPE_member_t asn_MBR_SingleEtsiTs103097Certificate_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SingleEtsiTs103097Certificate, only),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_EtsiTs103097Certificate,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"only"
		},
};
static const ber_tlv_tag_t asn_DEF_SingleEtsiTs103097Certificate_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_SingleEtsiTs103097Certificate_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* only */
};
static asn_SEQUENCE_specifics_t asn_SPC_SingleEtsiTs103097Certificate_specs_1 = {
	sizeof(struct SingleEtsiTs103097Certificate),
	offsetof(struct SingleEtsiTs103097Certificate, _asn_ctx),
	asn_MAP_SingleEtsiTs103097Certificate_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_SingleEtsiTs103097Certificate = {
	"SingleEtsiTs103097Certificate",
	"SingleEtsiTs103097Certificate",
	&asn_OP_SEQUENCE,
	asn_DEF_SingleEtsiTs103097Certificate_tags_1,
	sizeof(asn_DEF_SingleEtsiTs103097Certificate_tags_1)
		/sizeof(asn_DEF_SingleEtsiTs103097Certificate_tags_1[0]), /* 1 */
	asn_DEF_SingleEtsiTs103097Certificate_tags_1,	/* Same as above */
	sizeof(asn_DEF_SingleEtsiTs103097Certificate_tags_1)
		/sizeof(asn_DEF_SingleEtsiTs103097Certificate_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_SingleEtsiTs103097Certificate_1,
	1,	/* Elements count */
	&asn_SPC_SingleEtsiTs103097Certificate_specs_1	/* Additional specs */
};

