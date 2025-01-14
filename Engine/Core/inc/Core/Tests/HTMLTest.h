//
// Created by Monika on 15.08.2024.
//

#ifndef SR_ENGINE_CORE_HTML_TEST_H
#define SR_ENGINE_CORE_HTML_TEST_H

#include <Utils/Resources/ResourceManager.h>
#include <Utils/Web/HTML/HTMLParser.h>
#include <Utils/Web/CSS/CSSParser.h>

namespace SR_CORE_NS::Tests {
    class HTMLTest {
    public:
        static bool Run() {
            const SR_UTILS_NS::Path path = "Tests/editor.html";
            //auto&& pPage = SR_UTILS_NS::Web::HTMLParser::Instance().Parse(path);
            //const std::string text = SR_UTILS_NS::Web::HTMLParser::Instance().DebugPageTostring(pPage);
            //SR_DEBUG_LOG("{}", text);
            return true;
        }
    };

    class CSSTest {
    public:
        static bool Run() {
            const SR_UTILS_NS::Path path = "Tests/editor.css";
            //auto&& pCSS = SR_UTILS_NS::Web::CSSParser::Instance().Parse(path);
            //const std::string text = pCSS->ToString();
            //SR_DEBUG_LOG("{}", text);
            return true;
        }
    };
}

#endif //SR_ENGINE_CORE_HTML_TEST_H
