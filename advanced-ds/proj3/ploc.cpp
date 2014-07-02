/*
 * Zheng He
 */

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

// Solution to having a larger triangle encompass a smaller one
// use cond1

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
            while(m_triangles.back().size() >= 3) {
                std::vector<std::vector<int> > holes;
                // holds the indices for triangles to be deleted
                std::vector<std::vector<Triangle *> > deletedTr;
                // pointInfo - holds information as to whether a point needs to be deleted
                // and if it can be added to the independent set
                std::vector<char> pointInfo;

                std::vector<int> independentSet;
                pointInfo = getIndependentSet(holes, n, independentSet);

                deletedTr = deleteIS(pointInfo);

                // retriangulate
                retriangulate(deletedTr, independentSet, pointInfo, holes);
            }
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
            std::vector<Triangle *> newTriangles;

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
                } else {
                    newTriangles.push_back(prevTr[i]);
                }
            }

            for(int i = 0; i < m_vertices.size(); i++) {
                if(pointInfo[i] == 2 && m_vertices[i] != NULL) {
                    m_vertices[i] = NULL;
                }
            }

            m_triangles.push_back(newTriangles);

            return deletedTr;
        }

        // hole of size 3 - already a triangle, just add the 3 subfaces
        //
        void retriangulate(std::vector<std::vector<Triangle *> > &deletedTr, std::vector<int> &independentSet, std::vector<char> &pointInfo, std::vector<std::vector<int> > holes) {
            for(int i = 0; i < independentSet.size(); i++) {
                int holeSize = holes[i].size();
                int delTrSize = deletedTr[independentSet[i]].size();
                // hole size n = n - 2 triangulations

                if(holeSize == 3) {
                    Triangle *newTriangle = new Triangle(i, holes[i][0], holes[i][1], holes[i][2]);
                    newTriangle->addSubface(deletedTr[independentSet[i]][0]);
                    newTriangle->addSubface(deletedTr[independentSet[i]][1]);
                    newTriangle->addSubface(deletedTr[independentSet[i]][2]);
                    m_triangles.back().push_back(newTriangle);
                } else if(holeSize > 3) {
                    for(int j = 0; j < holeSize; j++) {
                        for(int k = 0; k < holeSize; k++) {
                            // points j and k are not the same, and not neighbors
                            // cannot add diagonal to neighbor to create triangles
                            bool pitest = false;
                            bool itest = false;
                            if(j != k
                                    && std::find(m_vertices[holes[i][j]]->neighbors.begin(),
                                        m_vertices[holes[i][j]]->neighbors.end(),
                                        holes[i][k]) == m_vertices[holes[i][j]]->neighbors.end()) {
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
                                triangulate(holes[i], j, k, deletedTr[independentSet[i]]);
                            }
                        }
                    }
                }
            }
        }

        void triangulate(std::vector<int> hole, int j, int k, std::vector<Triangle *> delTr) {
            for(int ni = 0; ni < hole.size(); ni++) {
                // find same neighbor, ni of j and k to create triangle ni j k
                if(std::find(m_vertices[hole[j]]->neighbors.begin(), 
                            m_vertices[hole[j]]->neighbors.end(),
                            hole[ni]) != m_vertices[hole[j]]->neighbors.end()
                        && std::find(m_vertices[hole[k]]->neighbors.begin(),
                            m_vertices[hole[k]]->neighbors.end(),
                            hole[ni]) != m_vertices[hole[k]]->neighbors.end()
                        && std::find(hole.begin(), hole.end(), hole[ni]) != hole.end()) {
                    bool midpit = false;
                    cond1(hole, delTr, midpit, j, ni, delTr.size());
                    if(!midpit) continue;
                    midpit = false;
                    cond1(hole, delTr, midpit, k, ni, delTr.size());
                    if(!midpit) continue;
                    Triangle *newTriangle = new Triangle(ni, hole[j], hole[k], hole[ni]);
                    m_vertices[hole[j]]->addNeighbor(hole[k]);
                    m_vertices[hole[k]]->addNeighbor(hole[j]);

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
                    m_triangles.back().push_back(newTriangle);
                }
            }

        }

        int query(const float x, const float y) {
            Triangle *currTr = m_triangles.back()[1];
            while(currTr->subTriangles.size() != 0) {
                bool insideCurrTr = false;
                for(int i = 0; i < currTr->subTriangles.size(); i++) {
                    int a, b, c;
                    a = currTr->subTriangles[i]->m_a;
                    b = currTr->subTriangles[i]->m_b;
                    c = currTr->subTriangles[i]->m_c;
                    if(pit(originalPoints[a][0], originalPoints[a][1],
                                originalPoints[b][0], originalPoints[b][1],
                                originalPoints[c][0], originalPoints[c][1],
                                x, y)) {
                        insideCurrTr = true;
                        currTr = currTr->subTriangles[i];
                        break;
                    }
                }
                if(insideCurrTr == true) continue;
                for(int j = 0; j < currTr->subTriangles.size(); j++) {
                    int a, b, c;
                    a = currTr->subTriangles[j]->m_a;
                    b = currTr->subTriangles[j]->m_b;
                    c = currTr->subTriangles[j]->m_c;
                    if(pit(originalPoints[a][0], originalPoints[a][1],
                                originalPoints[b][0], originalPoints[b][1],
                                originalPoints[c][0], originalPoints[c][1],
                                x, y, true)) {
                        insideCurrTr = true;
                        currTr = currTr->subTriangles[j];
                        break;
                    }
                }
                if(insideCurrTr == false) {

                    return -1;
                }
            }

            return currTr->m_label;

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
                return pol(x1,y1,x2,y2,x,y) || pol(x1,y1,x3,y3,x,y) || pol(x2,y2,x3,y3,x,y) ||
                    (alpha >= 0 && beta >= 0 && gamma >= 0);
            }
            return (alpha > 0) && (beta > 0) && (gamma > 0);
        }

        // point on line test
        bool pol(float x1, float y1, float x2, float y2, float x, float y) {
            if((x2-x1) == 0 && x == x1 && ((y <= y1 && y >= y2) || (y >= y1 && y <= y2))) return true;
            float a = (y2-y1) / (x2 - x1);
            float b = y1 - a * x1;
            if(std::abs(y - (a * x + b)) < 0.001) {
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
        bool lit(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float    p3_y, bool sl = false)
        {
            float s1_x, s1_y, s2_x, s2_y;
            s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
            s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

            float s, t;
            s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
            t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

            if(!sl) {
                if((p1_x - p0_x) == 0 && (p3_x - p2_x) == 0 && p3_x == p1_x) return true;
                float slope1 = (p1_y - p0_y) / (p1_x - p0_x);
                float yint1 = p0_y - slope1 * p0_x;
                float slope2 = (p3_y - p2_y) / (p3_x - p2_x);
                float yint2 = p2_y - slope2 * p2_x;
                float maxx1 = std::max(p0_x, p1_x);
                float minx1 = std::min(p0_x, p1_x);
                float maxx2 = std::max(p2_x, p3_x);
                float minx2 = std::min(p2_x, p3_x);

                if(std::abs(slope1 - slope2) < 0.001 && std::abs(yint1 - yint2) < 0.001
                        && ((maxx1 == maxx2 && (minx2 >= minx1 && minx2 <= maxx1))
                            || ((minx1 == minx2 && (maxx2 >= minx1 && maxx2 <= maxx1))))) return true;
            }
            return (s > 0 && s < 1 && t > 0 && t < 1);
        }
};

ploc_t *create_ploc(std::vector<std::vector<int> > points, std::vector<std::vector<int> > triangles, int n, int m) {
    ploc_t *ploc = new ploc_t(points, triangles, n, m);
    return ploc;
}

int query_ploc(ploc_t *pl, float x, float y) {
    return pl->query(x, y);
}


int main() {
    std::vector<std::vector<int> > points(90003, std::vector<int>(2, 0));
    std::vector<std::vector<int> > triangles(180003, std::vector<int>(3,0));
    int i,j, k;
    k=0;
    for(i=0; i<300; i++)
    {  for(j=0; j<300; j++)
        {  points[k][0] = 15*i;  points[k][1] = 15*j;
            k +=1;
        }
    }
    points[k][0] = -4500;  points[k++][1] = -15;
    points[k][0] =  9000;  points[k++][1] = -15;
    points[k][0] =  2250;  points[k++][1] = 9000;
    printf("Prepared %d points\n", k);
    k=0;
    triangles[k][0]=90000;  
    triangles[k][1]=90001;  
    triangles[k++][2]=90002;
    for(i=0; i<299; i++)
    {   triangles[k][0]=i;  
        triangles[k][1]=i+1;  
        triangles[k++][2]=90000;
    }
    triangles[k][0]=299;  
    triangles[k][1]=90000;  
    triangles[k++][2]=90002;
    for(i=0; i<299; i++)
    {   triangles[k][0]=299 + 300*i;  
        triangles[k][1]=299 + 300*(i+1);  
        triangles[k++][2]=90002;
    }
    triangles[k][0]=90002;  
    triangles[k][1]=89999;  
    triangles[k++][2]=90001;
    for(i=0; i<299; i++)
    {   triangles[k][0]=299*300 +i;  
        triangles[k][1]=299*300 +i+1;  
        triangles[k++][2]=90001;
    }
    triangles[k][0]=90000;  
    triangles[k][1]=89700;  
    triangles[k++][2]=90001;
    for(i=0; i<299; i++)
    {   triangles[k][0]=300*i;  
        triangles[k][1]=300*(i+1);  
        triangles[k++][2]=90000;
    }
    for(i=0; i<299; i++)
    {  for(j=0; j<299; j++)
        {  triangles[k][0]=300*i + j;  
            triangles[k][1]=300*(i+1) + j;  
            triangles[k++][2]=300*i+j+1;
            triangles[k][0]=300*(i+1) + j+1;  
            triangles[k][1]=300*(i+1) + j;  
            triangles[k++][2]=300*i+j+1;
        }
    }
    printf("Prepared %d triangles\n", k);   

    { ploc_t *pl;
        pl = create_ploc( points, triangles, 90003, 180002); 
        for(i=0; i<10000; i++)
        {  int a,b,c,x,y, t; 
            j = (rand()%180001)+1;
            a = triangles[j][0]; b=triangles[j][1]; c=triangles[j][2];       
            x = (points[a][0]+points[b][0]+points[c][0])/3;
            y = (points[a][1]+points[b][1]+points[c][1])/3;
            t = query_ploc(pl, x,y);
            if ( t!= j )
            {  printf("Error on triangle %d, misidentified as triangle %d\n",j,t);
                printf("Point (%d,%d) should be in triangle (%d,%d), (%d,%d), (%d,%d)\n",
                        x,y, points[a][0],points[a][1],points[b][0], points[b][1],
                        points[c][0], points[c][1]);
                printf("Instead claimed in triangle (%d,%d), (%d,%d), (%d,%d)\n",
                        points[triangles[t][0]][0], points[triangles[t][0]][1],
                        points[triangles[t][1]][0], points[triangles[t][1]][1],
                        points[triangles[t][2]][0], points[triangles[t][2]][1] );
                exit(0);
            }
        }
    }
    printf("Passed test\n");
}
