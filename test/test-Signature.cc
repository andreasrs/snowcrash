//
//  test-Signature.cc
//  snowcrash
//
//  Created by Zdenek Nemec on 7/5/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "snowcrashtest.h"
#include "Signature.h"

using namespace snowcrash;

struct ElementTraits : public SignatureTraits {

    ElementTraits() {
        allowIdentifier = false;
    }
    
    static ElementTraits& get() {
        static ElementTraits s;
        return s;
    }
};

TEST_CASE("signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("id: 42 (yes, no) - a good message", SignatureTraits::get(), s);
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value == "42");
    REQUIRE(s.traits.size() == 2);
    REQUIRE(s.traits[0] == "yes");
    REQUIRE(s.traits[1] == "no");
    REQUIRE(s.content == "a good message");
}

TEST_CASE("identifier only signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("id", SignatureTraits::get(), s);
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("identifier description signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("id - a good - info", SignatureTraits::get(), s);
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.empty());
    REQUIRE(s.content == "a good - info");
}

TEST_CASE("identifier value signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("id : a good data", SignatureTraits::get(), s);
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value == "a good data");
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("identifier traits signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("id (number)", SignatureTraits::get(), s);
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content.empty());
}

TEST_CASE("element signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("42 (yes, no) - a good message", ElementTraits::get(), s);
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value == "42");
    REQUIRE(s.traits.size() == 2);
    REQUIRE(s.traits[0] == "yes");
    REQUIRE(s.traits[1] == "no");
    REQUIRE(s.content == "a good message");
}

TEST_CASE("element value only signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("42", ElementTraits::get(), s);
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value == "42");
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("element traits only signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("(number)", ElementTraits::get(), s);
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content.empty());
}

TEST_CASE("element traits description signature", "[signature]")
{
    Signature s;
    SignatureParser::parseSignature("(number) - something () cool", ElementTraits::get(), s);
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content == "something () cool");
}
