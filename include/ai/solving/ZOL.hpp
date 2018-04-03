#ifndef AI_SOLVING_ZOL_HPP
#define AI_SOLVING_ZOL_HPP

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <algorithm>

namespace ai {
namespace solving {

class Variable {
private:
    std::string name_;
public:
    Variable(std::string name): name_{std::move(name)} {}

    std::string name() const { return name_; }

}; // class Variable

#define VAR(v) Variable v{#v}

bool operator == (const Variable& a, const Variable& b);

bool operator != (const Variable& a, const Variable& b);

bool operator < (const Variable& a, const Variable& b);

typedef std::map<Variable, bool> ZOLMap;

namespace detail {

struct Node {
    virtual bool evaluate(const ZOLMap& map) const = 0;

    virtual const std::type_info& type_info() const = 0;

    virtual std::unique_ptr<Node> clone() const = 0;

    virtual bool operator == (const Node& other) = 0;

    virtual ~Node() {}
};

} // namespace detail

class Formula {
private:
    std::unique_ptr<detail::Node> node_ = nullptr;

public:
    Formula(const Variable& v);
    
    Formula() {}

    Formula(const Formula& other);

    Formula(Formula&& other);

    Formula& operator = (const Formula& other);
    
    Formula& operator = (Formula&& other);

    bool operator == (const Formula& other);

    bool operator == (Formula&& other);

    bool operator != (const Formula& other);

    bool operator != (Formula&& other);

    bool evaluate(const ZOLMap& map) const;

    std::unique_ptr<detail::Node>& data() { return node_; }

    const detail::Node *data() const { return node_.get(); }

    const std::type_info& data_type_info() const;
};

namespace detail {

} // namespace detail

Formula And(const Formula& a, const Formula& b);

Formula Or(const Formula& a, const Formula& b);

Formula Imply(const Formula& a, const Formula& b);

Formula Equiv(const Formula& a, const Formula& b);

Formula Not(const Formula& a);

void replace_imply_equiv(Formula& f);

void push_down_not(Formula& f);

void CNF(Formula& f);

struct DisjunctForm {
    class InitError: public std::exception {
    public:
        const char *what() const noexcept override {
            return "can't init disjunction form";
        }
    };
    struct Element {
        Variable v;
        bool neg = false;

        bool operator == (const Element& other) const {
            return v == other.v && neg == other.neg;
        }
    };

    std::vector<Element> elements;

    DisjunctForm() = default;

    DisjunctForm(std::initializer_list<Formula> forms);

    bool operator == (const DisjunctForm& other) const;

    bool operator != (const DisjunctForm& other) const;
};

std::vector<DisjunctForm> to_disjunction_list(const Formula& cnf);

template <typename It>
auto search_for_occurence(It first, It last, Variable v, bool neg = false) {
    return std::find_if(first, last, [&v, neg](auto form) {
                return std::find(form.elements.begin(), form.elements.end(), 
                        DisjunctForm::Element{v, neg}) != form.elements.end();
            });
}

std::string to_string(const DisjunctForm& form, Variable v, bool neg = false);

struct NegVar {
    Variable v;
    bool neg = false;

    bool operator == (const NegVar& other) const {
        return v == other.v && neg == other.neg;
    }
};

std::vector<std::vector<DisjunctForm>::const_iterator> 
resolution(const std::vector<DisjunctForm>& forms, const std::vector<NegVar>& list);

} // namespace solving
} // namespace ai

#endif // AI_SOLVING_ZOL_HPP
