#include <iostream>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include <sstream>

void begin(std::string s)
{
    std::cout << "\\begin{" << s << "}";
}

void end(std::string s)
{
    std::cout << "\\end{" << s << "}";
}

template<class T>
void DrawVector(const std::vector<T> &v, std::string sep = "")
{
    std::cout << v[0];
    for (size_t i = 1; i < v.size(); i++)
        std::cout << sep << v[i];
}

namespace patch
{
    template<class T>
    std::string ToString(const T &x)
    {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }

    int ToInt(const std::string &s)
    {
        int x;
        std::stringstream ss(s);
        ss >> x;
        return x;
    }
}

std::vector<std::string> GenIt(int n)
{
    std::vector<std::string> ret;
    for (int i = 1; i <= n; i++)
        ret.push_back("Iteration " + patch::ToString(i));
    return ret;
}


template<class T>
void DrawTable(const std::vector<std::string> &columnNames,
               const std::vector<std::string> &rowNames,
               const std::vector<std::vector<T>> &mat,
               const std::string &topLeft)
{
    putchar('\n');
    putchar('\n');

    begin("tabular");
    std::cout << "{ | c || c ";
    for (size_t i = 1; i < columnNames.size(); i++)
        std::cout << " | c";
    std::cout << " | } \n";

    std::cout << "\\hline\n";
    // Draws the column names
    std::cout << topLeft << " & ";
    DrawVector(columnNames, " & ");
    std::cout << "\\\\\n \\hline\n \\hline \n";

    for (int i = 0; i < mat.size(); i++)
    {
        if (i < rowNames.size())
            std::cout << rowNames[i] << " & ";
        else
            std::cout << "Iteracija " << i + 1 << " & ";

        DrawVector(mat[i], " & ");
        std::cout << "\\\\\n \\hline\n";
    }
    end("tabular");
}

template<class T>
class Edge
{
public:
    T u, v;
    int w;

    Edge() { }
    Edge(const T &_v, int _w) : v(_v), w(_w) { }
    Edge(const T &_u, const T &_v, int _w) : u(_u), v(_v), w(_w) { }

    bool operator< (const Edge &e) const
    {
        if (w != e.w)
            return w > e.w;
        else if (u != e.u)
            return u < e.u;
        else
            return v < e.v;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "(" << u << ", " << v << ", " << w << ")";
        return ss.str();
    }
};

template<class T>
void DrawVertices(const std::set<T> &s)
{
    std::cout << "\\{ ";
    DrawVector(std::vector<T>(s.begin(), s.end()), ", ");
    std::cout << "\\}";
}

template<class T>
void DrawEdges(const std::vector<Edge<T>> &s, const std::string &name = "")
{
    if (name != "")
        std::cout << name << " = ";
    std::cout << " \\{ ";
    std::cout << s[0].ToString();
    for (size_t i = 1; i < s.size(); i++)
        std::cout << ", " << s[i].ToString();
    std::cout << " \\}";
}


template<class T>
class Graph
{
    std::map<T, std::vector<Edge<T>>> g;
    std::set<T> vertices;
    std::vector<Edge<T>> edges;
public:

    std::vector<Edge<T>>& operator[](const T &u)
    {
        return g[u];
    }

    std::vector<Edge<T>> operator[](const T &u) const
    {
        return g[u];
    }

    void AddEdgeDir(const T &u, const T &v, int w)
    {
        edges.emplace_back(u, v, w);
        g[u].emplace_back(u, v, w);
        vertices.insert(u);
        vertices.insert(v);
    }

    void AddEdge(const T &u, const T &v, int w)
    {
        AddEdgeDir(u, v, w);
        AddEdgeDir(v, u, w);
    }

    std::vector<Edge<T>> GetEdges(bool filter = false) const
    {
        if (!filter)
            return edges;
        else
        {
            std::vector<Edge<T>> v;
            for (int i = 0; i < edges.size(); i += 2)
                v.push_back(edges[i]);
            return v;
        }
    }

    std::set<T> GetVertices() const
    {
        return vertices;
    }

    int V() const
    {
        return (int)vertices.size();
    }

    int E() const
    {
        return (int)edges.size();
    }

    void DrawGraph(const std::string &name) const
    {
        std::cout << name << "_V = ";
        DrawVertices(vertices);
        std::cout << '\n';
        std::cout << name << "_E = ";
        DrawEdges(edges);
    }

