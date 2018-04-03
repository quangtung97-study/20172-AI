#include <ai/solving/ZOL.hpp>

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
    Formula child_;

    NotNode(const Formula& f): child_{f} {}

    bool evaluate(const ZOLMap& map) const override {
        return !child_.evaluate(map);
    }

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<NotNode>(child_);
    }

    bool operator == (const Node& other) override {
        if (other.type_info() != typeid(*this)) 
            return false;
        return child_ == static_cast<const NotNode&>(other).child_;
    }
};

struct BinaryNode: public Node {
    Formula left_, right_;

    virtual bool binary_connective(bool a, bool b) const = 0;

    BinaryNode(const Formula& a, const Formula& b)
        : left_{a}, right_{b} {}

    BinaryNode(Formula&& a, Formula&& b)
        : left_{std::move(a)}, right_{std::move(b)} {}

    bool evaluate(const ZOLMap& map) const override {
        bool a = left_.evaluate(map);
        bool b = right_.evaluate(map);
        return binary_connective(a, b);
    }

    bool operator == (const Node& other) override;
};

const Formula& left_formula(const Node& other) {
    return static_cast<const BinaryNode&>(other).left_;
}

const Formula& right_formula(const Node& other) {
    return static_cast<const BinaryNode&>(other).right_;
}

bool BinaryNode::operator == (const Node& other) {
    if (other.type_info() != typeid(*this)) 
        return false;
    return left_ == left_formula(other) && right_ == right_formula(other);
}

struct AndNode final: public BinaryNode {
    bool binary_connective(bool a, bool b) const override {
        return a && b;
    }

    AndNode(const Formula& a, const Formula& b): BinaryNode{a, b} {}

    const std::type_info& type_info() const override {
        return typeid(*this);
    }

    std::unique_ptr<Node> clone() const override { 
        return std::make_unique<AndNode>(left_, right_);
    }

    bool operator == (const Node& other) override {
        if (other.type_info() != typeid(*this)) 
            return false;
        return (left_ == left_formula(other) && right_ == right_formula(other))
            || (right_ == left_formula(other) && left_ == right_formula(other));
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
        return std::make_unique<OrNode>(left_, right_);
    }

    bool operator == (const Node& other) override {
        if (other.type_info() != typeid(*this)) 
            return false;
        return (left_ == left_formula(other) && right_ == right_formula(other))
            || (right_ == left_formula(other) && left_ == right_formula(other));
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
        return std::make_unique<ImplyNode>(left_, right_);
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
        return std::make_unique<EquivNode>(left_, right_);
    }
};

} // namespace detail

const std::type_info& ATOMIC_NODE = typeid(detail::AtomicNode);
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

// Helper Functions
static Formula& child_formula(Formula& form) {
    auto& node = static_cast<detail::NotNode&>(*form.data());
    return node.child_;
}

static const Formula& child_formula(const Formula& form) {
    auto& node = static_cast<const detail::NotNode&>(*form.data());
    return node.child_;
}

static Formula& left_formula(Formula& form) {
    auto& node = static_cast<detail::BinaryNode&>(*form.data());
    return node.left_;
}

static const Formula& left_formula(const Formula& f) {
    auto& node = static_cast<const detail::BinaryNode&>(*f.data());
    return node.left_;
}

static Formula& right_formula(Formula& form) {
    auto& node = static_cast<detail::BinaryNode&>(*form.data());
    return node.right_;
}

static const Formula& right_formula(const Formula& form) {
    auto& node = static_cast<const detail::BinaryNode&>(*form.data());
    return node.right_;
}


void replace_imply_equiv(Formula& f) {
    if (f.data_type_info() == IMPLY_NODE) {
        auto& a = left_formula(f);
        auto& b = right_formula(f);

        replace_imply_equiv(a);
        replace_imply_equiv(b);

        f = Or(Not(a), b);
    }
    else if (f.data_type_info() == EQUIV_NODE) {
        auto& a = left_formula(f);
        auto& b = right_formula(f);

        replace_imply_equiv(a);
        replace_imply_equiv(b);

        f = Or(And(a, b), And(Not(a), Not(b)));
    }
}

void push_down_not(Formula& f) {
    if (f.data_type_info() == AND_NODE || f.data_type_info() == OR_NODE) {
        auto& a = left_formula(f);
        auto& b = right_formula(f);

        push_down_not(a);
        push_down_not(b);
    }
    else if (f.data_type_info() == NOT_NODE) {
        auto& child = child_formula(f);
        if (child.data_type_info() == AND_NODE) {
            auto a = Not(left_formula(child));
            auto b = Not(right_formula(child));

            push_down_not(a);
            push_down_not(b);

            f = Or(a, b);
        }
        else if (child.data_type_info() == OR_NODE) {
            auto a = Not(left_formula(child));
            auto b = Not(right_formula(child));

            push_down_not(a);
            push_down_not(b);

            f = And(a, b);
        }
        else if (child.data_type_info() == NOT_NODE) {
            auto a = child_formula(child);
            push_down_not(a);
            f = a;
        }
    }
}

