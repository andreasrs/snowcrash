//
//  Signature.h
//  snowcrash
//
//  Created by Zdenek Nemec on 5/14/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_SIGNATURE_H
#define SNOWCRASH_SIGNATURE_H

#include "SourceAnnotation.h"
#include "MarkdownNode.h"
#include "RegexMatch.h"
#include "StringUtility.h"
#include "SectionParserData.h"

namespace snowcrash {
    
    /** 
     *  \brief Query whether a node has keyword-defined signature.
     *  \param node     A Markdown AST node to check.
     *  \return True if the node has a recognized keyword signature, false otherwise
     */
    extern bool HasSectionKeywordSignature(const mdp::MarkdownNodeIterator& node);
    
    /**
     *  \brie   (List-defined) Signature Data
     *
     *  Data as parsed from a keyword-less list-defined signature.
     */
    struct Signature {
        std::string identifier;             /// Signature identifier
        std::string value;                  /// Signature value
        std::vector<std::string> traits;    /// Signature traits
        std::string content;                /// Signature content
    };
    
    /**
     *  \brief  Traits charactestics of a list-defined signature
     *
     *  Traits describe identifier - defined or value/type defined signatures.
     *  If %expectIdentifier is true the signature must start with an identifier.
     *  If %expectIdentifier is false and %allowValue or %maxTraitsCount is set
     *  the signature must start with value, if any, or signature traits.
     */
    struct SignatureTraits {
        
        const std::string contentDelimiter; /// Signature content delimiter e.g. "-", "->", "..."
        bool expectIdentifier;              /// True to expect an identifier in the signature
        bool allowValue;                    /// True to allow value in signature
        size_t maxTraitsCount;              /// Max number of traits, 0 for no traits
        bool allowContent;                  /// True to allow content in signature
        
        SignatureTraits()
        : contentDelimiter("-"), expectIdentifier(true),
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
        
        /**
         *  \brief Parse list-defined line signature
         *
         *  \param  node    Node its text is to be parsed as the signature
         *  \param  t       Signature traits
         *  \param  pd      Parser data
         *  \param  report  Output report buffer
         *  \param  remainingContent    Remaining additional lines of node text buffer
         *  \param  s       Parsed signature
         */
        static void parseSignature(const mdp::MarkdownNodeIterator& node,
                                   const SignatureTraits& t,
                                   const SectionParserData& pd,
                                   Report& report,
                                   mdp::ByteBuffer& remainingContent,
                                   Signature& s) {
            
            mdp::ByteBuffer work = GetFirstLine(node->text, remainingContent);
            
            if (t.expectIdentifier) {
                s.identifier = RegexCaptureFirst(work, UnescapedIdentifier);

                work = work.substr(s.identifier.size());
                TrimStringStart(work);
                
                TrimString(s.identifier);
                
                if (s.identifier.empty()) {
                    MissingIdentifierWarning(node, work, t, pd, report);
                }
            }
            
            if (t.allowValue) {
                if (!t.expectIdentifier || work.find_first_of(':') == 0) {
                    
                    if (t.expectIdentifier)
                        work = work.substr(1);
                    
                    s.value = RegexCaptureFirst(work, UnescapedValue);

                    work = work.substr(s.value.size());
                    TrimStringStart(work);

                    TrimString(s.value);

                    if (t.expectIdentifier && s.value.empty()) {
                        MissingValueWarning(node, work, t, pd, report);
                    }
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
            
            if (!t.expectIdentifier && s.value.empty() && s.traits.empty()) {
                MissingElementDefinitionWarning(node, work, t, pd, report);
            }

            if (t.allowContent) {
                if (work.find(t.contentDelimiter) == 0) {
                    s.content = work.substr(t.contentDelimiter.size());
                    TrimStringStart(s.content);
                    work.clear();
                }
            }
            
            if (!work.empty()) {
                UnexpectedContentWarning(node, work, t, pd, report);
            }
        }
        
        /** \brief Compose missing element definition warning */
        static void MissingElementDefinitionWarning(const mdp::MarkdownNodeIterator& node,
                                                    const mdp::ByteBuffer& work,
                                                    const SignatureTraits& t,
                                                    const SectionParserData& pd,
                                                    Report& report) {
            
            // WARN: Missing element definition */
            mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);
            report.warnings.push_back(Warning("missing element value or trait(s), 'expected '<value> (<traits>)'",
                                              SignatureSyntaxWarning,
                                              sourceMap));
        }
        
        /** \brief Compose missing identifier warning */
        static void MissingIdentifierWarning(const mdp::MarkdownNodeIterator& node,
                                             const mdp::ByteBuffer& work,
                                             const SignatureTraits& t,
                                             const SectionParserData& pd,
                                             Report& report) {
            
            // WARN: Missing identifier */
            mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);
            report.warnings.push_back(Warning("missing expected identifier",
                                              SignatureSyntaxWarning,
                                              sourceMap));
        }

        /** \brief Compose missing value warning */
        static void MissingValueWarning(const mdp::MarkdownNodeIterator& node,
                                        const mdp::ByteBuffer& work,
                                        const SignatureTraits& t,
                                        const SectionParserData& pd,
                                        Report& report) {
            
            // WARN: Missing value after ':` */
            mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);
            report.warnings.push_back(Warning("missing value after ':', expected '<identifier>: <value>'",
                                              SignatureSyntaxWarning,
                                              sourceMap));
        }
        
        
        
        /** \brief Compose unexpected content warning */
        static void UnexpectedContentWarning(const mdp::MarkdownNodeIterator& node,
                                             const mdp::ByteBuffer& work,
                                             const SignatureTraits& t,
                                             const SectionParserData& pd,
                                             Report& report) {
            
            // WARN: Unexpected content
            std::stringstream ss;
            ss << "ignoring '" << work << "', expected '";
            if (t.expectIdentifier) {
                ss << "<identifier>";
                
                if (t.allowValue)
                    ss << ":";
            }
            
            if (t.allowValue) {
                if (t.expectIdentifier)
                    ss << " ";
                
                ss << "<value>";
            }
            
            if (t.maxTraitsCount) {
                ss << " (";
                for (size_t i = 0; i < t.maxTraitsCount; ++i) {
                    ss << "<t" << i + 1 << ">";
                    if (i < t.maxTraitsCount - 1)
                        ss << ", ";
                }
                
                ss << ")";
            }
            
            if (t.allowContent) {
                ss << " " << t.contentDelimiter << " <content>";
            }
            
            ss << "'";
            
            mdp::CharactersRangeSet sourceMap = mdp::BytesRangeSetToCharactersRangeSet(node->sourceMap, pd.sourceData);
            report.warnings.push_back(Warning(ss.str(),
                                              SignatureSyntaxWarning,
                                              sourceMap));
        }
    };
}

#endif
