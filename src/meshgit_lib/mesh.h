#ifndef _MESH_H_
#define _MESH_H_

#include "kdtree.h"
#include "binheap.h"

#include "../vec.h"
#include "../debug.h"
#include "../std.h"

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "../gls.h"
#include "../geom.h"


using namespace std;


typedef int iface;
typedef int ivert;



struct Costs {
    float alpha = 1.0f; //3.0f;
    float dist = 1.0f, norm = 1.0f;  float dist_mult = 0.8f, norm_mult = 0.8f;
    float unm = 1.0f, mis = 1.0f; //4.0f;
    float valence = 0.0f, stretch = 1.0f; //0.1f;
    int knn = 10, srball = 2;
    bool expand_knn = true;
};





class Mesh;

class MGNode {
public:
    enum Type { Vertex, Face };
    
    MGNode( Mesh *m, int i, Type type, vec3f pos, vec3f norm ) :
    m(m), i(i), type(type), pos(pos), norm(norm) { }
    ~MGNode() { }
    
    void add_edge( MGNode *other ) {
        if( this == other ) return;
        if( other->type == Face ) afnodes.insert(other); else avnodes.insert(other);
        if( type == Face ) other->afnodes.insert(this); else other->avnodes.insert(this);
        anodes.insert(other); other->anodes.insert(this);
    }
    
    bool has_edge( MGNode *other ) { return anodes.count(other)!=0; }
    
    // back reference
    Mesh *m;
    int i;
    int i_comp;
    
    // node labels
    Type type;
    vec3f pos;
    vec3f norm;
    
    // edge (node-node) information
    unordered_set<MGNode*> avnodes;
    unordered_set<MGNode*> afnodes;
    unordered_set<MGNode*> anodes;
};



class Mesh {
public:
    
    Mesh( string &fn_ply ) : Mesh(fn_ply.c_str()) { }
    Mesh( const char *fn_ply ) {
        filename = fn_ply;
        
        dlog.start("Creating mesh from %s",fn_ply);
        
        dlog.start("Loading");
        loadply();
        dlog.print("verts = %d", n_v);
        dlog.print("faces = %d", n_f);
        dlog.end();
        
        vknn.resize(n_v); vsrball.resize(n_v);
        fknn.resize(n_f); fsrball.resize(n_f);
        
        dlog.end();
    }
    
