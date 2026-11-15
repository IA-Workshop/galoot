// Forms of the rigid(3) transforms by a motor, as straight-line kernels over
// any scalar T, for timing against each other. Written by gen_forms.py from
// the programs of the forms; do not edit.
//
// Layouts: a point on e1 e2 e3 e4, a line on e41 e42 e43 e23 e31 e12, a
// plane on e423 e431 e412 e321, and a motor on e41 e42 e43 e1234 e23 e31
// e12 1, with e41 = -e14, e31 = -e13, e321 = -e123.
//
// a  The terms of Q v x v antireverse(Q) that reach x's blades, two stages,
//    as the library's transform computes them, with each sign carried into
//    a subtraction.
// b  The collected form: the transform is linear in x, y = M(Q) x, and each
//    entry of M is a sum of products of two motor coefficients. setup makes
//    each distinct product once and each entry once, applying a factor 2
//    common to an entry's products once to the entry's sum; apply is M x.
//    It equals a for every Q.
// c  A form that assumes the motor is unitized and satisfies its geometric
//    constraint, with v = (Qvx, Qvy, Qvz), s = Qvw, m = (Qmx, Qmy, Qmz),
//    t = Qmw, |v|^2 + s^2 = 1 and v . m + s t = 0, and a = v x u:
//      point  p' = p + 2 (s c + v x c - w t v), c = v x p + w m, w' = w
//      plane  n' = n + 2 (s a + v x a), g_w' = g_w - n' . T
//      line   d' = d + 2 (s a + v x a), k' = R k + T x d'
//    with T = 2 (s m - t v + v x m) and R the rotation of the first line.
//    setup doubles v and s once, and makes T where it is read. It equals a
//    only on such motors.
//
// Each form is a struct of two functions: setup(q) holds the work that
// reads only the motor, apply(m, q, x) the rest, so a batch of objects under
// one motor calls setup once.
// clang-format off
#ifndef SPIKE_E_BENCH_FORMS_HPP
#define SPIKE_E_BENCH_FORMS_HPP

#include <array>

namespace forms {


template <class T>
struct transform_point_a {
  static constexpr char name[] = "a";

  [[gnu::always_inline]] static constexpr std::array<T, 0>
  setup(const std::array<T, 8>& /*q*/) {
    return {};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  apply(const std::array<T, 0>& /*m*/, const std::array<T, 8>& q,
        const std::array<T, 4>& x) {
    const T t12 = q[0] * x[0];
    const T t13 = q[0] * x[1];
    const T t14 = q[0] * x[2];
    const T t15 = q[0] * x[3];
    const T t16 = q[1] * x[0];
    const T t17 = q[1] * x[1];
    const T t18 = q[1] * x[2];
    const T t19 = q[1] * x[3];
    const T t20 = q[2] * x[0];
    const T t21 = q[2] * x[1];
    const T t22 = q[2] * x[2];
    const T t23 = q[2] * x[3];
    const T t24 = q[3] * x[0];
    const T t25 = q[3] * x[1];
    const T t26 = q[3] * x[2];
    const T t27 = q[3] * x[3];
    const T t28 = q[4] * x[3];
    const T t29 = q[5] * x[3];
    const T t30 = q[6] * x[3];
    const T t31 = q[7] * x[3];
    const T t32 = t12 + t17;
    const T t33 = t22 + t32;
    const T t34 = t33 - t31;
    const T t35 = t13 - t16;
    const T t36 = t26 + t35;
    const T t37 = t30 + t36;
    const T t38 = t20 - t14;
    const T t39 = t25 + t38;
    const T t40 = t29 + t39;
    const T t41 = t18 - t21;
    const T t42 = t24 + t41;
    const T t43 = t28 + t42;
    const T t44 = q[1] * t43;
    const T t45 = q[2] * t43;
    const T t46 = q[3] * t43;
    const T t47 = q[0] * t40;
    const T t48 = q[2] * t40;
    const T t49 = q[3] * t40;
    const T t50 = q[0] * t37;
    const T t51 = q[1] * t37;
    const T t52 = q[3] * t37;
    const T t53 = q[3] * t27;
    const T t54 = q[4] * t27;
    const T t55 = q[5] * t27;
    const T t56 = q[6] * t27;
    const T t57 = q[0] * t34;
    const T t58 = q[1] * t34;
    const T t59 = q[2] * t34;
    const T t60 = q[2] * t23;
    const T t61 = q[4] * t23;
    const T t62 = q[5] * t23;
    const T t63 = q[7] * t23;
    const T t64 = q[1] * t19;
    const T t65 = q[4] * t19;
    const T t66 = q[6] * t19;
    const T t67 = q[7] * t19;
    const T t68 = q[0] * t15;
    const T t69 = q[5] * t15;
    const T t70 = q[6] * t15;
    const T t71 = q[7] * t15;
    const T t72 = t46 - t48;
    const T t73 = t51 + t72;
    const T t74 = t54 + t73;
    const T t75 = t57 + t74;
    const T t76 = t75 - t62;
    const T t77 = t66 + t76;
    const T t78 = t77 - t71;
    const T t79 = t45 + t49;
    const T t80 = t79 - t50;
    const T t81 = t55 + t80;
    const T t82 = t58 + t81;
    const T t83 = t61 + t82;
    const T t84 = t83 - t67;
    const T t85 = t84 - t70;
    const T t86 = t47 - t44;
    const T t87 = t52 + t86;
    const T t88 = t56 + t87;
    const T t89 = t59 + t88;
    const T t90 = t89 - t63;
    const T t91 = t90 - t65;
    const T t92 = t69 + t91;
    const T t93 = t53 + t60;
    const T t94 = t64 + t93;
    const T t95 = t68 + t94;
    return {t78, t85, t92, t95};
  }
};


template <class T>
struct transform_point_b {
  static constexpr char name[] = "b";

