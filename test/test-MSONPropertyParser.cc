//
//  test-MSONPropertyParser.cc
//  snowcrash
//
//  Created by Zdenek Nemec on 6/23/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#include "snowcrashtest.h"
#include "MSONPropertyParser.h"

using namespace snowcrash;
using namespace snowcrashtest;

const mdp::ByteBuffer UnescapedPropertyFixture = \
"- id: 42 (number) - Identifier of the resource\n";

const mdp::ByteBuffer EscapedPropertyFixture = \
"- `id:1`: `42` (number) - Identifier of the resource\n";

TEST_CASE("recognize unescaped property", "[mson][property]")
{
    mdp::MarkdownParser markdownParser;
    mdp::MarkdownNode markdownAST;
    markdownParser.parse(UnescapedPropertyFixture, markdownAST);
    
    REQUIRE(!markdownAST.children().empty());
    REQUIRE(SectionProcessor<mson::Property>::sectionType(markdownAST.children().begin()) == MSONPropertySectionType);
}

TEST_CASE("recognize escaped property", "[mson][property]")
{
    mdp::MarkdownParser markdownParser;
    mdp::MarkdownNode markdownAST;
    markdownParser.parse(EscapedPropertyFixture, markdownAST);
    
    REQUIRE(!markdownAST.children().empty());
    REQUIRE(SectionProcessor<mson::Property>::sectionType(markdownAST.children().begin()) == MSONPropertySectionType);
}

//TEST_CASE("parse unescaped property", "[mson][property]")
//{
//    mson::Property property;
//    Report report;
//    SectionParserHelper<mson::Property, MSONPropertyParser>::parse(UnescapedPropertyFixture, MSONObjectSectionType, report, property);
//    
//    REQUIRE(report.error.code == Error::OK);
//    REQUIRE(report.warnings.empty());
//
//    REQUIRE(property.name == "id");
//    REQUIRE(property.description.empty());
//}

TEST_CASE("parse escaped property", "[mson][property]")
{
    mson::Property property;
    Report report;
    SectionParserHelper<mson::Property, MSONPropertyParser>::parse(EscapedPropertyFixture, MSONObjectSectionType, report, property);
    
    REQUIRE(report.error.code == Error::OK);
    REQUIRE(report.warnings.empty());
    
    REQUIRE(property.name == "id:1");
    REQUIRE(property.description.empty());
}
