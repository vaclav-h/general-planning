#include <stdio.h>
#include "problem.h"
#include<bits/stdc++.h>
using namespace std;


/* *
 * Checks if operator is applicable in given state
 *
 * @param op Operator to check
 * @param state Vector containing indices of facts that are true in given state
 * @returns true if operator is applicable, false otherwise
 */
bool is_valid_op(strips_operator_t &op, set<int> &state) {
    for (int i = 0; i < op.pre_size; i++) {
        if (!(find(state.begin(), state.end(), op.pre[i]) != state.end())) {
            return false;
        }
    }
    return true;
}

/**
 * Finds applicable operators in given state
 *
 * @param strips The STRIPS problem given in strips_t
 * @param state Vector containing indices of facts that are true in given state
 * @returns Vector containing indices of operators applicable in given state
 */
set<int> expand(strips_t &strips, set<int> &state) {
    set<int> valid_ops;
    for (int i = 0; i < strips.num_operators; i++) {
        if (is_valid_op(strips.operators[i], state)) {
            valid_ops.insert(i);
        }
    }
    return valid_ops;
}

/**
 * Applies operator on a state and returns resulting state
 *
 * @param strips The STRIPS problem given in strips_t
 * @param state Vector containing indices of facts that are true in given state
 * @param op Operator to apply
 * @returns Vector containing indices of facts that are true in new state
 */
set<int> next_state(strips_t &strips, set<int> &state, strips_operator_t &op) {
    set<int> new_state;
    bool is_del;
    for (int fact : state) {
        is_del = false;
        for (int i = 0; i < op.del_eff_size; i++) {
            if (fact == op.del_eff[i]) {
                is_del = true;
                break;
            }
        }
        if (!is_del)
            new_state.insert(fact);
    }
    for (int i = 0; i < op.add_eff_size; i++) {
            new_state.insert(op.add_eff[i]);
    }
    return new_state;
}

/**
 * Computes the heuristic in given state
 */
int h(set<int> &state) {
    return 0;
}

/**
 * The A* algorithm
 *
 */
void a_star(strips_t &strips) {
    
}

int main(int argc, char *argv[])
{
    strips_t strips;
    //fdr_t fdr;
 
    if (argc != 3){
        fprintf(stderr, "Usage: %s problem.strips problem.fdr\n", argv[0]);
        return -1;
    }
 
    // Load the planning problem in STRIPS or FDR
    stripsRead(&strips, argv[1]);
    //fdrRead(&fdr, argv[2]);

    // Implement the search here
    set<int> init;
    for (int i = 0; i < strips.init_size; i++) {
        init.insert(strips.init[i]);
        cout << strips.fact_names[strips.init[i]] << "\n";
    }
    cout << "\n\n";
    set<int> valid = expand(strips, init);
    for (int i : valid) {
        cout << strips.operators[i].name << "\n";
    }

    set<int> new_state;
    for (int i : valid) {
        new_state = next_state(strips, init, strips.operators[i]);
    }
    cout << "\n\n";
    for (int i : new_state) {
        cout << strips.fact_names[i] << "\n";
    }
    

    stripsFree(&strips);
    //fdrFree(&fdr);
}
