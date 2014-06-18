#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>

/*
 * Point Location:
 *      get a triangulation
 *      find independent set of points
 *      delete points from independent set to create holes in triangulation
 *      retriangulate the holes
 *          have new triangulations point to previous triangulations if they overlap
 *      repeat until there is only the outer triangle left
 */

class Node {

    public:
        std::vector<int> neighbors;

        explicit Node() {}
        explicit Node(const Node &n) : neighbors(n.neighbors) {}
        ~Node();

        void addNeighbor(int n) {
            if(std::find(neighbors.begin(), neighbors.end(), n) == neighbors.end()) {
                neighbors.push_back(n);
            }
        }

        void removeNeighbor(int n) {
            neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), n), neighbors.end());
        }

};

class Triangle {

    public:
        int m_label;
        int m_a;
        int m_b;
        int m_c;
        std::vector<Triangle *> subTriangles;

        explicit Triangle() {}
        explicit Triangle(int label, int a, int b, int c) : m_label(label),
        m_a(a),
        m_b(b),
        m_c(c) {}
        explicit Triangle(const Triangle &t) : m_label(t.m_label), 
        m_a(t.m_a), 
        m_b(t.m_b),
        m_c(t.m_c) {}

        bool containsVertex(int v) {
            return (m_a == v || m_b == v || m_c == v);
        }

        void addSubface(Triangle *t) {
            subTriangles.push_back(t);
        }

};

class ploc_t {

    public:
        std::vector<std::vector<int> > originalPoints;
        std::vector<Node *> m_vertices;
        std::vector<std::vector<Triangle *> > m_triangles;

        explicit ploc_t() {}
        explicit ploc_t(std::vector<std::vector<int> > points, std::vector<std::vector<int> > triangles, int n, int m) : originalPoints(points) {
            init(points, triangles, n, m);

            // triangulation refinement
            // loop until last triangulation has only 1 triangle
            std::vector<std::vector<int> > holes;
            // holds the indices for triangles to be deleted
            std::vector<std::vector<Triangle *> > deletedTr;
            // pointInfo - holds information as to whether a point needs to be deleted
            // and if it can be added to the independent set
            std::vector<char> pointInfo;

            std::vector<int> independentSet;
            pointInfo = getIndependentSet(holes, /*deletedTr, */n, independentSet);

            deletedTr = deleteIS(pointInfo);

            // retriangulate
            retriangulate(deletedTr, independentSet, pointInfo, holes);

            //printTr();
        }

        // initializes the ploc structure with original points and triangles
        void init(std::vector<std::vector<int> > points, std::vector<std::vector<int> > triangles, int n, int m) {
            for(int i = 0; i < n; i++) {
                m_vertices.push_back(new Node());
            }

            std::vector<Triangle *> v_triangles;
            for(int i = 0; i < m; i++) {
                const int a = triangles[i][0];
                const int b = triangles[i][1];
                const int c = triangles[i][2];

                v_triangles.push_back(new Triangle(i, a, b, c));

                // add neighbors
                m_vertices[a]->addNeighbor(b);
                m_vertices[a]->addNeighbor(c);
                m_vertices[b]->addNeighbor(a);
                m_vertices[b]->addNeighbor(c);
                m_vertices[c]->addNeighbor(a);
                m_vertices[c]->addNeighbor(b);
            }

            m_triangles.push_back(v_triangles);
        }

        std::vector<char> getIndependentSet(std::vector<std::vector<int> > &holes, int n, std::vector<int> &independentSet) {
            // badVertices - 1 = cannot add to the independent set
            //               2 = point to be deleted
            std::vector<char> pointInfo(m_vertices.size());
            std::fill_n(pointInfo.begin(), n, 0);

            // find points for independent set
            // for each point, get its neighbors as a hole
            for(int i = 0; i < m_vertices.size() - 3; i++) {
                if(m_vertices[i] != NULL 
                        && pointInfo[i] == 0 
                        && m_vertices[i]->neighbors.size() <= 6) {
                    pointInfo[i] = 2;
                    std::vector<int> hole;
                    for(int j = 0; j < m_vertices[i]->neighbors.size(); j++) {
                        pointInfo[m_vertices[i]->neighbors[j]] = 1;
                        hole.push_back(m_vertices[i]->neighbors[j]);
                    }
                    independentSet.push_back(i);
                    m_vertices[i] = NULL;
                    holes.push_back(hole);

                }
            }

            return pointInfo;
        }

