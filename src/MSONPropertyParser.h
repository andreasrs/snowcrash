//
//  MSONPropertyParser.
//  snowcrash
//
//  Created by Zdenek Nemec on 6/23/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_MSONPROPERTYPARSER_H
#define SNOWCRASH_MSONPROPERTYPARSER_H

#include "SectionParser.h"
#include "MSON.h"

/** MSON Property Name */
#define MSON_PROPERTY_NAME "([^`]+)"

namespace mson {
    
    /** Property name matching regex */
    const char* const PropertyRegex = "^[[:blank:]]*[`]?[[:blank:]]*" MSON_PROPERTY_NAME "[[:blank:]]*[`]?[[:blank:]]*";
}

namespace snowcrash {
    
    /**
     *  MSON Property Section Processor
     */
    template<>
    struct SectionProcessor<mson::Property> : public SectionProcessorBase<mson::Property> {
        
        static MarkdownNodeIterator processSignature(const MarkdownNodeIterator& node,
                                                     SectionParserData& pd,
                                                     Report& report,
                                                     mson::Property& out) {
            
            mdp::ByteBuffer signature, remainingContent;
            signature = GetFirstLine(node->text, remainingContent);
            parseSignature(node, pd, signature, report, out);
            
            if (!remainingContent.empty()) {
                out.description += "\n" + remainingContent + "\n";
            }
            
            return ++MarkdownNodeIterator(node);
        }
        
        static SectionType sectionType(const MarkdownNodeIterator& node) {
            if (node->type != mdp::ListItemMarkdownNodeType ||
                node->children().empty())
                return UndefinedSectionType;
            
            mdp::ByteBuffer subject = node->children().front().text;
            if (RegexMatch(subject, mson::PropertyRegex))
                return MSONPropertySectionType;
            
            return UndefinedSectionType;
        }
        
        /**
         *  Parse property signature
         */
        static void parseSignature(const mdp::MarkdownNodeIterator& node,
                                   SectionParserData& pd,
                                   mdp::ByteBuffer& signature,
                                   Report& report,
                                   mson::Property& out) {
            TrimString(signature);
            CaptureGroups captureGroups;
            if (RegexCapture(signature, mson::PropertyRegex, captureGroups) &&
                captureGroups.size() > 2) {
                
                // TODO: Validate signature
                out.name = captureGroups[1];
            }
        }
    };
    
    /** MSON Property Section Parser */
    typedef SectionParser<mson::Property, ListSectionAdapter> MSONPropertyParser;
}

#endif
