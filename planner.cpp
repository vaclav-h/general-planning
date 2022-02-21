#include <stdio.h>
#include "problem.h"
#include<bits/stdc++.h>
using namespace std;


struct compare_pri {
    int operator() (const pair<set<int>, int> &p1, const pair<set<int>, int> &p2) {
        return p1.second > p2.second;
    }
};

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
 * Returns the weight of current optimal path from initial state to u
 *
 */
int g(set<int> &u, map<set<int>, int> &dist) {
    if (dist.find(u) == dist.end()) {
        dist[u] = INT_MAX;
        return dist[u];
    } else {
        return dist[u]; 
    }
}



void improve(strips_t &strips, set<int> &u, pair<set<int>, int> &v, priority_queue<qpair, vector<qpair>, compare_pri> &open,
                map<set<int>,bool> &in_queue, set<set<int>> &closed, map<set<int>, int> &dist) {
    if (in_queue[v.first]) {
        if (g(u, dist) + strips.operators[v.second].cost < g(v.first, dist)) {
            //TODO 
        }
    // if v is closed
    } else if (closed.find(v.first) != closed.end()) {
        if (g(u, dist) + strips.operators[v.second].cost < g(v.first, dist)) {
            //TODO 
        }
    } else {
        //TODO
    }
}



/**
 * The A* algorithm
 *
 */
void a_star(strips_t &strips) {
    // Initialize structures
    set<set<int>> closed; 
    priority_queue<qpair, vector<qpair>, compare_pri> open; // pair<state, f_value>
	map<set<int>, set<int>> parent; //pointers to the parent nodes
	map<set<int>, int> parent_op; //operator applied in parent node
    map<set<int>, int> dist; //map for string the g_values
    map<set<int>, bool> in_queue; // indicator if state is in queue
    
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
    in_queue[init] = true;
    dist[init] = 0;
    // Main loop
    qpair u;
    while(!open.empty()) {
        u = open.top();
        open.pop();
        closed.insert(u.first);
		if (is_goal(u.first, goal)) {
			//return plan
		} else {
			vector<pair<set<int>, int>> succ = generate_succ(strips, u.first);
			for (pair<set<int>, int> v : succ) {
				improve(strips, u.first, v, open, in_queue, closed, dist);	
			}	
		}
    }
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
    
    a_star(strips);
    stripsFree(&strips);
    //fdrFree(&fdr);
}