        // deletes points from the independent set
        // returns deleted triangles indexed by the independent point that caused them to be deleted
        std::vector<std::vector<Triangle *> > deleteIS(std::vector<char> &pointInfo) {
            std::vector<Triangle *> prevTr = m_triangles.back();
            int m = prevTr.size();
            std::vector<std::vector<Triangle *> > deletedTr(originalPoints.size());

            for(int i = 0; i < m; i++) {
                if(pointInfo[prevTr[i]->m_a] == 2) {
                    m_vertices[prevTr[i]->m_b]->removeNeighbor(prevTr[i]->m_a);
                    m_vertices[prevTr[i]->m_c]->removeNeighbor(prevTr[i]->m_a);
                    deletedTr[prevTr[i]->m_a].push_back(prevTr[i]);
                } else if(pointInfo[prevTr[i]->m_b] == 2) {
                    m_vertices[prevTr[i]->m_a]->removeNeighbor(prevTr[i]->m_b);
                    m_vertices[prevTr[i]->m_c]->removeNeighbor(prevTr[i]->m_b);
                    deletedTr[prevTr[i]->m_b].push_back(prevTr[i]);
                } else if(pointInfo[prevTr[i]->m_c] == 2) {
                    m_vertices[prevTr[i]->m_a]->removeNeighbor(prevTr[i]->m_c);
                    m_vertices[prevTr[i]->m_b]->removeNeighbor(prevTr[i]->m_c);
                    deletedTr[prevTr[i]->m_c].push_back(prevTr[i]);
                }
            }

            return deletedTr;
        }

        // hole of size 3 - already a triangle, just add the 3 subfaces
        //
        void retriangulate(std::vector<std::vector<Triangle *> > &deletedTr, std::vector<int> &independentSet, std::vector<char> &pointInfo, std::vector<std::vector<int> > holes) {
            std::vector<Triangle *> newTriangles;
            for(int i = 0; i < independentSet.size(); i++) {
                int holeSize = holes[i].size();
                int delTrSize = deletedTr[independentSet[i]].size();

                if(holeSize == 3) {
                    Triangle *newTriangle = new Triangle(i, holes[i][0], holes[i][1], holes[i][2]);
                    newTriangle->addSubface(deletedTr[independentSet[i]][0]);
                    newTriangle->addSubface(deletedTr[independentSet[i]][1]);
                    newTriangle->addSubface(deletedTr[independentSet[i]][2]);
                    newTriangles.push_back(newTriangle);
                } else if(holeSize > 3) {
                    for(int j = 0; j < holeSize; j++) {
                        bool pitest = false;
                        bool itest = false;
                        for(int k = j; k < holeSize; k++) {
                            // points j and k are not the same, and not neighbors
                            // cannot add diagonal to neighbor to create triangles
                            if(j != k
                                    && std::find(m_vertices[holes[i][j]]->neighbors.begin(),
                                        m_vertices[holes[i][j]]->neighbors.end(),
                                        holes[i][k]) == m_vertices[holes[i][j]]->neighbors.end()) {
                                //std::cout << "\nIteration: " << i << ". Considering diagonal: " << holes[i][j] << " " << holes[i][k] << std::endl;
                                // check condition 1: if midpoint of diagonal is in any of previous triangles
                                cond1(holes[i], deletedTr[independentSet[i]], pitest, j, k, delTrSize);
                                // if does not pass point in triangle test, continue to next k
                                if(!pitest) continue;

                                // check condition 2: if diagonal intersects any non-adjacient line to j
                                cond2(holes[i], itest, j, k, holeSize);
                                // if there is intersection, continue to next k
                                if(itest) continue;
                                
                                // can triangulate, all tests passed
                                // add diagonal from j to k
                                // create new triangles and add them to new triangles vector
                                triangulate(holes[i], j, k, deletedTr[independentSet[i]], newTriangles);
                            }
                        }
                    }
                } else {
                    std::cout << "\nDEBUG: hole size = " << holeSize << std::endl;
                }
            }
            m_triangles.push_back(newTriangles);
        }

