#include "meshgit.h"

void material_setdarker( material &mat, material &mat_) {
    mat_ = mat;
    mat_.diffuse *= 0.5f;
    mat_.ambient *= 0.5f;
}
vec3f get_move_color( vec3f &v0, vec3f &v1, vec3f &color0, vec3f &color1 ) {
    float d = 0.1f / (0.1f + lengthSqr(v0-v1));
    return color0 * d + color1 * (1.0f-d);
}
vec3f get_move_color( float dist, vec3f &color0, vec3f &color1 ) {
    float d = 0.1f / (0.1f + dist*dist);
    return color0 * d + color1 * (1.0f-d);
}

void set_material( material &mat, material &mat_ ) {
    if( glob_mesh_faces_twotoned ) glsMaterial( mat, mat_ );
    else glsMaterial( mat, mat );
}
void set_material( GLenum pname, vec3f c, vec3f c_ ) {
    if( glob_mesh_faces_twotoned ) glsMaterialfv( pname, c, c_ );
    else glsMaterialfv( pname, c, c );
}


void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos ) {
    int n = vinds.size();
    glsNormal(norm);
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        glsVertex(vpos[vinds[i0]]);
        glsVertex(vpos[vinds[i1]]);
        glsVertex(vpos[vinds[i2]]);
    }
}

void draw_face( vector<ivert> &vinds, vector<vec3f> &vpos ) {
    int n = vinds.size(); if( n < 3 ) return;
    glsNormal( triangle_normal(vpos[vinds[0]],vpos[vinds[1]],vpos[vinds[2]]) );
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        glsVertex(vpos[vinds[i0]]);
        glsVertex(vpos[vinds[i1]]);
        glsVertex(vpos[vinds[i2]]);
    }
}

void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vector<int> &vm, vector<vec3f> &vpos_, vec3f &c0, vec3f &c0_, vec3f &c1, vec3f &c1_ ) {
    vec3f c_bad = { 1.0, 0.0, 0.0 }, c_bad_ = c_bad * 0.5f;
    int n = vinds.size();
    glsNormal(norm);
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        vec3f &v0=vpos[vinds[i0]], &v1=vpos[vinds[i1]], &v2=vpos[vinds[i2]];
        int iv0_ = vm[vinds[i0]], iv1_=vm[vinds[i1]], iv2_=vm[vinds[i2]];
        if( iv0_ == -1 ) {
            set_material(GL_AMBIENT_AND_DIFFUSE, c_bad, c_bad_ );
        } else {
            vec3f &v0_ = vpos_[iv0_];
            set_material(GL_AMBIENT_AND_DIFFUSE, get_move_color(v0,v0_,c0,c1), get_move_color(v0,v0_,c0_,c1_));
        }
        glsVertex(v0);
        
        if( iv1_ == -1 ) {
            set_material(GL_AMBIENT_AND_DIFFUSE, c_bad, c_bad_ );
        } else {
            vec3f &v1_ = vpos_[iv1_];
            set_material(GL_AMBIENT_AND_DIFFUSE, get_move_color(v1,v1_,c0,c1), get_move_color(v1,v1_,c0_,c1_));
        }
        glsVertex(v1);
        
        if( iv2_ == -1 ) {
            set_material(GL_AMBIENT_AND_DIFFUSE, c_bad, c_bad_ );
        } else {
            vec3f &v2_ = vpos_[iv2_];
            set_material(GL_AMBIENT_AND_DIFFUSE, get_move_color(v2,v2_,c0,c1), get_move_color(v2,v2_,c0_,c1_));
        }
        glsVertex(v2);
    }
}
void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vector<vec3f> &vpos_, vec3f &c0, vec3f &c0_, vec3f &c1, vec3f &c1_ ) {
    int n = vinds.size();
    glsNormal(norm);
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        vec3f &v0=vpos[vinds[i0]], &v1=vpos[vinds[i1]], &v2=vpos[vinds[i2]];
        vec3f &v0_=vpos_[vinds[i0]],&v1_=vpos_[vinds[i1]],&v2_=vpos_[vinds[i2]];
        set_material(GL_AMBIENT_AND_DIFFUSE, get_move_color(v0,v0_,c0,c1), get_move_color(v0,v0_,c0_,c1_));
        glsVertex(v0);
        set_material(GL_AMBIENT_AND_DIFFUSE, get_move_color(v1,v1_,c0,c1), get_move_color(v1,v1_,c0_,c1_));
        glsVertex(v1);
        set_material(GL_AMBIENT_AND_DIFFUSE, get_move_color(v2,v2_,c0,c1), get_move_color(v2,v2_,c0_,c1_));
        glsVertex(v2);
    }
}

