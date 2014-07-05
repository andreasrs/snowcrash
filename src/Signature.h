//
//  Signature.h
//  snowcrash
//
//  Created by Zdenek Nemec on 5/14/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_SIGNATURE_H
#define SNOWCRASH_SIGNATURE_H

#include "MarkdownNode.h"
#include "RegexMatch.h"
#include "StringUtility.h"

namespace snowcrash {
    
    /** 
     *  \brief Query whether a node has keyword-defined signature.
     *  \param node     A Markdown AST node to check.
     *  \return True if the node has a recognized keyword signature, false otherwise
     */
    extern bool HasSectionKeywordSignature(const mdp::MarkdownNodeIterator& node);
    
    /**
     *  \brief (List-defined) Signature Data
     *
     *  Data as parsed from a keyword-less list-defined signature.
     */
    struct Signature {
        std::string identifier;             /// < Signature identifier
        std::string value;                  /// < Signature value
        std::vector<std::string> traits;    /// < Signature traits
        std::string content;                /// < Signature content
    };
    
    /**
     *  Trait charactestics of a list-defined signature
     */
    struct SignatureTraits {
        
        const std::string contentDelimiter; /// < Signature content delimiter e.g. "-", "->", "..."
        bool allowIdentifier;               /// < True to allow identifier in signature
        bool allowValue;                    /// < True to allow value in signature
        size_t maxTraitsCount;              /// < Max number of traits, 0 for no traits
        bool allowContent;                  /// < True to allow content in signature
        
        SignatureTraits()
        : contentDelimiter("-"), allowIdentifier(true),
        allowValue(true), maxTraitsCount(2), allowContent(true) {}
        
        /** \return Default signature traits */
        static SignatureTraits& get() {
            static SignatureTraits s;
            return s;
        }
    };
    
    /**
     *  List-defined, line signature parser
     */
    struct SignatureParser {
        
        static const char* const UnescapedIdentifier;
        static const char* const UnescapedValue;
        static const char* const CSVTraits;
        
        static void parseSignature(const std::string& subject, const SignatureTraits& t, Signature& s) {
            
            std::string work = subject;
            
            if (t.allowIdentifier) {
                s.identifier = RegexCaptureFirst(work, UnescapedIdentifier);
                TrimString(s.identifier);
                work = work.substr(s.identifier.size());
                TrimStringStart(work);
            }
            
            if (t.allowValue) {
                if (!t.allowIdentifier || work.find_first_of(':') == 0) {
                    
                    if (t.allowIdentifier)
                        work = work.substr(1);
                    
                    s.value = RegexCaptureFirst(work, UnescapedValue);
                    work = work.substr(s.value.size());
                    TrimString(s.value);
                }
            }
            
            if (t.maxTraitsCount) {
                if (work.find_first_of('(') == 0) {
                    CaptureGroups groups;
                    if (RegexCapture(work, CSVTraits, groups, 2) && groups.size() == 2) {
                        Split(groups[1], ',', s.traits);
                        for (std::vector<std::string>::iterator it = s.traits.begin();
                             it != s.traits.end();
                             ++it) {
                            TrimString(*it);
                        }
                        
                        work = work.substr(groups[0].size());
                        TrimStringStart(work);
                    }
                }
            }

            if (t.allowContent) {
                if (work.find(t.contentDelimiter) == 0) {
                    s.content = work.substr(t.contentDelimiter.size());
                    TrimStringStart(s.content);
                    work.clear();
                }
            }
            
            if (!work.empty()) {
                // TODO: WARN
                int i = 42;
                ++i;
            }
        }
        
    };
}

#endif