  [[gnu::always_inline]] static constexpr std::array<T, 13>
  setup(const std::array<T, 8>& q) {
    const T t12 = q[0] * q[0];
    const T t13 = q[1] * q[1];
    const T t14 = q[2] * q[2];
    const T t15 = q[3] * q[3];
    const T t16 = t12 - t13;
    const T t17 = t16 - t14;
    const T t18 = t15 + t17;
    const T t19 = q[0] * q[1];
    const T t20 = q[2] * q[3];
    const T t21 = t19 - t20;
    const T t22 = t21 + t21;
    const T t23 = q[0] * q[2];
    const T t24 = q[1] * q[3];
    const T t25 = t23 + t24;
    const T t26 = t25 + t25;
    const T t27 = q[0] * q[7];
    const T t28 = q[1] * q[6];
    const T t29 = q[2] * q[5];
    const T t30 = q[3] * q[4];
    const T t31 = t28 - t27;
    const T t32 = t31 - t29;
    const T t33 = t30 + t32;
    const T t34 = t33 + t33;
    const T t35 = t19 + t20;
    const T t36 = t35 + t35;
    const T t37 = t13 - t12;
    const T t38 = t37 - t14;
    const T t39 = t15 + t38;
    const T t40 = q[0] * q[3];
    const T t41 = q[1] * q[2];
    const T t42 = t41 - t40;
    const T t43 = t42 + t42;
    const T t44 = q[0] * q[6];
    const T t45 = q[1] * q[7];
    const T t46 = q[2] * q[4];
    const T t47 = q[3] * q[5];
    const T t48 = t44 + t45;
    const T t49 = t46 - t48;
    const T t50 = t47 + t49;
    const T t51 = t50 + t50;
    const T t52 = t23 - t24;
    const T t53 = t52 + t52;
    const T t54 = t40 + t41;
    const T t55 = t54 + t54;
    const T t56 = t12 + t13;
    const T t57 = t14 - t56;
    const T t58 = t15 + t57;
    const T t59 = q[0] * q[5];
    const T t60 = q[1] * q[4];
    const T t61 = q[2] * q[7];
    const T t62 = q[3] * q[6];
    const T t63 = t59 - t60;
    const T t64 = t63 - t61;
    const T t65 = t62 + t64;
    const T t66 = t65 + t65;
    const T t67 = t14 + t56;
    const T t68 = t15 + t67;
    return {t18, t22, t26, t34, t36, t39, t43, t51, t53, t55, t58, t66, t68};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  apply(const std::array<T, 13>& m, const std::array<T, 8>& /*q*/,
        const std::array<T, 4>& x) {
    const T t69 = x[0] * m[0];
    const T t70 = x[1] * m[1];
    const T t71 = x[2] * m[2];
    const T t72 = x[3] * m[3];
    const T t73 = t69 + t70;
    const T t74 = t71 + t73;
    const T t75 = t72 + t74;
    const T t76 = x[0] * m[4];
    const T t77 = x[1] * m[5];
    const T t78 = x[2] * m[6];
    const T t79 = x[3] * m[7];
    const T t80 = t76 + t77;
    const T t81 = t78 + t80;
    const T t82 = t79 + t81;
    const T t83 = x[0] * m[8];
    const T t84 = x[1] * m[9];
    const T t85 = x[2] * m[10];
    const T t86 = x[3] * m[11];
    const T t87 = t83 + t84;
    const T t88 = t85 + t87;
    const T t89 = t86 + t88;
    const T t90 = x[3] * m[12];
    return {t75, t82, t89, t90};
  }
};


template <class T>
struct transform_point_c {
  static constexpr char name[] = "c";

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  setup(const std::array<T, 8>& q) {
    const T t37 = q[0] + q[0];
    const T t38 = q[1] + q[1];
    const T t39 = q[2] + q[2];
    const T t40 = q[3] + q[3];
    return {t37, t38, t39, t40};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  apply(const std::array<T, 4>& m, const std::array<T, 8>& q,
        const std::array<T, 4>& x) {
    const T t12 = q[1] * x[2];
    const T t13 = q[2] * x[1];
    const T t14 = t12 - t13;
    const T t15 = q[2] * x[0];
    const T t16 = q[0] * x[2];
    const T t17 = t15 - t16;
    const T t18 = q[0] * x[1];
    const T t19 = q[1] * x[0];
    const T t20 = t18 - t19;
    const T t21 = q[4] * x[3];
    const T t22 = t14 + t21;
    const T t23 = q[5] * x[3];
    const T t24 = t17 + t23;
    const T t25 = q[6] * x[3];
    const T t26 = t20 + t25;
    const T t36 = q[7] * x[3];
    const T t41 = t26 * m[1];
    const T t42 = t24 * m[2];
    const T t43 = t41 - t42;
    const T t44 = t22 * m[2];
    const T t45 = t26 * m[0];
    const T t46 = t44 - t45;
    const T t47 = t24 * m[0];
    const T t48 = t22 * m[1];
    const T t49 = t47 - t48;
    const T t50 = t22 * m[3];
    const T t51 = t36 * m[0];
    const T t52 = t43 + t50;
    const T t53 = t52 - t51;
    const T t54 = x[0] + t53;
    const T t55 = t24 * m[3];
    const T t56 = t36 * m[1];
    const T t57 = t46 + t55;
    const T t58 = t57 - t56;
    const T t59 = x[1] + t58;
    const T t60 = t26 * m[3];
    const T t61 = t36 * m[2];
    const T t62 = t49 + t60;
    const T t63 = t62 - t61;
    const T t64 = x[2] + t63;
    return {t54, t59, t64, x[3]};
  }
};


template <class T>
struct transform_line_a {
  static constexpr char name[] = "a";

