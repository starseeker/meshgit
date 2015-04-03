#include <string>
#include <vector>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mesh.h"
#include "quicksort.h"
#include "binheap.h"

#include "../geom.h"

#include "../debug.h"
#include "../vec.h"
#include "../std.h"
#include "../geom.h"
#include "../gls.h"
#include "../draw_utils.h"
#include "../std_utils.h"

#include "../io_bin.h"


#include <thread>

using namespace std;

bool str_startsame( const char *s0, const char *s1 ) {
    for( int i = 0; s0[i] && s1[i]; i++ ) if(s0[i]!=s1[i]) return false;
    return true;
}

//inline void set_material( GLenum pname, vec3f c, vec3f c_ ) {
//    if( glob_mesh_faces_twotoned ) glsMaterialfv( pname, c, c_ );
//    else glsMaterialfv( pname, c, c );
//}

inline vec3f get_move_color( float d, vec3f &color0, vec3f &color1 ) {
    return color0 * d + color1 * (1.0f-d);
}

inline void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vector<SimpleMesh::VertInfo> &vinfo, vec3f &c0, vec3f &c0_, vec3f &c1, vec3f &c1_ ) {
    int n = vinds.size();
    glsNormal(norm);
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        vec3f col0 = get_move_color(vinfo[vinds[i0]].dist, c0, c1);
        vec3f col1 = get_move_color(vinfo[vinds[i1]].dist, c0, c1);
        vec3f col2 = get_move_color(vinfo[vinds[i2]].dist, c0, c1);
        glsMaterialfv(GL_AMBIENT_AND_DIFFUSE, col0, col0 );
        glsVertex(vpos[vinds[i0]]);
        glsMaterialfv(GL_AMBIENT_AND_DIFFUSE, col1, col1 );
        glsVertex(vpos[vinds[i1]]);
        glsMaterialfv(GL_AMBIENT_AND_DIFFUSE, col2, col2 );
        glsVertex(vpos[vinds[i2]]);
    }
}
inline void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos ) {
    int n = vinds.size();
    glsNormal(norm);
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        glsVertex(vpos[vinds[i0]]);
        glsVertex(vpos[vinds[i1]]);
        glsVertex(vpos[vinds[i2]]);
    }
}

void SimpleMesh::draw(bool b_faces, bool b_edges, material &match, material &match_) {
    int n_f = finds.size();
    vec3f color0 = match.diffuse, color1 = { match.diffuse.x * 0.5f, match.diffuse.y * 0.5f, 1.0f };
    vec3f color0_ = match_.diffuse, color1_ = { match_.diffuse.x * 0.5f, match_.diffuse.y * 0.5f, 1.0f };
    //get_move_color(v0,v0_,c0,c1);
    
    if( b_faces ) {
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            glsMaterial( fmats[i_f], fmats_[i_f] );
            if( fcolor[i_f] ) {
                draw_face( finds[i_f], fnorms[i_f], vpos );
            } else {
                draw_face( finds[i_f], fnorms[i_f], vpos, vinfo, color0, color0_, color1, color1_ );
            }
        }
        glEnd();
    }
    
    if( b_edges ) {
        glDepthRange(0.0f,0.9999999f);
        glLineWidth(1.0f);
        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LIGHTING);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if(true) {
            glColor4f(0.2f,0.2f,0.2f,0.1f); glLineWidth(1.0f);
            glBegin(GL_LINES);
            for( int i_f = 0; i_f < n_f; i_f++ ) {
                //if( !fshow[i_f] ) continue;
                vector<ivert> &_vinds = finds[i_f];
                int n = _vinds.size();
                for( int i0 = 0; i0 < n; i0++ ) {
                    int i1 = (i0+1)%n;
                    glsVertex(vpos[_vinds[i0]]);
                    glsVertex(vpos[_vinds[i1]]);
                }
            }
            glEnd();
        }
        
        glDepthRange(0.0f,0.999995f);
        glColor4f(0.2f,0.2f,0.2f,0.5f); glLineWidth(1.0f);
        glBegin(GL_LINES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            //if( !fshow[i_f] ) continue;
            vector<ivert> &_vinds = finds[i_f];
            int n = _vinds.size();
            for( int i0 = 0; i0 < n; i0++ ) {
                int i1 = (i0+1)%n;
                if( !vinfo[_vinds[i0]].edge || !vinfo[_vinds[i1]].edge ) continue;
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
        
        glEnable(GL_LIGHTING);
        glDepthRange(0.0f,1.0f);
    }
}