    void init( float scale, vec3f &o ) {
        dlog.start("Initializing mesh");
        
        dlog.start("Initializing mesh");
        dlog.print("Offset: (%0.2f,%0.2f,%0.2f)",o.x,o.y,o.z);
        dlog.print("Scale: %f", scale);
        for( int i_v = 0; i_v < n_v; i_v++ ) vpos[i_v] = (vpos[i_v]+o) * scale;
        for( int i_f = 0; i_f < n_f; i_f++ ) fpos[i_f] = (fpos[i_f]+o) * scale;
        dlog.end_quiet();
        
        dlog.start("Building kd-trees");
        // build kd-trees
        vkdt = new MGKDTree(); vkdt->insert_shuffle(vpos);
        fkdt = new MGKDTree(); fkdt->insert_shuffle(fpos);
        dlog.end_quiet();
        
        dlog.fn_quiet("Building nodes",[&]{
        
            dlog.fn_quiet("Creating nodes for vertices and faces",[&]{
                for( ivert i_v = 0; i_v < n_v; i_v++ ) {
                    MGNode *n = new MGNode( this, i_v, MGNode::Vertex, vpos[i_v], vnorms[i_v] );
                    vnodes.push_back(n); nodes.insert(n);
                }
                for( iface i_f = 0; i_f < n_f; i_f++ ) {
                    MGNode *n = new MGNode( this, i_f, MGNode::Face, fpos[i_f], fnorms[i_f] );
                    fnodes.push_back(n); nodes.insert(n);
                }
            });
            
            dlog.fn_quiet("Creating edges between nodes",[&]{
                for( iface i_f = 0; i_f < n_f; i_f++ ) {
                    MGNode *f = fnodes[i_f];
                    vector<ivert> &vinds = finds[i_f];
                    int n_vinds = vinds.size();
                    
                    // face-vert edges
                    for( ivert i_v : finds[i_f] ) vnodes[i_v]->add_edge(f);
                    
                    // vert-vert edges
                    for( int i0 = 0; i0 < n_vinds; i0++ ) vnodes[vinds[i0]]->add_edge(vnodes[vinds[(i0+1)%n_vinds]]);
                }
                // face-face edges
                for( MGNode *v : vnodes ) {
                    unordered_set<MGNode*> &fnodes = v->afnodes;
                    for( MGNode *av : v->avnodes ) {
                        unordered_set<MGNode*> &afnodes = av->afnodes;
                        unordered_set<MGNode*> intersection;
                        for( MGNode *f : fnodes ) if( afnodes.count(f) ) intersection.insert(f);
                        for( MGNode *f : intersection ) for( MGNode *af : intersection ) f->add_edge(af);
                    }
                }
            });
            
            dlog.fn_quiet("Determining connected components",[&]{
                unordered_set<MGNode*> left(nodes.begin(),nodes.end());
                while(!left.empty()) {
                    unordered_set<MGNode*> comp_nodes;
                    unordered_set<MGNode*> grow;
                    MGNode *n = *left.begin(); left.erase(n); grow.insert(n);
                    while(!grow.empty()) {
                        MGNode *n = *grow.begin();
                        grow.erase(n);
                        left.erase(n);
                        comp_nodes.insert(n);
                        for( MGNode *an : n->anodes ) if(left.count(an)) grow.insert(an);
                    }
                    int i_comp = components.size(); components.push_back(comp_nodes);
                    for( MGNode *n : comp_nodes ) n->i_comp = i_comp;
                }
                dlog.print("Component count: %d", components.size());
            });
            
        });
        
        dlog.end();
    }
    
    void loadply();
    
    // k-nearest neighbors to self by euclidean distance
    void recompute_knn( int knn ) {
#       pragma omp parallel for default(none) shared(knn)
        for( ivert i_v = 0; i_v < n_v; i_v++ ) {
            vkdt->knn( vpos[i_v], knn, vknn[i_v] );
        }
        for( iface i_f = 0; i_f < n_f; i_f++ ) {
            fkdt->knn( fpos[i_f], knn, fknn[i_f] );
        }
    }
    
    // all neighbors within surface "radius" ball
    void recompute_srball( int r ) {
        //dlog.start_("Recomputing surface r-balls");
#       pragma omp parallel for default(none) shared(r)
        for( int i_v = 0; i_v < n_v; i_v++ ) {
            MGNode *v = vnodes[i_v];
            unordered_set<MGNode*> srball; srball.reserve(150); srball.insert(v);
            unordered_set<MGNode*> search; search.reserve(150); search.insert(v);
            for( int d = 0; d < r; d++ ) {
                unordered_set<MGNode*> next; next.reserve(150);
                for( MGNode *n : search ) next.insert(n->anodes.begin(),n->anodes.end());
                for( MGNode *n : srball ) next.erase(n);
                for( MGNode *n : next ) srball.insert(n);
                if( i_v < n_v-1 ) { search.clear(); search.insert ( next.begin(), next.end() ); }
            }
            vsrball[v->i].assign( srball.begin(),srball.end() );
        }
#       pragma omp parallel for default(none) shared(r)
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            MGNode *f = fnodes[i_f];
            unordered_set<MGNode*> srball; srball.reserve(150); srball.insert(f);
            unordered_set<MGNode*> search; search.reserve(150); search.insert(f);
            for( int d = 0; d < r; d++ ) {
                unordered_set<MGNode*> next; next.reserve(150);
                for( MGNode *n : search ) next.insert(n->anodes.begin(),n->anodes.end());
                for( MGNode *n : srball ) next.erase(n);
                for( MGNode *n : next ) srball.insert(n);
                if( i_f < n_f-1 ) { search.clear(); search.insert( next.begin(), next.end() ); }
            }
            fsrball[f->i].assign( srball.begin(),srball.end() );
        }
        //dlog.end();
    }
    
    string                  filename;
    
    ivert                   n_v;
    vector<vec3f>           vpos;
    vector<vec3f>           vnorms;
    MGKDTree                *vkdt;
    vector<MGNode*>         vnodes;
    
    iface                   n_f;
    vector<vector<ivert>>   finds;
    vector<vec3f>           fpos;
    vector<vec3f>           fnorms;
    MGKDTree                *fkdt;
    vector<MGNode*>         fnodes;
    
    unordered_set<MGNode*>  nodes;
    
    vector<vector<ivert>>   vknn;
    vector<vector<iface>>   fknn;
    vector<vector<MGNode*>> vsrball;
    vector<vector<MGNode*>> fsrball;
    
    vector<unordered_set<MGNode*>> components;
};