  [[gnu::always_inline]] static constexpr std::array<T, 0>
  setup(const std::array<T, 8>& /*q*/) {
    return {};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 6>
  apply(const std::array<T, 0>& /*m*/, const std::array<T, 8>& q,
        const std::array<T, 6>& x) {
    const T t14 = q[0] * x[0];
    const T t15 = q[0] * x[1];
    const T t16 = q[0] * x[2];
    const T t17 = q[0] * x[3];
    const T t18 = q[0] * x[4];
    const T t19 = q[0] * x[5];
    const T t20 = q[1] * x[0];
    const T t21 = q[1] * x[1];
    const T t22 = q[1] * x[2];
    const T t23 = q[1] * x[3];
    const T t24 = q[1] * x[4];
    const T t25 = q[1] * x[5];
    const T t26 = q[2] * x[0];
    const T t27 = q[2] * x[1];
    const T t28 = q[2] * x[2];
    const T t29 = q[2] * x[3];
    const T t30 = q[2] * x[4];
    const T t31 = q[2] * x[5];
    const T t32 = q[3] * x[0];
    const T t33 = q[3] * x[1];
    const T t34 = q[3] * x[2];
    const T t35 = q[3] * x[3];
    const T t36 = q[3] * x[4];
    const T t37 = q[3] * x[5];
    const T t38 = q[4] * x[0];
    const T t39 = q[4] * x[1];
    const T t40 = q[4] * x[2];
    const T t41 = q[5] * x[0];
    const T t42 = q[5] * x[1];
    const T t43 = q[5] * x[2];
    const T t44 = q[6] * x[0];
    const T t45 = q[6] * x[1];
    const T t46 = q[6] * x[2];
    const T t47 = q[7] * x[0];
    const T t48 = q[7] * x[1];
    const T t49 = q[7] * x[2];
    const T t50 = t14 + t21;
    const T t51 = t28 + t50;
    const T t52 = t20 - t15;
    const T t53 = t52 - t34;
    const T t54 = t16 - t26;
    const T t55 = t54 - t33;
    const T t56 = t17 + t24;
    const T t57 = t31 + t56;
    const T t58 = t38 + t57;
    const T t59 = t42 + t58;
    const T t60 = t46 + t59;
    const T t61 = t18 - t23;
    const T t62 = t37 + t61;
    const T t63 = t39 + t62;
    const T t64 = t63 - t41;
    const T t65 = t49 + t64;
    const T t66 = t19 - t29;
    const T t67 = t66 - t36;
    const T t68 = t40 + t67;
    const T t69 = t68 - t44;
    const T t70 = t69 - t48;
    const T t71 = t27 - t22;
    const T t72 = t71 - t32;
    const T t73 = t25 - t30;
    const T t74 = t35 + t73;
    const T t75 = t43 + t74;
    const T t76 = t75 - t45;
    const T t77 = t47 + t76;
    const T t78 = q[0] * t60;
    const T t79 = q[1] * t60;
    const T t80 = q[2] * t60;
    const T t81 = q[0] * t65;
    const T t82 = q[1] * t65;
    const T t83 = q[3] * t65;
    const T t84 = q[0] * t70;
    const T t85 = q[2] * t70;
    const T t86 = q[3] * t70;
    const T t87 = q[1] * t72;
    const T t88 = q[2] * t72;
    const T t89 = q[3] * t72;
    const T t90 = q[5] * t72;
    const T t91 = q[6] * t72;
    const T t92 = q[7] * t72;
    const T t93 = q[1] * t77;
    const T t94 = q[2] * t77;
    const T t95 = q[3] * t77;
    const T t96 = q[0] * t55;
    const T t97 = q[2] * t55;
    const T t98 = q[3] * t55;
    const T t99 = q[4] * t55;
    const T t100 = q[6] * t55;
    const T t101 = q[7] * t55;
    const T t102 = q[0] * t53;
    const T t103 = q[1] * t53;
    const T t104 = q[3] * t53;
    const T t105 = q[4] * t53;
    const T t106 = q[5] * t53;
    const T t107 = q[7] * t53;
    const T t108 = q[0] * t51;
    const T t109 = q[1] * t51;
    const T t110 = q[2] * t51;
    const T t111 = q[4] * t51;
    const T t112 = q[5] * t51;
    const T t113 = q[6] * t51;
    const T t114 = t97 - t89;
    const T t115 = t114 - t103;
    const T t116 = t108 + t115;
    const T t117 = t88 + t98;
    const T t118 = t102 - t117;
    const T t119 = t109 + t118;
    const T t120 = t87 - t96;
    const T t121 = t120 - t104;
    const T t122 = t110 + t121;
    const T t123 = t78 + t82;
    const T t124 = t85 + t123;
    const T t125 = t124 - t92;
    const T t126 = t95 + t125;
    const T t127 = t100 + t126;
    const T t128 = t127 - t106;
    const T t129 = t111 + t128;
    const T t130 = t79 - t81;
    const T t131 = t130 - t86;
    const T t132 = t131 - t91;
    const T t133 = t94 + t132;
    const T t134 = t133 - t101;
    const T t135 = t105 + t134;
    const T t136 = t112 + t135;
    const T t137 = t80 + t83;
    const T t138 = t137 - t84;
    const T t139 = t90 + t138;
    const T t140 = t139 - t93;
    const T t141 = t140 - t99;
    const T t142 = t141 - t107;
    const T t143 = t113 + t142;
    return {t116, t119, t122, t129, t136, t143};
  }
};


template <class T>
struct transform_line_b {
  static constexpr char name[] = "b";

