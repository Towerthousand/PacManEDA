#include "Player.hh"
#include <cmath>
#include <algorithm>

using namespace std;
#define PLAYER_NAME itsshowtime
#define FLEE_RADIUS 5.0f
typedef pair<int,Pos> distloc;
typedef vector<distloc> v_distloc;

struct PLAYER_NAME : public Player {
		static Player* factory () {return new PLAYER_NAME;}

		Dir d[4];

		enum Strategy {
			Gather = 0,
			Flee = 1,
			PillRush = 2,
			KillRush = 3,
			HammerRush = 4
		};

		virtual void play () {
			bool debug = false;
			// initialization
			if (round() == 0) {
				d[0] = Top;
				d[1] = Bottom;
				d[2] = Left;
				d[3] = Right;
			}

			/////////////////////////PACMAN
			//position scores
			Pos p = pacman(me()).pos;
			vector<double> scores(4,0);
			vector<bool> possible(4,false);
			Strategy s = chooseStrategyPac();
			for(int i = 0; i < 4; ++i) {
				if(!can_move(p,d[i],PacMan)) continue;
				possible[i] = true;
				scores[i] = scorePositionPacMan(dest(p,d[i]), s);
			}
			//minimax
			double max = -100000000000.0;
			int chosen = -1;
			for(int i = 0; i < 4; ++i) {
				if(possible[i] && max < scores[i]) {
					max = scores[i];
					chosen = i;
				}
			}
			if(debug) {
				for(unsigned int i = 0; i < scores.size(); ++i) cerr << "D:" << d2c(d[i]) << " P:" << possible[i] << " S:" << scores[i] << endl;
				if(chosen == -1)  {chosen = rand()%4; cerr << "(RAND)";};
				cerr << " CHOSEN: " << d2c(d[chosen]) << endl;
			}
			move_my_pacman(d[chosen]);

			/////////////////////////GHOSTS
			// move ghosts
			for (int ghostnum=0; ghostnum < nb_ghosts(); ++ghostnum) {
				Robot g = ghost(me(),ghostnum);
				vector<double> scores(4,0);
				vector<bool> possible(4,false);
				for(int i = 0; i < 4; ++i) {
					if(!can_move(g.pos,d[i],Ghost)) continue;
					possible[i] = true;
					v_distloc pacs = getRobotsByDistance(PacMan, dest(g.pos,d[i]), Ghost, 10, false);
					while(!pacs.empty() && cell(pacs[0].second).id == pacman(me()).id) pacs.erase(pacs.begin());
					if(pacs.empty()) possible[i] = false;
					else scores[i] = -pacs[0].first;
				}
				double max = -100000000000.0;
				int chosen = -1;
				for(int i = 0; i < 4; ++i) {
					if(possible[i] && max < scores[i]) {
						max = scores[i];
						chosen = i;
					}
				}
				if(chosen == -1) chosen = rand()%4;
				move_my_ghost(ghostnum,d[chosen]);
			}
		}

		Strategy chooseStrategyPac() {
			Robot p = pacman(me());
			distloc closestHammer = getClosestCell(Hammer, p.pos, p.type, false);
			if(closestHammer.first >= 0) {
				v_distloc v(3);
				v[0] = getClosestRobot(PacMan,closestHammer.second,PacMan,true);
				v[1] = getClosestRobot(Ghost,closestHammer.second,Ghost,true);
				v[2] = getClosestRobot(PowerPacMan,closestHammer.second,PowerPacMan,true);
				if(v[2].first > 1) v[2].first /= 2;
				sort(v.begin(),v.end());
				while(!v.empty() && v[0].first < 0) v.erase(v.begin());
				if(!v.empty() && v[0].second == p.pos) return HammerRush;
			}
			v_distloc gs = getRobotsByDistance(Ghost, p.pos, p.type, nb_ghosts()+1, false);
			while(!gs.empty() && robot(cell(gs[0].second).id).player == me()) gs.erase(gs.begin());
			if(!gs.empty() && p.type == PowerPacMan && gs[0].first < 20) return KillRush;
			distloc closestPill = getClosestCell(Pill, p.pos, p.type, false);
			if(closestPill.first >= 0 && closestPill.first < 15) {
				v_distloc v(3);
				v[0] = getClosestRobot(PacMan,closestPill.second,PacMan,true);
				v[1] = getClosestRobot(Ghost,closestPill.second,Ghost,true);
				v[2] = getClosestRobot(PowerPacMan,closestPill.second,PowerPacMan,true);
				if(v[2].first > 1) v[2].first /= 2;
				sort(v.begin(),v.end());
				while(!v.empty() && v[0].first < 0) v.erase(v.begin());
				if(!v.empty() && v[0].second == p.pos) return PillRush;
			}
			if(!gs.empty() && gs[0].first < 5) return Flee;
			return Gather;
		}

