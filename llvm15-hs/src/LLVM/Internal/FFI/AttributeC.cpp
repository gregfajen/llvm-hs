#define __STDC_LIMIT_MACROS
#include "llvm-c/Core.h"
#include "llvm/IR/DerivedTypes.h"
#include "LLVM/Internal/FFI/AttributeC.hpp"
#include "llvm/IR/LLVMContext.h"

#include <tuple>

extern "C" {

static_assert(sizeof(AttributeList) == sizeof(AttributeListImpl *),
              "AttributeList implementation has changed");

static_assert(sizeof(Attribute) == sizeof(AttributeImpl *),
              "Attribute implementation has changed");

#define CHECK(name, p, r, f)                                                   \
    static_assert(unsigned(llvm::Attribute::name) ==                           \
                      unsigned(LLVM_Hs_AttributeKind_##name),                  \
                  "LLVM_Hs_AttributeKind enum out of sync w/ "                 \
                  "llvm::Attribute::AttrKind for " #name);
LLVM_HS_FOR_EACH_ATTRIBUTE_KIND(CHECK)
#undef CHECK

uint64_t LLVM_Hs_AttributeValueAsInt(LLVMAttributeRef a) {
    return unwrap(a).getValueAsInt();
}

void LLVM_Hs_AttributeEnsureUWTableKindDefault(LLVMAttributeRef a) {
    assert(unwrap(a).getUWTableKind() == UWTableKind::Default);
}

LLVMTypeRef LLVM_Hs_AttributeValueAsType(LLVMAttributeRef a) {
    return wrap(unwrap(a).getValueAsType());
}

LLVMBool LLVM_Hs_IsStringAttribute(LLVMAttributeRef a) {
    return unwrap(a).isStringAttribute();
}

const char *LLVM_Hs_AttributeKindAsString(LLVMAttributeRef a, size_t &l) {
    const StringRef s = unwrap(a).getKindAsString();
    l = s.size();
    return s.data();
}

const char *LLVM_Hs_AttributeValueAsString(LLVMAttributeRef a, size_t &l) {
    const StringRef s = unwrap(a).getValueAsString();
    l = s.size();
    return s.data();
}

AttributeList * LLVM_Hs_GetAttributeList(LLVMContextRef context,
                                         unsigned index,
                                         AttributeSet *as) {
    return new AttributeList(AttributeList::get(*unwrap(context), index, *as));
}

AttributeList * LLVM_Hs_BuildAttributeList(LLVMContextRef context,
                                           AttributeSet *fAttrs,
                                           AttributeSet *rAttrs,
                                           AttributeSet **pAttrs,
                                           unsigned numPAttrs) {
    std::vector<AttributeSet> pAttrSets{numPAttrs};
    for (unsigned i = 0; i < numPAttrs; ++i) {
        pAttrSets[i] = *pAttrs[i];
    }
    return new AttributeList(
        AttributeList::get(*unwrap(context), *fAttrs, *rAttrs, pAttrSets));
}

void LLVM_Hs_DisposeAttributeList(AttributeList *attributeList) {
    delete attributeList;
}

AttributeSet *LLVM_Hs_GetAttributeSet(LLVMContextRef context,
                                      const AttrBuilder &ab) {
    return new AttributeSet(AttributeSet::get(*unwrap(context), ab));
}

void LLVM_Hs_DisposeAttributeSet(AttributeList *attributeList) {
    delete attributeList;
}

LLVMBool LLVM_Hs_AttributeSetsEqual(AttributeSet *as1,
                                    AttributeSet *as2) {
    return *as1 == *as2;
}

LLVMBool LLVM_Hs_AttributeSetHasAttributes(AttributeSet *as) {
    return as->hasAttributes();
}

unsigned LLVM_Hs_getNumAttributes(AttributeSet *attributeSet) {
    return attributeSet->getNumAttributes();
}

void LLVM_Hs_getAttributes(AttributeSet *attributeSet,
                           LLVMAttributeRef *attrs) {
    for (auto a : *attributeSet) {
        *attrs++ = wrap(a);
    }
}

size_t LLVM_Hs_GetAttrBuilderSize() { return sizeof(AttrBuilder); }

AttrBuilder *LLVM_Hs_ConstructAttrBuilder(LLVMContextRef context, char *p) {
    return new (p) AttrBuilder(*unwrap(context));
}

AttrBuilder *LLVM_Hs_AttrBuilderFromAttrSet(LLVMContextRef context, LLVMAttributeSetRef as) {
    return new AttrBuilder(*unwrap(context), *as);
}

void LLVM_Hs_DisposeAttrBuilder(AttrBuilder *as) { delete as; }

void LLVM_Hs_AttrBuilderMerge(AttrBuilder *ab1, AttrBuilder *ab2) {
    ab1->merge(*ab2);
}

void LLVM_Hs_DestroyAttrBuilder(AttrBuilder *a) { a->~AttrBuilder(); }

void LLVM_Hs_AttrBuilderAddAttributeKind(AttrBuilder &ab, unsigned kind) {
    ab.addAttribute(Attribute::AttrKind(kind));
}

void LLVM_Hs_AttrBuilderAddTypeAttribute(AttrBuilder &ab, unsigned kind, LLVMTypeRef t) {
    ab.addTypeAttr(Attribute::AttrKind(kind), unwrap(t));
}

void LLVM_Hs_AttrBuilderAddStringAttribute(AttrBuilder &ab, const char *kind,
                                           size_t kind_len, const char *value,
                                           size_t value_len) {
    ab.addAttribute(StringRef(kind, kind_len), StringRef(value, value_len));
}

void LLVM_Hs_AttrBuilderAddAlignment(AttrBuilder &ab, uint64_t v) {
    ab.addAlignmentAttr(MaybeAlign(v));
}

void LLVM_Hs_AttrBuilderAddStackAlignment(AttrBuilder &ab, uint64_t v) {
    ab.addStackAlignmentAttr(MaybeAlign(v));
}

void LLVM_Hs_AttrBuilderAddUWTable(AttrBuilder &ab) {
    ab.addUWTableAttr(UWTableKind::Default);
}

void LLVM_Hs_AttrBuilderAddAllocSize(AttrBuilder &ab, unsigned x, unsigned y,
                                     LLVMBool optionalIsThere) {
    if (optionalIsThere) {
        ab.addAllocSizeAttr(x, Optional<unsigned>(y));
    } else {
        ab.addAllocSizeAttr(x, Optional<unsigned>());
    }
}

void LLVM_Hs_AttrBuilderAddDereferenceableAttr(AttrBuilder &ab, uint64_t v) {
    ab.addDereferenceableAttr(v);
}

void LLVM_Hs_AttrBuilderAddDereferenceableOrNullAttr(AttrBuilder &ab,
                                                     uint64_t v) {
    ab.addDereferenceableOrNullAttr(v);
}

LLVMBool LLVM_Hs_AttributeGetAllocSizeArgs(LLVMAttributeRef a, unsigned *x,
                                           unsigned *y) {
    auto pair = unwrap(a).getAllocSizeArgs();
    *x = pair.first;
    if (pair.second.hasValue()) {
        *y = pair.second.getValue();
        return 1;
    } else {
        return 0;
    }
}

void LLVM_Hs_AttributeGetVScaleRangeArgs(LLVMAttributeRef a, unsigned *min, unsigned *max) {
  *min = unwrap(a).getVScaleRangeMin();
  *max = unwrap(a).getVScaleRangeMax().value_or(0);
}

void LLVM_Hs_AttrBuilderAddVScaleRange(AttrBuilder &ab, unsigned min, unsigned max) {
  ab.addVScaleRangeAttr(min, max);
}

}