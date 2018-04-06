#include <gmock/gmock.h>
#include <ai/solving/ZOL.hpp>

namespace ai {
namespace solving {

TEST(ZOL, variable) {
    Variable p("p");
    VAR(q);
    VAR(pro);

    ASSERT_TRUE(p == p);
    ASSERT_TRUE(p != q);
    ASSERT_EQ(q.name(), "q");
    ASSERT_EQ(pro.name(), "pro");
}

TEST(ZOL, Formula) {
    VAR(p);
    VAR(q);
    VAR(r);
    Formula f = p;
    auto g = And(p, q);
    ZOLMap map;

    map[p] = true;
    map[q] = false;
    ASSERT_TRUE(f.evaluate(map));
    ASSERT_FALSE(g.evaluate(map));

    map[p] = false;
    map[q] = true;
    ASSERT_FALSE(f.evaluate(map));
    ASSERT_FALSE(g.evaluate(map));

    map[p] = true;
    map[q] = true;
    ASSERT_TRUE(g.evaluate(map));

    auto h = Or(r, g);

    map[p] = true;
    map[q] = false;
    map[r] = false;
    ASSERT_EQ(h.evaluate(map), map[p] && map[q] || map[r]);

    map[p] = true;
    map[q] = false;
    map[r] = true;
    ASSERT_EQ(h.evaluate(map), map[p] && map[q] || map[r]);

    auto k = Imply(p, q);
    map[p] = false;
    map[q] = false;
    ASSERT_EQ(k.evaluate(map), !map[p] || map[q]);
    map[p] = true;
    map[q] = false;
    ASSERT_EQ(k.evaluate(map), !map[p] || map[q]);

    auto l = Equiv(p, q);
    map[p] = false;
    map[q] = false;
    ASSERT_TRUE(l.evaluate(map));
    map[p] = true;
    map[q] = false;
    ASSERT_FALSE(l.evaluate(map));
    map[p] = true;
    map[q] = true;
    ASSERT_TRUE(l.evaluate(map));

    f = q;
    auto m = Not(f);
    map[q] = true;
    ASSERT_EQ(m.evaluate(map), !map[q]);
    map[q] = false;
    ASSERT_EQ(m.evaluate(map), !map[q]);

    f = p;
    map[q] = true;
    ASSERT_EQ(m.evaluate(map), !map[q]);
    map[q] = false;
    ASSERT_EQ(m.evaluate(map), !map[q]);
}

TEST(ZOL, equals) {
    VAR(a); VAR(b); VAR(c);
    ASSERT_TRUE(Imply(a, b) == Imply(a, b));
    ASSERT_FALSE(Imply(a, b) == Or(a, b));
    ASSERT_FALSE(Or(a, b) == And(a, b));
    ASSERT_FALSE(Imply(a, b) == Imply(a, c));
    ASSERT_TRUE(Or(a, b) == Or(b, a));
    ASSERT_TRUE(And(a, b) == And(b, a));
    // ASSERT_TRUE(Or(a, Or(b, c)) == Or(b, Or(a, c)));
}

TEST(ZOL, replace_imply_equiv_for_imply) {
    VAR(a); VAR(b); VAR(c);
    Formula f = Imply(a, b);

    replace_imply_equiv(f);
    Formula g = Or(Not(a), b);
    Formula h = Or(a, b);

    ASSERT_TRUE(f == g);
    ASSERT_FALSE(f == h);

    Formula m = Imply(a, Imply(b, c));
    Formula n = Or(Not(a), Or(Not(b), c));
    ASSERT_TRUE(m != n);
    replace_imply_equiv(m);
    ASSERT_TRUE(m == n);
    replace_imply_equiv(n);
    ASSERT_TRUE(m == n);
}

TEST(ZOL, replace_imply_equiv_for_equiv) {
    VAR(a); VAR(b); VAR(c);
    auto f = Equiv(a, b);
    auto g = Or(And(a, b), And(Not(a), Not(b)));
    ASSERT_TRUE(f != g);
    replace_imply_equiv(f);
    ASSERT_TRUE(f == g);
}

TEST(ZOL, push_down_not) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Not(And(a, b));
    auto g = Or(Not(a), Not(b));
    push_down_not(f);
    ASSERT_TRUE(f == g);

    auto m = And(Not(And(Not(a), Or(b, c))), d);
    push_down_not(m);
    auto n = And(Or(a, And(Not(b), Not(c))), d);
    ASSERT_TRUE(m == n);

