// Copyright (c) 2014-2016 Dropbox, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PYSTON_CODEGEN_COMPVARS_H
#define PYSTON_CODEGEN_COMPVARS_H

#include <stdint.h>
#include <vector>

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

#include "core/ast.h"
#include "core/types.h"

namespace pyston {

class OpInfo;

class CompilerType;
class IREmitter;

class BoxedInt;
class BoxedFloat;
class BoxedLong;
class BoxedString;

typedef llvm::SmallVector<uint64_t, 1> FrameVals;

class CompilerType {
public:
    enum Result { Yes, No, Maybe };

    virtual ~CompilerType() {}
    virtual std::string debugName() = 0;
    virtual ConcreteCompilerType* getConcreteType() = 0;
    virtual ConcreteCompilerType* getBoxType() = 0;
    virtual bool canConvertTo(CompilerType* other_type) = 0;
    virtual CompilerType* getattrType(BoxedString* attr, bool cls_only) = 0;
    virtual CompilerType* getPystonIterType();
    virtual Result hasattr(BoxedString* attr);
    virtual CompilerType* callType(ArgPassSpec argspec, const std::vector<CompilerType*>& arg_types,
                                   const std::vector<llvm::StringRef>* keyword_names) = 0;

    virtual BoxedClass* guaranteedClass() = 0;
    virtual Box* deserializeFromFrame(const FrameVals& vals) = 0;
    virtual int numFrameArgs() = 0;
    virtual std::vector<CompilerType*> unpackTypes(int num_into);