  [[gnu::always_inline]] static constexpr std::array<T, 18>
  setup(const std::array<T, 8>& q) {
    const T t14 = q[0] * q[0];
    const T t15 = q[1] * q[1];
    const T t16 = q[2] * q[2];
    const T t17 = q[3] * q[3];
    const T t18 = t14 - t15;
    const T t19 = t18 - t16;
    const T t20 = t17 + t19;
    const T t21 = q[0] * q[1];
    const T t22 = q[2] * q[3];
    const T t23 = t21 - t22;
    const T t24 = t23 + t23;
    const T t25 = q[0] * q[2];
    const T t26 = q[1] * q[3];
    const T t27 = t25 + t26;
    const T t28 = t27 + t27;
    const T t29 = t21 + t22;
    const T t30 = t29 + t29;
    const T t31 = t15 - t14;
    const T t32 = t31 - t16;
    const T t33 = t17 + t32;
    const T t34 = q[0] * q[3];
    const T t35 = q[1] * q[2];
    const T t36 = t35 - t34;
    const T t37 = t36 + t36;
    const T t38 = t25 - t26;
    const T t39 = t38 + t38;
    const T t40 = t34 + t35;
    const T t41 = t40 + t40;
    const T t42 = t14 + t15;
    const T t43 = t16 - t42;
    const T t44 = t17 + t43;
    const T t45 = q[0] * q[4];
    const T t46 = q[1] * q[5];
    const T t47 = q[2] * q[6];
    const T t48 = q[3] * q[7];
    const T t49 = t45 - t46;
    const T t50 = t49 - t47;
    const T t51 = t48 + t50;
    const T t52 = t51 + t51;
    const T t53 = q[0] * q[5];
    const T t54 = q[1] * q[4];
    const T t55 = q[2] * q[7];
    const T t56 = q[3] * q[6];
    const T t57 = t53 + t54;
    const T t58 = t57 - t55;
    const T t59 = t58 - t56;
    const T t60 = t59 + t59;
    const T t61 = q[0] * q[6];
    const T t62 = q[1] * q[7];
    const T t63 = q[2] * q[4];
    const T t64 = q[3] * q[5];
    const T t65 = t61 + t62;
    const T t66 = t63 + t65;
    const T t67 = t64 + t66;
    const T t68 = t67 + t67;
    const T t69 = t55 + t57;
    const T t70 = t56 + t69;
    const T t71 = t70 + t70;
    const T t72 = t46 - t45;
    const T t73 = t72 - t47;
    const T t74 = t48 + t73;
    const T t75 = t74 + t74;
    const T t76 = q[0] * q[7];
    const T t77 = q[1] * q[6];
    const T t78 = q[2] * q[5];
    const T t79 = q[3] * q[4];
    const T t80 = t77 - t76;
    const T t81 = t78 + t80;
    const T t82 = t81 - t79;
    const T t83 = t82 + t82;
    const T t84 = t61 - t62;
    const T t85 = t63 + t84;
    const T t86 = t85 - t64;
    const T t87 = t86 + t86;
    const T t88 = t76 + t77;
    const T t89 = t78 + t88;
    const T t90 = t79 + t89;
    const T t91 = t90 + t90;
    const T t92 = t45 + t46;
    const T t93 = t47 - t92;
    const T t94 = t48 + t93;
    const T t95 = t94 + t94;
    return {t20, t24, t28, t30, t33, t37, t39, t41, t44, t52, t60, t68, t71, t75, t83, t87, t91, t95};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 6>
  apply(const std::array<T, 18>& m, const std::array<T, 8>& /*q*/,
        const std::array<T, 6>& x) {
    const T t96 = x[0] * m[0];
    const T t97 = x[1] * m[1];
    const T t98 = x[2] * m[2];
    const T t99 = t96 + t97;
    const T t100 = t98 + t99;
    const T t101 = x[0] * m[3];
    const T t102 = x[1] * m[4];
    const T t103 = x[2] * m[5];
    const T t104 = t101 + t102;
    const T t105 = t103 + t104;
    const T t106 = x[0] * m[6];
    const T t107 = x[1] * m[7];
    const T t108 = x[2] * m[8];
    const T t109 = t106 + t107;
    const T t110 = t108 + t109;
    const T t111 = x[0] * m[9];
    const T t112 = x[1] * m[10];
    const T t113 = x[2] * m[11];
    const T t114 = x[3] * m[0];
    const T t115 = x[4] * m[1];
    const T t116 = x[5] * m[2];
    const T t117 = t111 + t112;
    const T t118 = t113 + t117;
    const T t119 = t114 + t118;
    const T t120 = t115 + t119;
    const T t121 = t116 + t120;
    const T t122 = x[0] * m[12];
    const T t123 = x[1] * m[13];
    const T t124 = x[2] * m[14];
    const T t125 = x[3] * m[3];
    const T t126 = x[4] * m[4];
    const T t127 = x[5] * m[5];
    const T t128 = t122 + t123;
    const T t129 = t124 + t128;
    const T t130 = t125 + t129;
    const T t131 = t126 + t130;
    const T t132 = t127 + t131;
    const T t133 = x[0] * m[15];
    const T t134 = x[1] * m[16];
    const T t135 = x[2] * m[17];
    const T t136 = x[3] * m[6];
    const T t137 = x[4] * m[7];
    const T t138 = x[5] * m[8];
    const T t139 = t133 + t134;
    const T t140 = t135 + t139;
    const T t141 = t136 + t140;
    const T t142 = t137 + t141;
    const T t143 = t138 + t142;
    return {t100, t105, t110, t121, t132, t143};
  }
};


template <class T>
struct transform_line_c {
  static constexpr char name[] = "c";

