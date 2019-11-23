#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME JarvisDemoWinner


struct PLAYER_NAME : public Player {
    
    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }
    
	// Capaz de vencer a la Demo
	
    /**
     * Types and attributes for your player can be defined here.
     */
    
    void BFS_dwarves(int id) {
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
                    if (cell(p1).type == Outside or cell(p1).type == Cave) {
                        if (cell(p1).id == -1 or
                            cell(p1).treasure or
                            (cell(p1).id != -1 and
                                (unit(cell(p1).id).type == Orc or unit(cell(p1).id).player != me()))) {
                            command(id, directions[d]);
                            return;
                        }
                    } else if (cell(p1).type == Rock) {
                        command(id, directions[d]);
                    }
                }
            }
        }
    }
    
    void BFS_wizards(int id) {
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
            
            for (int d : dirR) {
                Pos p1 = p + directions[d];
                if (pos_ok(p1) and !visited[p1.i][p1.j]) {
                    visited[p1.i][p1.j] = true;
                    q.push(p1);
                    if (cell(p1).id == -1 or not cell(p1).treasure) {
                        command(id, directions[d]);
                        return;
                    } else if (cell(p1).id != -1 and unit(cell(p1).id).player == me()) {
                        command(id, None);
                        return;
                    }
                }
            }
        }
    }
    
    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        vector<int> my_dwarves = dwarves(me());
        vector<int> dwarR = random_permutation(my_dwarves.size());
        for (int d: dwarR) {
            BFS_dwarves(my_dwarves[d]);
        }
        
        vector<int> my_wizards = wizards(me());
        vector<int> wizR = random_permutation(my_wizards.size());
        for (int w: wizR) {
            BFS_wizards(my_wizards[w]);
        }
    }
    
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

