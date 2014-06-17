Point Location:

Point location is a data structure for locating which area a point is in a planar graph.

Input: n points, m triangles - a complete triangulation of the graph

Method: This data structure locates the area through the use of Kirkpatrick's triangulation refinement algorithm.

Algorithm: With n points, m triangles are created as input for the point location data structure. The m triangles create a complete triangulation of the graph. There is also 3 points that encompass the entire triangulation with a single large triangle. For each iteration until the triangulation consists of only a single triangle, the data structure finds the independent set of points. 
And independent set of points consists of points that are not neighbors with each other. This independent set of points is deleted from the triangulation. In doing so, holes are created.
Holes are a series of connected points that create a shape that may have more vertices than a triangle (# vertices >= 3). 
The holes are retriangulated, which results in a new complete triangulation of the graph that consists of fewer triangles than the previous triangulation. The new triangles that are created from the hole have to point to the previous triangles in the same area, only if any part of the previous triangles are inside the new triangles.
This is repeated until the size of the triangulation is only 3. The original triangulation continually is refined until the data structure only has a single triangle.

Result: The algorithm creates a directed acyclic graph, with the root being a single triangle that originally encompassed the entire triangulation, that results from the last iteration of the algorithm. That single triangle points to 3 other triangles from the previous triangulation. Those 3 triangles point to an arbitrary number of triangles from another previous triangulation. The leaves of the directed acyclic graph is the original triangulation, consisting of m triangles. 

Locating a point: To find which area of the original triangulation a point, p is, start with the root of the graph (the very large triangle). Go to the next level (the 3 triangles that the large triangle points to) and perform the point in triangle test with each one until the test succeeds. Do the same with the next level, until it reaches the original triangles to receive the result of which area the point is in.

Analysis: Preprocessing takes O(nlogn), querying a point takes O(logn).
