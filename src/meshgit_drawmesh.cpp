#include "meshgit.h"


SimpleMesh *subdivide_mesh(Mesh *m0, Mesh *m1, int subd_level, MeshMatch *mm, const material_set &matset) {
    const auto &vpos = m0->vpos; int sz_vpos = vpos.size();
    const auto &finds = m0->finds; int sz_finds = finds.size();
    
    bool b01 = m0 == mm->m0;
    
    const auto &vm = (b01 ? mm->vm01 : mm->vm10);
    const auto &fm = (b01 ? mm->fm01 : mm->fm10);
    
    const auto &mat_neu  = matset.face;
    const auto &mat_neu_ = matset.face_;
    const auto &mat_chg  = (b01 ? matset.face_del : matset.face_add);
    const auto &mat_chg_ = (b01 ? matset.face_del_ : matset.face_add_);
    
    auto mesh = new SimpleMesh();
    
    mesh->vpos = vpos;
    for(int i_v0 = 0; i_v0 < sz_vpos; i_v0++) {
        int i_v1 = vm[i_v0];
        if(!b01 and i_v1 != -1) mesh->vinfo.push_back({ length(vpos[i_v0] - m1->vpos[i_v1]), true });
        else mesh->vinfo.push_back({ 0.0f, true });
    }
    mesh->finds = finds;
    for(int i_f0 = 0; i_f0 < sz_finds; i_f0++) {
        if(fm[i_f0] == -1) {
            mesh->fmats.push_back(mat_chg);
            mesh->fmats_.push_back(mat_chg_);
            mesh->fcolor.push_back(true);
        } else {
            mesh->fmats.push_back(mat_neu);
            mesh->fmats_.push_back(mat_neu_);
            mesh->fcolor.push_back(false);
        }
    }
    
    mesh->clean();
    mesh->recompute_normals();
    
    for( int l = 0; l < subd_level; l++ ) {
        auto mesh_ = mesh->subd_catmullclark();
        delete mesh;
        mesh = mesh_;
    }
    
    return mesh;
}


void draw_mesh( Mesh *m ) {
    vector<vec3f> &vpos = m->vpos;
    vector<vector<ivert>> &finds = m->finds;
    vector<vec3f> &fnorms = m->fnorms;
    int n_f = m->n_f;
    
    vec3f c_n = { 0.7f, 0.7f, 0.7f }, c_n_ = c_n * 0.5f;
    
    if( glob_mesh_faces ) {
        set_material(GL_AMBIENT_AND_DIFFUSE,c_n,c_n_);
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            draw_face( finds[i_f], fnorms[i_f], vpos );
        }
        glEnd();
    }
    
    glDepthRange(0.0f,0.999999f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LIGHTING);
    
    if( glob_mesh_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( glob_mesh_edges_darkness ) {
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
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
    }
    
    glEnable(GL_LIGHTING);
    glDepthRange(0.0f,1.0f);
}

