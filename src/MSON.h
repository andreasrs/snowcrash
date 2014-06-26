//
//  MSON.h
//
//  Created by Zdenek Nemec on 6/23/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_MSON_H
#define SNOWCRASH_MSON_H

#include <vector>
#include <string>
#include <utility>
#include <map>
#include "ByteBuffer.h"

/**
 *  MSON Abstract Syntax Tree
 */
namespace mson {
    
    typedef mdp::ByteBuffer ByteBuffer;
    
    /// Value Type
    enum ValueType {
        StringValueType = 0,    /// < String
        NumberValueType,        /// < Number
        ObjectValueType,        /// < Object
        ArrayValueType,         /// < Array
        BooleanValueType,       /// < Boolean
        UndefinedValueType = -1 /// < Undefined Type
    };
    
    /** \brief MSON Value base */
    struct ValueBase {
        virtual ~ValueBase() {}
        virtual ValueBase* duplicate() const = 0;
    };
    
    /**
     *  \brief MSON Value wrapper
     *
     *  Wraps generic (heterogenous) MSON data
     */
    template<typename T>
    struct Value : ValueBase {
        
        typedef T value_type;
        
        Value() {}
        Value(const T &v_) : v(v_) {}
        T v;
        
        virtual ValueBase* duplicate() const {
            return ::new Value<T>(v);
        }
    };

    /** \brief Element of an MSON Array */
    struct Element {
        mdp::ByteBuffer description;
        std::auto_ptr<ValueBase> value;
        ValueType type;
        
        Element() : type(UndefinedValueType) {}
        Element(const Element& e) { init(e); }
        Element& operator=(const Element& e) { init(e); return *this; }
        ~Element() { value.reset(); }
        
    private:
        void init(const Element& e) {
            description = e.description;
            type = e.type;
            if (e.value.get())
                value.reset(e.value->duplicate());
        }
    };
    
    /** \brief Property of an MSON Object */
    struct Property : public Element {
        mdp::ByteBuffer name;
    };
    
    /** \brief MSON String */
    typedef Value<mdp::ByteBuffer> StringValue;
    
    /** \brief MSON Array */
    typedef Value<std::vector<Element> > ArrayValue;
    
    /** \brief MSON Object */
    typedef Value<std::vector<Property> > ObjectValue;
    
    /** \brief Element Value access helper */
    template<typename T>
    struct ElementValue {
        static T& get(Element& e) {
            return dynamic_cast<Value<T> &>(*e.value).v;
        }
        
        static const T& get(const Element& e) {
            return dynamic_cast<const Value<T> &>(*e.value).v;
        }
    };
}

#endif