        void triangulate(std::vector<int> hole, int j, int k, std::vector<Triangle *> delTr, std::vector<Triangle *> &newTriangles) {
            m_vertices[hole[j]]->addNeighbor(hole[k]);
            m_vertices[hole[k]]->addNeighbor(hole[j]);

            for(int ni = 0; ni < hole.size(); ni++) {
                // find same neighbor, ni of j and k to create triangle ni j k
                if(std::find(m_vertices[hole[j]]->neighbors.begin(), 
                            m_vertices[hole[j]]->neighbors.end(),
                            hole[ni]) != m_vertices[hole[j]]->neighbors.end()
                        && std::find(m_vertices[hole[k]]->neighbors.begin(),
                            m_vertices[hole[k]]->neighbors.end(),
                            hole[ni]) != m_vertices[hole[k]]->neighbors.end()) {
                    Triangle *newTriangle = new Triangle(ni, hole[j], hole[k], hole[ni]);
                    
                    // add sub triangles
                    for(int dtri = 0; dtri < delTr.size(); dtri++) {
                        // when do triangles overlap?
                        // either all previous 3 points are on lines or
                        // any previous line intersects a line
                        Triangle *t = delTr[dtri];
                        if(pit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                               originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                               originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                               originalPoints[t->m_a][0], originalPoints[t->m_a][1], true)
                            && pit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                   originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                   originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                   originalPoints[t->m_b][0], originalPoints[t->m_b][1], true)
                            && pit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                   originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                   originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                   originalPoints[t->m_c][0], originalPoints[t->m_c][1], true)) {
                            newTriangle->addSubface(t);
                        }
                        else if((lit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                 originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                 originalPoints[t->m_a][0], originalPoints[t->m_a][1],
                                 originalPoints[t->m_b][0], originalPoints[t->m_b][1], true)
                             || lit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                    originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                    originalPoints[t->m_a][0], originalPoints[t->m_a][1],
                                    originalPoints[t->m_c][0], originalPoints[t->m_c][1], true)
                             || lit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                    originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                    originalPoints[t->m_b][0], originalPoints[t->m_b][1],
                                    originalPoints[t->m_c][0], originalPoints[t->m_c][1], true)
                             || lit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                    originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                    originalPoints[t->m_a][0], originalPoints[t->m_a][1],
                                    originalPoints[t->m_b][0], originalPoints[t->m_b][1], true)
                             || lit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                    originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                    originalPoints[t->m_a][0], originalPoints[t->m_a][1],
                                    originalPoints[t->m_c][0], originalPoints[t->m_c][1], true)
                             || lit(originalPoints[hole[j]][0], originalPoints[hole[j]][1],
                                    originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                    originalPoints[t->m_b][0], originalPoints[t->m_b][1],
                                    originalPoints[t->m_c][0], originalPoints[t->m_c][1], true)
                             || lit(originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                    originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                    originalPoints[t->m_a][0], originalPoints[t->m_a][1],
                                    originalPoints[t->m_b][0], originalPoints[t->m_b][1], true)
                             || lit(originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                    originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                    originalPoints[t->m_a][0], originalPoints[t->m_a][1],
                                    originalPoints[t->m_c][0], originalPoints[t->m_c][1], true)
                             || lit(originalPoints[hole[k]][0], originalPoints[hole[k]][1],
                                    originalPoints[hole[ni]][0], originalPoints[hole[ni]][1],
                                    originalPoints[t->m_b][0], originalPoints[t->m_b][1],
                                    originalPoints[t->m_c][0], originalPoints[t->m_c][1], true))) {
                            newTriangle->addSubface(t);
                        }
                    }
                    newTriangles.push_back(newTriangle);
                }
            }
        }

        void cond1(std::vector<int> hole, std::vector<Triangle *> tr, bool &pitest, int j, int k, int delTrSize) {
            float midx, midy;
            midx = (static_cast<float>(originalPoints[hole[j]][0]) + static_cast<float>(originalPoints[hole[k]][0])) / 2;
            midy = (static_cast<float>(originalPoints[hole[j]][1]) + static_cast<float>(originalPoints[hole[k]][1])) / 2;
            for(int dtr = 0; dtr < delTrSize; dtr++) {
                Triangle *tri = tr[dtr];
                if(pit(originalPoints[tri->m_a][0], originalPoints[tri->m_a][1],
                            originalPoints[tri->m_b][0], originalPoints[tri->m_b][1],
                            originalPoints[tri->m_c][0], originalPoints[tri->m_c][1],
                            midx, midy, true)) {
                    pitest = true;
                    return;
                }
            }
        }

        void cond2(std::vector<int> hole, bool &itest, int j, int k, int holeSize) {
            // find points inti that is not j
            // find neighbors of inti, intj that is not j
            // check if line j-k intersects inti-intj
            for(int inti = 0; inti < holeSize; inti++) {
                if(inti != j) {
                    for(int intj = 0; intj < holeSize; intj++) {
                        if(intj != inti && intj != j 
                                && std::find(m_vertices[hole[inti]]->neighbors.begin(),
                                    m_vertices[hole[inti]]->neighbors.end(),
                                    hole[intj]) != m_vertices[hole[inti]]->neighbors.end()) {
                            if(lit(originalPoints[hole[j]][0],
                                        originalPoints[hole[j]][1],
                                        originalPoints[hole[k]][0],
                                        originalPoints[hole[k]][1],
                                        originalPoints[hole[inti]][0],
                                        originalPoints[hole[inti]][1],
                                        originalPoints[hole[intj]][0],
                                        originalPoints[hole[intj]][1])) {
                                itest = true;
                                return;
                            }
                        }
                    }
                }
            }
        }

        // point in triangle test
        // edge == false : point on edge is not in triangle
        // edge == true  : point on edge is in triangle
        bool pit(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y, bool edge = false) { 
            float alpha = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 -  y3)); 
            float beta = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 -   y3)); 
            float gamma = 1.0f - alpha - beta;
            if(edge == true) {
                return (alpha >= 0) && (beta >= 0) && (gamma >= 0);
            }
            return (alpha > 0) && (beta > 0) && (gamma > 0);
        }

        // point on line test
        bool pol(float x1, float y1, float x2, float y2, float x, float y) {
            float a = (y2-y1) / (x2 - x1);
            float b = y1 - a * x1;
            if(std::abs(y1 - (a * x + b)) < 0.001) {
                float maxx = std::max(x1,x2);
                float minx = std::min(x1,x2);
                float maxy = std::max(y1,y2);
                float miny = std::min(y1,y2);
                if(x >= minx && x <= maxx && y >= miny && y <= maxy)
                    return true;
            }
            return false;
        }

        // line intersection test
        // if touching a vertex, not intersecting
        // but if same slope and y-intercept, it is same line and is intersecting
        // test for same line intersecting if sl == false
        //      do not use for checking whether midpoint is part of triangle for condition 1
        bool lit(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float    p3_y, bool sl = false)
        {
            float s1_x, s1_y, s2_x, s2_y;
            s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
            s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

            float s, t;
            s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
            t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

            float slope1 = (p1_y - p0_y) / (p1_x - p0_x);
            float yint1 = p0_y - slope1 * p0_x;
            float slope2 = (p3_y - p2_y) / (p3_x - p2_x);
            float yint2 = p2_y - slope2 * p2_x;
            if(!sl) {
                if(std::abs(slope1 - slope2) < 0.001 && std::abs(yint1 - yint2) < 0.001) return true;
            }
            return (s > 0 && s < 1 && t > 0 && t < 1);
        }

        void printTr() {
            for(int i = 0; i < m_triangles.size(); i++) {
                std::cout << "\nIteration: " << i << std::endl;
                for(int j = 0; j < m_triangles[i].size(); j++) {
                    std::cout << m_triangles[i][j]->m_label << " " << m_triangles[i][j]->m_a << " " << m_triangles[i][j]->m_b << " " << m_triangles[i][j]->m_c << std::endl;
                    if(m_triangles[i][j]->subTriangles.size() > 0) {
                        for(int k = 0; k < m_triangles[i][j]->subTriangles.size(); k++) {
                            std::cout << "Sub-triangle: " << m_triangles[i][j]->subTriangles[k]->m_label << " " << m_triangles[i][j]->subTriangles[k]->m_a << " " << m_triangles[i][j]->subTriangles[k]->m_b << " " << m_triangles[i][j]->subTriangles[k]->m_c << std::endl;
                        }
                    }
                }
            }
        }
};

