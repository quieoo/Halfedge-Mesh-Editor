#include "HalfEdge.h"
#include <cstring>
#include <cstdio>
#include "../tinyobjloader/tiny_obj_loader.h"
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>
#include <algorithm>
#include <fstream>
#include <sstream>
//using namespace tinyobj;

static std::string GetBaseDir(const std::string &filepath)
{
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

static bool parseTriple(const char **token, tinyobj::vertex_index_t *ret)
{
    if (!ret)
    {
        return false;
    }

    tinyobj::vertex_index_t vi(-1);

    vi.v_idx = atoi((*token));

    (*token) += strcspn((*token), "/ \t\r");
    if ((*token)[0] != '/')
    {
        (*ret) = vi;
        return true;
    }
    (*token)++;

    // i//k
    if ((*token)[0] == '/')
    {
        (*token)++;
        vi.vn_idx = atoi((*token));
        (*token) += strcspn((*token), "/ \t\r");
        (*ret) = vi;
        return true;
    }

    // i/j/k or i/j
    vi.vt_idx = atoi((*token));
    (*token) += strcspn((*token), "/ \t\r");
    if ((*token)[0] != '/')
    {
        (*ret) = vi;
        return true;
    }

    // i/j/k
    (*token)++; // skip '/'
    vi.vn_idx = atoi((*token));
    (*token) += strcspn((*token), "/ \t\r");

    (*ret) = vi;
    return true;
}

bool Load(halfedgemesh &mesh, const char *filename)
{
    mesh.ClearLists();
    std::string base_dir = GetBaseDir(filename);
    if (base_dir.empty())
    {
        base_dir = ".";
    }
    base_dir += "/";

    std::stringstream errss;

    std::ifstream ifs(filename);
    if (!ifs)
    {
        errss << "Cannot open file [" << filename << "]" << std::endl;
        return false;
    }

    istream *ins = &ifs;

    string linebuf;
    while (ins->peek() != -1)
    {
        tinyobj::safeGetline(*ins, linebuf);
        if (linebuf.size() > 0)
        {
            if (linebuf[linebuf.size() - 1] == '\n')
                linebuf.erase(linebuf.size() - 1);
        }
        if (linebuf.size() > 0)
        {
            if (linebuf[linebuf.size() - 1] == '\r')
                linebuf.erase(linebuf.size() - 1);
        }

        // Skip if empty line.
        if (linebuf.empty())
        {
            continue;
        }
        // Skip leading space.
        const char *token = linebuf.c_str();
        token += strspn(token, " \t");

        assert(token);
        if (token[0] == '\0')
            continue; // empty line

        if (token[0] == '#')
            continue; // comment line

        if (token[0] == 'v' && token[1] == ' ')
        {
            token += 2;
            float x, y, z;
            tinyobj::parseReal3(&x, &y, &z, &token);
            mesh.vertexs.push_back(vertex(x, y, z, mesh.vertexindex++));
            continue;
        }
        if (token[0] == 'f' && token[1] == ' ')
        {
            token += 2;
            token += strspn(token, "\t");
            //long index[3];
            long* index=new long[3];
            int x = 0;
            //只取顶点坐标索引
            while (!(((token[0]) == '\r') || ((token[0]) == '\n') || ((token[0]) == '\0')))
            {
                tinyobj::vertex_index_t vi;
                if (!parseTriple(&token, &vi))
                {

                    std::stringstream ss;
                    ss << "Failed parse `f' line(e.g. zero value for face index. line "
                       << ".)\n";

                    return false;
                }
                index[x++] = vi.v_idx;
                int n = strspn(token, " \t\r");
                token += n;
                //cout<<vi.v_idx<<" "<<vi.vn_idx<<" "<<vi.vt_idx<<endl;
            }
            //cout<<"face get vertex index "<<index[0]<<" "<<index[1]<<" "<<index[2]<<endl;

            //mesh.output();
            mesh.createface(index);
            continue;
        }
        
    }
    mesh.halfedge_opposite();
    cout<<"Load mesh get faces:"<<mesh.faces.size()<<endl;
}