void draw_face( vector<ivert> &vinds, vec3f &norm, vector<vec3f> &vpos, vec3f &offset ) {
    int n = vinds.size();
    glsNormal(norm);
    for( int i = 1, i0 = 0; i < n; i++ ) {
        int i1 = i%n, i2 = (i+1)%n;
        glsVertex(vpos[vinds[i0]]+offset);
        glsVertex(vpos[vinds[i1]]+offset);
        glsVertex(vpos[vinds[i2]]+offset);
    }
}


SimpleMesh * subd_mesh(Mesh *m, int subd_level) {
    auto mm = new SimpleMesh();
    mm->vpos = m->vpos;
    for( auto v : m->vpos ) mm->vinfo.push_back( { 0.0f, true } );
    auto &finds = m->finds;
    for( int i_f = 0; i_f < finds.size(); i_f++ ) {
        //if( !fshow[i_f] ) continue;
        mm->finds.push_back(finds[i_f]);
        mm->fmats.push_back(mat_neutral);
        mm->fmats_.push_back(mat_neutral);
        mm->fcolor.push_back(true);
    }
    mm->clean();
    mm->recompute_normals();
    for( int l = 0; l < subd_level; l++ ) {
        auto mm_ = mm->subd_catmullclark();
        swap(mm,mm_);
        delete mm_;
    }
    return mm;
}

SimpleMesh * remove_same(SimpleMesh *m) {
    auto *m2 = new SimpleMesh();
    m2->vpos = m->vpos;
    m2->vinfo = m->vinfo;
    for(int i_f=0; i_f<m->finds.size(); i_f++) {
        if(!m->fcolor[i_f]) continue;
        m2->finds.push_back(m->finds[i_f]);
        m2->fmats.push_back(m->fmats[i_f]);
        m2->fmats_.push_back(m->fmats_[i_f]);
        m2->fcolor.push_back(m->fcolor[i_f]);
    }
    m2->clean();
    m2->recompute_normals();
    return m2;
}