class SimpleMesh {
public:
    struct VertInfo {
        float dist;
        bool edge;
    };
    vector<vec3f> vpos;
    vector<VertInfo> vinfo;
    
    vector<vector<ivert>> finds;
    vector<vec3f> fnorms;
    vector<material> fmats;
    vector<material> fmats_;
    vector<bool> fcolor;
    
    void draw( bool b_faces, bool b_edges, material &match, material &match_ );
    void draw_override( bool b_faces, bool b_edges, material &mat, material &mat_, int edge_darkness );
    
    SimpleMesh *subd_catmullclark();
    
    void xform( vec3f offset, float scale ) {
        for( int i_v = 0; i_v < vpos.size(); i_v++ ) {
            vpos[i_v] = vpos[i_v] * scale + offset;
        }
    }
    
    void save_ply( const char *fn, const char *comment );
    void save_light(const char *fn);
    void save_light_changes(const char *fn);
    
    void clean() {
        vector<int> map_iv_iv( vpos.size(), -1 );
        vector<vec3f> vpos_new;
        vector<VertInfo> vinfo_new;
        vector<vector<ivert>> finds_new;
        for( vector<ivert> &vinds : finds ) {
            vector<ivert> vinds_new;
            for( int i_v : vinds ) {
                if( map_iv_iv[i_v] == -1 ) {
                    map_iv_iv[i_v] = vpos_new.size();
                    vpos_new.push_back(vpos[i_v]);
                    vinfo_new.push_back(vinfo[i_v]);
                }
                vinds_new.push_back(map_iv_iv[i_v]);
            }
            finds_new.push_back(vinds_new);
        }
        swap(vpos,vpos_new);
        swap(finds,finds_new);
        swap(vinfo,vinfo_new);
    }
    
    void recompute_normals() {
        auto f_face_normal = [&](vector<ivert>&vinds)->vec3f {
            vec3f norm = zero3f;
            vec3f f = zero3f;
            int n = vinds.size();
            
            for( ivert i : vinds ) f += vpos[i];
            f /= (float)n;
            
            for( int i0 = 0; i0 < n; i0++ ) {
                int i1 = (i0+1)%n;
                norm += triangle_normal(f,vpos[vinds[i0]],vpos[vinds[i1]]);
            }
            return normalize(norm);
        };
        
        fnorms.clear();
        for( vector<ivert> &vinds : finds ) fnorms.push_back( f_face_normal(vinds) );
    }
};



class MeshMatchChange {
public:
    
    MeshMatchChange( MGNode *n_0, MGNode *n_1, bool addmatch ) :
    n_0(n_0), n_1(n_1), addmatch(addmatch) { }
    
    MGNode *n_0;
    MGNode *n_1;
    bool addmatch;
    float cost_delta;
};




class MeshMatch {
public:
    
    MeshMatch( Mesh *m0, Mesh *m1 ) : m0(m0), m1(m1) {
        n_v0 = m0->n_v; n_f0 = m0->n_f;
        n_v1 = m1->n_v; n_f1 = m1->n_f;
        
        reset();
    }
    
