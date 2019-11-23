#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Jarvis


struct PLAYER_NAME : public Player {
    
    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }
    
    /**
     * Types and attributes for your player can be defined here.
     */
    
    // MARK: - Dwarves
    
    bool dwarvesConditions(Pos p, int id) {
        Unit dwarve = unit(id);
        // Check if the position is correct
        if (pos_ok(p)) return true;
        
        // Check if the cell type is walkable by the Dwarves
        if (cell(p).type == Cave or cell(p).type == Rock) return true;
        
        // Check if it is a treasure to capture
        if (cell(p).treasure) return true;
        
        // Check if there is an enemy
        if (cell(p).id != -1) {
            Unit enemy = unit(cell(p).id);
            if (enemy.player != me()) return true;
            
            /* Safe Area for the Balrog:
             it contains all the adjacents cells to the Balrog position
             
             | --- | --- | --- |
             | TL  | Top | TR  |
             | --- | --- | --- |
             | Le  | Bal | Ri  |
             | --- | --- | --- |
             | BL  | Bot | BR  |
             | --- | --- | --- |
             
             */
            
            Pos balrogP = unit(balrog_id()).pos;
            // Attack only if the dwarve health is big enough to kill the enemy in 3 turns
            int turns = 3;
            if (dwarve.health >= (enemy.health - turns*min_damage_dwarves())) return true;
            else {
                bool safeArea = (balrogP == p and (p == (balrogP + Bottom) or
                                                   p == (balrogP + BR) or
                                                   p == (balrogP + Right) or
                                                   p == (balrogP + RT) or
                                                   p == (balrogP + Top) or
                                                   p == (balrogP + TL) or
                                                   p == (balrogP + Left) or
                                                   p == (balrogP + LB)));
                return not safeArea;
            }
        }
        
        return false;
    }
    
    Dir BFS_dwarves(int id) {
        Pos origen = unit(id).pos;
        
        vector< vector<bool> > visited(rows(), vector<bool>(cols(), false));
        vector<Dir> directions = {Bottom, BR, Right, RT, Top, TL, Left, LB};
        vector<int> dirR = random_permutation(directions.size());
        
        queue<Pos> q;
        
        visited[origen.i][origen.j] = true;
        q.push(origen);
        
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            
            for (int d : dirR) {
                Pos p1 = p + directions[d];
                if (pos_ok(p1) and !visited[p1.i][p1.j]) {
                    visited[p1.i][p1.j] = true;
                    q.push(p1);
                    if (dwarvesConditions(p1, id)) {
                        return directions[d];
                    }
                }
            }
        }
        
        return directions[random(0, (directions.size() - 1))]
    }
    
    // MARK: - Wizards
    bool wizardsConditions(Pos p, int id) {
        Unit wizard = unit(id);
        // Check if the position is correct
        if (pos_ok(p)) return true;
        
        // Check if the cell type is walkable by the Wizards
        if (cell(p).type == Cave) return true;
        
        // Check if there is an enemy
        if (cell(p).id != -1) {
            Unit enemy = unit(cell(p).id);
            // Go only if the "enemy" is mine, if not we stablish a Safe Area
            if (enemy.player == me()) return true;
            else {
                /* Safe Area:
                 it contains all the adjacents cells to the enemy position
                 
                 | --- | --- | --- |
                 | TL  | Top | TR  |
                 | --- | --- | --- |
                 | Le  | Ene | Ri  |
                 | --- | --- | --- |
                 | BL  | Bot | BR  |
                 | --- | --- | --- |
                 
                 */
                
                Pos enemyP = unit(enemy.id).pos;
                bool safeArea = (enemyP == p and
                                 (p == (enemyP + Bottom) or
                                  p == (enemyP + BR) or
                                  p == (enemyP + Right) or
                                  p == (enemyP + RT) or
                                  p == (enemyP + Top) or
                                  p == (enemyP + TL) or
                                  p == (enemyP + Left) or
                                  p == (enemyP + LB)));
                
                return safeArea;
            }
        }
        
        return false;
    }
    
    Dir BFS_wizards(int id) {
        Pos origen = unit(id).pos;
        
        vector< vector<bool> > visited(rows(), vector<bool>(cols(), false));
        vector<Dir> directions = {Bottom, Right, Top, Left};
        vector<int> dirR = random_permutation(directions.size());
        
        queue<Pos> q;
        
        visited[origen.i][origen.j] = true;
        q.push(origen);
        
        while (not q.empty()) {
            Pos p = q.front();
            q.pop();
            
            for (Dir d : directions) {
                Pos p1 = p + d;
                if (pos_ok(p1) and !visited[p1.i][p1.j]) {
                    visited[p1.i][p1.j] = true;
                    q.push(p1);
                    if (wizardsConditions(p1, id)) {
                        return d;
                    }
                }
            }
        }
        
        return directions[random(0, (directions.size() - 1))]
    }
    
    
    // MARK: - Play
    
    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        vector<int> my_dwarves = dwarves(me());
        vector<int> dwarR = random_permutation(my_dwarves.size());
        for (int d: dwarR) {
            Dir go = BFS_dwarves(my_dwarves[d]);
            if (dwarvesConditions((unit(my_dwarves[d]).pos + go), my_dwarves[d])) {
                command(my_dwarves[d], go);
            } else {
                vector<Dir> directions = {Bottom, BR, Right, RT, Top, TL, Left, LB};
                vector<int> dirR = random_permutation(directions.size());
                for (int i: dirR) {
                    if (dwarvesConditions((unit(my_dwarves[d]).pos + directions[i]), my_dwarves[d])) {
                        command(my_dwarves[d], directions[i]);
                    }
                }
            }
        }
        
        vector<int> my_wizards = wizards(me());
        vector<int> wizR = random_permutation(my_wizards.size());
        for (int w: my_wizards) {
            Dir go = BFS_wizards(w);
            if (wizardsConditions((unit(w).pos + go), w)) {
                command(w, go);
            } else {
                vector<Dir> directions = {Bottom, Right, Top, Left};
                vector<int> dirR = random_permutation(directions.size());
                for (int i: dirR) {
                    if (wizardsConditions((unit(w).pos + directions[i]), w)) {
                        command(w, directions[i]);
                    }
                }
            }
        }
    }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

