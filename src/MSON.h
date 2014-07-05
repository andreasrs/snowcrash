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
    
    /** MSON Data Types */
    enum DataType {
        StringDataType = 0,    /// < String
        NumberDataType,        /// < Number
        ObjectDataType,        /// < Object
        ArrayDataType,         /// < Array
        BooleanDataType,       /// < Boolean
        UndefinedValueType = -1 /// < Undefined Type
    };
    
    /** \brief MSON Value base interface */
    struct ValueBase {
        virtual ~ValueBase() {}
        
        /** 
         * \brief Create a copy of a Value 
         *
         *  The value returned must be deallocated by the caller.
         */
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
        Value(const value_type &v_) : v(v_) {}
        value_type v;
        
        virtual ValueBase* duplicate() const {
            return ::new Value<value_type>(v);
        }
    };

    /** \brief Element of an MSON Array */
    struct Element {
        
        mdp::ByteBuffer description;        /// < Element description
        std::auto_ptr<ValueBase> value;     /// < Element value
        DataType type;                      /// < Element data type
        bool required;                      /// < True for required, false otherwise (default)
        
        Element() : type(UndefinedValueType), required(false) {}
        Element(const Element& e) { init(e); }
        Element& operator=(const Element& e) { init(e); return *this; }
        ~Element() { value.reset(); }
        
    private:
        void init(const Element& e) {
            description = e.description;
            type = e.type;
            required = e.required;
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
        static typename T::value_type& get(Element& e) {
            return dynamic_cast<T &>(*e.value).v;
        }
        
        static const typename T::value_type& get(const Element& e) {
            return dynamic_cast<const T &>(*e.value).v;
        }
    };
}

#endif