    static void init_meshes( vector<Mesh*> &meshes ) {
        vec3f offset; float s;
        init_meshes( meshes, offset, s );
    }
    static void init_meshes( vector<Mesh*> &meshes, vec3f &offset, float &s ) {
        float el = 0.0f; int n = 0;
        
        dlog.start("Initializing meshes");
        
        vec3f pmin = meshes[0]->vpos[0], pmax = meshes[0]->vpos[0];
        
        for( Mesh *m : meshes ) {
            for( vector<ivert> &vinds : m->finds ) {
                int c = vinds.size();
                for( int i = 0; i < c; i++ ) el += length(m->vpos[vinds[i]] - m->vpos[vinds[(i+1)%c]]);
                n += c;
            }
            for( vec3f &p : m->vpos ) {
                pmin = min_component(pmin,p); pmax = max_component(pmax,p);
            }
        }
        
        s = (float)n / el;
        dlog.print("Scaling factor: %0.2f",s);
        
        offset = -(pmin+pmax)/2.0f;
        
        for( Mesh *m : meshes ) m->init(s,offset);
        
        dlog.end();
    }
    
    void load( const char *fn );
    void save( const char *fn );
    
    float compute_cost( MGNode *n_0, MGNode *n_1 );
    float compute_cost_after_change( MGNode *n_0, MGNode *n_1 );
    float compute_cost();
    float compute_cost_delta( MeshMatchChange *mmc );
    
    void algorithm();
    void init_greedy();
    void assign_greedy();
    void assign_greedy_step(int max=-1);
    void greedy_backtrack( float per );
    
    void assign_exact( int knn, float d );
    void assign_no_geo_cost();
    void assign_bipartite();
    
    void unassign_mismatched();
    void unassign_mismatched_faces();
    void unassign_twisted();
    void unassign_verts_with_any_unmatched_faces();
    void unassign_verts_with_all_unmatched_faces();
    void unassign_faces_with_unmatched_verts();
    void unassign_small_patches( int sz );
    void unassign_small_patches( float per );
    
    void cleanup();
    
    
    void reset() {
        vm01.assign(n_v0,-1); fm01.assign(n_f0,-1);
        vm10.assign(n_v1,-1); fm10.assign(n_f1,-1);
    }
    
    void assert_consistency() {
        bool good = true;
        for( int i_v0 = 0; i_v0 < n_v0; i_v0++ ) good &= (vm01[i_v0]==-1 || vm10[vm01[i_v0]]==i_v0);
        for( int i_v1 = 0; i_v1 < n_v1; i_v1++ ) good &= (vm10[i_v1]==-1 || vm01[vm10[i_v1]]==i_v1);
        for( int i_f0 = 0; i_f0 < n_f0; i_f0++ ) good &= (fm01[i_f0]==-1 || fm10[fm01[i_f0]]==i_f0);
        for( int i_f1 = 0; i_f1 < n_f1; i_f1++ ) good &= (fm10[i_f1]==-1 || fm01[fm10[i_f1]]==i_f1);
        if( good ) return;
        
        printf("\n\n\nConsistency assertion failed\n");
        printf("v0\n"); for( int i_v0 = 0; i_v0 < n_v0; i_v0++ ) printf( "%d->%d  ",i_v0,vm01[i_v0]);
        printf("v1\n"); for( int i_v1 = 0; i_v1 < n_v1; i_v1++ ) printf( "%d->%d  ",i_v1,vm10[i_v1]);
        printf("f0\n"); for( int i_f0 = 0; i_f0 < n_f0; i_f0++ ) printf( "%d->%d  ",i_f0,fm01[i_f0]);
        printf("f1\n"); for( int i_f1 = 0; i_f1 < n_f1; i_f1++ ) printf( "%d->%d  ",i_f1,fm10[i_f1]);
        assert(false);
    }
    
    // does not reassign or break a previous assignment!
    void vassign(int i_v0,int i_v1) {
        assert(vm01[i_v0]==-1 && vm10[i_v1]==-1);
        vm01[i_v0] = i_v1; vm10[i_v1] = i_v0;
    }
    void fassign(int i_f0,int i_f1) {
        assert(fm01[i_f0]==-1 && fm10[i_f1]==-1);
        fm01[i_f0] = i_f1; fm10[i_f1] = i_f0;
    }
    void assign(MGNode *n_0,MGNode *n_1) {
        assert(n_0->type==n_1->type);
        if(n_0->type == MGNode::Vertex) {
            vassign(n_0->i,n_1->i);
        } else {
            fassign(n_0->i,n_1->i);
        }
    }
    