    f = Not(a);
    push_down_not(f);
    ASSERT_TRUE(f == Not(a));
}

void push_down_or(Formula& f);

TEST(ZOL, push_down_or) {
    VAR(a); VAR(b); VAR(c); VAR(d);

    auto f = And(a, b);
    push_down_or(f);
    ASSERT_TRUE(f == And(a, b));

    f = Not(a);
    push_down_or(f);
    ASSERT_TRUE(f == Not(a));

    f = a;
    push_down_or(f);
    ASSERT_TRUE(f == a);

    f = Or(And(a, b), c);
    push_down_or(f);
    ASSERT_TRUE(f == And(Or(a, c), Or(b, c)));

    f = Or(a, And(b, c));
    push_down_or(f);
    ASSERT_TRUE(f == And(Or(a, b), Or(a, c)));
}

TEST(ZOL, CNF) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Not(And(a, b));
    CNF(f);
    auto g = Or(Not(a), Not(b));
    ASSERT_TRUE(f == g);

    f = Or(a, And(b, c));
    CNF(f);
    g = And(Or(a, b), Or(a, c));
    ASSERT_TRUE(f == g);
}

TEST(ZOL, CNF2) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Imply(a, b);
    auto g = Or(Not(a), b);
    CNF(f);
    ASSERT_TRUE(f == g);
}

TEST(ZOL, CNF3) {
    VAR(a); VAR(b); VAR(c); VAR(d);

    auto f = Imply(Imply(a, b), Imply(c, d));
    auto k = Or(Not(c), d);

    CNF(f);
    auto g = And(Or(a, k), Or(Not(b), k));
    ASSERT_TRUE(f == g);
}

TEST(ZOL, DisjunctForm_equals) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    DisjunctForm f = {a, Not(b), c};
    DisjunctForm g = {Not(b), a, c};
    DisjunctForm h = {Not(b), d, c};
    ASSERT_TRUE(f == g);
    ASSERT_TRUE(f != h);
}

TEST(ZOL, DisjunctForm_init) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    DisjunctForm f = {a, b, Not(c)};

    try {
        DisjunctForm g = {a, And(a, b), d};
        FAIL() << "Expected InitError";
    }
    catch (const DisjunctForm::InitError& e) {
    }
    catch (...) {
        FAIL() << "Expected InitError";
    }
}

TEST(ZOL, to_disjunction_list) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Or(a, b);
    auto forms = to_disjunction_list(f);
    ASSERT_EQ(forms.size(), 1);
    ASSERT_TRUE(forms[0] == DisjunctForm({a, b}));

    f = And(Or(a, Or(b, c)), Or(Or(a, d), Not(c)));
    forms = to_disjunction_list(f);
    ASSERT_EQ(forms.size(), 2);
    ASSERT_TRUE(forms[0] == DisjunctForm({a, b, c}));
    ASSERT_TRUE(forms[1] == DisjunctForm({a, d, Not(c)}));
}

TEST(ZOL, search_for_occurence) {
    VAR(a); VAR(b); VAR(c); VAR(d); VAR(e);
    auto f = And(Or(a, Or(b, c)), Or(Or(a, d), Not(c)));
    CNF(f);
    auto forms = to_disjunction_list(f);

    auto it = search_for_occurence(forms.begin(), forms.end(), {a});
    ASSERT_EQ(it, forms.begin());

    it = search_for_occurence(forms.begin(), forms.end(), {a, true});
    ASSERT_EQ(it, forms.end());

    it = search_for_occurence(forms.begin(), forms.end(), {c, true});
    ASSERT_EQ(it, forms.begin() + 1);
}

TEST(ZOL, to_string) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    DisjunctForm f = {a, Not(b), c};

    auto s = to_string(f, {d});
    ASSERT_EQ(s, "");

    s = to_string(f, {c, false});
    ASSERT_EQ(s, "~a & b -> c");
    
    f = {a, b, Not(c), Not(d)};;
    s = to_string(f, {d, true});
    ASSERT_EQ(s, "~a & ~b & c -> ~d");
}

TEST(ZOL, NegVar_lessthan) {
    VAR(a); VAR(b); 
    NegVar v1 = {a, false};
    NegVar v2 = {a, true};
    NegVar v3 = {b, false};
    ASSERT_TRUE(v1 < v2);
    ASSERT_FALSE(v3 < v1);
    ASSERT_TRUE(v1 < v3);
}

Premise premise_of(const DisjunctForm& f, const NegVar& conclusion);

TEST(ZOL, premise_of) {
    VAR(a); VAR(b); VAR(c);
    DisjunctForm f = {a, b, Not(c)};

    Premise compared = {{a, true}, {c, false}};
    ASSERT_EQ(premise_of(f, b), compared);
}

bool in(const NegVar& v, const Premise& p);

TEST(ZOL, negvar_in_premise) {
    VAR(a); VAR(b); VAR(c);
    Premise p = {a, b, {c, true}};
    ASSERT_TRUE(in(a, p));
    ASSERT_FALSE(in(c, p));
}

