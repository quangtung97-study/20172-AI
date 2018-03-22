#include <ai/solving/ZOL.hpp>
#include <iostream>

namespace ai {
namespace solving {

bool operator == (const Variable& a, const Variable& b) {
    return a.name() == b.name();
}

bool operator != (const Variable& a, const Variable& b) {
    return a.name() != b.name();
}

bool operator < (const Variable& a, const Variable& b) {
    return a.name() < b.name();
}

namespace detail {

struct AtomicNode final: public Node {
    Variable var_;

    AtomicNode(const Variable& v): var_{v} {}

    bool evaluate(const ZOLMap& map) const override {
        auto it = map.find(var_);
        if (it == map.end())
            return false;
        return it->second;
    }

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<AtomicNode>(var_);
    }

    bool operator == (const Node& other) override {
        if (other.type_info() != typeid(*this)) 
            return false;
        return var_ == static_cast<const AtomicNode&>(other).var_;
    }
};

struct NotNode final: public Node {
    Formula form_;

    NotNode(const Formula& form): form_{form} {}

    bool evaluate(const ZOLMap& map) const override {
        return !form_.evaluate(map);
    }

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<NotNode>(form_);
    }

    bool operator == (const Node& other) override {
        if (other.type_info() != typeid(*this)) 
            return false;
        return form_ == static_cast<const NotNode&>(other).form_;
    }
};

struct BinaryNode: public Node {
    Formula forms_[2];

    virtual bool binary_connective(bool a, bool b) const = 0;

    BinaryNode(const Formula& a, const Formula& b)
        : forms_{a, b} {}

    BinaryNode(Formula&& a, Formula&& b)
        : forms_{std::move(a), std::move(b)} {}

    bool evaluate(const ZOLMap& map) const override {
        bool a = forms_[0].evaluate(map);
        bool b = forms_[1].evaluate(map);
        return binary_connective(a, b);
    }

    bool operator == (const Node& other) override {
        if (other.type_info() != typeid(*this)) 
            return false;
        return forms_[0] == static_cast<const BinaryNode&>(other).forms_[0]
            && forms_[1] == static_cast<const BinaryNode&>(other).forms_[1];
    }
};

struct AndNode final: public BinaryNode {
    bool binary_connective(bool a, bool b) const override {
        return a && b;
    }

    AndNode(const Formula& a, const Formula& b): BinaryNode{a, b} {}

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<AndNode>(forms_[0], forms_[1]);
    }
};

struct OrNode final: public BinaryNode {
    bool binary_connective(bool a, bool b) const override {
        return a || b;
    }

    OrNode(const Formula& a, const Formula& b): BinaryNode{a, b} {}

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<OrNode>(forms_[0], forms_[1]);
    }
};

struct ImplyNode final: public BinaryNode {
    bool binary_connective(bool a, bool b) const override {
        return !a || b;
    }

    ImplyNode(const Formula& a, const Formula& b): BinaryNode{a, b} {}

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<ImplyNode>(forms_[0], forms_[1]);
    }
};

struct EquivNode final: public BinaryNode {
    bool binary_connective(bool a, bool b) const override {
        return (a && b) || (!a && !b);
    }

    EquivNode(const Formula& a, const Formula& b): BinaryNode{a, b} {}

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<EquivNode>(forms_[0], forms_[1]);
    }
};

} // namespace detail

const std::type_info& OR_NODE = typeid(detail::OrNode);
const std::type_info& AND_NODE = typeid(detail::AndNode);
const std::type_info& NOT_NODE = typeid(detail::NotNode);
const std::type_info& IMPLY_NODE = typeid(detail::ImplyNode);
const std::type_info& EQUIV_NODE = typeid(detail::EquivNode);

Formula::Formula(const Variable& v) {
    node_ = std::make_unique<detail::AtomicNode>(v);
}

bool Formula::evaluate(const ZOLMap& map) const {
    return node_->evaluate(map);
}

Formula::Formula(const Formula& other) {
    node_ = other.node_->clone();
}

Formula::Formula(Formula&& other) {
    node_ = std::move(other.node_);
}

Formula& Formula::operator = (const Formula& other) {
    node_ = other.node_->clone();
    return *this;
}

Formula& Formula::operator = (Formula&& other) {
    node_ = std::move(other.node_);
    return *this;
}

bool Formula::operator == (const Formula& other) {
    return *data() == *other.data();
}

bool Formula::operator == (Formula&& other) {
    return (*this) == other;
}

bool Formula::operator != (const Formula& other) {
    return !((*this) == other);
}

bool Formula::operator != (Formula&& other) {
    return !((*this) == other);
}

const std::type_info& Formula::data_type_info() const{
    return node_->type_info();
}

//----------------------------------------------------

Formula And(const Formula& a, const Formula& b) {
    Formula f;
    f.data() = std::make_unique<detail::AndNode>(a, b);
    return f;
}

Formula Or(const Formula& a, const Formula& b) {
    Formula f;
    f.data() = std::make_unique<detail::OrNode>(a, b);
    return f;
}

Formula Imply(const Formula& a, const Formula& b) {
    Formula f;
    f.data() = std::make_unique<detail::ImplyNode>(a, b);
    return f;
}

Formula Equiv(const Formula& a, const Formula& b) {
    Formula f;
    f.data() = std::make_unique<detail::EquivNode>(a, b);
    return f;
}

Formula Not(const Formula& a) {
    Formula f;
    f.data() = std::make_unique<detail::NotNode>(a);
    return f;
}

void replace_imply_equiv(Formula& f) {
    if (f.data_type_info() == IMPLY_NODE) {
        auto old = std::move(f.data());
        auto& old_node = static_cast<detail::ImplyNode&>(*old);

        auto& a = old_node.forms_[0];
        auto& b = old_node.forms_[1];

        replace_imply_equiv(a);
        replace_imply_equiv(b);

        f.data() = std::make_unique<detail::OrNode>(Not(a), b);
    }
    else if (f.data_type_info() == EQUIV_NODE) {
        auto old = std::move(f.data());
        auto& old_node = static_cast<detail::EquivNode&>(*old);

        auto& a = old_node.forms_[0];
        auto& b = old_node.forms_[1];

        replace_imply_equiv(a);
        replace_imply_equiv(b);

        f.data() = std::make_unique<detail::OrNode>(
                And(a, b), And(Not(a), Not(b)));
    }
}

void push_down_or(Formula& f) {
    if (f.data_type_info() == NOT_NODE) {
        auto& old_node = static_cast<detail::NotNode&>(*f.data());

        if (old_node.form_.data_type_info() == AND_NODE) {
            auto inside_old = std::move(old_node.form_.data());
            auto& inside_old_node = static_cast<detail::AndNode&>(*inside_old);

            auto a = Not(inside_old_node.forms_[0]);
            auto b = Not(inside_old_node.forms_[1]);

            push_down_or(a);
            push_down_or(b);

            f.data() = std::make_unique<detail::OrNode>(a, b);
            return;
        }
        else if (old_node.form_.data_type_info() == OR_NODE) {
            auto inside_old = std::move(old_node.form_.data());
            auto& inside_old_node = static_cast<detail::OrNode&>(*inside_old);

            auto a = Not(inside_old_node.forms_[0]);
            auto b = Not(inside_old_node.forms_[1]);

            push_down_or(a);
            push_down_or(b);

            f.data() = std::make_unique<detail::AndNode>(a, b);
            return;
        }
    }
}

void CNF(Formula& f) {
    replace_imply_equiv(f);
    push_down_or(f);
}

} // namespace solving
} // namespace ai