  [[gnu::always_inline]] static constexpr std::array<T, 7>
  setup(const std::array<T, 8>& q) {
    const T t23 = q[0] + q[0];
    const T t24 = q[1] + q[1];
    const T t25 = q[2] + q[2];
    const T t26 = q[3] + q[3];
    const T t72 = q[1] * q[6];
    const T t73 = q[2] * q[5];
    const T t74 = t72 - t73;
    const T t75 = q[2] * q[4];
    const T t76 = q[0] * q[6];
    const T t77 = t75 - t76;
    const T t78 = q[0] * q[5];
    const T t79 = q[1] * q[4];
    const T t80 = t78 - t79;
    const T t81 = q[3] * q[4];
    const T t82 = q[0] * q[7];
    const T t83 = t81 - t82;
    const T t84 = t74 + t83;
    const T t85 = q[3] * q[5];
    const T t86 = q[1] * q[7];
    const T t87 = t85 - t86;
    const T t88 = t77 + t87;
    const T t89 = q[3] * q[6];
    const T t90 = q[2] * q[7];
    const T t91 = t89 - t90;
    const T t92 = t80 + t91;
    const T t93 = t84 + t84;
    const T t94 = t88 + t88;
    const T t95 = t92 + t92;
    return {t23, t24, t25, t26, t93, t94, t95};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 6>
  apply(const std::array<T, 7>& m, const std::array<T, 8>& q,
        const std::array<T, 6>& x) {
    const T t14 = q[1] * x[2];
    const T t15 = q[2] * x[1];
    const T t16 = t14 - t15;
    const T t17 = q[2] * x[0];
    const T t18 = q[0] * x[2];
    const T t19 = t17 - t18;
    const T t20 = q[0] * x[1];
    const T t21 = q[1] * x[0];
    const T t22 = t20 - t21;
    const T t27 = t22 * m[1];
    const T t28 = t19 * m[2];
    const T t29 = t27 - t28;
    const T t30 = t16 * m[2];
    const T t31 = t22 * m[0];
    const T t32 = t30 - t31;
    const T t33 = t19 * m[0];
    const T t34 = t16 * m[1];
    const T t35 = t33 - t34;
    const T t36 = t16 * m[3];
    const T t37 = t29 + t36;
    const T t38 = x[0] + t37;
    const T t39 = t19 * m[3];
    const T t40 = t32 + t39;
    const T t41 = x[1] + t40;
    const T t42 = t22 * m[3];
    const T t43 = t35 + t42;
    const T t44 = x[2] + t43;
    const T t45 = q[1] * x[5];
    const T t46 = q[2] * x[4];
    const T t47 = t45 - t46;
    const T t48 = q[2] * x[3];
    const T t49 = q[0] * x[5];
    const T t50 = t48 - t49;
    const T t51 = q[0] * x[4];
    const T t52 = q[1] * x[3];
    const T t53 = t51 - t52;
    const T t54 = m[1] * t53;
    const T t55 = m[2] * t50;
    const T t56 = t54 - t55;
    const T t57 = m[2] * t47;
    const T t58 = m[0] * t53;
    const T t59 = t57 - t58;
    const T t60 = m[0] * t50;
    const T t61 = m[1] * t47;
    const T t62 = t60 - t61;
    const T t63 = m[3] * t47;
    const T t64 = t56 + t63;
    const T t65 = x[3] + t64;
    const T t66 = m[3] * t50;
    const T t67 = t59 + t66;
    const T t68 = x[4] + t67;
    const T t69 = m[3] * t53;
    const T t70 = t62 + t69;
    const T t71 = x[5] + t70;
    const T t96 = t44 * m[5];
    const T t97 = t41 * m[6];
    const T t98 = t96 - t97;
    const T t99 = t38 * m[6];
    const T t100 = t44 * m[4];
    const T t101 = t99 - t100;
    const T t102 = t41 * m[4];
    const T t103 = t38 * m[5];
    const T t104 = t102 - t103;
    const T t105 = t65 + t98;
    const T t106 = t68 + t101;
    const T t107 = t71 + t104;
    return {t38, t41, t44, t105, t106, t107};
  }
};


template <class T>
struct transform_plane_a {
  static constexpr char name[] = "a";

