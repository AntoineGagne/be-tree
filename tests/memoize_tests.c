#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "minunit.h"
#include "utils.h"
#include "value.h"
#include "var.h"


struct report test(const char* expr_a, const char* expr_b, const char* event, struct config* config)
{
    struct cnode* cnode = make_cnode(config, NULL);
    betree_insert(config, 1, expr_a, cnode);
    betree_insert(config, 2, expr_b, cnode);
    struct matched_subs* matched_subs = make_matched_subs();
    struct report report = make_empty_report();
    betree_search(config, event, cnode, matched_subs, &report);
    free_cnode(cnode);
    free_matched_subs(matched_subs);
    config->pred_count = 0;
    free(config->preds);
    config->preds = NULL;
    return report;
}

bool test_same(const char* expr, const char* event, struct config* config)
{
    struct report report = test(expr, expr, event, config);
    return
      report.expressions_evaluated == 2 &&
      report.expressions_matched == 2 &&
      report.expressions_memoized == 1;
}

bool test_diff(const char* expr_a, const char* expr_b, const char* event, struct config* config)
{
    struct report report = test(expr_a, expr_b, event, config);
    return
      report.expressions_evaluated == 2 &&
      report.expressions_matched == 2 &&
      report.expressions_memoized == 0;
}

int test_numeric_compare_integer()
{
    struct config* config = make_default_config();
    add_attr_domain_i(config, "i", 0, 10, false);

    mu_assert(test_same("i > 1", "{\"i\": 2}", config), "integer gt");
    mu_assert(test_same("i >= 1", "{\"i\": 2}", config), "integer ge");
    mu_assert(test_same("i < 1", "{\"i\": 0}", config), "integer lt");
    mu_assert(test_same("i <= 1", "{\"i\": 1}", config), "integer le");
    mu_assert(test_diff("i > 0", "i > 1", "{\"i\": 2}", config), "integer diff");

    free_config(config);
    return 0;
}

int test_numeric_compare_float()
{
    struct config* config = make_default_config();
    add_attr_domain_f(config, "f", 0., 10., false);

    mu_assert(test_same("f > 1.", "{\"f\": 2.}", config), "float gt");
    mu_assert(test_same("f >= 1.", "{\"f\": 2.}", config), "float ge");
    mu_assert(test_same("f < 1.", "{\"f\": 0.}", config), "float lt");
    mu_assert(test_same("f <= 1.", "{\"f\": 1.}", config), "float le");
    mu_assert(test_diff("f > 0.", "f > 1.", "{\"f\": 2.}", config), "float diff");

    return 0;
}

int test_equality_integer()
{
    struct config* config = make_default_config();
    add_attr_domain_i(config, "i", 0, 10, false);

    mu_assert(test_same("i = 1", "{\"i\": 1}", config), "integer eq");
    mu_assert(test_same("i <> 1", "{\"i\": 0}", config), "integer ne");
    mu_assert(test_diff("i <> 0", "i <> 1", "{\"i\": 2}", config), "integer diff");

    return 0;
}

int test_equality_float()
{
    struct config* config = make_default_config();
    add_attr_domain_f(config, "f", 0., 10., false);

    mu_assert(test_same("f = 1.", "{\"f\": 1.}", config), "float eq");
    mu_assert(test_same("f <> 1.", "{\"f\": 0.}", config), "float ne");
    mu_assert(test_diff("f <> 0.", "f <> 1.", "{\"f\": 2.}", config), "float diff");

    return 0;
}

int test_equality_string()
{
    struct config* config = make_default_config();
    add_attr_domain_s(config, "s", false);

    mu_assert(test_same("s = \"a\"", "{\"s\": \"a\"}", config), "string eq");
    mu_assert(test_same("s <> \"a\"", "{\"s\": \"b\"}", config), "string ne");
    mu_assert(test_diff("s <> \"a\"", "s <> \"b\"", "{\"s\": \"c\"}", config), "string diff");

    return 0;
}

int test_set_var_integer()
{
    struct config* config = make_default_config();
    add_attr_domain_i(config, "i", 0, 10, false);

    // "i in (1,2)"
    // "i not in (1,2)"

    return 0;
}

int test_set_var_string()
{
    struct config* config = make_default_config();
    add_attr_domain_s(config, "s", false);

    // "s in (\"1\",\"2\")"
    // "s not in (\"1\",\"2\")"

    return 0;
}

int test_set_list_integer()
{
    struct config* config = make_default_config();
    add_attr_domain_il(config, "il", false);

    // "1 in il"
    // "1 not in il"

    return 0;
}

int test_set_list_string()
{
    struct config* config = make_default_config();
    add_attr_domain_sl(config, "sl", false);

    // "\"1\" in sl"
    // "\"1\" not in sl"

    return 0;
}

int test_list_integer()
{
    struct config* config = make_default_config();
    add_attr_domain_i(config, "i", 0, 10, false);

    // "i one of (1, 2)"
    // "i none of (1, 2)"
    // "i all of (1, 2)"

    return 0;
}

int test_list_string()
{
    struct config* config = make_default_config();
    add_attr_domain_s(config, "s", false);

    // "s one of (\"1\", \"2\")"
    // "s none of (\"1\", \"2\")"
    // "s all of (\"1\", \"2\")"

    return 0;
}

int test_special_frequency()
{
    struct config* config = make_default_config();

    // "within_frequency_cap(\"flight\", \"namespace\", 1, 2)"

    return 0;
}

int test_special_segment()
{
    struct config* config = make_default_config();

    // "segment_within(1, 2)"
    // "segment_within(segment, 1, 2)"
    // "segment_before(1, 2)"
    // "segment_before(segment, 1, 2)"

    return 0;
}

int test_special_geo()
{
    struct config* config = make_default_config();

    // "geo_within_radius(1, 2, 3)"
    // "geo_within_radius(1., 2., 3.)"

    return 0;
}

int test_special_string()
{
    struct config* config = make_default_config();
    add_attr_domain_s(config, "s", false);
    add_attr_domain_s(config, "s2", false);

    // "contains(s, \"abc\")"
    // "starts_with(s, \"abc\")"
    // "ends_with(s, \"abc\")"

    return 0;
}

int test_bool()
{
    struct config* config = make_default_config();
    add_attr_domain_b(config, "b", false, true, false);
    add_attr_domain_i(config, "i", 0, 10, false);

    // "b"
    // "not b"
    // "b and b"
    // "b or b"
    // "not (i = 0)"
    // "(i = 0) and (i = 0)"

    return 0;
}

int all_tests() 
{
    mu_run_test(test_numeric_compare_integer);
    mu_run_test(test_numeric_compare_float);
    mu_run_test(test_equality_integer);
    mu_run_test(test_equality_float);
    mu_run_test(test_equality_string);
    mu_run_test(test_set_var_integer);
    mu_run_test(test_set_var_string);
    mu_run_test(test_set_list_integer);
    mu_run_test(test_set_list_string);
    mu_run_test(test_list_integer);
    mu_run_test(test_list_string);
    mu_run_test(test_special_frequency);
    mu_run_test(test_special_segment);
    mu_run_test(test_special_geo);
    mu_run_test(test_special_string);
    mu_run_test(test_bool);

    return 0;
}

RUN_TESTS()