		double scorePositionPacMan(Pos p, Strategy s) {
			switch(s) {
				case Flee:{
					float f_contrib = 1-(float(getClosestRobot(Ghost,pacman(me()).pos,Ghost, true).first)/FLEE_RADIUS);
					//cerr << "FLEE (" << f_contrib << ")" << endl;
					return  scorePositionPacManGather(p)*(1-f_contrib)+scorePositionPacManFlee(p)*f_contrib;
				}
				case Gather:
					//cerr << "GATHER" << endl;
					return scorePositionPacManGather(p);
				case PillRush: {
					//cerr << "PILLRUSH" << endl;
					double dist = getClosestCell(Pill, p, PacMan, false).first;
					return (dist == -1? -100000000 : -dist);
				}
				case HammerRush: {
					//cerr << "HAMMERRUSH" << endl;
					double dist = getClosestCell(Hammer , p, PacMan, false).first;
					return (dist == -1? -100000000 : -dist);
				}
				case KillRush: {
					//cerr << "KILLRUSH" << endl;
					v_distloc gs = getRobotsByDistance(Ghost, p, PacMan, nb_ghosts()+1, false);
					while(!gs.empty() && robot(cell(gs[0].second).id).player == me()) gs.erase(gs.begin());
					return -gs[0].first;
				}
			}
			return 0.0f;
		}

		double scorePositionPacManGather(Pos p) {
			double currentScore = 0;
			Robot pac = pacman(me());

			v_distloc hammers = getCellsByDistance(Hammer, p, pac.type, 5, false);
			if(!hammers.empty()) currentScore -= hammers[0].first*100;
			else currentScore -= -100000;

			v_distloc dots = getCellsByDistance(Dot, p, pac.type, 5, false);
			if(!dots.empty()) currentScore -= dots[0].first;
			else currentScore -= 100;

			return currentScore;
		}

		double scorePositionPacManFlee(Pos p) {
			double currentScore = 0;
			v_distloc ghosts = getRobotsByDistance(Ghost, p, Ghost, nb_ghosts()*nb_players(), true);
			for(unsigned int i = 0; i < ghosts.size() && i < 1; ++i) {
				currentScore -= ghosts[i].first;
			}
			return currentScore;
		}

		v_distloc getCellsByDistance(CType target, Pos o, RType bot, unsigned int max, bool fly) {
			vector<vector<unsigned int> > distances(rows(),vector<unsigned int>(cols(), maxint));
			vector<vector<bool> > visited(rows(),vector<bool>(cols(), false));
			queue<Pos> q;
			visited[o.i][o.j] = true;
			distances[o.i][o.j] = 0;
			q.push(o);
			v_distloc res;
			while(!q.empty() && res.size() < max) {
				Pos c = q.front();
				q.pop();
				if(cell(c).type == target) res.push_back(distloc(distances[c.i][c.j],c));
				for(int i = 0; i < 4; ++i) {
					Pos nPos = dest(c, d[i]);
					if(visited[nPos.i][nPos.j]
					   || (!fly && !can_move(c,d[i],bot))
						   || (fly && (cell(c).type == Wall || (bot != Ghost && cell(c).type == Gate)))
						   )
					   continue;
					distances[nPos.i][nPos.j] = distances[c.i][c.j]+1;
					visited[nPos.i][nPos.j] = true;
					q.push(nPos);
				}
			}
			return res;
		}