    void vclear0(int i_v0) { vclear(i_v0,vm01[i_v0]); }
    void vclear1(int i_v1) { vclear(vm10[i_v1],i_v1); }
    void vclear(int i_v0,int i_v1) {
        assert(i_v0!=-1 && i_v1!=-1 && vm01[i_v0]==i_v1 && vm10[i_v1]==i_v0);
        vm01[i_v0] = -1; vm10[i_v1] = -1;
    }
    void fclear0(int i_f0) { fclear(i_f0,fm01[i_f0]); }
    void fclear1(int i_f1) { fclear(fm10[i_f1],i_f1); }
    void fclear(int i_f0,int i_f1) {
        assert(i_f0!=-1 && i_f1!=-1 && fm01[i_f0]==i_f1 && fm10[i_f1]==i_f0);
        fm01[i_f0] = -1; fm10[i_f1] = -1;
    }
    void clear(MGNode *n_0,MGNode *n_1) {
        assert(n_0->type==n_1->type);
        if(n_0->type == MGNode::Vertex) {
            vclear(n_0->i,n_1->i);
        } else {
            fclear(n_0->i,n_1->i);
        }
    }
    void clear(MGNode *n) {
        assert(n->m==m0 || n->m==m1);
        if(n->type == MGNode::Vertex) {
            if(n->m == m0) vclear0(n->i);
            else vclear1(n->i);
        } else {
            if(n->m == m0) fclear0(n->i);
            else fclear1(n->i);
        }
    }
    
    MGNode *get_match(MGNode *n) {
        assert(n->m==m0 || n->m==m1);
        if( n->m == m0 ) {
            if( n->type == MGNode::Vertex ) {
                int i1 = vm01[n->i];
                if( i1 == -1 ) return nullptr;
                else return m1->vnodes[i1];
            } else {
                int i1 = fm01[n->i];
                if( i1 == -1 ) return nullptr;
                else return m1->fnodes[i1];
            }
        } else {
            if( n->type == MGNode::Vertex ) {
                int i0 = vm10[n->i];
                if( i0 == -1 ) return nullptr;
                else return m0->vnodes[i0];
            } else {
                int i0 = fm10[n->i];
                if( i0 == -1 ) return nullptr;
                else return m0->fnodes[i0];
            }
        }
    }
    
    bool has_mismatch(MGNode *n) {
        assert(n->m==m0 || n->m==m1);
        MGNode *n_o = get_match(n);
        if( !n_o ) return false;
        for( MGNode *an : n->anodes ) {
            MGNode *an_o = get_match(an);
            if( !an_o ) continue;
            if( !n_o->has_edge(an_o) ) return true;
        }
        return false;
    }
    
    void get_mismatches(MGNode *n, unordered_set<MGNode*> &ln_o) {
        assert(n->m==m0 || n->m==m1);
        MGNode *n_o = get_match(n); if( !n_o ) return;
        for( MGNode *an : n->anodes ) {
            if( n->type == an->type ) continue;
            MGNode *an_o = get_match(an); if( !an_o ) continue;
            if( !n_o->has_edge(an_o) ) {
                //ln_o.insert(n); ln_o.insert(n_o); ln_o.insert(an_o); ln_o.insert(an);
                if( n->type == MGNode::Face ) { ln_o.insert(n); ln_o.insert(n_o); }
                else { ln_o.insert(an_o); ln_o.insert(an); }
            }
        }
        //for( MGNode *an_o : n_o->anodes ) {
        //    MGNode *an = get_match(an_o); if( !an ) continue;
        //    if( !n->has_edge(an) ) { ln_o.insert(n); ln_o.insert(n_o); ln_o.insert(an_o); ln_o.insert(an); }
        //}
    }
    