SimpleMesh * subd_m1_2way(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level) {
    return subdivide_mesh(m1, m0, subd_level, mm01, matset);
}
SimpleMesh * subd_m0_2way(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level) {
    return subdivide_mesh(m0, m1, subd_level, mm01, matset);
}
SimpleMesh * subd_m1_2way_changes(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level) {
    auto m = subdivide_mesh(m1, m0, subd_level, mm01, matset);
    auto m2 = remove_same(m);
    delete m;
    return m2;
}
SimpleMesh * subd_m0_2way_changes(Mesh *m0, Mesh *m1, MeshMatch *mm01, material_set &matset, int subd_level) {
    auto m = subdivide_mesh(m0, m1, subd_level, mm01, matset);
    auto m2 = remove_same(m);
    delete m;
    return m2;
}
SimpleMesh * subd_m0_3way(Mesh *m0, MeshMatch *mm0a, MeshMatch *mm0b, int subd_level) {
    auto mm = new SimpleMesh();
    mm->vpos = m0->vpos;
    for( int i_v = 0; i_v < m0->vpos.size(); i_v++ ) mm->vinfo.push_back( { 0, true } );
    for( int i_f = 0; i_f < m0->finds.size(); i_f++ ) {
        material mat, mat_;
        bool c = true;
        
        if( mm0a->fm01[i_f] == -1 && mm0b->fm01[i_f] == -1 ) {
            mat = mat_ = mat_unmatch0ab;
        } else if( mm0a->fm01[i_f] == -1 ) {
            mat = mat_ = mat_unmatch0a;
        } else if( mm0b->fm01[i_f] == -1 ) {
            mat = mat_ = mat_unmatch0b;
        } else {
            mat = mat_ = mat_match;
            c = false;
        }
        
        mm->finds.push_back(m0->finds[i_f]);
        mm->fmats.push_back(mat);
        mm->fmats_.push_back(mat_);
        mm->fcolor.push_back(c);
    }
    mm->clean();
    mm->recompute_normals();
    
    for( int l = 0; l < subd_level; l++ ) {
        auto mm_ = mm->subd_catmullclark();
        swap(mm,mm_);
        delete mm_;
    }
    
    return mm;
}
SimpleMesh * subd_m0_3way_changes(Mesh *m0, MeshMatch *mm0a, MeshMatch *mm0b, int subd_level) {
    auto m = subd_m0_3way(m0,mm0a,mm0b,subd_level);
    auto m2 = remove_same(m);
    delete m;
    return m2;
}
SimpleMesh * subd_seq(Mesh *mprev, Mesh *m, Mesh *mnext, MeshMatch *mmprev, MeshMatch *mmnext, int subd_level) {
    auto mm = new SimpleMesh();
    
    mm->vpos = m->vpos;
    for( int i_v = 0; i_v < m->vpos.size(); i_v++ ) {
        if(!mprev) {
            mm->vinfo.push_back({0,true});
        } else {
            if(mmprev->vm10[i_v] == -1) mm->vinfo.push_back({0,true});
            else mm->vinfo.push_back({length(m->vpos[i_v]-mprev->vpos[mmprev->vm10[i_v]]), true});
        }
    }
    
    for( int i_f = 0; i_f < m->finds.size(); i_f++ ) {
        material mat = mat_match, mat_ = mat_match;
        bool c = false;
        
        if(!mprev and !mnext) {
        } else if(!mprev) {
            if(mmnext->fm01[i_f] == -1) {
                mat = mat_ = mat_unmatch0;
                c = true;
            }
        } else if(!mnext) {
            if(mmprev->fm10[i_f] == -1) {
                mat = mat_ = mat_unmatch1;
                c = true;
            }
        } else {
            if(mmprev->fm10[i_f] == -1 and mmnext->fm01[i_f] == -1) {
                mat = mat_ = mat_unmatch01;
                c = true;
            } else if(mmprev->fm10[i_f] == -1) {
                mat = mat_ = mat_unmatch1;
                c = true;
            } else if(mmnext->fm01[i_f] == -1) {
                mat = mat_ = mat_unmatch0;
                c = true;
            } else {
            }
        }
        
        mm->finds.push_back(m->finds[i_f]);
        mm->fmats.push_back(mat);
        mm->fmats_.push_back(mat_);
        mm->fcolor.push_back(c);
    }
    mm->clean();
    mm->recompute_normals();
    
    for( int l = 0; l < subd_level; l++ ) {
        auto mm_ = mm->subd_catmullclark();
        swap(mm,mm_);
        delete mm_;
    }
    
    return mm;
}

SimpleMesh * subd_mesh(Mesh *m0, const vector<vec3f> &vpos, const vector<vector<int>> &finds, const vector<bool> &fshow, const vector<material> &fmats, const vector<bool> &fcolor, int subd_level) {
    auto mm = new SimpleMesh();
    
    mm->vpos = vpos;
    for( int i_v = 0; i_v < vpos.size(); i_v++ ) {
        float dist = lengthSqr( m0->vpos[i_v] - vpos[i_v] );
        float d = sqrt(dist);
        mm->vinfo.push_back( { d, true } );
    }
    for( int i_f = 0; i_f < finds.size(); i_f++ ) {
        if( !fshow[i_f] ) continue;
        mm->finds.push_back(finds[i_f]);
        mm->fmats.push_back(fmats[i_f]);
        mm->fmats_.push_back(fmats[i_f]);
        mm->fcolor.push_back(fcolor[i_f]);
    }
    mm->clean();
    mm->recompute_normals();
    
    for( int l = 0; l < subd_level; l++ ) {
        auto mm_ = mm->subd_catmullclark();
        swap(mm,mm_);
        delete mm_;
    }
    
    return mm;
}

