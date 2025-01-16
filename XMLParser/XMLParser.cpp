#include <algorithm>
#include <chrono>
#include <memory>
#include <print>
#include <queue>
#include <ranges>
#include <stack>
#include <string>
#include <cassert>

enum CloseType : uint8_t {
    UNKNOW,                   // 暂时还不知道
    Multi_Close,              // <xxx></xxx>
    Multi_CDATA,              // <![CDATA[ ]]>, NOT USED
    Single_Close,             // < xxx />
    Single_Close_Exclamation, // <! >
    Single_Close_Question,    // <? ?>
};

class XmlNode_Share : public std::enable_shared_from_this<XmlNode_Share> {
    using Attr = std::pair<std::string, std::string>;
    std::weak_ptr<XmlNode_Share> parent_layer_;

public:
    size_t                                       depth_layer = 0;
    std::string                                  label;     // 当前层的标签数据
    std::vector<Attr>                            attrs;     // 当前层的属性
    std::vector<std::shared_ptr<XmlNode_Share> > children_; // 当前层的子节点


    /**
     * use only for root
     * @param label 
     */
    explicit XmlNode_Share(std::string label) : label(std::move(label)) {}

    explicit XmlNode_Share(std::shared_ptr<XmlNode_Share> parent, std::string label, const size_t depth_layer)
        : parent_layer_(parent), depth_layer(depth_layer + 1), label(std::move(label)) {}

    void pushChildren(const std::string &label) {
        auto children = std::make_shared<XmlNode_Share>(shared_from_this(), label, depth_layer);
        children_.emplace_back(children);
    }

    std::shared_ptr<XmlNode_Share> enterNextLayer(const std::string &label) {
        pushChildren(label);
        return *children_.rbegin();
    }

    std::shared_ptr<XmlNode_Share> leaveLayer() {
        return parent_layer_.lock();
    }
};

std::shared_ptr<XmlNode_Share> xmlParser(std::string &text) {
    auto current_iter = text.begin();
    auto next_iter    = text.begin();

    auto begin_iter = text.begin();
    auto end_iter   = text.end();

    auto safe_get_next = [&end_iter](auto &curr, int offset = 1) {
        auto next = std::ranges::next(curr, offset);
        if (next == end_iter) {
            throw std::runtime_error("unexpected end");
        }
        return next;
    };

    const auto startTime = std::chrono::system_clock::now();

    std::stack<CloseType> end_mode_stack;

    const auto xml_node_root = std::make_shared<XmlNode_Share>("root");
    auto       xml_node_curr = xml_node_root;

    try {
        while (current_iter != end_iter) {
            CloseType closeType = UNKNOW;

            if (*current_iter == '<') {
                next_iter = safe_get_next(current_iter);
                if (*next_iter == '!') {
                    // 直接解析 CDATA 数据
                    if (std::string_view cdata_start_prefix = "<![CDATA[", cdata_end_prefix = "]]>";
                        std::ranges::equal(current_iter, std::ranges::next(current_iter, cdata_start_prefix.size()), cdata_start_prefix.begin(), cdata_start_prefix.end())) {
                        // ignore CDATA prefix
                        std::ranges::advance(current_iter, cdata_start_prefix.size());

                        auto cdata_end = std::ranges::search(current_iter, end_iter, cdata_end_prefix.begin(), cdata_end_prefix.end());

                        xml_node_curr->pushChildren({ current_iter, cdata_end.begin() });
                        std::ranges::advance(current_iter, cdata_end.end() - current_iter);
                        continue;
                    } else {
                        closeType = Single_Close_Exclamation;
                    }
                } else if (*next_iter == '?') {
                    closeType = Single_Close_Question;
                } else if (*next_iter == '/') {
                    // STACK POP
                    xml_node_curr        = xml_node_curr->leaveLayer();
                    end_mode_stack.top() = Multi_Close;
                    end_mode_stack.pop();

                    std::ranges::advance(current_iter, std::ranges::find(current_iter, end_iter, '>') + 1);
                    continue;
                }
                begin_iter = current_iter;
                end_mode_stack.emplace(closeType);
            }

            if (*current_iter == '>') {
                if (auto &topMode = end_mode_stack.top();
                    topMode == Single_Close_Exclamation) {
                    xml_node_curr->pushChildren({ begin_iter, current_iter + 1 });
                    end_mode_stack.pop();
                    // MODE END
                } else if (topMode == Single_Close_Question) {
                    xml_node_curr->pushChildren({ begin_iter + 2, current_iter - 1 });
                    end_mode_stack.pop();
                    // MODE END
                } else {
                    // 绝对是 Multi_Close
                    // STACK PUSH
                    xml_node_curr = xml_node_curr->enterNextLayer({ begin_iter, current_iter + 1 });
                }
            }

            if (*current_iter == '/' && *safe_get_next(current_iter) == '>') {
                if (auto topMode = end_mode_stack.top();
                    topMode == UNKNOW) {
                    assert(topMode == UNKNOW && "TYPE SHOULD BE UNKNOW(TYPE2)");
                    topMode = Single_Close;
                    end_mode_stack.pop();
                    xml_node_curr->pushChildren({ begin_iter, current_iter + 2 });
                    std::ranges::advance(current_iter, 1);
                    // MODE END
                }
            }

            std::ranges::advance(current_iter, 1);
        }
    } catch (std::exception &Exception) {
        std::println("parse error {}", Exception.what());
    }

    std::println("stack remain: {}, parse time: {}ms", end_mode_stack.size(),
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count());

    return xml_node_root;
}

void traversal_bfs(const std::shared_ptr<XmlNode_Share> &root) {
    std::queue<std::shared_ptr<XmlNode_Share> > s;

    s.push(root);

    int layer = 0;
    while (!s.empty()) {
        std::shared_ptr<XmlNode_Share> current = s.front();
        s.pop();

        std::println("layer: {}, {}", layer, current->label);

        bool inserted = false;
        for (auto it = current->children_.begin(); it != current->children_.end(); ++it) {
            if (!inserted) {
                ++layer;
                inserted = !inserted;
            }


            s.push(*it);
        }
    }
}

void traversal_dfs(const std::shared_ptr<XmlNode_Share> &root) {
    std::stack<std::shared_ptr<XmlNode_Share> > s;

    s.push(root);
    while (!s.empty()) {
        std::shared_ptr<XmlNode_Share> current = s.top();
        s.pop();

        std::println("{} -> {}", current->depth_layer, current->label.substr(0, 10));

        for (auto it = current->children_.rbegin(); it != current->children_.rend(); ++it) {
            s.push(*it);
        }
    }
}

int main(int argc, char *argv[]) {
    // disable output buffering
    setvbuf(stdout, nullptr, _IONBF, 0);

    // setting global encoding utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));

    std::string content = "<test />";

    // parser(content);
    auto ptr = xmlParser(content);

    traversal_dfs(ptr);
}