  [[gnu::always_inline]] static constexpr std::array<T, 0>
  setup(const std::array<T, 8>& /*q*/) {
    return {};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  apply(const std::array<T, 0>& /*m*/, const std::array<T, 8>& q,
        const std::array<T, 4>& x) {
    const T t12 = q[0] * x[0];
    const T t13 = q[0] * x[1];
    const T t14 = q[0] * x[2];
    const T t15 = q[0] * x[3];
    const T t16 = q[1] * x[0];
    const T t17 = q[1] * x[1];
    const T t18 = q[1] * x[2];
    const T t19 = q[1] * x[3];
    const T t20 = q[2] * x[0];
    const T t21 = q[2] * x[1];
    const T t22 = q[2] * x[2];
    const T t23 = q[2] * x[3];
    const T t24 = q[3] * x[0];
    const T t25 = q[3] * x[1];
    const T t26 = q[3] * x[2];
    const T t27 = q[3] * x[3];
    const T t28 = q[4] * x[0];
    const T t29 = q[4] * x[1];
    const T t30 = q[4] * x[2];
    const T t31 = q[5] * x[0];
    const T t32 = q[5] * x[1];
    const T t33 = q[5] * x[2];
    const T t34 = q[6] * x[0];
    const T t35 = q[6] * x[1];
    const T t36 = q[6] * x[2];
    const T t37 = q[7] * x[0];
    const T t38 = q[7] * x[1];
    const T t39 = q[7] * x[2];
    const T t40 = t12 + t17;
    const T t41 = t22 + t40;
    const T t42 = t13 - t16;
    const T t43 = t26 + t42;
    const T t44 = t14 - t20;
    const T t45 = t44 - t25;
    const T t46 = t15 + t33;
    const T t47 = t46 - t35;
    const T t48 = t37 + t47;
    const T t49 = t18 - t21;
    const T t50 = t24 + t49;
    const T t51 = t19 - t30;
    const T t52 = t34 + t51;
    const T t53 = t38 + t52;
    const T t54 = t23 + t29;
    const T t55 = t54 - t31;
    const T t56 = t39 + t55;
    const T t57 = t28 - t27;
    const T t58 = t32 + t57;
    const T t59 = t36 + t58;
    const T t60 = q[0] * t48;
    const T t61 = q[1] * t53;
    const T t62 = q[2] * t56;
    const T t63 = q[0] * t41;
    const T t64 = q[1] * t41;
    const T t65 = q[2] * t41;
    const T t66 = q[7] * t41;
    const T t67 = q[3] * t59;
    const T t68 = q[0] * t43;
    const T t69 = q[1] * t43;
    const T t70 = q[3] * t43;
    const T t71 = q[6] * t43;
    const T t72 = q[0] * t45;
    const T t73 = q[2] * t45;
    const T t74 = q[3] * t45;
    const T t75 = q[5] * t45;
    const T t76 = q[1] * t50;
    const T t77 = q[2] * t50;
    const T t78 = q[3] * t50;
    const T t79 = q[4] * t50;
    const T t80 = t63 + t69;
    const T t81 = t73 + t80;
    const T t82 = t78 + t81;
    const T t83 = t64 - t68;
    const T t84 = t83 - t74;
    const T t85 = t77 + t84;
    const T t86 = t65 + t70;
    const T t87 = t86 - t72;
    const T t88 = t87 - t76;
    const T t89 = t60 + t61;
    const T t90 = t62 + t89;
    const T t91 = t66 + t90;
    const T t92 = t91 - t67;
    const T t93 = t92 - t71;
    const T t94 = t75 + t93;
    const T t95 = t94 - t79;
    return {t82, t85, t88, t95};
  }
};


template <class T>
struct transform_plane_b {
  static constexpr char name[] = "b";

