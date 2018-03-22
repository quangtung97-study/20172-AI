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
    Formula f = Imply(a, b);
    Formula g = Imply(a, b);
    Formula h = Or(a, b);
    Formula j = And(a, b);
    Formula k = Imply(a, c);

    ASSERT_TRUE(f == g);
    ASSERT_FALSE(f == h);
    ASSERT_FALSE(h == j);
    ASSERT_FALSE(f == k);
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

TEST(ZOL, push_down_or) {
    VAR(a); VAR(b); VAR(c); VAR(d);
    auto f = Not(And(a, b));
    auto g = Or(Not(a), Not(b));
    push_down_or(f);
    ASSERT_TRUE(f == g);

    auto m = Not(And(a, Or(b, c)));
    push_down_or(m);
    auto n = Or(Not(a), And(Not(b), Not(c)));
    ASSERT_TRUE(m == n);
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
}

} // namespace solving
} // namespace ai