    bool is_twisted(MGNode *n) {
        assert(n->m==m0 || n->m==m1);
        assert(n->type==MGNode::Face);
        
        MGNode *n_ = get_match(n); if(!n_) return false;
        
        vector<ivert> &vm_0 = (n->m==m0 ? vm01 : vm10);
        
        // tranlate vertex inds to other mesh
        vector<ivert> &lvi0 = (n->m==m0 ? m0->finds[n->i] : m1->finds[n->i]);
        vector<ivert> lvi_0; for( ivert i : lvi0 ) lvi_0.push_back(vm_0[i]);
        vector<ivert> &lvi_1 = (n_->m==m0 ? m0->finds[n_->i] : m1->finds[n_->i]);
        
        if( lvi_0.size() != lvi_1.size() ) return false;
        for( ivert i : lvi_0 ) {
            bool f = false;
            for( ivert i_ : lvi_1 ) if( i == i_ ) { f=true; break; }
            if( !f ) return false;
        }
        for( ivert i : lvi_0 ) if( i==-1 ) return false;
        for( ivert i : lvi_1 ) if( i==-1 ) return false;
        
        // check if inds are out-of-order, but allow for rotation
        int nv = lvi_0.size();
        bool all;
        for( int i0 = 0; i0 < nv; i0++ ) {
            all=true;
            for( int io = 0; io < nv; io++ ) {
                int i1 = (i0+io)%nv;
                if( lvi_0[io] != lvi_1[i1] ) { all=false; break; }
            }
            if( all ) return false;
            all = true;
            for( int io = 0; io < nv; io++ ) {
                int i1 = (nv+i0-io)%nv;
                if( lvi_0[io] != lvi_1[i1] ) { all=false; break; }
            }
            if( all ) return false;
        }
        return true;
    }
    
    void dochange( MeshMatchChange *mmc ) {
        if( mmc->addmatch ) assign(mmc->n_0,mmc->n_1);
        else clear(mmc->n_0,mmc->n_1);
    }
    void undochange( MeshMatchChange *mmc ) {
        if( mmc->addmatch ) clear(mmc->n_0,mmc->n_1);
        else assign(mmc->n_0,mmc->n_1);
    }
    
    string name;
    
    Mesh *m0; int n_v0, n_f0;
    Mesh *m1; int n_v1, n_f1;
    
    // must assert consistency!
    vector<int> vm01, vm10;
    vector<int> fm01, fm10;
    
    Costs costs;

    // greedy algorithm variables
    BinaryHeap<MeshMatchChange*,float> *bh_changes = nullptr;
    vector<vector<int>> v0_knn1, v1_knn0, f0_knn1, f1_knn0;
    bool map_initialized = false; int l_knn = -1, l_srball = -1;
    unordered_map<MGNode*,unordered_set<BinaryHeapNode<MeshMatchChange*,float>*>> map_node_bhn;
    unordered_map<MGNode*,unordered_set<MGNode*>> map_node_nodes;
    void add_potential_changes( MGNode *n, BinaryHeap<MeshMatchChange*,float> *bh );
};


class MeshMatchPartition {
public:
    