  [[gnu::always_inline]] static constexpr std::array<T, 13>
  setup(const std::array<T, 8>& q) {
    const T t12 = q[0] * q[0];
    const T t13 = q[1] * q[1];
    const T t14 = q[2] * q[2];
    const T t15 = q[3] * q[3];
    const T t16 = t12 - t13;
    const T t17 = t16 - t14;
    const T t18 = t15 + t17;
    const T t19 = q[0] * q[1];
    const T t20 = q[2] * q[3];
    const T t21 = t19 - t20;
    const T t22 = t21 + t21;
    const T t23 = q[0] * q[2];
    const T t24 = q[1] * q[3];
    const T t25 = t23 + t24;
    const T t26 = t25 + t25;
    const T t27 = t19 + t20;
    const T t28 = t27 + t27;
    const T t29 = t13 - t12;
    const T t30 = t29 - t14;
    const T t31 = t15 + t30;
    const T t32 = q[0] * q[3];
    const T t33 = q[1] * q[2];
    const T t34 = t33 - t32;
    const T t35 = t34 + t34;
    const T t36 = t23 - t24;
    const T t37 = t36 + t36;
    const T t38 = t32 + t33;
    const T t39 = t38 + t38;
    const T t40 = t12 + t13;
    const T t41 = t14 - t40;
    const T t42 = t15 + t41;
    const T t43 = q[0] * q[7];
    const T t44 = q[1] * q[6];
    const T t45 = q[2] * q[5];
    const T t46 = q[3] * q[4];
    const T t47 = t43 + t44;
    const T t48 = t47 - t45;
    const T t49 = t48 - t46;
    const T t50 = t49 + t49;
    const T t51 = q[0] * q[6];
    const T t52 = q[1] * q[7];
    const T t53 = q[2] * q[4];
    const T t54 = q[3] * q[5];
    const T t55 = t52 - t51;
    const T t56 = t53 + t55;
    const T t57 = t56 - t54;
    const T t58 = t57 + t57;
    const T t59 = q[0] * q[5];
    const T t60 = q[1] * q[4];
    const T t61 = q[2] * q[7];
    const T t62 = q[3] * q[6];
    const T t63 = t59 - t60;
    const T t64 = t61 + t63;
    const T t65 = t64 - t62;
    const T t66 = t65 + t65;
    const T t67 = t14 + t40;
    const T t68 = t15 + t67;
    return {t18, t22, t26, t28, t31, t35, t37, t39, t42, t50, t58, t66, t68};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  apply(const std::array<T, 13>& m, const std::array<T, 8>& /*q*/,
        const std::array<T, 4>& x) {
    const T t69 = x[0] * m[0];
    const T t70 = x[1] * m[1];
    const T t71 = x[2] * m[2];
    const T t72 = t69 + t70;
    const T t73 = t71 + t72;
    const T t74 = x[0] * m[3];
    const T t75 = x[1] * m[4];
    const T t76 = x[2] * m[5];
    const T t77 = t74 + t75;
    const T t78 = t76 + t77;
    const T t79 = x[0] * m[6];
    const T t80 = x[1] * m[7];
    const T t81 = x[2] * m[8];
    const T t82 = t79 + t80;
    const T t83 = t81 + t82;
    const T t84 = x[0] * m[9];
    const T t85 = x[1] * m[10];
    const T t86 = x[2] * m[11];
    const T t87 = x[3] * m[12];
    const T t88 = t84 + t85;
    const T t89 = t86 + t88;
    const T t90 = t87 + t89;
    return {t73, t78, t83, t90};
  }
};


template <class T>
struct transform_plane_c {
  static constexpr char name[] = "c";