void SimpleMesh::draw_override(bool b_faces, bool b_edges, material &mat, material &mat_, int edge_darkness) {
    int n_f = finds.size();
    
    if( b_faces ) {
        glBegin(GL_TRIANGLES);
        glsMaterial( mat, mat_ );
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            draw_face( finds[i_f], fnorms[i_f], vpos );
        }
        glEnd();
    }
    
    if( b_edges ) {
        glDepthRange(0.0f,0.9999999f);
        glLineWidth(1.0f);
        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LIGHTING);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( edge_darkness ) {
            case 1: { glColor4f(0.2f,0.2f,0.2f,0.02f); glLineWidth(1.0f); } break;
            case 2: { glColor4f(0.2f,0.2f,0.2f,0.10f); glLineWidth(1.0f); } break;
            case 3: { glColor4f(0.2f,0.2f,0.2f,0.14f); glLineWidth(1.0f); } break;
            case 4: { glColor4f(0.2f,0.2f,0.2f,0.10f); glLineWidth(2.0f); } break;
            case 5: { glColor4f(0.2f,0.2f,0.2f,0.14f); glLineWidth(2.0f); } break;
        }
        
        glBegin(GL_LINES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            vector<ivert> &_vinds = finds[i_f];
            int n = _vinds.size();
            for( int i0 = 0; i0 < n; i0++ ) {
                int i1 = (i0+1)%n;
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
        
        glDepthRange(0.0f,0.999999f);
        
        switch( edge_darkness ) {
            case 1: { glColor4f(0.2f,0.2f,0.2f,0.1f); glLineWidth(1.0f); } break;
            case 2: { glColor4f(0.2f,0.2f,0.2f,0.5f); glLineWidth(1.0f); } break;
            case 3: { glColor4f(0.2f,0.2f,0.2f,0.7f); glLineWidth(1.0f); } break;
            case 4: { glColor4f(0.2f,0.2f,0.2f,0.5f); glLineWidth(2.0f); } break;
            case 5: { glColor4f(0.2f,0.2f,0.2f,0.7f); glLineWidth(2.0f); } break;
        }
        
        glBegin(GL_LINES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            vector<ivert> &_vinds = finds[i_f];
            int n = _vinds.size();
            for( int i0 = 0; i0 < n; i0++ ) {
                int i1 = (i0+1)%n;
                if( !vinfo[_vinds[i0]].edge || !vinfo[_vinds[i1]].edge ) continue;
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
        
        glEnable(GL_LIGHTING);
        glDepthRange(0.0f,1.0f);
    }
}


// http://www.rorydriscoll.com/2008/08/01/catmull-clark-subdivision-the-basics/
// DeRose, Kass, Truong. Subdivision Surfaces in Character Animation
// http://graphics.pixar.com/library/Geri/paper.pdf
SimpleMesh *SimpleMesh::subd_catmullclark() {
    SimpleMesh *smnew = new SimpleMesh();
    vector<vec3f> &vpos_new = smnew->vpos;
    vector<VertInfo> &vinfo_new = smnew->vinfo;
    vector<vector<ivert>> &finds_new = smnew->finds;
    vector<material> &fmats_new = smnew->fmats;
    vector<material> &fmats_new_ = smnew->fmats_;
    vector<bool> &fcolor_new = smnew->fcolor;
    
    vector<int> vmap, emap;
    
    int n_v = vpos.size();
    int n_f = finds.size();
    
    
    // collect edge and vert information
    struct edgeinfo {
        int i_v0, i_v1;
        vec3f a; int count;
        bool b_sharp;
        VertInfo vi;
        unordered_set<int> s_if_adj;
    };
    struct vertinfo {
        unordered_set<int> s_ie_adj;
        unordered_set<int> s_if_adj;
        bool b_crease = false;
        bool b_corner = false;
    };
    vector<vertinfo> lvi(n_v);
    unordered_map<int,int> map_edge_ie;
    vector<edgeinfo> edges;
    for( int i_f = 0; i_f < n_f; i_f++ ) {
        vector<ivert> &vinds = finds[i_f];
        int n = vinds.size();
        
        vec3f a_f = zero3f;
        for( ivert i : vinds ) a_f += vpos[i];
        a_f /= (float)vinds.size();
        
        for( int i0 = 0; i0 < n; i0++ ) {
            int i1 = (i0+1)%n;
            int i_v0 = vinds[i0], i_v1 = vinds[i1];
            int e = min(i_v0,i_v1)*n_v + max(i_v0,i_v1);
            
            if( !map_edge_ie.count(e) ) {
                edgeinfo ei = { i_v0, i_v1, zero3f, 0, false };
                map_edge_ie[e] = edges.size();
                edges.push_back( ei );
            }
            
            int i_e = map_edge_ie[e];
            edgeinfo &ei = edges[i_e];
            if( !ei.s_if_adj.count(i_f)) {
                ei.a += a_f; ei.count++;
                ei.s_if_adj.insert(i_f);
            }
            
            lvi[i_v0].s_if_adj.insert(i_f);
            lvi[i_v1].s_if_adj.insert(i_f);
            lvi[i_v0].s_ie_adj.insert(i_e);
            lvi[i_v1].s_ie_adj.insert(i_e);
        }
    }
    for( edgeinfo &ei : edges ) {
        ei.b_sharp = (ei.s_if_adj.size()!=2);
        if( ei.b_sharp ) {
            ei.a = (vpos[ei.i_v0] + vpos[ei.i_v1] ) / 2.0f;
        } else {
            ei.a = (ei.a + vpos[ei.i_v0] + vpos[ei.i_v1] ) / (float)(ei.count+2);
        }
        ei.vi = {
            (vinfo[ei.i_v0].dist + vinfo[ei.i_v1].dist)/2.0f,
            (vinfo[ei.i_v0].edge && vinfo[ei.i_v1].edge)
        };
    }
    for( vertinfo &vi : lvi ) {
        int n_sharp = 0;
        for( int i_e : vi.s_ie_adj ) if( edges[i_e].b_sharp ) n_sharp++;
        if( n_sharp == 2 ) vi.b_crease = true;
        if( n_sharp >= 3 ) vi.b_corner = true;
    }
    
    
    
    
    // create face points
    for( vector<ivert> &vinds : finds ) {
        vec3f a_f = zero3f;
        float d = 0.0f;
        for( ivert i : vinds ) {
            a_f += vpos[i];
            d += vinfo[i].dist;
        }
        a_f /= (float)vinds.size();
        d /= (float)vinds.size();
        vpos_new.push_back(a_f);
        vinfo_new.push_back({d,false});
    }
    
    // create edge points
    for( edgeinfo &ei : edges ) {
        emap.push_back((int)vpos_new.size());
        vpos_new.push_back(ei.a);
        vinfo_new.push_back(ei.vi);
    }
    
    // create vert points
    for( int i_v = 0; i_v < n_v; i_v++ ) {
        vertinfo &vi = lvi[i_v];
        
        vmap.push_back((int)vpos_new.size());
        if( vi.b_crease ) {
            vec3f S = 6.0f * vpos[i_v];
            for( int i_e : vi.s_ie_adj ) {
                edgeinfo &ei = edges[i_e];
                if( !ei.b_sharp ) continue;
                int i_v1 = ( ei.i_v0 == i_v ? ei.i_v1 : ei.i_v0 );
                S += vpos[i_v1];
            }
            vpos_new.push_back( S / 8.0f );
        } else if( vi.b_corner ) {
            vpos_new.push_back( vpos[i_v] );
        } else {
            int n = vi.s_ie_adj.size();
            
            vec3f Q = zero3f, R = zero3f, S = vpos[i_v];
            
            for( int i_f : vi.s_if_adj ) Q += vpos_new[i_f];
            Q /= (float)vi.s_if_adj.size();
            
            for( int i_e : vi.s_ie_adj ) R += vpos[edges[i_e].i_v0] + vpos[edges[i_e].i_v1];
            R /= (float)vi.s_ie_adj.size() * 2.0f;
            
            vpos_new.push_back( (Q/(float)n) + (2.0f*R/(float)n) + (S*(float)(n-3)/(float)n) );
        }
        vinfo_new.push_back({ vinfo[i_v].dist, vinfo[i_v].edge });
    }
    
    
    // create new faces
    for( int i_f = 0; i_f < n_f; i_f++ ) {
        vector<ivert> &vinds = finds[i_f];
        material &fmat = fmats[i_f];
        material &fmat_ = fmats_[i_f];
        int n = vinds.size();
        
        for( int i0 = 0; i0 < n; i0++ ) {
            int i1=(i0+1)%n, i2=(i1+1)%n;
            
            int i_v0=vinds[i0], i_v1=vinds[i1], i_v2=vinds[i2];
            int i_e01 = map_edge_ie[min(i_v0,i_v1)*n_v + max(i_v0,i_v1)];
            int i_e12 = map_edge_ie[min(i_v1,i_v2)*n_v + max(i_v1,i_v2)];
            
            vector<ivert> vinds_new = { emap[i_e01], vmap[i_v1], emap[i_e12], i_f };
            
            finds_new.push_back( vinds_new );
            fmats_new.push_back( fmat );
            fmats_new_.push_back( fmat_ );
            fcolor_new.push_back( fcolor[i_f] );
        }
    }
    
    smnew->recompute_normals();
    
    return smnew;
}

void SimpleMesh::save_ply( const char *fn, const char *comment ) {
    FILE *fp = fopen(fn,"wt");
    
    fprintf( fp, "ply\n" );
    fprintf( fp, "format ascii 1.0\n" );
    fprintf( fp, "comment Merged by MeshGit\n" );
    fprintf( fp, "commont %s\n", comment );
    fprintf( fp, "element vertex %d\n", (int)vpos.size() );
    fprintf( fp, "property float x\n" );
    fprintf( fp, "property float y\n" );
    fprintf( fp, "property float z\n" );
    fprintf( fp, "element face %d\n", (int)finds.size() );
    fprintf( fp, "property list uchar uint vertex_indices\n" );
    fprintf( fp, "end_header\n" );
    
    for( vec3f &v : vpos ) fprintf( fp, "%f %f %f\n", v.x, v.y, v.z );
    for( vector<ivert> &vinds : finds ) {
        fprintf( fp, "%d ", (int)vinds.size() );
        for( int i_v : vinds ) fprintf( fp, "%d ", i_v );
        fprintf( fp, "\n" );
    }
    
    fclose(fp);
}

void SimpleMesh::save_light(const char *fn) {
    set<vec2i,vec2icomp> lines_soft, lines_hard;
    vector<vec3f> tri_pos, tri_norm, tri_color;
    
    vec3f c0 = { 0.7f, 0.7f, 0.7f };
    vec3f c1 = { c0.x * 0.5f, c0.y * 0.5f, c0.z * 1.0f };
    
    int sz_finds = finds.size();
    for(int i_finds=0; i_finds<sz_finds; i_finds++) {
        auto &inds = finds[i_finds];
        vec3f n = fnorms[i_finds];
        vec3f c = fmats[i_finds].diffuse;
        
        int i0 = inds.back();
        for(int i1 : inds) {
            vec2i p = { min(i0,i1), max(i0,i1) };
            if(vinfo[i0].edge and vinfo[i1].edge) {
                lines_hard.insert(p);
            } else {
                lines_soft.insert(p);
            }
            i0 = i1;
        }
        
        int sz_inds = inds.size();
        int it0 = inds[0], it1 = inds[1];
        for(int i=2; i<sz_inds; i++) {
            int it2 = inds[i];
            tri_pos.push_back(vpos[it0]);
            tri_pos.push_back(vpos[it1]);
            tri_pos.push_back(vpos[it2]);
            
            tri_norm.push_back(n);
            tri_norm.push_back(n);
            tri_norm.push_back(n);
            
            if(fcolor[i_finds]) {
                tri_color.push_back(c);
                tri_color.push_back(c);
                tri_color.push_back(c);
            } else {
                tri_color.push_back(get_move_color(vinfo[it0].dist, c0, c1));
                tri_color.push_back(get_move_color(vinfo[it1].dist, c0, c1));
                tri_color.push_back(get_move_color(vinfo[it2].dist, c0, c1));
            }
            
            it1 = it2;
        }
    }
    
    int sz_tris = tri_pos.size();
    printf("%d %d %d\n", (int)lines_soft.size()/2, (int)lines_hard.size()/2, sz_tris/3);
    
    FILE *fp = fopen(fn, "wb");
    
    SaveBinaryFile(fp, 2*(int)lines_soft.size());
    for(auto l : lines_soft) {
        SaveBinaryFile(fp, vpos[l.x]);
        SaveBinaryFile(fp, vpos[l.y]);
    }
    
    SaveBinaryFile(fp, 2*(int)lines_hard.size());
    for(auto l : lines_hard) {
        SaveBinaryFile(fp, vpos[l.x]);
        SaveBinaryFile(fp, vpos[l.y]);
    }
    
    SaveBinaryFile(fp, sz_tris);
    for(int i=0; i<sz_tris; i++) SaveBinaryFile(fp, tri_pos[i]);
    for(int i=0; i<sz_tris; i++) SaveBinaryFile(fp, tri_norm[i]);
    for(int i=0; i<sz_tris; i++) SaveBinaryFile(fp, tri_color[i]);
    
    fclose(fp);
}

void SimpleMesh::save_light_changes(const char *fn) {
    set<vec2i,vec2icomp> lines_soft, lines_hard;
    vector<vec3f> tri_pos, tri_norm, tri_color;
    
    vec3f c0 = { 0.7f, 0.7f, 0.7f };
    vec3f c1 = { c0.x * 0.5f, c0.y * 0.5f, c0.z * 1.0f };
    
    int sz_finds = finds.size();
    for(int i_finds=0; i_finds<sz_finds; i_finds++) {
        auto &inds = finds[i_finds];
        vec3f n = fnorms[i_finds];
        vec3f c = fmats[i_finds].diffuse;
        
        int i0 = inds.back();
        for(int i1 : inds) {
            vec2i p = { min(i0,i1), max(i0,i1) };
            if(vinfo[i0].edge and vinfo[i1].edge) {
                lines_hard.insert(p);
            } else {
                lines_soft.insert(p);
            }
            i0 = i1;
        }
        
        if(fcolor[i_finds]) {
            int sz_inds = inds.size();
            int it0 = inds[0], it1 = inds[1];
            for(int i=2; i<sz_inds; i++) {
                int it2 = inds[i];
                tri_pos.push_back(vpos[it0]);
                tri_pos.push_back(vpos[it1]);
                tri_pos.push_back(vpos[it2]);
                
                tri_norm.push_back(n);
                tri_norm.push_back(n);
                tri_norm.push_back(n);
                
                tri_color.push_back(c);
                tri_color.push_back(c);
                tri_color.push_back(c);
                
                it1 = it2;
            }
        }
    }
    
    int sz_tris = tri_pos.size();
    printf("%d %d %d\n", (int)lines_soft.size()/2, (int)lines_hard.size()/2, sz_tris/3);
    
    FILE *fp = fopen(fn, "wb");
    
    SaveBinaryFile(fp, 2*(int)lines_soft.size());
    for(auto l : lines_soft) {
        SaveBinaryFile(fp, vpos[l.x]);
        SaveBinaryFile(fp, vpos[l.y]);
    }
    
    SaveBinaryFile(fp, 2*(int)lines_hard.size());
    for(auto l : lines_hard) {
        SaveBinaryFile(fp, vpos[l.x]);
        SaveBinaryFile(fp, vpos[l.y]);
    }
    
    SaveBinaryFile(fp, sz_tris);
    for(int i=0; i<sz_tris; i++) SaveBinaryFile(fp, tri_pos[i]);
    for(int i=0; i<sz_tris; i++) SaveBinaryFile(fp, tri_norm[i]);
    for(int i=0; i<sz_tris; i++) SaveBinaryFile(fp, tri_color[i]);
    
    fclose(fp);
}


// WARNING!!! ASSUMES PLY IS IN A VERY SPECIFIC FORMAT!!!  NOT ROBUST!
void Mesh::loadply() {
    char buf[1024];
    
    n_v = n_f = 0;
    
    FILE *fp = fopen( filename.c_str(), "rt" ); assert(fp);
    
    fgets(buf,1024,fp); assert(str_startsame(buf,"ply"));
    fgets(buf,1024,fp); assert(str_startsame(buf,"format ascii 1.0"));
    
    // read header
    while(true) {
        fgets(buf,1024,fp);
        if( str_startsame(buf,"comment") ) continue;
        if( str_startsame(buf,"element") ) {
            char element[20],type[20]; int num;
            sscanf(buf,"%s %s %d", element, type, &num);
            if( str_startsame(type,"vertex") ) n_v = num;
            if( str_startsame(type,"face") ) n_f = num;
        }
        if( str_startsame(buf,"property") ) continue; // ignored
        if( str_startsame(buf,"end_header") ) break;
    }
    
    vpos.resize(n_v);
    finds.resize(n_f);
    
    // read data
    for( int i_v = 0; i_v < n_v; i_v++ ) {
        fscanf(fp,"%f %f %f",&vpos[i_v].x,&vpos[i_v].y,&vpos[i_v].z);
    }
    for( int i_f = 0; i_f < n_f; i_f++ ) {
        int n_inds;
        fscanf(fp,"%d",&n_inds);
        finds[i_f].resize(n_inds);
        for( int i = 0; i < n_inds; i++ ) {
            fscanf(fp,"%d",&finds[i_f][i]);
        }
    }
    
    fclose(fp);
    
    // build extra data (face position and normal, vertex normal)
    vnorms.resize(n_v,zero3f);
    fpos.resize(n_f,zero3f);
    fnorms.resize(n_f,zero3f);
    for( int i_f = 0; i_f < n_f; i_f++ ) {
        vector<ivert> &vinds = finds[i_f];
        int n = vinds.size();
        vec3f &fp = fpos[i_f];
        vec3f norm = zero3f;
        for( int i = 0; i < n; i++ ) {
            vec3f &v0 = vpos[vinds[i]];
            vec3f &v1 = vpos[vinds[(i+1)%n]];
            vec3f &v2 = vpos[vinds[(i+2)%n]];
            fp += v0;
        }
        if( n ) fp /= (float)n;
        for( int i = 0; i < n; i++ ) {
            vec3f &v0 = vpos[vinds[i]];
            vec3f &v1 = vpos[vinds[(i+1)%n]];
            norm += triangle_normal(fp,v0,v1);
        }
        norm = normalize(norm);
        fnorms[i_f] = norm;
        for( int i_v : vinds ) vnorms[i_v] += norm;
    }
    for( int i_v = 0; i_v < n_v; i_v++ ) {
        vnorms[i_v] = normalize(vnorms[i_v]);
    }
}

void MeshMatch::load( const char *fn ) {
    reset();
    
    if( 0 ) {
        FILE *fp = fopen(fn,"rb");
        assert(LoadBinaryFile_int(fp)==n_v0 && LoadBinaryFile_int(fp)==n_f0);
        assert(LoadBinaryFile_int(fp)==n_v1 && LoadBinaryFile_int(fp)==n_f1);
        LoadBinaryFile(fp,vm01); LoadBinaryFile(fp,fm01);
        LoadBinaryFile(fp,vm10); LoadBinaryFile(fp,fm10);
        fclose(fp);
    } else {
        FILE *fp = fopen(fn,"rt");
        int count_verts, count_faces; float _;
        fscanf(fp,"%d %d",&count_verts,&count_faces); assert(count_verts==n_v0 && count_faces==n_f0 );
        fscanf(fp,"%f", &_);
        for( int i_v0 = 0; i_v0 < n_v0; i_v0++ ) {
            int i_v1; fscanf(fp,"%d",&i_v1);
            if( i_v1 != -1 ) vassign(i_v0,i_v1);
        }
        for( int i_f0 = 0; i_f0 < n_f0; i_f0++ ) {
            int i_f1; fscanf(fp,"%d",&i_f1);
            if( i_f1 != -1 ) fassign(i_f0,i_f1);
        }
        fclose(fp);
    }
    
    assert_consistency();
}

void MeshMatch::save( const char *fn ) {
    if(0) {
        FILE *fp = fopen(fn,"wb");
        SaveBinaryFile(fp,n_v0); SaveBinaryFile(fp,n_f0);
        SaveBinaryFile(fp,n_v1); SaveBinaryFile(fp,n_f1);
        SaveBinaryFile(fp,vm01); SaveBinaryFile(fp,fm01);
        SaveBinaryFile(fp,vm10); SaveBinaryFile(fp,fm10);
        fclose(fp);
    } else {
        FILE *fp = fopen(fn, "wt");
        fprintf(fp, "%d %d\n%f\n", n_v0, n_f0, 0.0f );
        for(int i_v0 = 0; i_v0 < n_v0; i_v0++) fprintf(fp, "%d ", vm01[i_v0]);
        fprintf(fp, "\n");
        for(int i_f0 = 0; i_f0 < n_f0; i_f0++) fprintf(fp, "%d ", fm01[i_f0]);
        fprintf(fp, "\n");
        fclose(fp);
    }
}

float MeshMatch::compute_cost( MGNode *n_0, MGNode *n_1 ) {
    float cost = 0.0f;
    
    assert(n_0->type==n_1->type);
    assert(n_0->m==m0 && n_1->m==m1);
    
    MGNode *n_01 = get_match(n_0), *n_10 = get_match(n_1);
    assert( (n_01==n_1 && n_10==n_0) || (!n_01 && !n_10) );
    
    float sz0 = n_0->anodes.size();
    float sz1 = n_1->anodes.size();
    vec3f &p_0 = n_0->pos;
    vec3f &p_1 = n_1->pos;
    
    if( !n_01 && !n_10 ) {                                           // unmatched
        cost += costs.alpha * 2.0f;
    } else {
        float dist = length(p_0 - p_1);
        float norm = dot(n_0->norm,n_1->norm);
        cost += costs.dist * dist / (dist + 1.0f);
        cost += costs.norm * (1.0f - norm);
        
        cost += costs.valence * fabs(sz0-sz1) / (float)(sz0+sz1);
    }
    
    
    for( MGNode *a_0 : n_0->anodes ) {
        MGNode *a_1 = get_match(a_0);
        float sz = sz0 + a_0->anodes.size();
        //float sz = sz0f + a_0->afnodes.size();
        if( !n_01 || !a_1 ) {
            cost += costs.unm / sz;
        } else {
            if( !n_1->has_edge(a_1) ) {
                cost += costs.mis / sz;
            }
            
            float d0=length(p_0-a_0->pos), d1=length(p_1-a_1->pos);
            cost += costs.stretch * fabs(d0-d1) / (d0+d1+0.0001f) / sz;
        }
    }
    
    for( MGNode *a_1 : n_1->anodes ) {
        MGNode *a_0 = get_match(a_1);
        float sz = sz1 + a_1->anodes.size();
        //float sz = sz1f + a_1->afnodes.size();
        if( !n_10 || !a_0 ) {
            cost += costs.unm / sz;
        } else {
            if( !n_0->has_edge(a_0) ) {
                cost += costs.mis / sz;
            }
            
            float d0=length(p_0-a_0->pos), d1=length(p_1-a_1->pos);
            cost += costs.stretch * fabs(d0-d1) / (d0+d1+0.0001f) / sz;
        }
    }
    
    return cost;
}

float MeshMatch::compute_cost_after_change( MGNode *n_0, MGNode *n_1 ) {
    float cost = 0.0f;
    
    assert(n_0->type==n_1->type);
    assert(n_0->m==m0 && n_1->m==m1);
    
    MGNode *n_01 = get_match(n_0), *n_10 = get_match(n_1);
    assert( (n_01==n_1 && n_10==n_0) || (!n_01 && !n_10) );
    
    float sz0 = n_0->anodes.size();
    float sz1 = n_1->anodes.size();
    vec3f &p_0 = n_0->pos;
    vec3f &p_1 = n_1->pos;
    
    if( n_01 && n_10 ) {
        cost += costs.alpha * 2.0f;
    } else {
        float dist = length(p_0 - p_1);
        float norm = dot(n_0->norm,n_1->norm);
        cost += costs.dist * dist / (dist + 1.0f);
        cost += costs.norm * (1.0f - norm);
        
        cost += costs.valence * fabs(sz0-sz1) / (float)(sz0+sz1);
    }
    
    
    for( MGNode *a_0 : n_0->anodes ) {
        MGNode *a_1 = get_match(a_0);
        float sz = sz0 + a_0->anodes.size();
        if( n_01 || !a_1 ) {
            cost += costs.unm / sz;
        } else {
            if( !n_1->has_edge(a_1) ) {
                cost += costs.mis / sz;
            }
            
            float d0=length(p_0-a_0->pos), d1=length(p_1-a_1->pos);
            cost += costs.stretch * fabs(d0-d1) / (d0+d1+0.0001f) / sz;
        }
    }
    
    for( MGNode *a_1 : n_1->anodes ) {
        MGNode *a_0 = get_match(a_1);
        float sz = sz1 + a_1->anodes.size();
        if( n_10 || !a_0 ) {
            cost += costs.unm / sz;
        } else {
            if( !n_0->has_edge(a_0) ) {
                cost += costs.mis / sz;
            }
            
            float d0=length(p_0-a_0->pos), d1=length(p_1-a_1->pos);
            cost += costs.stretch * fabs(d0-d1) / (d0+d1+0.0001f) / sz;
        }
    }
    
    return cost;
}

// TODO: fix
float MeshMatch::compute_cost() {
    float cost = 0.0f;
    
    // accum costs of matched nodes
    for( MGNode *n0 : m0->vnodes ) {
        MGNode *n1 = get_match(n0);
        if(n1) cost += compute_cost(n0,n1);
        else {
            cost += costs.alpha;
            float sz0 = n0->anodes.size();
            for( MGNode *a0 : n0->anodes ) {
                float sza = a0->anodes.size();
                cost += costs.unm / (float) (sz0+sza);
            }
        }
    }
    for( MGNode *n0 : m0->fnodes ) {
        MGNode *n1 = get_match(n0);
        if(n1) cost += compute_cost(n0,n1);
        else {
            cost += costs.alpha;
            float sz0 = n0->anodes.size();
            for( MGNode *a0 : n0->anodes ) {
                float sza = a0->anodes.size();
                cost += costs.unm / (float) (sz0+sza);
            }
        }
    }
    for( MGNode *n1 : m1->vnodes ) if(!get_match(n1)) {
        cost += costs.alpha;
        float sz1 = n1->anodes.size();
        for( MGNode *a1 : n1->anodes ) {
            float sza = a1->anodes.size();
            cost += costs.unm / (float) (sz1+sza);
        }
    }
    for( MGNode *n1 : m1->fnodes ) if(!get_match(n1)) {
        cost += costs.alpha;
        float sz1 = n1->anodes.size();
        for( MGNode *a1 : n1->anodes ) {
            float sza = a1->anodes.size();
            cost += costs.unm / (float) (sz1+sza);
        }
    }
    
    return cost;
}

float MeshMatch::compute_cost_delta( MeshMatchChange *mmc ) {
    //dochange(mmc);
    mmc->cost_delta = compute_cost_after_change( mmc->n_0, mmc->n_1 );
    //undochange(mmc);
    mmc->cost_delta -= compute_cost( mmc->n_0, mmc->n_1 );
    return mmc->cost_delta;
}



void MeshMatch::add_potential_changes( MGNode *n, BinaryHeap<MeshMatchChange*,float> *bh ) {
    int i = n->i;
    Mesh *m = n->m;
    MGNode::Type t = n->type;
    
    auto fn_add = [&](MGNode *n, MGNode *n_, bool add) {
        if( n->m == m1 ) swap(n,n_);
        if( map_node_nodes[n].count(n_) ) return;
        
        MeshMatchChange *mmc = new MeshMatchChange( n, n_, add );
        compute_cost_delta(mmc);
        if( mmc->cost_delta > 0.0f ) { delete mmc; return; }
        
        auto chg = bh->insert(mmc,mmc->cost_delta);
        map_node_bhn[n].insert(chg); map_node_bhn[n_].insert(chg);
        map_node_nodes[n].insert(n_); map_node_nodes[n_].insert(n);
    };
    
    MGNode *n_ = get_match(n);
    
    if( n_ ) {
        fn_add( n, n_, false );
        return;
    }
    
    // add knn as potential matches
    vector<vector<int>> *knn_;
    vector<MGNode*> *nodes_;
    vector<int> *match;
    if( m == m0 ) {
        if( t == MGNode::Vertex ) {
            knn_   = &v0_knn1;
            nodes_ = &m1->vnodes;
            match  = &vm10;
        } else {
            knn_   = &f0_knn1;
            nodes_ = &m1->fnodes;
            match  = &fm10;
        }
    } else {
        if( t == MGNode::Vertex ) {
            knn_   = &v1_knn0;
            nodes_ = &m0->vnodes;
            match  = &vm01;
        } else {
            knn_   = &f1_knn0;
            nodes_ = &m0->fnodes;
            match  = &fm01;
        }
    }
    for( int i_ : (*knn_)[i] ) {
        if( (*match)[i_] == -1 ) fn_add( n, (*nodes_)[i_], true );
    }
    
    // add srball and expanded knn as potential matches
    for( MGNode *an : n->anodes ) {
        MGNode *an_ = get_match(an); if( !an_ ) continue;
        
        Mesh *m_ = an_->m;
        bool isvert = ( an_->type == MGNode::Vertex );
        vector<MGNode*> &ln_pot     = ((isvert ? m_->vsrball : m_->fsrball)[an_->i]);
        vector<int> &li_pot_knn     = ((isvert ? m_->vknn : m_->fknn)[an_->i]);
        vector<MGNode*> &ln_pot_knn = (isvert ? m_->vnodes : m_->fnodes);
        
        for( MGNode *n_ : ln_pot ) {
            if( n_->type != n->type ) continue;         // wrong type
            if( get_match(n_) ) continue;               // already matched
            fn_add( n, n_, true );
        }
        
        if( costs.expand_knn ) {
            for( int i_ : li_pot_knn ) {
                if(i_ == -1) continue;                      // can be -1 if k > number of nodes!
                MGNode *n_ = ln_pot_knn[i_];
                if( n_->type != n->type ) continue;         // wrong type
                if( get_match(n_) ) continue;               // already matched
                fn_add( n, n_, true );
            }
        }
    }
    
}


void MeshMatch::assign_exact( int knn, float d ) {
    dlog.start_("Assigning exactly aligned nodes");
    
    vector<int> li_knn;
    
    for( int i_v0 = 0; i_v0 < n_v0; i_v0++ ) {
        if( vm01[i_v0] != -1 ) continue;
        vec3f &v0 = m0->vpos[i_v0];
        
        m1->vkdt->knn( v0, knn, li_knn );
        for( int i_v1 : li_knn ) {
            if( vm10[i_v1] != -1 ) continue;
            if( lengthSqr( v0 - m1->vpos[i_v1] ) < d ) { vassign(i_v0,i_v1); break; }
        }
    }
    for( int i_f0 = 0; i_f0 < n_f0; i_f0++ ) {
        if( fm01[i_f0] != -1 ) continue;
        vec3f &f0 = m0->fpos[i_f0];
        
        m1->fkdt->knn( f0, knn, li_knn );
        for( int i_f1 : li_knn ) {
            if( fm10[i_f1] != -1 ) continue;
            if( lengthSqr( f0 - m1->fpos[i_f1] ) < d ) { fassign(i_f0,i_f1); break; }
        }
    }
    
    dlog.end();
}


void MeshMatch::assign_no_geo_cost() {
    dlog.start_("Assigning exactly aligned nodes");
    
    int knn = 15;
    float epsilon = 0.01f;
    vector<int> li_knn;
    
    int count = 0;
    
    for( int i_v0 = 0; i_v0 < n_v0; i_v0++ ) {
        if( vm01[i_v0] != -1 ) continue;
        vec3f &v0 = m0->vpos[i_v0];
        vec3f &n0 = m0->vnorms[i_v0];
        
        m1->vkdt->knn( v0, knn, li_knn );
        for( int i_v1 : li_knn ) {
            if( vm10[i_v1] != -1 ) continue;
            float cost = 0.0f;
            float dist = length( v0 - m1->vpos[i_v1] );
            float norm = dot( n0, m1->vnorms[i_v1] );
            cost += costs.dist * dist / (dist + 1.0f);
            cost += costs.norm * (1.0f - norm);
            if( cost > epsilon ) continue;
            count++;
            vassign(i_v0,i_v1); break;
        }
    }
    for( int i_f0 = 0; i_f0 < n_f0; i_f0++ ) {
        if( fm01[i_f0] != -1 ) continue;
        vec3f &f0 = m0->fpos[i_f0];
        vec3f &n0 = m0->fnorms[i_f0];
        
        m1->fkdt->knn( f0, knn, li_knn );
        for( int i_f1 : li_knn ) {
            if( fm10[i_f1] != -1 ) continue;
            float cost = 0.0f;
            float dist = length( f0 - m1->fpos[i_f1] );
            float norm = dot( n0, m1->fnorms[i_f1] );
            cost += costs.dist * dist / (dist + 1.0f);
            cost += costs.norm * (1.0f - norm);
            if( cost > epsilon ) continue;
            count++;
            fassign(i_f0,i_f1); break;
        }
    }
    
    dlog.print("Assigned: %d", count);
    
    dlog.end();
}

void MeshMatch::unassign_mismatched() {
    dlog.start("Unassigning nodes with mismatches");
    
    unordered_set<MGNode*> nodes;
    for( MGNode *n : m0->vnodes ) get_mismatches(n,nodes);
    for( MGNode *n : m0->fnodes ) get_mismatches(n,nodes);
    for( MGNode *n : m1->vnodes ) get_mismatches(n,nodes);
    for( MGNode *n : m1->fnodes ) get_mismatches(n,nodes);
    
    dlog.print("nodes: %d", nodes.size());
    
    for( MGNode *n : nodes ) {
        if( get_match(n) ) clear(n);
    }
    
    dlog.end();
}

void MeshMatch::unassign_mismatched_faces() {
    dlog.start("Unassigning faces with mismatches");
    
    /*unordered_set<MGNode*> nodes;
    for( MGNode *n : m0->fnodes ) {
        for( MGNode *nv : n->avnodes ) {
            
        }
        for( MGNode *an : n->anodes ) {
            if( n->type == an->type ) continue;
            MGNode *an_o = get_match(an); if( !an_o ) continue;
            if( !n_o->has_edge(an_o) ) {
                //ln_o.insert(n); ln_o.insert(n_o); ln_o.insert(an_o); ln_o.insert(an);
                if( n->type == MGNode::Face ) { ln_o.insert(n); ln_o.insert(n_o); }
                else { ln_o.insert(an_o); ln_o.insert(an); }
            }
        }
        
    }
    
    for( MGNode *n : m0->vnodes ) get_mismatches(n,nodes);
    for( MGNode *n : m0->fnodes ) get_mismatches(n,nodes);
    for( MGNode *n : m1->vnodes ) get_mismatches(n,nodes);
    for( MGNode *n : m1->fnodes ) get_mismatches(n,nodes);
    
    dlog.print("nodes: %d", nodes.size());
    
    for( MGNode *n : nodes ) {
        if( get_match(n) ) clear(n);
    }*/
    
    dlog.end();
}

void MeshMatch::unassign_twisted() {
    dlog.start("Unassigning twisted faces");
    
    unordered_set<MGNode*> nodes;
    for( MGNode *n : m0->fnodes ) if(is_twisted(n)) {
        //nodes.insert(n);
        nodes.insert( n->avnodes.begin(), n->avnodes.end() );
    }
    
    dlog.print("nodes: %d", nodes.size());
    
    for( MGNode *n : nodes ) {
        if( get_match(n) ) clear(n);
    }
    
    dlog.end();
}

void MeshMatch::unassign_verts_with_any_unmatched_faces() {
    dlog.start("Unassigning verts with any unmatched faces");
    
    unordered_set<MGNode*> nodes;
    for( MGNode *n : m0->vnodes ) {
        MGNode *n_ = get_match(n);
        if( !n_ ) continue;
        
        for( MGNode *n_f : n->afnodes ) if( !get_match(n_f) ) {
            nodes.insert(n);
            nodes.insert(n_);
            break;
        }
        for( MGNode *n_f : n_->afnodes ) if( !get_match(n_f) ) {
            nodes.insert(n);
            nodes.insert(n_);
            break;
        }
    }
    
    dlog.print("nodes: %d", nodes.size());
    
    for( MGNode *n : nodes ) {
        if( get_match(n) ) clear(n);
    }
    
    dlog.end();
}

void MeshMatch::unassign_verts_with_all_unmatched_faces() {
    dlog.start("Unassigning verts with all unmatched faces");
    
    unordered_set<MGNode*> nodes;
    for( MGNode *n : m0->vnodes ) {
        MGNode *n_ = get_match(n);
        if( !n_ ) continue;
        
        bool all = true;
        for( MGNode *n_f : n->afnodes ) if( get_match(n_f) ) { all = false; break; }
        for( MGNode *n_f : n_->afnodes ) if( get_match(n_f) ) { all = false; break; }
        if( all ) { nodes.insert(n); nodes.insert(n_); }
    }
    
    dlog.print("nodes: %d", nodes.size());
    
    for( MGNode *n : nodes ) {
        if( get_match(n) ) clear(n);
    }
    
    dlog.end();
}

void MeshMatch::unassign_faces_with_unmatched_verts() {
    dlog.start("Unassigning faces with unmatched vertices");
    
    unordered_set<MGNode*> nodes;
    for( MGNode *n : m0->fnodes ) {
        if(!get_match(n)) continue;
        bool any = false;
        for( MGNode *v : n->avnodes ) {
            if( !get_match(v) ) { any = true; break; }
        }
        if( any ) nodes.insert(n);
    }
    for( MGNode *n : m1->fnodes ) {
        if(!get_match(n)) continue;
        bool any = false;
        for( MGNode *v : n->avnodes ) {
            if( !get_match(v) ) { any = true; break; }
        }
        if( any ) nodes.insert(n);
    }
    
    dlog.print("nodes: %d", nodes.size());
    
    for( MGNode *n : nodes ) {
        if( get_match(n) ) clear(n);
    }
    
    dlog.end();
}

void MeshMatch::unassign_small_patches(int sz) {
    dlog.start("Unassigning small patches of matched faces");
    
    unordered_set<MGNode*> nodes;
    
    for( MGNode *n : m0->fnodes ) if( get_match(n) ) nodes.insert(n);
    while( !nodes.empty() ) {
        unordered_set<MGNode*> patch;
        unordered_set<MGNode*> grow;
        
        MGNode *n = *nodes.begin();
        grow.insert(n);
        while( !grow.empty() ) {
            MGNode *n = *grow.begin(); grow.erase(n);
            if( patch.count(n) ) continue;
            patch.insert(n);
            for( MGNode *an : n->afnodes ) if( nodes.count(an) ) grow.insert(an);
        }
        
        for( MGNode *n : patch ) nodes.erase(n);
        
        if( patch.size() >= sz ) continue;
        
        for( MGNode *f : patch ) {
            for( MGNode *v : f->avnodes ) if(get_match(v)) clear(v);
            clear(f);
        }
    }
    
    for( MGNode *n : m1->fnodes ) if( get_match(n) ) nodes.insert(n);
    while( !nodes.empty() ) {
        unordered_set<MGNode*> patch;
        unordered_set<MGNode*> grow;
        
        MGNode *n = *nodes.begin();
        grow.insert(n);
        while( !grow.empty() ) {
            MGNode *n = *grow.begin(); grow.erase(n);
            if( patch.count(n) ) continue;
            patch.insert(n);
            for( MGNode *an : n->afnodes ) if( nodes.count(an) ) grow.insert(an);
        }
        
        for( MGNode *n : patch ) nodes.erase(n);
        
        if( patch.size() >= sz ) continue;
        
        for( MGNode *f : patch ) {
            for( MGNode *v : f->avnodes ) if(get_match(v)) clear(v);
            clear(f);
        }
    }
    
    dlog.end();
}
void MeshMatch::unassign_small_patches(float per) {
    dlog.start("Unassigning small patches of matched faces");
    
    unordered_set<MGNode*> nodes;
    
    for( MGNode *n : m0->fnodes ) if( get_match(n) ) nodes.insert(n);
    while( !nodes.empty() ) {
        unordered_set<MGNode*> patch;
        unordered_set<MGNode*> grow;
        
        MGNode *n = *nodes.begin();
        grow.insert(n);
        while( !grow.empty() ) {
            MGNode *n = *grow.begin(); grow.erase(n);
            if( patch.count(n) ) continue;
            patch.insert(n);
            for( MGNode *an : n->afnodes ) if( nodes.count(an) ) grow.insert(an);
        }
        
        for( MGNode *n : patch ) nodes.erase(n);
        
        int i_comp = (*patch.begin())->i_comp;
        int sz_comp = m0->components[i_comp].size();
        if( patch.size() >= (int)((float)sz_comp*per) ) continue;
        
        for( MGNode *f : patch ) {
            //for( MGNode *v : f->avnodes ) if(get_match(v)) clear(v);
            clear(f);
        }
    }
    
    for( MGNode *n : m1->fnodes ) if( get_match(n) ) nodes.insert(n);
    while( !nodes.empty() ) {
        unordered_set<MGNode*> patch;
        unordered_set<MGNode*> grow;
        
        MGNode *n = *nodes.begin();
        grow.insert(n);
        while( !grow.empty() ) {
            MGNode *n = *grow.begin(); grow.erase(n);
            if( patch.count(n) ) continue;
            patch.insert(n);
            for( MGNode *an : n->afnodes ) if( nodes.count(an) ) grow.insert(an);
        }
        
        for( MGNode *n : patch ) nodes.erase(n);
        
        int i_comp = (*patch.begin())->i_comp;
        int sz_comp = m1->components[i_comp].size();
        if( patch.size() >= (int)((float)sz_comp*per) ) continue;
        
        for( MGNode *f : patch ) {
            //for( MGNode *v : f->avnodes ) if(get_match(v)) clear(v);
            clear(f);
        }
    }
    
    unassign_verts_with_all_unmatched_faces();
    
    dlog.end();
}

void MeshMatch::cleanup() {
    dlog.start("Cleanup");
    unassign_twisted();
    unassign_mismatched();
    unassign_faces_with_unmatched_verts();
    unassign_verts_with_all_unmatched_faces();
    dlog.end();
}

void MeshMatch::algorithm() {
    dlog.start("Performing MeshGit Algorithm");
    
    if(true) {
        dlog.start("Priming with exact matches");
        //assign_exact(costs.knn, 0.001f);
        //cleanup();
        reset();
        assign_no_geo_cost();
        dlog.end();
    }
    init_greedy();
    
    costs.dist_mult = costs.norm_mult = 0.75f;
    
    assign_greedy();
    
    dlog.end();
}

void MeshMatch::assign_greedy() {
    
    dlog.start("Greedy Assignment");
    
    float d = costs.dist;
    float n = costs.norm;
    
    costs.dist = 1.0f;
    costs.norm = 1.0f;
    
    for( float per = 0.08f; per >= 0.01f; per *= 0.5f ) {
        assign_greedy_step();
        cleanup();
        
        dlog.start("Backtracking");
        unassign_small_patches(per);
        dlog.end();
        
        if( true ) {
            costs.dist *= costs.dist_mult;
            costs.norm *= costs.norm_mult;
        }
    }
    
    //assign_greedy_step();
    //cleanup();
    
    costs.dist = d;
    costs.norm = n;
    
    dlog.end();
}

void MeshMatch::init_greedy() {
    if( l_srball != costs.srball ) {
        dlog.start_("Recomputing surface r-balls (r=%d)", costs.srball);
        auto t0 = thread([&]{ m0->recompute_srball(costs.srball); });
        auto t1 = thread([&]{ m1->recompute_srball(costs.srball); });
        t0.join();
        t1.join();
        dlog.end();
        
        l_srball = costs.srball;
    }
    
    if( l_knn != costs.knn ) {
        dlog.start_("Recomputing knn (k=%d) between meshes",costs.knn);
        
        v0_knn1.resize(n_v0); f0_knn1.resize(n_f0);
        v1_knn0.resize(n_v1); f1_knn0.resize(n_f1);
        auto t_v01 = thread([&] {
            for( int i_v0 = 0; i_v0 < n_v0; i_v0++ ) m1->vkdt->knn( m0->vpos[i_v0], costs.knn, v0_knn1[i_v0] );
        });
        auto t_f01 = thread([&] {
            for( int i_f0 = 0; i_f0 < n_f0; i_f0++ ) m1->fkdt->knn( m0->fpos[i_f0], costs.knn, f0_knn1[i_f0] );
        });
        auto t_v10 = thread([&] {
            for( int i_v1 = 0; i_v1 < n_v1; i_v1++ ) m0->vkdt->knn( m1->vpos[i_v1], costs.knn, v1_knn0[i_v1] );
        });
        auto t_f10 = thread([&] {
            for( int i_f1 = 0; i_f1 < n_f1; i_f1++ ) m0->fkdt->knn( m1->fpos[i_f1], costs.knn, f1_knn0[i_f1] );
        });
        
        t_v01.join(); t_f01.join(); t_v10.join(); t_f10.join();
        
        m0->recompute_knn(costs.knn);
        m1->recompute_knn(costs.knn);
        
        dlog.end();
        
        l_knn = costs.knn;
    }
    
    if( !map_initialized ) {
        dlog.start_("Initializing changes data structure");
        map_node_bhn.reserve(m0->n_v+m0->n_f+m1->n_v+m1->n_f);
        map_node_nodes.reserve(m0->n_v+m0->n_f+m1->n_v+m1->n_f);
        for( auto n : m0->vnodes ) { map_node_bhn[n].reserve(75); map_node_nodes[n].reserve(75); }
        for( auto n : m0->fnodes ) { map_node_bhn[n].reserve(75); map_node_nodes[n].reserve(75); }
        for( auto n : m1->vnodes ) { map_node_bhn[n].reserve(75); map_node_nodes[n].reserve(75); }
        for( auto n : m1->fnodes ) { map_node_bhn[n].reserve(75); map_node_nodes[n].reserve(75); }
        map_initialized=true;
        dlog.end();
    }
    
}

void MeshMatch::assign_greedy_step(int max) {
    timer uptimer;
    
    init_greedy();
    
    dlog.start( "Greedy Step" );
    float delta = 0.0f;
    
    bh_changes = new BinaryHeap<MeshMatchChange*,float>();
    
    dlog.start_("Adding initial potential changes");
    if(false) {
        auto bh_v01 = new BinaryHeap<MeshMatchChange*,float>();
        auto bh_v10 = new BinaryHeap<MeshMatchChange*,float>();
        auto bh_f01 = new BinaryHeap<MeshMatchChange*,float>();
        auto bh_f10 = new BinaryHeap<MeshMatchChange*,float>();
        auto t_v01 = thread([&]{ for( MGNode *n : m0->vnodes ) add_potential_changes( n, bh_v01 ); });
        auto t_v10 = thread([&]{ for( MGNode *n : m1->vnodes ) add_potential_changes( n, bh_v10 ); });
        auto t_f01 = thread([&]{ for( MGNode *n : m0->fnodes ) add_potential_changes( n, bh_f01 ); });
        auto t_f10 = thread([&]{ for( MGNode *n : m1->fnodes ) add_potential_changes( n, bh_f10 ); });
        t_v01.join(); bh_changes->combine(bh_v01);
        t_v10.join(); bh_changes->combine(bh_v10);
        t_f01.join(); bh_changes->combine(bh_f01);
        t_f10.join(); bh_changes->combine(bh_f10);
    } else {
        for( MGNode *n : m0->vnodes ) add_potential_changes( n, bh_changes );
        for( MGNode *n : m1->vnodes ) add_potential_changes( n, bh_changes );
        for( MGNode *n : m0->fnodes ) add_potential_changes( n, bh_changes );
        for( MGNode *n : m1->fnodes ) add_potential_changes( n, bh_changes );
    }
    dlog.end();
    
    dlog.print("Number of potential changes: %d", bh_changes->size() );
    
    dlog.start("Greedily applying changes");
    int iter = 0;
    double t_rem = 0.0f;
    double t_add = 0.0f;
    double s;
    while( !bh_changes->empty() ) {
        auto bn_min = bh_changes->minimum();
        
        float c = bn_min->key();
        if( c > 0.0f ) break;
        delta += c;
        
        MeshMatchChange *mmc = bn_min->data();
        
        // find nodes that are affected by mmc
        unordered_set<MGNode*> nodes;
        nodes.insert(mmc->n_0); nodes.insert(mmc->n_0->anodes.begin(),mmc->n_0->anodes.end());
        nodes.insert(mmc->n_1); nodes.insert(mmc->n_1->anodes.begin(),mmc->n_1->anodes.end());
        
        // perform change
        dochange(mmc);
        
        // remove old changes involving nodes affected by mmc
        s = uptimer.elapsed();
        for( auto n : nodes ) {
            for( auto bhn : map_node_bhn[n] ) {
                auto mmc = bhn->data();
                if( mmc->n_0 == n ) {
                    map_node_bhn[mmc->n_1].erase(bhn);
                    map_node_nodes[mmc->n_1].erase(n);
                } else {
                    map_node_bhn[mmc->n_0].erase(bhn);
                    map_node_nodes[mmc->n_0].erase(n);
                }
                bh_changes->remove_delete(bhn);
            }
            map_node_bhn[n].clear();
            map_node_nodes[n].clear();
        }
        t_rem += uptimer.elapsed()-s;
        
        // add new potential changes for nodes affected by mmc
        s = uptimer.elapsed();
        auto bh_local = new BinaryHeap<MeshMatchChange*,float>();
        for( auto n : nodes ) add_potential_changes(n,bh_local);
        bh_changes->combine(bh_local);
        delete bh_local;
        //for( auto n : nodes ) add_potential_changes(n,bh_local);
        t_add += uptimer.elapsed()-s;
        
        iter++;
        if(iter == max) break;
    }
    dlog.print("time del: %f", t_rem);
    dlog.print("time add: %f", t_add);
    dlog.end();
    
    dlog.start_("Cleaning up");
    for( auto n : m0->vnodes ) { map_node_bhn[n].clear(); map_node_nodes[n].clear(); }
    for( auto n : m0->fnodes ) { map_node_bhn[n].clear(); map_node_nodes[n].clear(); }
    for( auto n : m1->vnodes ) { map_node_bhn[n].clear(); map_node_nodes[n].clear(); }
    for( auto n : m1->fnodes ) { map_node_bhn[n].clear(); map_node_nodes[n].clear(); }
    bh_changes->clear_delete(); delete bh_changes; bh_changes = nullptr;
    dlog.end();
    
    dlog.print( "Cost Delta: %0.2f", delta );
    dlog.print( "Iterations: %d", iter );
    
    dlog.end();
}

void MeshMatch::greedy_backtrack( float per ) {
    dlog.fn("Greedy Backtrack",[&]{
        //dlog.start("Cleanup");
        //unassign_twisted();
        //unassign_mismatched();
        //unassign_faces_with_unmatched_verts();
        //unassign_verts_with_all_unmatched_faces();
        //dlog.end();
        
        dlog.start("Backtracking");
        unassign_small_patches(per);
        dlog.end();
    });
}