    static vector<MeshMatchPartition*> partition( MeshMatch *mm ) {
        vector<MeshMatchPartition*> partitions;
        Mesh *m0 = mm->m0;
        Mesh *m1 = mm->m1;
        vector<ivert> &vm01 = mm->vm01, &vm10 = mm->vm10;
        vector<iface> &fm01 = mm->fm01, &fm10 = mm->fm10;
        
        
        // fills s_if_part and s_iv_part with elements on same partition as i_f (has no matches).  fills s_iv_border with elements on border (has match)
        auto fn_getpartition = []( Mesh *m, vector<int>&vm, vector<int>&fm, iface i_f, unordered_set<iface> &s_if_part, unordered_set<ivert> &s_iv_part, unordered_set<ivert> &s_iv_border ) {
            unordered_set<iface> s_if_crawl; s_if_crawl.insert(i_f);
            while( !s_if_crawl.empty() ) {
                iface i_f = pop(s_if_crawl);
                if( s_if_part.count(i_f) ) continue;        // already seen this?
                
                s_if_part.insert(i_f);
                
                // crawl
                for( auto a : m->fnodes[i_f]->anodes ) {
                    if( a->type == MGNode::Face ) {
                        if( fm[a->i] == -1 ) s_if_crawl.insert(a->i);
                    } else {
                        if( vm[a->i] == -1 ) s_iv_part.insert(a->i);
                        else s_iv_border.insert(a->i);
                    }
                }
            }
        };
        
        
        // build partitions
        unordered_set<iface> s_if_del, s_if_add;
        for( int i_f = 0; i_f < m0->n_f; i_f++ ) if( mm->fm01[i_f] == -1 ) s_if_del.insert(i_f);
        for( int i_f = 0; i_f < m1->n_f; i_f++ ) if( mm->fm10[i_f] == -1 ) s_if_add.insert(i_f);
        while( !s_if_del.empty() || !s_if_add.empty() ) {
            unordered_set<iface> s_if0_part, s_if1_part;
            unordered_set<ivert> s_iv0_part, s_iv0_border, s_iv1_part, s_iv1_border;
            
            unordered_set<iface> s_if0_crawl, s_if1_crawl;
            
            if( !s_if_del.empty() ) {
                fn_getpartition( m0, vm01, fm01, *s_if_del.begin(), s_if0_part, s_iv0_part, s_iv0_border );
                for( int i_v0 : s_iv0_border ) {
                    for( auto a : m1->vnodes[vm01[i_v0]]->afnodes ) if( fm10[a->i] == -1 ) s_if1_crawl.insert(a->i);
                }
            } else {
                fn_getpartition( m1, vm10, fm10, *s_if_add.begin(), s_if1_part, s_iv1_part, s_iv1_border );
                for( int i_v1 : s_iv1_border ) {
                    for( auto a : m0->vnodes[vm10[i_v1]]->afnodes ) if( fm01[a->i] == -1 ) s_if0_crawl.insert(a->i);
                }
            }
            
            for( bool growing = true; growing; ) {
                growing = false;
                
                while( !s_if0_crawl.empty() ) {
                    int i_f0 = pop(s_if0_crawl);
                    if( s_if0_part.count(i_f0) ) continue;
                    
                    growing = true;
                    
                    fn_getpartition( m0, vm01, fm01, i_f0, s_if0_part, s_iv0_part, s_iv0_border );
                    for( int i_v0 : s_iv0_border ) {
                        for( auto a : m1->vnodes[vm01[i_v0]]->afnodes ) if( fm10[a->i] == -1 ) s_if1_crawl.insert(a->i);
                    }
                }
                
                while( !s_if1_crawl.empty() ) {
                    int i_f1 = pop(s_if1_crawl);
                    if( s_if1_part.count(i_f1) ) continue;
                    
                    growing = true;
                    
                    fn_getpartition( m1, vm10, fm10, i_f1, s_if1_part, s_iv1_part, s_iv1_border );
                    for( int i_v1 : s_iv1_border ) {
                        for( auto a : m0->vnodes[vm10[i_v1]]->afnodes ) if( fm01[a->i] == -1 ) s_if0_crawl.insert(a->i);
                    }
                }
            }
            
            MeshMatchPartition *mmp = new MeshMatchPartition();
            mmp->mm = mm;
            mmp->m0 = m0;
            mmp->m1 = m1;
            
            // remove partitioned set from set of deleted and added elements
            for( iface i_f : s_if0_part ) s_if_del.erase(i_f);
            for( iface i_f : s_if1_part ) s_if_add.erase(i_f);
            
            // record partition information
            swap( mmp->siv0_border, s_iv0_border );
            swap( mmp->siv0, s_iv0_part );
            swap( mmp->sif0, s_if0_part );
            
            swap( mmp->siv1_border, s_iv1_border );
            swap( mmp->siv1, s_iv1_part );
            swap( mmp->sif1, s_if1_part );
            
            partitions.push_back(mmp);
            
        }
        
        return partitions;
    }
    
    MeshMatch *mm;
    Mesh *m0, *m1;
    
    unordered_set<ivert> siv0_border, siv1_border;
    unordered_set<ivert> siv0, siv1;
    unordered_set<iface> sif0, sif1;
};





#endif
