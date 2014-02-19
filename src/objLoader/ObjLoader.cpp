#include <iostream>
#include <fstream>
#include <cstring>

#include <vector>

#include "ObjLoader.h"
#include "Vector3f.h"

ObjLoader::ObjLoader (std::string const& filename) : filename(filename) {
    parse();
    print();
}
void ObjLoader::print () {
    std::cout << "size vertices = " << nv << std::endl;
    std::cout << "size normals = " << nn << std::endl;

    std::cout << "vertices" << std::endl;
    for (unsigned int i = 0; i < nv; i++) {
        std::cout << vertices[i] << " ";
        if ((i + 1) % 3 == 0) {
            std::cout << std::endl;
        }
    }

    std::cout << "normals" << std::endl;
    for (unsigned int i = 0; i < nn; i++) {
        std::cout << normals[i] << " ";
        if ((i + 1) % 3 == 0) {
            std::cout << std::endl;
        }
    }
}

void ObjLoader::parse () {
    std::ifstream file(filename.c_str());

    std::vector<int> iv, in, it;
    std::vector<Vector3f> ver, nor, tex;
    std::vector<std::string> face;

    // vertex : v x y z
    std::string line;
    while (getline(file, line)) {
        // skip comments and blank lines
        if (line[0] == '#' || line == "")
            continue;

        switch (line[0]) {
            // vertex, normal or texture
            case 'v':
                switch (line[1]) {
                    case ' ':
                        // vertex
                        char x[255], y[255], z[255];
                        sscanf(line.c_str(), "v  %s  %s  %s", x, y, z);

                        ver.push_back(Vector3f(atof(x), atof(y), atof(z)));
                        break;

                    case 'n':
                        // normal
                        char xn[255], yn[255], zn[255];
                        sscanf(line.c_str(), "vn  %s  %s  %s", xn, yn, zn);

                        nor.push_back(Vector3f(atof(xn), atof(yn), atof(zn)));
                        break;

                    case 't':
                        // textures
                        char xt[255], yt[255];
                        sscanf(line.c_str(), "vt  %s  %s", xt, yt);

                        tex.push_back(Vector3f(atof(xt), atof(yt)));
                        break;

                    default:
                        break;
                }
                break;

            // face
            case 'f':
                face.clear();
                face = splitOnWS(line.substr(2));
                int v, vt, vn;

                for (unsigned int i = 0; i < face.size(); i++) {
                    switch (countSlashes(face[i])) {
                        case 0:
                            sscanf(face[i].c_str(), "%d", &v);
                            vt = 0;
                            vn = 0;
                            break;

                        case 2:
                            sscanf(face[i].c_str(), "%d/%d/%d", &v, &vt, &vn);
                            break;

                        default:
                            std::cerr << "Unsuported format" << std::endl;
                            break;
                    }
                    iv.push_back(v - 1);
                    it.push_back(vt - 1);
                    in.push_back(vn - 1);
                }
                break;

            default:
                break;
        }
    }

    file.close();
    // Parsing is done!
    // Now, we have to create vectors of vertices, normals and textures in the correct order
    std::vector<GLfloat> tv, tn, tt;
    for (unsigned int i = 0; i < iv.size(); i++) {
        if (iv[i] < int(ver.size())) {
            tv.push_back(ver[iv[i]].getX());
            tv.push_back(ver[iv[i]].getY());
            tv.push_back(ver[iv[i]].getZ());
        }
    }

    for (unsigned int i = 0; i < in.size(); i++) {
        if (in[i] < int(nor.size()) && in[i] != -1) {
            tn.push_back(nor[in[i]].getX());
            tn.push_back(nor[in[i]].getY());
            tn.push_back(nor[in[i]].getZ());
        }
    }

    for (unsigned int i = 0; i < it.size(); i++) {
        if (it[i] < int(tex.size()) && it[i] != -1) {
            tt.push_back(tex[it[i]].getX());
            tt.push_back(tex[it[i]].getZ());
        }
    }

    // Everything is in the appropriate order, we convert the vectors into GLfloat*
    vertices = vector2float(tv);
    normals = vector2float(tn);
    textures = vector2float(tt);
    nv = tv.size();
    nn = tn.size();

    ver.clear();
    nor.clear();
    tex.clear();

    iv.clear();
    in.clear();
    it.clear();
}

/* Count the number of '/' in a string */
int ObjLoader::countSlashes (std::string const& str) {
    int res = 0;

    for (unsigned int i = 0; i < str.size(); i++) {
        if (str[i] == '/') {
            res++;
        }
    }

    return res;
}

/* Convert a vector of a float into a GLfloat array */
GLfloat* ObjLoader::vector2float (std::vector<float>& array) {
    GLfloat *res = new GLfloat[array.size()];

    for (unsigned int i = 0; i < array.size(); i++) {
        res[i] = array[i];
    }

    return res;
}

/* Split a string on whitespaces */
std::vector<std::string> ObjLoader::splitOnWS (std::string const& str) {
    std::vector<std::string> res;
    std::string tmp;

    for (unsigned int i = 0; i < str.size(); i++) {
        if (str[i] == ' ') {
            if (tmp != "") {
                res.push_back(tmp);
                tmp = "";
            }
        } else {
            tmp += str[i];
        }
    }
    res.push_back(tmp);

    return res;
}

void ObjLoader::draw () {
    glPushMatrix();

    if (nn > 0) {
        glEnableClientState(GL_NORMAL_ARRAY);
    }
    glEnableClientState(GL_VERTEX_ARRAY);

    if (nn > 0) {
        glNormalPointer(GL_FLOAT, 0, normals);
    }
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_TRIANGLES, 0, nv / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    if (nn > 0) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    glPopMatrix();
}

ObjLoader::~ObjLoader () {
    delete vertices;
    delete normals;
    delete textures;
}