void push_down_or(Formula& f) {
    if (f.data_type_info() == AND_NODE) {
        push_down_or(left_formula(f));
        push_down_or(right_formula(f));
    }
    else if (f.data_type_info() == OR_NODE) {
        if (left_formula(f).data_type_info() == AND_NODE) {
            auto& left = left_formula(f);

            auto a = left_formula(left);
            auto b = right_formula(left);
            auto c = right_formula(f);

            f = And(Or(a, c), Or(b, c));
        }

        if (right_formula(f).data_type_info() == AND_NODE) {
            auto& right = right_formula(f);

            auto a = left_formula(f);
            auto b = left_formula(right);
            auto c = right_formula(right);

            f = And(Or(a, b), Or(a, c));
        }

        push_down_or(left_formula(f));
        push_down_or(right_formula(f));
    }
}

void CNF(Formula& f) {
    replace_imply_equiv(f);
    push_down_not(f);
    push_down_or(f);
}

DisjunctForm::DisjunctForm(std::initializer_list<Formula> forms) {
    for (auto& f: forms) {
        if (f.data_type_info() == ATOMIC_NODE) {
            auto& node = static_cast<const detail::AtomicNode&>(*f.data());
            elements.push_back({node.var_, false});
        }
        else if (f.data_type_info() == NOT_NODE 
                && child_formula(f).data_type_info() == ATOMIC_NODE) {
            auto& node = static_cast<const detail::AtomicNode&>(
                    *child_formula(f).data());
            elements.push_back({node.var_, true});
        }
        else {
            throw InitError();
        }
    }
}

bool DisjunctForm::operator == (const DisjunctForm& other) const {
    return std::all_of(elements.begin(), elements.end(), 
            [&other](auto& f) {
                auto it = std::find(other.elements.begin(), other.elements.end(), f);
                return it != other.elements.end();
            });
}

bool DisjunctForm::operator != (const DisjunctForm& other) const {
    return !((*this) == other);
}

void collect_variables(const Formula& f, DisjunctForm& result) {
    if (f.data_type_info() == NOT_NODE) {
        auto& node = static_cast<const detail::NotNode&>(*f.data());
        auto& child = static_cast<const detail::AtomicNode&>(*node.child_.data());
        result.elements.push_back({child.var_, true});
    }
    else if (f.data_type_info() == ATOMIC_NODE) {
        auto& node = static_cast<const detail::AtomicNode&>(*f.data());
        result.elements.push_back({node.var_, false});
    }
    else if (f.data_type_info() == OR_NODE) {
        collect_variables(left_formula(f), result);
        collect_variables(right_formula(f), result);
    }
}

void reach_disjunctions(const Formula& f, std::vector<DisjunctForm>& vec) {
    if (f.data_type_info() == AND_NODE) {
        reach_disjunctions(left_formula(f), vec);
        reach_disjunctions(right_formula(f), vec);
    }
    else if (f.data_type_info() == OR_NODE) {
        DisjunctForm form;
        collect_variables(f, form);
        vec.push_back(form);
    }
}

std::vector<DisjunctForm> to_disjunction_list(const Formula& cnf) {
    std::vector<DisjunctForm> result;
    reach_disjunctions(cnf, result);
    return result;
}

std::string to_string(const DisjunctForm& form, Variable v, bool neg) {
    auto find_it = std::find(form.elements.begin(), form.elements.end(), 
            DisjunctForm::Element{v, neg});
    if (find_it == form.elements.end())
        return "";

    std::string result;
    for (auto it = form.elements.begin(); it != form.elements.end(); ++it) {
        if (it == find_it)
            continue;

        if (result.size() >= 1)
            result += " & ";

        if (!it->neg)
            result.push_back('~');
        result += it->v.name();
    }

    result += " -> ";
    if (neg)
        result.push_back('~');
    result += v.name();
    return result;
}

std::vector<std::vector<DisjunctForm>::const_iterator> 
resolution(const std::vector<DisjunctForm>& forms, const std::vector<NegVar>& list) {
    std::vector<std::vector<DisjunctForm>::const_iterator> result;
    return result;
}

} // namespace solving
} // namespace ai

