//
//  test-Signature.cc
//  snowcrash
//
//  Created by Zdenek Nemec on 7/5/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "snowcrashtest.h"
#include "Signature.h"

static const mdp::ByteBuffer PropertySignatureFixture = "id: 42 (yes, no) - a good message";
static const mdp::ByteBuffer ElementSignatureFixture = "42 (number) - a good number";

using namespace snowcrash;

struct SignatureParserHelper {
    
    static void parse(const mdp::ByteBuffer& signature,
                      const SignatureTraits& traits,
                      Report& report,
                      mdp::ByteBuffer& remaining,
                      Signature& s) {
        
        mdp::MarkdownNode n;
        n.text = signature;
        n.sourceMap.push_back(mdp::Range(0, n.text.size()));
        mdp::MarkdownNodes v;
        v.push_back(n);
        
        Blueprint bp;
        SectionParserData pd(0, n.text, bp);
        
        SignatureParser::parseSignature(v.begin(), traits, pd, report, remaining, s);
    }
};

struct ElementTraits : public SignatureTraits {

    ElementTraits() {
        expectIdentifier = false;
    }
    
    static ElementTraits& get() {
        static ElementTraits s;
        return s;
    }
};

TEST_CASE("signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse(PropertySignatureFixture,
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value == "42");
    REQUIRE(s.traits.size() == 2);
    REQUIRE(s.traits[0] == "yes");
    REQUIRE(s.traits[1] == "no");
    REQUIRE(s.content == "a good message");
}

TEST_CASE("mutliline signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("id\nLine 2\nLine 3\n",
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc == "Line 2\nLine 3\n");
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("identifier only signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("id",
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("identifier description signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("id - a good - info",
                                 SignatureTraits::get(), r, rc, s);

    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.empty());
    REQUIRE(s.content == "a good - info");
}

TEST_CASE("identifier value signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("id : a good data",
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value == "a good data");
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("identifier traits signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("id (number)",
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content.empty());
}

TEST_CASE("element signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse(ElementSignatureFixture,
                                 ElementTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value == "42");
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content == "a good number");
}

TEST_CASE("element value only signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("42",
                                 ElementTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value == "42");
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("element traits only signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("(number)",
                                 ElementTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content.empty());
}

TEST_CASE("element traits description signature", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("(number) - something () cool",
                                 ElementTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.empty());
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content == "something () cool");
}

TEST_CASE("identifier with element traits", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse(PropertySignatureFixture,
                                 ElementTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.size() == 1);
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value == "id");
    REQUIRE(s.traits.empty());
    REQUIRE(s.content.empty());
}

TEST_CASE("property signature without identifier", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("(x)",
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.size() == 1);
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "x");
    REQUIRE(s.content.empty());
}

TEST_CASE("property signature colon without a value", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("id: (number)",
                                 SignatureTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.size() == 1);
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier == "id");
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.size() == 1);
    REQUIRE(s.traits[0] == "number");
    REQUIRE(s.content.empty());
}

TEST_CASE("element signature value and traits", "[signature]")
{
    Report r;
    mdp::ByteBuffer rc;
    Signature s;
    
    SignatureParserHelper::parse("- content is the king",
                                 ElementTraits::get(), r, rc, s);
    
    REQUIRE(r.error.code == Error::OK);
    REQUIRE(r.warnings.size() == 1);
    REQUIRE(rc.empty());
    
    REQUIRE(s.identifier.empty());
    REQUIRE(s.value.empty());
    REQUIRE(s.traits.empty());
    REQUIRE(s.content == "content is the king");
}

