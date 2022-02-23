#include <stdio.h>
#include "problem.h"
#include<bits/stdc++.h>
using namespace std;

// Compare type for priority queue
struct compare_pri {
    int operator() (const pair<set<int>, int> &p1, const pair<set<int>, int> &p2) {
        return p1.second > p2.second;
    }
};

// Type used in priority queue
typedef pair<set<int>, int> qpair;


/* *
 * Checks if operator is applicable in given state
 *
 * @param op Operator to check
 * @param state Set containing indices of facts that are true in given state
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
 * Checks if state is a goal state
 *
 * @param state Set containing indices of facts that are true in given state
 * @param goal Set containing indices of facts that are true in goal state
 * @returns true if state is a goal state
 */
bool is_goal(set<int> &state, set<int> &goal) {
	return includes(state.begin(), state.end(), goal.begin(), goal.end()); 
}

/**
 * Finds applicable operators in given state
 *
 * @param strips The STRIPS problem given in strips_t
 * @param state Set containing indices of facts that are true in given state
 * @returns Set containing indices of operators applicable in given state
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
 * @param state Set containing indices of facts that are true in given state
 * @param op Operator to apply
 * @returns Set containing indices of facts that are true in new state
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
 * Generates vector of succesor states 
 * 
 * @param strips The STRIPS problem given in strips_t
 * @param state Set containing indices of facts that are true in given state
 * @returns vector of pairs pair<state, operator_applied>
 *
 */
vector<pair<set<int>, int>> generate_succ(strips_t strips, set<int> &state) {
    vector<pair<set<int>, int>> succ;	
	set<int> ops = expand(strips, state);
    pair<set<int>, int> x; // pair<state, operator>
	for (int o : ops) {
		set<int> n_state = next_state(strips, state, strips.operators[o]);
        x = make_pair(n_state, o);
		succ.push_back(x);	
	}
	return succ;
}

/**
 * Computes the heuristic in given state
 *
 */
int h(set<int> &state) {
    return 0;
}

/**
 * Prints the plan
 *
 * @param strips The STRIPS problem given in strips_t
 * @param init Initial state given as set
 * @param f Final state from which to backtrack the solution
 * @param parent Map of pointer to parent states 
 * @param parent_op Map of operators applied on parents
 */
void print_plan(strips_t &strips, set<int> &init, set<int> &f, map<set<int>, set<int>> &parent, map<set<int>, int> &parent_op) {
    set<int> s = f;
    stack<int> plan;
    while (s != init) {
        plan.push(parent_op[s]);
        s = parent[s];
    }
    while (!plan.empty()) {
        cout << strips.operators[plan.top()].name << "\n";
        plan.pop();
    }
}


/**
 * The A* algorithm
 * Finds a plan to a problem given in STRIPS representation and prints it.
 *
 * @param strips The STRIPS problem given in strips_t
 */
void a_star(strips_t &strips) {
    // Initialize structures
    priority_queue<qpair, vector<qpair>, compare_pri> open; // pair<state, f_value>
	map<set<int>, set<int>> parent; //pointers to the parent nodes
	map<set<int>, int> parent_op; //operator applied in parent node
    map<set<int>, int> dist; // map for the g_values
    map<set<int>, bool> seen; // indicator if state was already visited     

    // Convert init and goal states into sets
    set<int> init;
    for (int i = 0; i < strips.init_size; i++) {
        init.insert(strips.init[i]);
    }
	set<int> goal;
	for (int i = 0; i < strips.goal_size; i++) {
        goal.insert(strips.goal[i]);
    }
    
    // Enqueue initial state
    open.push(make_pair(init, h(init)));
    seen[init] = true;
    dist[init] = 0;

    // Main loop
    qpair u;
    while(!open.empty()) {
        u = open.top();
        open.pop();
		if (is_goal(u.first, goal)) {
		    // return plan
            printf(";; Optimal cost: %d\n", u.second);
            printf(";; h^max for init: \n\n");
            print_plan(strips, init, u.first, parent, parent_op);
            break;
		}
    	vector<pair<set<int>, int>> succ = generate_succ(strips, u.first);
		for (pair<set<int>, int> v : succ) {
            int new_dist = dist[u.first] + strips.operators[v.second].cost; 
            if (!seen[v.first]) {
                // first time visit
                parent[v.first] = u.first;
                parent_op[v.first] = v.second;
                dist[v.first] = new_dist;
                seen[v.first] = true;
                open.push(make_pair(v.first, new_dist + h(v.first))); 
            } else {
                // state already seen
                if (new_dist < dist[v.first]) {
                    // better path found
                    parent[v.first] = u.first;
                    parent_op[v.first] = v.second;
                    dist[v.first] = new_dist;
                    open.push(make_pair(v.first, new_dist + h(v.first)));
                }
            }
		}
    }
}


int main(int argc, char *argv[]) {
    strips_t strips;
    //fdr_t fdr;
 
    if (argc != 3) {
        fprintf(stderr, "Usage: %s problem.strips problem.fdr\n", argv[0]);
        return -1;
    }
 
    // Load the planning problem in STRIPS or FDR
    stripsRead(&strips, argv[1]);
    //fdrRead(&fdr, argv[2]);

    // Implement the search here
    a_star(strips);
    stripsFree(&strips);
    //fdrFree(&fdr);
}