    // Some types are not "usable" even if they are "concrete".  Concrete means that it's possible to
    // represent the value as an llvm::Value*; usable means that we are allowed to use it as a variable's
    // type.  Some concrete types are not usable; for example unboxed ints are concrete (can be represented
    // as an i64) but are not usable, since we need to use the form that remembers what it gets boxed to.
    virtual CompilerType* getUsableType() { return this; }
    bool isUsable() { return this == getUsableType(); }
};

typedef std::unordered_map<CompilerVariable*, CompilerVariable*> DupCache;

enum BinExpType {
    AugBinOp,
    BinOp,
    Compare,
};

template <class V> class _ValuedCompilerType : public CompilerType {
public:
    typedef ValuedCompilerVariable<V> VAR;

    virtual void assertMatches(V v) = 0;

    virtual CompilerVariable* dup(VAR* v, DupCache& cache) {
        printf("dup not defined for %s\n", debugName().c_str());
        abort();
    }
    ConcreteCompilerType* getConcreteType() override {
        printf("getConcreteType not defined for %s\n", debugName().c_str());
        abort();
    }
    ConcreteCompilerType* getBoxType() override {
        printf("getBoxType not defined for %s\n", debugName().c_str());
        abort();
    }
    bool canConvertTo(CompilerType* other_type) override {
        printf("canConvertTo not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual ConcreteCompilerVariable* makeConverted(IREmitter& emitter, VAR* var, ConcreteCompilerType* other_type) {
        printf("makeConverted not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* nonzero(IREmitter& emitter, const OpInfo& info, VAR* var) {
        printf("nonzero not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* unaryop(IREmitter& emitter, const OpInfo& info, VAR* var, AST_TYPE::AST_TYPE op_type);
    virtual CompilerVariable* hasnext(IREmitter& emitter, const OpInfo& info, VAR* var) {
        printf("hasnext not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* getattr(IREmitter& emitter, const OpInfo& info, VAR* var, BoxedString* attr,
                                      bool cls_only) {
        printf("getattr not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual void setattr(IREmitter& emitter, const OpInfo& info, VAR* var, BoxedString* attr, CompilerVariable* v) {
        printf("setattr not defined for %s\n", debugName().c_str());
        abort();
    }

    virtual void delattr(IREmitter& emitter, const OpInfo& info, VAR* value, BoxedString* attr) {
        printf("delattr not defined for %s\n", debugName().c_str());
        abort();
    }

    virtual CompilerVariable* callattr(IREmitter& emitter, const OpInfo& info, VAR* var, BoxedString* attr,
                                       CallattrFlags flags, const std::vector<CompilerVariable*>& args,
                                       const std::vector<BoxedString*>* keyword_names) {
        printf("callattr not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* call(IREmitter& emitter, const OpInfo& info, VAR* var, struct ArgPassSpec argspec,
                                   const std::vector<CompilerVariable*>& args,
                                   const std::vector<BoxedString*>* keyword_names);
    virtual CompilerVariable* len(IREmitter& emitter, const OpInfo& info, VAR* var) {
        printf("len not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* getitem(IREmitter& emitter, const OpInfo& info, VAR* var, CompilerVariable* v) {
        // Can almost do this, except for error messages + types:
        // static const std::string attr("__getitem__");
        // return callattr(emitter, info, var, &attr, true, ArgPassSpec(1, 0, 0, 0), {v}, NULL);
        printf("getitem not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* getPystonIter(IREmitter& emitter, const OpInfo& info, VAR* var);
    virtual CompilerVariable* binexp(IREmitter& emitter, const OpInfo& info, VAR* var, CompilerVariable* rhs,
                                     AST_TYPE::AST_TYPE op_type, BinExpType exp_type) {
        printf("binexp not defined for %s\n", debugName().c_str());
        abort();
    }
    virtual CompilerVariable* contains(IREmitter& emitter, const OpInfo& info, VAR* var, CompilerVariable* lhs);
    virtual llvm::Value* makeClassCheck(IREmitter& emitter, VAR* var, BoxedClass* c) {
        printf("makeClassCheck not defined for %s\n", debugName().c_str());
        abort();
    }
    CompilerType* getattrType(BoxedString* attr, bool cls_only) override {
        printf("getattrType not defined for %s\n", debugName().c_str());
        abort();
    }
    CompilerType* callType(struct ArgPassSpec argspec, const std::vector<CompilerType*>& arg_types,
                           const std::vector<llvm::StringRef>* keyword_names) override;
    BoxedClass* guaranteedClass() override {
        ASSERT((CompilerType*)getConcreteType() != this, "%s", debugName().c_str());
        return getConcreteType()->guaranteedClass();
    }
    virtual void serializeToFrame(VAR* v, std::vector<llvm::Value*>& stackmap_args) = 0;

    virtual std::vector<CompilerVariable*> unpack(IREmitter& emitter, const OpInfo& info, VAR* var, int num_into);
};

template <class V> class ValuedCompilerType : public _ValuedCompilerType<V> { public: };

template <> class ValuedCompilerType<llvm::Value*> : public _ValuedCompilerType<llvm::Value*> {
public:
    virtual llvm::Type* llvmType() = 0;
    std::string debugName() override;

    void assertMatches(llvm::Value* v) override {
        if (v->getType() != llvmType()) {
            v->getType()->dump();
            llvmType()->dump();
            fprintf(stderr, "\n");
        }
        assert(v->getType() == llvmType());
    }

    virtual bool isFitBy(BoxedClass*) {
        printf("isFitBy not defined for %s\n", debugName().c_str());
        abort();
    }

    CompilerVariable* dup(ConcreteCompilerVariable* v, DupCache& cache) override;
    ConcreteCompilerType* getConcreteType() override { return this; }
    bool canConvertTo(CompilerType* other_type) override { return other_type == this || other_type == UNKNOWN; }
    ConcreteCompilerVariable* makeConverted(IREmitter& emitter, ConcreteCompilerVariable* var,
                                            ConcreteCompilerType* other_type) override;
    void serializeToFrame(VAR* var, std::vector<llvm::Value*>& stackmap_args) override;
    int numFrameArgs() override { return 1; }
};

class CompilerVariable {
public:
    CompilerVariable() {}
    virtual ~CompilerVariable() {}

    virtual CompilerVariable* dup(DupCache& cache) = 0;

    virtual CompilerType* getType() = 0;
    virtual ConcreteCompilerType* getConcreteType() = 0;
    virtual ConcreteCompilerType* getBoxType() = 0;
    virtual bool canConvertTo(CompilerType* other_type) = 0;
    virtual ConcreteCompilerVariable* makeConverted(IREmitter& emitter, ConcreteCompilerType* other_type) = 0;
    virtual llvm::Value* makeClassCheck(IREmitter& emitter, BoxedClass* cls) = 0;
    virtual BoxedClass* guaranteedClass() = 0;

    virtual CompilerVariable* nonzero(IREmitter& emitter, const OpInfo& info) = 0;
    virtual CompilerVariable* unaryop(IREmitter& emitter, const OpInfo& info, AST_TYPE::AST_TYPE op_type) = 0;
    virtual CompilerVariable* hasnext(IREmitter& emitter, const OpInfo& info) = 0;
    virtual CompilerVariable* getattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr, bool cls_only) = 0;
    virtual void setattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr, CompilerVariable* v) = 0;
    virtual void delattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr) = 0;
    virtual CompilerVariable* callattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr, CallattrFlags flags,
                                       const std::vector<CompilerVariable*>& args,
                                       const std::vector<BoxedString*>* keyword_names) = 0;
    virtual CompilerVariable* call(IREmitter& emitter, const OpInfo& info, struct ArgPassSpec argspec,
                                   const std::vector<CompilerVariable*>& args,
                                   const std::vector<BoxedString*>* keyword_names) = 0;
    virtual CompilerVariable* len(IREmitter& emitter, const OpInfo& info) = 0;
    virtual CompilerVariable* getitem(IREmitter& emitter, const OpInfo& info, CompilerVariable*) = 0;
    virtual CompilerVariable* getPystonIter(IREmitter& emitter, const OpInfo& info) = 0;
    virtual CompilerVariable* binexp(IREmitter& emitter, const OpInfo& info, CompilerVariable* rhs,
                                     AST_TYPE::AST_TYPE op_type, BinExpType exp_type) = 0;
    virtual CompilerVariable* contains(IREmitter& emitter, const OpInfo& info, CompilerVariable* lhs) = 0;

    virtual void serializeToFrame(std::vector<llvm::Value*>& stackmap_args) = 0;

    virtual std::vector<CompilerVariable*> unpack(IREmitter& emitter, const OpInfo& info, int num_into) = 0;
};

template <class V> class ValuedCompilerVariable : public CompilerVariable {
private:
    typedef ValuedCompilerType<V> T;
    T* type;
    V value;

public:
    ValuedCompilerVariable(T* type, V value) : CompilerVariable(), type(type), value(value) {
#ifndef NDEBUG
        type->assertMatches(value);
#endif
    }
    T* getType() override { return type; }
    V getValue() { return value; }

    ConcreteCompilerType* getConcreteType() override { return type->getConcreteType(); }
    ConcreteCompilerType* getBoxType() override { return type->getBoxType(); }

    CompilerVariable* dup(DupCache& cache) override {
        CompilerVariable* rtn = type->dup(this, cache);

        return rtn;
    }

    bool canConvertTo(CompilerType* other_type) override { return type->canConvertTo(other_type); }
    ConcreteCompilerVariable* makeConverted(IREmitter& emitter, ConcreteCompilerType* other_type) override {
        ConcreteCompilerVariable* rtn = type->makeConverted(emitter, this, other_type);
        ASSERT(rtn->getType() == other_type, "%s", type->debugName().c_str());
        return rtn;
    }
    CompilerVariable* nonzero(IREmitter& emitter, const OpInfo& info) override {
        return type->nonzero(emitter, info, this);
    }
    CompilerVariable* unaryop(IREmitter& emitter, const OpInfo& info, AST_TYPE::AST_TYPE op_type) override {
        return type->unaryop(emitter, info, this, op_type);
    }
    CompilerVariable* hasnext(IREmitter& emitter, const OpInfo& info) override {
        return type->hasnext(emitter, info, this);
    }
    CompilerVariable* getattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr, bool cls_only) override {
        return type->getattr(emitter, info, this, attr, cls_only);
    }
    void setattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr, CompilerVariable* v) override {
        type->setattr(emitter, info, this, attr, v);
    }

    void delattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr) override {
        type->delattr(emitter, info, this, attr);
    }

    CompilerVariable* callattr(IREmitter& emitter, const OpInfo& info, BoxedString* attr, CallattrFlags flags,
                               const std::vector<CompilerVariable*>& args,
                               const std::vector<BoxedString*>* keyword_names) override {
        return type->callattr(emitter, info, this, attr, flags, args, keyword_names);
    }
    CompilerVariable* call(IREmitter& emitter, const OpInfo& info, struct ArgPassSpec argspec,
                           const std::vector<CompilerVariable*>& args,
                           const std::vector<BoxedString*>* keyword_names) override {
        return type->call(emitter, info, this, argspec, args, keyword_names);
    }
    CompilerVariable* len(IREmitter& emitter, const OpInfo& info) override { return type->len(emitter, info, this); }
    CompilerVariable* getitem(IREmitter& emitter, const OpInfo& info, CompilerVariable* slice) override {
        return type->getitem(emitter, info, this, slice);
    }
    CompilerVariable* getPystonIter(IREmitter& emitter, const OpInfo& info) override {
        return type->getPystonIter(emitter, info, this);
    }
    CompilerVariable* binexp(IREmitter& emitter, const OpInfo& info, CompilerVariable* rhs, AST_TYPE::AST_TYPE op_type,
                             BinExpType exp_type) override {
        return type->binexp(emitter, info, this, rhs, op_type, exp_type);
    }
    CompilerVariable* contains(IREmitter& emitter, const OpInfo& info, CompilerVariable* lhs) override {
        return type->contains(emitter, info, this, lhs);
    }

    llvm::Value* makeClassCheck(IREmitter& emitter, BoxedClass* cls) override {
        return type->makeClassCheck(emitter, this, cls);
    }

    BoxedClass* guaranteedClass() override { return type->guaranteedClass(); }

    void serializeToFrame(std::vector<llvm::Value*>& stackmap_args) override {
        type->serializeToFrame(this, stackmap_args);
    }

    std::vector<CompilerVariable*> unpack(IREmitter& emitter, const OpInfo& info, int num_into) override {
        return type->unpack(emitter, info, this, num_into);
    }
};

// Emit the test for whether one variable 'is' another one.
ConcreteCompilerVariable* doIs(IREmitter& emitter, CompilerVariable* lhs, CompilerVariable* rhs, bool negate);

// These functions all return an INT variable, from either an unboxed representation (makeInt) or
// a boxed representation (makeUnboxedInt)
CompilerVariable* makeInt(int64_t);
CompilerVariable* makeInt(llvm::Value*);
CompilerVariable* makeUnboxedInt(IREmitter&, ConcreteCompilerVariable*);
CompilerVariable* makeUnboxedInt(IREmitter&, llvm::Value*);

// Same for floats:
CompilerVariable* makeFloat(llvm::Value*);
CompilerVariable* makeFloat(double);
CompilerVariable* makeUnboxedFloat(IREmitter&, ConcreteCompilerVariable*);
CompilerVariable* makeUnboxedFloat(IREmitter&, llvm::Value*);

llvm::Value* makeLLVMBool(bool b);
ConcreteCompilerVariable* makeBool(bool);
ConcreteCompilerVariable* makeLong(IREmitter&, Box*);
ConcreteCompilerVariable* makePureImaginary(IREmitter&, Box*);
CompilerVariable* makeStr(BoxedString*);
CompilerVariable* makeUnicode(Box*);

struct UnboxedSlice {
    CompilerVariable* start, *stop, *step;
};
CompilerVariable* makeSlice(CompilerVariable* start, CompilerVariable* stop, CompilerVariable* step);
UnboxedSlice extractSlice(CompilerVariable* slice);

#if 0
CompilerVariable* makeUnicode(IREmitter& emitter, llvm::StringRef);
#endif
CompilerVariable* makeFunction(IREmitter& emitter, FunctionMetadata*, llvm::Value* closure, llvm::Value* globals,
                               const std::vector<ConcreteCompilerVariable*>& defaults);
ConcreteCompilerVariable* undefVariable();
CompilerVariable* makeTuple(const std::vector<CompilerVariable*>& elts);

CompilerType* typeOfClassobj(BoxedClass*);
CompilerType* makeTupleType(const std::vector<CompilerType*>& elt_types);
CompilerType* makeFuncType(ConcreteCompilerType* rtn_type, const std::vector<ConcreteCompilerType*>& arg_types);

ConcreteCompilerVariable* boolFromI1(IREmitter&, llvm::Value*);
llvm::Value* i1FromBool(IREmitter&, ConcreteCompilerVariable*);
llvm::Value* i1FromLLVMBool(IREmitter&, llvm::Value*);

template <typename V>
CompilerVariable* _ValuedCompilerType<V>::getPystonIter(IREmitter& emitter, const OpInfo& info, VAR* var) {
    ConcreteCompilerVariable* converted = makeConverted(emitter, var, getBoxType());
    auto r = UNKNOWN->getPystonIter(emitter, info, converted);
    return r;
}

template <typename V>
CompilerVariable* _ValuedCompilerType<V>::contains(IREmitter& emitter, const OpInfo& info, VAR* var,
                                                   CompilerVariable* rhs) {
    ConcreteCompilerVariable* converted = makeConverted(emitter, var, getBoxType());
    auto r = UNKNOWN->contains(emitter, info, converted, rhs);
    return r;
}

template <typename V>
CompilerVariable* _ValuedCompilerType<V>::unaryop(IREmitter& emitter, const OpInfo& info, VAR* var,
                                                  AST_TYPE::AST_TYPE op_type) {
    ConcreteCompilerVariable* converted = makeConverted(emitter, var, getBoxType());
    auto r = UNKNOWN->unaryop(emitter, info, converted, op_type);
    return r;
}

template <typename V>
std::vector<CompilerVariable*> _ValuedCompilerType<V>::unpack(IREmitter& emitter, const OpInfo& info, VAR* var,
                                                              int num_into) {
    assert((CompilerType*)this != UNKNOWN);

    ConcreteCompilerVariable* converted = makeConverted(emitter, var, UNKNOWN);
    auto r = UNKNOWN->unpack(emitter, info, converted, num_into);
    return r;
}

template <typename V>
CompilerType* _ValuedCompilerType<V>::callType(struct ArgPassSpec argspec, const std::vector<CompilerType*>& arg_types,
                                               const std::vector<llvm::StringRef>* keyword_names) {
    return UNKNOWN;
}

template <typename V>
CompilerVariable* _ValuedCompilerType<V>::call(IREmitter& emitter, const OpInfo& info, VAR* var,
                                               struct ArgPassSpec argspec, const std::vector<CompilerVariable*>& args,
                                               const std::vector<BoxedString*>* keyword_names) {
    assert((CompilerType*)this != UNKNOWN);
    ConcreteCompilerVariable* converted = makeConverted(emitter, var, UNKNOWN);
    auto r = UNKNOWN->call(emitter, info, converted, argspec, args, keyword_names);
    return r;
}

} // namespace pyston

#endif