ploc_t *create_ploc(std::vector<std::vector<int> > points, std::vector<std::vector<int> > triangles, int n, int m) {
    ploc_t *ploc = new ploc_t(points, triangles, n, m);
}


void print(std::vector<std::vector<int> > points, std::vector<std::vector<int> > triangles) {
    std::cout << "\n---------- Points ----------" << std::endl;
    for(int i = 0; i < points.size(); i++) {
        std::cout << i << ": " << points[i][0] << "\t" << points[i][1] << std::endl; 
    }

    std::cout << "\n---------- Triangles ----------" << std::endl;
    for(int i = 0; i < triangles.size(); i++) {
        std::cout << i << ": " << triangles[i][0] << "\t" << triangles[i][1] << "\t" << triangles[i][2] << std::endl;
    }

    std::cout << std::endl;
}


int main() {
    // for iterations
    int i, j, k;
    k = 0;

    // distance between 2 points
    int pointOS = 5;
    int max = 5;
    int n = max*max + 3;
    int m = (max-1)*(max-1)*2 + (max*4);
    // init points and triangles vectors
    std::vector<std::vector<int> > points(n, std::vector<int>(2, 0));
    std::vector<std::vector<int> > triangles(m, std::vector<int>(3, 0));

    // generate points
    for(i = 0; i < max; i++) {
        for(j = 0; j < max; j++) {
            points[k][0] = pointOS*i;
            points[k][1] = pointOS*j;
            k++;
        }
    }

    // create outer points for the outside triangle
    points[k][0] = 0-(pointOS*max); points[k++][1] = 0-pointOS;
    points[k][0] = pointOS*max*2;   points[k++][1] = 0-pointOS;
    points[k][0] = (pointOS*max)/2; points[k++][1] = pointOS*max*2;
    printf("Prepared %d points\n", k);

    // generate triangles
    k = 0;
    // indices of outer points
    int outer1 = max*max;
    int outer2 = max*max + 1;
    int outer3 = max*max + 2;

    triangles[k][0] = outer1;
    triangles[k][1] = outer2;
    triangles[k++][2] = outer3;
    for(i = 0; i < max - 1; i++) {
        triangles[k][0] = i;
        triangles[k][1] = i + 1;
        triangles[k++][2] = outer1;
    }

    triangles[k][0] = max - 1;
    triangles[k][1] = outer1;
    triangles[k++][2] = outer3;
    for(i = 0; i < max - 1; i++) {
        triangles[k][0] = max-1 + max*i;
        triangles[k][1] = max-1 + max*(i+1);
        triangles[k++][2] = outer3;
    }

    triangles[k][0] = outer3;
    triangles[k][1] = outer1 - 1;
    triangles[k++][2] = outer2;
    for(i = 0; i < max - 1; i++) {
        triangles[k][0] = (max-1)*max + i;
        triangles[k][1] = (max-1)*max + i + 1;
        triangles[k++][2] = outer2;
    }

    triangles[k][0] = outer1;
    triangles[k][1] = outer1 - max;
    triangles[k++][2] = outer2;
    for(i = 0; i < max - 1; i++) {
        triangles[k][0] = max * i;
        triangles[k][1] = max * (i+1);
        triangles[k++][2] = outer1;
    }

    for(i = 0; i < max - 1; i++) {
        for(j = 0; j < max - 1; j++) {
            triangles[k][0] = max * i + j;
            triangles[k][1] = max * (i+1) + j;
            triangles[k++][2] = max * i + j + 1;
            triangles[k][0] = max * (i+1) + j + 1;
            triangles[k][1] = max * (i+1) + j;
            triangles[k++][2] = max * i + j + 1;
        }
    }
    printf("Prepared %d triangles \n", k);

    //print(points, triangles);

    ploc_t *pl;
    pl = create_ploc(points, triangles, n, m); 
}