  [[gnu::always_inline]] static constexpr std::array<T, 7>
  setup(const std::array<T, 8>& q) {
    const T t21 = q[0] + q[0];
    const T t22 = q[1] + q[1];
    const T t23 = q[2] + q[2];
    const T t24 = q[3] + q[3];
    const T t43 = q[1] * q[6];
    const T t44 = q[2] * q[5];
    const T t45 = t43 - t44;
    const T t46 = q[2] * q[4];
    const T t47 = q[0] * q[6];
    const T t48 = t46 - t47;
    const T t49 = q[0] * q[5];
    const T t50 = q[1] * q[4];
    const T t51 = t49 - t50;
    const T t52 = q[3] * q[4];
    const T t53 = q[0] * q[7];
    const T t54 = t52 - t53;
    const T t55 = t45 + t54;
    const T t56 = q[3] * q[5];
    const T t57 = q[1] * q[7];
    const T t58 = t56 - t57;
    const T t59 = t48 + t58;
    const T t60 = q[3] * q[6];
    const T t61 = q[2] * q[7];
    const T t62 = t60 - t61;
    const T t63 = t51 + t62;
    const T t64 = t55 + t55;
    const T t65 = t59 + t59;
    const T t66 = t63 + t63;
    return {t21, t22, t23, t24, t64, t65, t66};
  }

  [[gnu::always_inline]] static constexpr std::array<T, 4>
  apply(const std::array<T, 7>& m, const std::array<T, 8>& q,
        const std::array<T, 4>& x) {
    const T t12 = q[1] * x[2];
    const T t13 = q[2] * x[1];
    const T t14 = t12 - t13;
    const T t15 = q[2] * x[0];
    const T t16 = q[0] * x[2];
    const T t17 = t15 - t16;
    const T t18 = q[0] * x[1];
    const T t19 = q[1] * x[0];
    const T t20 = t18 - t19;
    const T t25 = t20 * m[1];
    const T t26 = t17 * m[2];
    const T t27 = t25 - t26;
    const T t28 = t14 * m[2];
    const T t29 = t20 * m[0];
    const T t30 = t28 - t29;
    const T t31 = t17 * m[0];
    const T t32 = t14 * m[1];
    const T t33 = t31 - t32;
    const T t34 = t14 * m[3];
    const T t35 = t27 + t34;
    const T t36 = x[0] + t35;
    const T t37 = t17 * m[3];
    const T t38 = t30 + t37;
    const T t39 = x[1] + t38;
    const T t40 = t20 * m[3];
    const T t41 = t33 + t40;
    const T t42 = x[2] + t41;
    const T t67 = t36 * m[4];
    const T t68 = t39 * m[5];
    const T t69 = t42 * m[6];
    const T t70 = t67 + t68;
    const T t71 = t69 + t70;
    const T t72 = x[3] - t71;
    return {t36, t39, t42, t72};
  }
};


} // namespace forms

#endif // SPIKE_E_BENCH_FORMS_HPP
