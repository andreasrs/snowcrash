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

const mdp::ByteBuffer PropertyFixture = \
"+ `id`\n";

TEST_CASE("recognize property", "[mson]")
{
    mdp::MarkdownParser markdownParser;
    mdp::MarkdownNode markdownAST;
    markdownParser.parse(PropertyFixture, markdownAST);
    
    REQUIRE(!markdownAST.children().empty());
    REQUIRE(SectionProcessor<mson::Property>::sectionType(markdownAST.children().begin()) == MSONPropertySectionType);
}

TEST_CASE("Parse property", "[mson]")
{
    mson::Property property;
    Report report;
    SectionParserHelper<mson::Property, MSONPropertyParser>::parse(PropertyFixture, MSONObjectSectionType, report, property);
    
    REQUIRE(report.error.code == Error::OK);
    REQUIRE(report.warnings.empty());

    REQUIRE(property.name == "id");
    REQUIRE(property.description.empty());
}