void draw_mesh( Mesh *m, vector<ivert> &vm, vector<iface> &fm, vec3f c_e_unm, Mesh *m_, vector<vector<ivert>> &vknn, vector<vector<ivert>> &fknn ) {
    static timer uptimer;
    
    vector<vec3f> &vpos = m->vpos;
    vector<vec3f> &fpos = m->fpos;
    vector<vector<ivert>> &finds = m->finds;
    vector<vec3f> &fnorms = m->fnorms;
    int n_v = m->n_v;
    int n_f = m->n_f;
    
    vector<vec3f> &vpos_ = m_->vpos;
    vector<vec3f> &fpos_ = m_->fpos;
    vector<vector<ivert>> &finds_ = m_->finds;
    vector<vec3f> &fnorms_ = m_->fnorms;
    int n_f_ = m_->n_f;
    
    vec3f c_n = { 0.7f, 0.7f, 0.7f }, c_n_ = c_n * 0.5f;
    vec3f c_unm = { 0.5f, 0.5f, 0.5f }, c_unm_ = c_unm * 0.5f;
    vec3f c_e_unm_ = c_e_unm * 0.5f, c_e_unm__ = c_e_unm_ * 0.5f;
    
    float morph_blend = uptimer.elapsed_sine(0.5f);
    float morph_blend_ = 1.0f - morph_blend;
    
    if( glob_mesh_faces ) {
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            if( fm[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                set_material(GL_AMBIENT_AND_DIFFUSE,c_unm,c_unm_);
                set_material(GL_EMISSION,c_e_unm,c_e_unm_);
            } else {
                if( glob_mesh_faces_chgsonly ) continue;
                set_material(GL_AMBIENT_AND_DIFFUSE,c_n,c_n_);
                set_material(GL_EMISSION, zero3f, zero3f);
            }
            
            draw_face( finds[i_f], fnorms[i_f], vpos );
        }
        glEnd();
        set_material(GL_EMISSION, zero3f, zero3f);
    }
    
    if( glob_mesh_components ) {
        srand(0);
        vector<vec3f> comp_colors, comp_colors_;
        for( int i_c = 0; i_c < m->components.size(); i_c++ ) {
            vec3f c = { (float)((rand()%128)+128)/256.0f,(float)((rand()%128)+128)/256.0f,(float)((rand()%128)+128)/256.0f };
            comp_colors.push_back( c );
            comp_colors_.push_back( c * 0.5f );
        }
        
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            set_material(GL_AMBIENT_AND_DIFFUSE,comp_colors[m->fnodes[i_f]->i_comp],comp_colors_[m->fnodes[i_f]->i_comp]);
            vector<ivert> &_vinds = finds[i_f];
            int n = _vinds.size();
            const vec3f &norm = fnorms[i_f];
            glsNormal(norm);
            int i0 = 0;
            for( int i = 1; i < n; i++ ) {
                int i1 = i%n, i2 = (i+1)%n;
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
                glsVertex(vpos[_vinds[i2]]);
            }
        }
        glEnd();
    }
    
    if( glob_morph_faces ) {
        glBegin(GL_TRIANGLES);
        set_material(GL_AMBIENT_AND_DIFFUSE,c_n,c_n_);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            int i_f_ = fm[i_f]; if(i_f_==-1) continue;
            
            vector<ivert> &_vinds = finds[i_f];
            int n = _vinds.size();
            const vec3f &norm = fnorms[i_f];
            const vec3f &norm_ = fnorms_[i_f_];
            
            glsNormal(normalize((norm*morph_blend)+(norm_*morph_blend_)));
            
            int i0 = 0;
            for( int i = 1; i < n; i++ ) {
                int i1 = i%n, i2 = (i+1)%n;
                int iv0 = _vinds[i0], iv1 = _vinds[i1], iv2 = _vinds[i2];
                int iv0_ = vm[iv0], iv1_ = vm[iv1], iv2_ = vm[iv2];
                if( iv0_ == -1 || iv1_ == -1 || iv2_ == -1 ) continue;
                glsVertex((vpos[iv0]*morph_blend) + (vpos_[iv0_]*morph_blend_));
                glsVertex((vpos[iv1]*morph_blend) + (vpos_[iv1_]*morph_blend_));
                glsVertex((vpos[iv2]*morph_blend) + (vpos_[iv2_]*morph_blend_));
            }
        }
        glEnd();
    }
    
    glDepthRange(0.0f,0.999999f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LIGHTING);
    
    if( glob_knn_vert ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glColor4f(0.2f,0.2f,0.8f,0.1f);
        
        glBegin(GL_LINES);
        for( int i_v = 0; i_v < n_v; i_v++ ) {
            for( int i_v_ : vknn[i_v] ) {
                glsVertex(vpos[i_v]);
                glsVertex(vpos_[i_v_]);
            }
        }
        glEnd();
    }
    if( glob_knn_face ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glColor4f(0.2f,0.2f,0.8f,0.1f);
        
        glBegin(GL_LINES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            for( int i_f_ : fknn[i_f] ) {
                glsVertex(fpos[i_f]);
                glsVertex(fpos_[i_f_]);
            }
        }
        glEnd();
    }
    
    if( glob_mesh_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( glob_mesh_edges_darkness ) {
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
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
    }
    
    if( glob_morph_verts ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPointSize(3.0f);
        glColor4f(0.2f,0.2f,0.2f,0.5f);
        glBegin(GL_POINTS);
        for( int i_v = 0; i_v < n_v; i_v++ ) {
            int i_v_ = vm[i_v];
            if( i_v_ == -1 ) continue;
            glsVertex( (vpos[i_v]*morph_blend) + (vpos_[i_v_]*morph_blend_) );
        }
        glEnd();
        glDisable(GL_BLEND);
    }
    
    if( glob_connect_matched_verts ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.0f);
        glColor4f(0.5f,0.5f,0.8f,0.5f);
        glBegin(GL_LINES);
        for( int i_v = 0; i_v < n_v; i_v++ ) {
            int i_v_ = vm[i_v];
            if( i_v_ == -1 ) continue;
            glsVertex( vpos[i_v] ); glsVertex( vpos_[i_v_] );
        }
        glEnd();
        glDisable(GL_BLEND);
    }
    if( glob_connect_matched_faces ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.0f);
        glColor4f(0.5f,0.5f,0.8f,0.5f);
        glBegin(GL_LINES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            int i_f_ = fm[i_f]; if( i_f_ == -1 ) continue;
            glsVertex( fpos[i_f] ); glsVertex( fpos_[i_f_] );
        }
        glEnd();
        glDisable(GL_BLEND);
    }
    
    if( glob_overlay_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        glColor4f(0.8f,0.8f,0.8f,0.25f);
        for( int i_f = 0; i_f < n_f_; i_f++ ) {
            vector<ivert> &_vinds = finds_[i_f];
            int n = _vinds.size();
            for( int i0 = 0; i0 < n; i0++ ) {
                int i1 = (i0+1)%n;
                glsVertex(vpos_[_vinds[i0]]);
                glsVertex(vpos_[_vinds[i1]]);
            }
        }
        glColor4f(0.6f,0.6f,0.8f,0.25f);
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
        glDisable(GL_BLEND);
    }
    
    glEnable(GL_LIGHTING);
    glDepthRange(0.0f,1.0f);
}