    void DrawGraphTable() const
    {
        std::map<T, std::map<T, int>> dist;
        std::vector<std::vector<std::string>> mat;
        for (auto& x : vertices)
            for (auto& y : vertices)
                dist[x][y] = std::numeric_limits<int>::max();
        for (auto& e : edges)
            dist[e.u][e.v] = e.w;

        for (auto& x : vertices)
        {
            mat.push_back(std::vector<std::string>());
            for (auto& y : vertices)
            {
                mat.back().push_back(
                    (dist[x][y] == std::numeric_limits<int>::max()) ? "-" :
                                    patch::ToString(dist[x][y]));
            }
        }

        DrawTable<T>(
                std::vector<T>(vertices.begin(), vertices.end()),
                std::vector<T>(vertices.begin(), vertices.end()),
                mat,
                ""
                );
    }
};


template<class T>
class BFNegCycleSolver
{
private:
    std::map<T, int> d;
    std::set<std::pair<int, T>> s;

    std::vector<Edge<T>> e;

    Graph<T> g;
    std::vector<std::vector<T>> mat;
    std::vector<std::string> rowNames;
    std::vector<std::vector<T>> parentmat;

public:
    BFNegCycleSolver(const Graph<T> &_g) : g(_g)
    {
        auto v = g.GetVertices();
        auto e = g.GetEdges();

        std::map<T, int> d;
        std::map<T, T> parent;

        for (auto& x : v)
            d[x] = std::numeric_limits<int>::max() / 2;
        
        d[*v.begin()] = 0;
        parent[*v.begin()] = *v.begin();

        std::sort(e.begin(), e.end(), 
                [](const Edge<T> &a, const Edge<T> &b) -> bool { return a.w < b.w; });

        rowNames.push_back("Inicijalno stanje: ");

        mat.push_back(std::vector<T>());
        parentmat.push_back(std::vector<T>());

        d[*v.begin()] = 0;

        for (auto& x : v)
        {
            if (d[x] != std::numeric_limits<int>::max() / 2)
                mat.back().push_back(patch::ToString(d[x]));
            else
                mat.back().push_back("\\(\\infty\\)");
            parentmat.back().push_back(x);
        }

        T node;

        for (int i = 0; i <= g.V(); i++)
        {
            rowNames.push_back("Iteracija " + patch::ToString(i + 1));
            for (auto& edge : e)
            {
                if (d[edge.v] > d[edge.u] + edge.w)
                {
                    node = edge.v;
                    d[edge.v] = d[edge.u] + edge.w;
                    if (i != g.V())
                        parent[edge.v] = edge.u;
                }
            }

            mat.push_back(std::vector<T>());
            parentmat.push_back(std::vector<T>());
            for (auto& x : v)
            {
                if (d[x] != std::numeric_limits<int>::max() / 2)
                    mat.back().push_back(patch::ToString(d[x]));
                else
                    mat.back().push_back("\\(\\infty\\)");
                parentmat.back().push_back(parent[x]);
            }
        }

        std::map<T, bool> vis;
        std::vector<std::string> sol;

        while (!vis[node])
        {
            sol.push_back(node);
            vis[node] = true;
            node = parent[node];
        }

        std::reverse(sol.begin(), sol.end());

        std::cout << "Negativni ciklus: ";

        DrawVector(sol, " \\rightarrow ");
        std::cout << " \\rightarrow " << sol[0] << '\n';
    }

    BFNegCycleSolver<T> Draw(const std::string &topLeft)
    {
        auto v = g.GetVertices();
        DrawTable<T>(std::vector<T>(v.begin(), v.end()),
                rowNames,
                mat,
                topLeft);
        std::cout << '\n';
        DrawTable<T>(std::vector<T>(v.begin(), v.end()),
                rowNames,
                parentmat,
                "");
        std::cout << "\n\n";
        return (*this);
    }
};

int main()
{
    freopen("cetvrti.txt", "r", stdin);
    int w;
    char u, v;
    Graph<std::string> g;
    while (scanf("(%c, %c, %d),", &u, &v, &w) == 3)
    {
        std::cout << u << ' ' << v << ' ' << w << '\n';
        g.AddEdgeDir(std::string(1, u), std::string(1, v), w);
        getchar();
    }

    g.DrawGraph("G");
    std::cout << '\n';
    g.DrawGraphTable();
    std::cout << "\n\n\n";
    BFNegCycleSolver<std::string>(g).Draw("");

    return 0;
}



/*
B -> A  45 
A -> H -95
H -> J -30
J -> I  55
I -> F -65
F -> B  70

A -> H -> J -> C -> E -> D -> B -> A

-95 - 30 + 65 - 105 + 65 + 45


   */