		v_distloc getRobotsByDistance(RType target, Pos o, RType bot, unsigned int max, bool fly) {
			vector<vector<unsigned int> > distances(rows(),vector<unsigned int>(cols(), maxint));
			vector<vector<bool> > visited(rows(),vector<bool>(cols(), false));
			queue<Pos> q;
			visited[o.i][o.j] = true;
			distances[o.i][o.j] = 0;
			q.push(o);
			v_distloc res;
			while(!q.empty() && res.size() < max) {
				Pos c = q.front();
				q.pop();
				if(cell(c).id != -1 && robot(cell(c).id).type == target) res.push_back(distloc(distances[c.i][c.j],c));
				for(int i = 0; i < 4; ++i) {
					Pos nPos = dest(c, d[i]);
					if(visited[nPos.i][nPos.j]
					   || (!fly && !can_move(c,d[i],bot))
						   || (fly && (cell(c).type == Wall || (bot != Ghost && cell(c).type == Gate)))
						   )
					   continue;
					distances[nPos.i][nPos.j] = distances[c.i][c.j]+1;
					visited[nPos.i][nPos.j] = true;
					q.push(nPos);
				}
			}
			return res;
		}

		distloc getClosestCell(CType target, Pos o, RType bot, bool fly) {
			pair<int, Pos> res(-1,Pos(-1,-1));
			vector<vector<unsigned int> > distances(rows(),vector<unsigned int>(cols(), maxint));
			vector<vector<bool> > visited(rows(),vector<bool>(cols(), false));
			queue<Pos> q;
			visited[o.i][o.j] = true;
			distances[o.i][o.j] = 0;
			q.push(o);
			bool found = false;
			while(!(q.empty() || found)) {
				Pos c = q.front();
				q.pop();
				if(cell(c).type == target) {
					found = true;
					res = distloc(distances[c.i][c.j],c);
					continue;
				}
				for(int i = 0; i < 4; ++i) {
					Pos nPos = dest(c, d[i]);
					if(visited[nPos.i][nPos.j]
					   || (!fly && !can_move(c,d[i],bot))
						   || (fly && (cell(c).type == Wall || (bot != Ghost && cell(c).type == Gate)))
						   )
					   continue;
					distances[nPos.i][nPos.j] = distances[c.i][c.j]+1;
					visited[nPos.i][nPos.j] = true;
					q.push(nPos);
				}
			}
			return res;
		}

		distloc getClosestRobot(RType target, Pos o, RType bot, bool fly) {
			pair<int, Pos> res(-1,Pos(-1,-1));
			vector<vector<unsigned int> > distances(rows(),vector<unsigned int>(cols(), 1 << 20));
			vector<vector<bool> > visited(rows(),vector<bool>(cols(), false));
			queue<Pos> q;
			visited[o.i][o.j] = true;
			distances[o.i][o.j] = 0;
			q.push(o);
			bool found = false;
			while(!(q.empty() || found)) {
				Pos c = q.front();
				q.pop();
				if(cell(c).id != -1 && robot(cell(c).id).type == target) {
					found = true;
					res = distloc(distances[c.i][c.j],c);
					continue;
				}
				for(int i = 0; i < 4; ++i) {
					Pos nPos = dest(c, d[i]);
					if(visited[nPos.i][nPos.j]
					   || (!fly && !can_move(c,d[i],bot))
						   || (fly && (cell(c).type == Wall || (bot != Ghost && cell(c).type == Gate)))
						   )
					   continue;
					distances[nPos.i][nPos.j] = distances[c.i][c.j]+1;
					visited[nPos.i][nPos.j] = true;
					q.push(nPos);
				}
			}
			return res;
		}

		inline bool can_move(Pos p, Dir d, RType r) {
			return (r == Ghost)? ghost_can_move(p,d) : pac_can_move(p,d);
		}


		inline bool pac_can_move (Pos p, Dir d) {
			CType t = cell(dest(p, d)).type;
			if (t == Wall or t == Gate) return false;
			if (pacman(me()).type == PacMan and cell(dest(p, d)).id != -1) return false;
			return true;
		}

		inline bool ghost_can_move (Pos p, Dir d) {
			CType t = cell(dest(p, d)).type;
			if (t == Wall) return false;
			int r = cell(dest(p, d)).id;
			if (r == -1) return true;
			if (robot(r).type==Ghost) return false;
			return true;
		}

		Dir rand_dir (Dir notd) {
			Dir a[] = {Left, Right, Bottom, Top};
			while (true) {
				Dir d = a[rand() % 4];
				if (d != notd) return d;
			}
		}

};

RegisterPlayer(PLAYER_NAME);