void draw_mesh( Mesh *m, vector<ivert> &vm, vector<iface> &fm, material &mat_unmatch, material &mat_unmatch_ ) {
    vector<vec3f> &vpos = m->vpos;
    vector<vector<ivert>> &finds = m->finds;
    vector<vec3f> &fnorms = m->fnorms;
    int n_f = m->n_f;
    
    if( glob_mesh_faces ) {
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            if( fm[i_f] == -1 && fm[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                set_material( mat_unmatch, mat_unmatch_ );
            } else {
                if( glob_mesh_faces_chgsonly ) continue;
                set_material( mat_match, mat_match_ );
            }
            
            draw_face( finds[i_f], fnorms[i_f], vpos );
        }
        glEnd();
        set_material(mat_neutral,mat_neutral);
    }
    
    glDepthRange(0.0f,0.999999f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LIGHTING);
    
    if( glob_mesh_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( glob_mesh_edges_darkness ) {
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
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
    }
    
    
    glEnable(GL_LIGHTING);
    glDepthRange(0.0f,1.0f);
}

void draw_mesh( Mesh *m, Mesh *m_prev, vector<ivert> &vm, vector<iface> &fm, material &mat_unmatch, material &mat_unmatch_ ) {
    vector<vec3f> &vpos = m->vpos;
    vector<vec3f> &vpos_prev = m_prev->vpos;
    vector<vector<ivert>> &finds = m->finds;
    vector<vec3f> &fnorms = m->fnorms;
    int n_f = m->n_f;
    
    if( glob_mesh_faces ) {
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            if( fm[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                set_material( mat_unmatch, mat_unmatch_ );
                draw_face( finds[i_f], fnorms[i_f], vpos );
            } else {
                if( glob_mesh_faces_chgsonly ) continue;
                set_material( mat_match, mat_match_ );
                vec3f color0 = mat_match.diffuse, color1 = { mat_match.diffuse.x * 0.5f, mat_match.diffuse.y * 0.5f, 1.0f };
                vec3f color0_ = mat_match_.diffuse, color1_ = { mat_match_.diffuse.x * 0.5f, mat_match_.diffuse.y * 0.5f, 1.0f };
                draw_face(finds[i_f], fnorms[i_f], vpos, vm, vpos_prev, color0, color0_, color1, color1_ );
            }
        }
        glEnd();
        set_material( mat_neutral, mat_neutral );
    }
    
    glDepthRange(0.0f,0.999999f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LIGHTING);
    
    if( glob_mesh_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( glob_mesh_edges_darkness ) {
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
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
    }
    
    
    glEnable(GL_LIGHTING);
    glDepthRange(0.0f,1.0f);
}


void draw_mesh( Mesh *m, vector<ivert> &vm0, vector<iface> &fm0, vector<ivert> &vm1, vector<iface> &fm1, bool ab ) {
    vector<vec3f> &vpos = m->vpos;
    vector<vector<ivert>> &finds = m->finds;
    vector<vec3f> &fnorms = m->fnorms;
    int n_f = m->n_f;
    
    if( glob_mesh_faces ) {
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            if( fm0[i_f] == -1 && fm1[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                if( ab ) set_material( mat_unmatch0ab, mat_unmatch0ab_ );
                else set_material( mat_unmatch01, mat_unmatch01_ );
            } else if( fm0[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                if( ab ) set_material( mat_unmatch0a, mat_unmatch0a_ );
                else set_material( mat_unmatch1, mat_unmatch1_ );
            } else if( fm1[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                if( ab ) set_material( mat_unmatch0b, mat_unmatch0b_ );
                else set_material( mat_unmatch0, mat_unmatch0_ );
            } else {
                if( glob_mesh_faces_chgsonly ) continue;
                set_material( mat_match, mat_match_ );
            }
            
            draw_face( finds[i_f], fnorms[i_f], vpos );
        }
        glEnd();
        set_material( mat_neutral, mat_neutral );
        set_material(GL_EMISSION, zero3f, zero3f);
    }
    
    glDepthRange(0.0f,0.999999f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LIGHTING);
    
    if( glob_mesh_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( glob_mesh_edges_darkness ) {
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
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
    }
    
    
    glEnable(GL_LIGHTING);
    glDepthRange(0.0f,1.0f);
}

void draw_mesh( Mesh *m, Mesh *m_prev, vector<ivert> &vm0, vector<iface> &fm0, vector<ivert> &vm1, vector<iface> &fm1, bool ab ) {
    vector<vec3f> &vpos = m->vpos;
    vector<vector<ivert>> &finds = m->finds;
    vector<vec3f> &fnorms = m->fnorms;
    vector<vec3f> &vpos_prev = m_prev->vpos;
    int n_f = m->n_f;
    
    if( glob_mesh_faces ) {
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            if( fm0[i_f] == -1 || fm1[i_f] == -1 ) {
                if( glob_mesh_faces_sameonly ) continue;
                if( fm0[i_f] == -1 && fm1[i_f] == -1 ) {
                    if( ab ) set_material( mat_unmatch0ab, mat_unmatch0ab_ );
                    else set_material( mat_unmatch01, mat_unmatch01_ );
                } else if( fm0[i_f] == -1 ) {
                    if( ab ) set_material( mat_unmatch0a, mat_unmatch0a_ );
                    else set_material( mat_unmatch1, mat_unmatch1_ );
                } else if( fm1[i_f] == -1 ) {
                    if( ab ) set_material( mat_unmatch0b, mat_unmatch0b_ );
                    else set_material( mat_unmatch0, mat_unmatch0_ );
                }
                draw_face( finds[i_f], fnorms[i_f], vpos );
            } else {
                if( glob_mesh_faces_chgsonly ) continue;
                
                set_material( mat_match, mat_match_ );
                
                vec3f color0 = mat_match.diffuse, color1 = { mat_match.diffuse.x * 0.5f, mat_match.diffuse.y * 0.5f, 1.0f };
                vec3f color0_ = mat_match_.diffuse, color1_ = { mat_match_.diffuse.x * 0.5f, mat_match_.diffuse.y * 0.5f, 1.0f };
                
                draw_face(finds[i_f], fnorms[i_f], vpos, vm0, vpos_prev, color0, color0_, color1, color1_ );
            }
        }
        glEnd();
        set_material( mat_neutral, mat_neutral );
        set_material(GL_EMISSION, zero3f,zero3f);
    }
    
    glDepthRange(0.0f,0.999999f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LIGHTING);
    
    if( glob_mesh_edges ) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        switch( glob_mesh_edges_darkness ) {
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
                glsVertex(vpos[_vinds[i0]]);
                glsVertex(vpos[_vinds[i1]]);
            }
        }
        glEnd();
    }
    
    
    glEnable(GL_LIGHTING);
    glDepthRange(0.0f,1.0f);
}