DisjunctFormIt find_premise(const std::vector<DisjunctForm>& rules, 
        const NegVar& conclusion, Premise& premise);

TEST(ZOL, find_premise) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    DisjunctForm f = {a, Not(b), c};
    DisjunctForm g = {Not(a), b, d};
    DisjunctForm h = {Not(b), c, d};
    std::vector<DisjunctForm> rules = {g, f, h};

    Premise p;
    auto it = find_premise(rules, c, p);
    ASSERT_TRUE(it == rules.begin() + 1);
    Premise compared = {{a, true}, b};
    ASSERT_EQ(p, compared);
}

TraceIt find_rule_contains(TraceIt first, TraceIt last, 
        const NegVar& v, Premise& premise, NegVar& conclusion);

TEST(ZOL, find_rule_contains) {
    VAR(a); VAR(b); VAR(c); VAR(d); VAR(e); VAR(e2);
    DisjunctForm f = {a, Not(b), c};
    DisjunctForm g = {Not(a), d};
    DisjunctForm h = {b, Not(e), Not(e2)};
    std::vector<DisjunctForm> rules = {f, g, h};

    std::vector<Trace> traces = {
        {c, rules.begin()}, 
        {{a, true}, rules.begin() + 1},
        {b, rules.begin() + 2}
    };

    Premise premise;
    NegVar conclusion;
    auto it = find_rule_contains(traces.rbegin(), traces.rend(), e, premise, conclusion);
    ASSERT_EQ(it, traces.rbegin());
}

DisjunctFormIt find_next_rule(DisjunctFormIt first, DisjunctFormIt last,
        const NegVar& conclusion, Premise& premise);

TEST(ZOL, find_next_rule) {
    VAR(a); VAR(b); VAR(c); VAR(d); VAR(e);
    DisjunctForm f = {a, Not(b), c};
    DisjunctForm g = {Not(a), d};
    DisjunctForm h = {b, Not(e), c};
    std::vector<DisjunctForm> rules = {f, g, h};

    Premise premise;
    auto it = find_next_rule(rules.begin() + 1, rules.end(), c, premise);
    ASSERT_EQ(it, rules.begin() + 2);
    Premise compared = {{b, true}, e};
    ASSERT_EQ(premise, compared);
}

TEST(ZOL, reuse_vector_after_move) {
    std::vector<int> vec = {1, 2, 3, 4};
    auto tmp = std::move(vec);
    vec.clear();
    ASSERT_EQ(vec.size(), 0);
    vec.push_back(10);
    ASSERT_EQ(vec[0], 10);
}

TEST(ZOL, resolve) {
    VAR(a); VAR(b); VAR(c); VAR(d); VAR(e);

    std::vector<DisjunctForm> rules = {};
    auto traces = resolve(rules, {{c, false}}, {{c, false}});
    ASSERT_EQ(traces.size(), 0);

    rules = {
        {Not(a), b}
    };

    std::vector<NegVar> assumptions = {a};
    std::vector<NegVar> conclusions = {b};

    traces = resolve(rules, assumptions, conclusions);
    ASSERT_EQ(traces.size(), 1);
    Trace compared{b, rules.begin()};
    ASSERT_TRUE(traces[0] == compared);

    rules = {
        {Not(a), Not(b), c},
        {Not(b), Not(e), c},
    };

    assumptions = {b, e};
    conclusions = {c};
    traces = resolve(rules, assumptions, conclusions);
    ASSERT_EQ(traces.size(), 1);
    compared = {c, rules.begin() + 1};
    ASSERT_TRUE(traces[0] == compared);
}

TEST(ZOL, resolve_fail) {
    VAR(a); VAR(b); VAR(c); VAR(d); VAR(e);
    std::vector<DisjunctForm> rules = {
        {Not(a), Not(b), c},
        {Not(b), Not(e), c},
    };
    std::vector<NegVar> assumptions = {b};
    std::vector<NegVar> conclusions = {c};
    try {
        auto traces = resolve(rules, assumptions, conclusions);
    }
    catch (const char *s) {
        ASSERT_STREQ(s, "Can't find a resolution");
        return;
    }
    catch (...) {
        FAIL() << "resolve should throw";
    }
    FAIL() << "resolve should throw";
}

TEST(ZOL, traces_to_string) {
    VAR(a); VAR(b); VAR(c); VAR(d); VAR(e);
    std::vector<DisjunctForm> rules = {
        {Not(a), Not(b), c},
        {Not(b), Not(e), c},
    };
    std::vector<Trace> traces = {
        {c, rules.begin()},
        {c, rules.begin() + 1},
    };
    auto s = to_string(traces);
    ASSERT_EQ(s, "a & b -> c\nb & e -> c");
}

} // namespace solving
} // namespace ai

