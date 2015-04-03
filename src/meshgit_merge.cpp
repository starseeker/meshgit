#include "meshgit.h"


void merge( Viewer *viewer, vector<Mesh*> &meshes ) {
    Mesh *m0 = meshes[0];
    Mesh *ma = meshes[1];
    Mesh *mb = meshes[2];
    
    SimpleMesh *mm = nullptr;
    int subd_level = 0;
    
    dlog.start("Mesh matching");
    auto mm0a = new MeshMatch(m0,ma);
    auto mm0b = new MeshMatch(m0,mb);
    mm0a->algorithm();
    mm0b->algorithm();
    vector<MeshMatch*> l_mm = { mm0a, mm0b };
    dlog.end();
    
    dlog.start("Partitioning");
    int num_del_faces0a = 0, num_add_faces0a = 0;
    vector<MeshMatchPartition*> l_mmps0a = MeshMatchPartition::partition(mm0a);
    vector<vec3f> l_mmp0a_norms0, l_mmp0a_norms1;
    for( auto mmp : l_mmps0a ) {
        vec3f n0 = zero3f, n1 = zero3f;
        for( int i_f0 : mmp->sif0 ) n0 += m0->fnorms[i_f0];
        n0 = normalize(n0);
        if( n0%n0<0.1f ) n0 = { 1,0,0 };
        l_mmp0a_norms0.push_back(n0);
        num_del_faces0a += mmp->sif0.size();
        
        for( int i_f1 : mmp->sif1 ) n1 += ma->fnorms[i_f1];
        n1 = normalize(n1);
        if( n1%n1<0.1f ) n1 = { 1,0,0 };
        l_mmp0a_norms1.push_back(n1);
        num_add_faces0a += mmp->sif1.size();
    }
    
    int num_del_faces0b = 0, num_add_faces0b = 0;
    vector<MeshMatchPartition*> l_mmps0b = MeshMatchPartition::partition(mm0b);
    vector<vec3f> l_mmp0b_norms0, l_mmp0b_norms1;
    for( auto mmp : l_mmps0b ) {
        vec3f n0 = zero3f, n1 = zero3f;
        for( int i_f0 : mmp->sif0 ) n0 += m0->fnorms[i_f0];
        n0 = normalize(n0);
        if( n0%n0<0.1f ) n0 = { 1,0,0 };
        l_mmp0b_norms0.push_back(n0);
        num_del_faces0b += mmp->sif0.size();
        
        for( int i_f1 : mmp->sif1 ) n1 += mb->fnorms[i_f1];
        n1 = normalize(n1);
        if( n1%n1<0.1f ) n1 = { 1,0,0 };
        l_mmp0b_norms1.push_back(n1);
        num_add_faces0b += mmp->sif1.size();
    }
    
    dlog.print( "Partitions: %d %d", l_mmps0a.size(), l_mmps0b.size() );
    dlog.end();
    
    
    dlog.start_("Detecting face change conflicts");
    vector<vector<bool>> conflicts;
    for( auto mmp0a : l_mmps0a ) {
        vector<bool> row;
        for( auto mmp0b : l_mmps0b ) {
            bool c = false;
            for( int i_f : mmp0a->sif0 ) if( mmp0b->sif0.count(i_f) ) { c = true; break; }
            row.push_back(c);
        }
        conflicts.push_back(row);
    }
    
    bool *is_a = new bool(l_mmps0a.size());
    bool *is_b = new bool(l_mmps0a.size());
    for( int ia = 0; ia < l_mmps0a.size(); ia++ ) {
        bool c = false;
        for( int ib = 0; ib < l_mmps0b.size(); ib++ ) c |= conflicts[ia][ib];
        is_a[ia] = !c;
    }
    for( int ib = 0; ib < l_mmps0b.size(); ib++ ) {
        bool c = false;
        for( int ia = 0; ia < l_mmps0a.size(); ia++ ) c |= conflicts[ia][ib];
        is_b[ib] = !c;
    }
    dlog.end();
    

    
    vector<vec3f> vpos = m0->vpos;
    vector<vector<ivert>> finds = m0->finds;
    vector<vec3f> fnorms = m0->fnorms;
    
    int m0_finds_sz = m0->finds.size();
    vector<material> fmats( m0_finds_sz, mat_match );
    vector<material> fmats_( m0_finds_sz, mat_match_ );
    vector<bool> fcolor( m0_finds_sz, false );
    
    vector<bool> fshow( m0_finds_sz, true );
    
    vector<vector<iface>> fswitcha;
    for( auto mmp : l_mmps0a ) {
        vector<ivert> l_iv_add;
        unordered_map<ivert,ivert> map_iv_iv;
        vector<iface> sw;
        for( int i_v : mmp->siv1 ) {
            map_iv_iv[i_v] = vpos.size();
            vpos.push_back(ma->vpos[i_v]);
        }
        for( int i_v : mmp->siv1_border ) {
            map_iv_iv[i_v] = mm0a->vm10[i_v];
        }
        for( int i_f : mmp->sif1 ) {
            vector<ivert> vinds;
            for( int i_v : ma->finds[i_f] ) vinds.push_back( map_iv_iv[i_v] );
            sw.push_back( finds.size() );
            finds.push_back(vinds);
            fshow.push_back(false);
            fnorms.push_back(ma->fnorms[i_f]);
            fmats.push_back(mat_unmatcha0);
            fmats_.push_back(mat_unmatcha0_);
            fcolor.push_back(true);
        }
        fswitcha.push_back(sw);
    }
    
    vector<vector<iface>> fswitchb;
    for( auto mmp : l_mmps0b ) {
        vector<ivert> l_iv_add;
        unordered_map<ivert,ivert> map_iv_iv;
        vector<iface> sw;
        for( int i_v : mmp->siv1 ) {
            map_iv_iv[i_v] = vpos.size();
            vpos.push_back(mb->vpos[i_v]);
        }
        for( int i_v : mmp->siv1_border ) {
            map_iv_iv[i_v] = mm0b->vm10[i_v];
        }
        for( int i_f : mmp->sif1 ) {
            vector<ivert> vinds;
            for( int i_v : mb->finds[i_f] ) vinds.push_back( map_iv_iv[i_v] );
            sw.push_back( finds.size() );
            finds.push_back(vinds);
            fshow.push_back(false);
            fnorms.push_back(ma->fnorms[i_f]);
            fmats.push_back(mat_unmatchb0);
            fmats_.push_back(mat_unmatchb0_);
            fcolor.push_back(true);
        }
        fswitchb.push_back(sw);
    }
    
    dlog.start_("Detect vert change conflicts");
    unordered_set<int> s_iv_conflict;
    vector<bool> l_vmove_a(ma->n_v,false), l_vmove_b(mb->n_v,false);
    for( int i_v0 = 0; i_v0 < m0->n_v; i_v0++ ) {
        int i_va = mm0a->vm01[i_v0];
        int i_vb = mm0b->vm01[i_v0];
        vec3f &v = m0->vpos[i_v0];
        bool mov0a = ( i_va!=-1 ? (lengthSqr(v-ma->vpos[i_va])>0.001f) : false );
        bool mov0b = ( i_vb!=-1 ? (lengthSqr(v-mb->vpos[i_vb])>0.001f) : false );
        if( mov0a && mov0b ) s_iv_conflict.insert(i_v0);
        if( mov0a ) l_vmove_a[i_va] = true;
        if( mov0b ) l_vmove_b[i_vb] = true;
    }
    vec3f alpha_blend = {0,0,1};
    float alpha_blend0a = 1.0f;
    float alpha_blend0b = 1.0f;
    dlog.end();
    
    
    auto fn_update_mergemesh = [&]{
        // turn on all faces of m0
        for( int i_f = 0; i_f < finds.size(); i_f++ ) fshow[i_f] = (i_f < m0->n_f);
        
        // turn off faces of m0 and on faces of ma
        for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
            if( !is_a[i_a] ) continue;
            auto mmp = l_mmps0a[i_a];
            for( int i_f : mmp->sif0 ) fshow[i_f] = false;
            for( int i_f : fswitcha[i_a] ) fshow[i_f] = true;
        }
        
        // turn off faces of m0 and on faces of mb
        for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
            if( !is_b[i_b] ) continue;
            auto mmp = l_mmps0b[i_b];
            for( int i_f : mmp->sif0 ) fshow[i_f] = false;
            for( int i_f : fswitchb[i_b] ) fshow[i_f] = true;
        }
    };
    
    auto fn_update_pos = [&]{
        vector<ivert> &vm0a = mm0a->vm01;
        vector<ivert> &vm0b = mm0b->vm01;
        for( int i_v0 = 0; i_v0 < m0->n_v; i_v0++ ) {
            int i_va = vm0a[i_v0];
            int i_vb = vm0b[i_v0];
            vec3f &v0 = m0->vpos[i_v0];
            if( s_iv_conflict.count(i_v0) ) {
                vec3f &va = ma->vpos[i_va];
                vec3f &vb = mb->vpos[i_vb];
                vpos[i_v0] = v0 * alpha_blend.x + va * alpha_blend.y + vb * alpha_blend.z;
            } else {
                if( i_va != -1 && l_vmove_a[i_va] ) {
                    vec3f &va = ma->vpos[i_va];
                    vpos[i_v0] = v0 * (1.0f-alpha_blend0a) + va * alpha_blend0a;
                }
                if( i_vb != -1 && l_vmove_b[i_vb] ) {
                    vec3f &vb = mb->vpos[i_vb];
                    vpos[i_v0] = v0 * (1.0f-alpha_blend0b) + vb * alpha_blend0b;
                }
            }
        }
        
        // approximate face normals
        for( int i_f = 0; i_f < finds.size(); i_f++ ) {
            vector<ivert> &vinds = finds[i_f];
            vec3f &v0 = vpos[vinds[0]];
            vec3f &v1 = vpos[vinds[1]];
            vec3f &v2 = vpos[vinds[2]];
            fnorms[i_f] = triangle_normal(v0,v1,v2);
        }
    };
    
    fn_update_mergemesh();
    fn_update_pos();
    
    VPCamera *camera = new VPCamera();
    camera->set_default_lights();
    camera->lookat( makevec3f(30,-50,30), makevec3f(0,0,0), makevec3f(0,0,1));
    camera->d = 0.2f;
    
    
    
    
    auto pnl_3dview0 = new VP_CallBacks();
    pnl_3dview0->name = "mesh0";
    pnl_3dview0->background = pbg_Black;// Maya;
    pnl_3dview0->data = &l_mm;
    pnl_3dview0->camera = camera;
    pnl_3dview0->s_display = {
        to_string("File: %s",m0->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)m0->n_v, (int)m0->n_f ),
        to_string("Changes: %d,%d parts, -%d,-%d faces", (int)l_mmps0a.size(), (int)l_mmps0b.size(), num_del_faces0a, num_del_faces0b),
    };
    pnl_3dview0->fndraw = [](VP_CallBacks *vp) {
        vector<MeshMatch*> *l_mm = (vector<MeshMatch*> *)vp->data;
        MeshMatch *mm0a = (*l_mm)[0];
        MeshMatch *mm0b = (*l_mm)[1];
        draw_mesh( mm0a->m0, mm0a->vm01, mm0a->fm01, mm0b->vm01, mm0b->fm01, true );
    };
    pnl_3dview0->fnmousedown = [&](VP_CallBacks*) {
        if( pnl_3dview0->mouse_but != MOUSE_LBUTTON && pnl_3dview0->mouse_but != MOUSE_RBUTTON ) return;
        bool add = ( pnl_3dview0->mouse_but == MOUSE_LBUTTON );
        
        ray3f r = pnl_3dview0->get_ray();
        int i_f_hit = -1; float d_f_best = 0.0f;
        for( int i_f = 0; i_f < mm0a->m0->n_f; i_f++ ) {
            vector<int> &f = mm0a->m0->finds[i_f];
            int n_i = f.size();
            for( int i0 = 0, i1 = 1, i2 = 2; i2 < n_i; i1++,i2++ ) {
                vec3f &v0 = mm0a->m0->vpos[f[i0]];
                vec3f &v1 = mm0a->m0->vpos[f[i1]];
                vec3f &v2 = mm0a->m0->vpos[f[i2]];
                float t,ba,bb;
                if( !intersect_triangle(r,v0,v1,v2,t,ba,bb) ) continue;
                if( i_f_hit == -1 || t < d_f_best ) { i_f_hit = i_f; d_f_best = t; }
            }
        }
        if( i_f_hit == -1 ) return;
        
        // find partition with i_f_hit
        if( add ) {
            int n_a = l_mmps0a.size();
            for( int i_a = 0; i_a < n_a; i_a++ ) {
                auto mmp0a = l_mmps0a[i_a];
                if( !mmp0a->sif0.count(i_f_hit) ) continue;
                is_a[i_a] = false;
            }
            int n_b = l_mmps0b.size();
            for( int i_b = 0; i_b < n_b; i_b++ ) {
                auto mmp0b = l_mmps0b[i_b];
                if( !mmp0b->sif0.count(i_f_hit) ) continue;
                is_b[i_b] = false;
            }
        } else {
            int i_a_in = -1, i_b_in = -1;
            int n_a = l_mmps0a.size();
            for( int i_a = 0; i_a < n_a; i_a++ ) {
                auto mmp0a = l_mmps0a[i_a];
                if( !mmp0a->sif0.count(i_f_hit) ) continue;
                i_a_in = i_a;
            }
            int n_b = l_mmps0b.size();
            for( int i_b = 0; i_b < n_b; i_b++ ) {
                auto mmp0b = l_mmps0b[i_b];
                if( !mmp0b->sif0.count(i_f_hit) ) continue;
                i_b_in = i_b;
            }
            if( i_a_in != -1 && i_b_in != -1 ) return;
            if( i_a_in != -1 ) {
                int i_a = i_a_in;
                for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
                    if( conflicts[i_a][i_b] ) is_b[i_b] = false;
                }
                is_a[i_a] = true;
            } else {
                int i_b = i_b_in;
                for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
                    if( conflicts[i_a][i_b] ) is_a[i_a] = false;
                }
                is_b[i_b] = true;
            }
        }
        fn_update_mergemesh();
    };
    
    auto pnl_3dviewa = new VP_CallBacks();
    pnl_3dviewa->name = "mesha";
    pnl_3dviewa->background = pbg_Black;
    pnl_3dviewa->data = &l_mm;
    pnl_3dviewa->camera = camera;
    pnl_3dviewa->s_display = {
        to_string("File: %s",ma->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)ma->n_v, (int)ma->n_f ),
        to_string("Changes: %d parts, +%d faces", (int)l_mmps0a.size(), num_add_faces0a),
    };
    pnl_3dviewa->fndraw = [](VP_CallBacks *vp) {
        vector<MeshMatch*> *l_mm = (vector<MeshMatch*> *)vp->data;
        MeshMatch *mm = (*l_mm)[0];
        draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatcha0, mat_unmatcha0_ );
    };
    pnl_3dviewa->fnmousedown = [&](VP_CallBacks*) {
        if( pnl_3dviewa->mouse_but != MOUSE_LBUTTON && pnl_3dviewa->mouse_but != MOUSE_RBUTTON ) return;
        bool add = ( pnl_3dviewa->mouse_but == MOUSE_LBUTTON );
        
        ray3f r = pnl_3dviewa->get_ray();
        int i_f_hit = -1; float d_f_best = 0.0f;
        for( int i_f = 0; i_f < mm0a->m1->n_f; i_f++ ) {
            vector<int> &f = mm0a->m1->finds[i_f];
            int n_i = f.size();
            for( int i0 = 0, i1 = 1, i2 = 2; i2 < n_i; i1++,i2++ ) {
                vec3f &v0 = mm0a->m1->vpos[f[i0]];
                vec3f &v1 = mm0a->m1->vpos[f[i1]];
                vec3f &v2 = mm0a->m1->vpos[f[i2]];
                float t,ba,bb;
                if( !intersect_triangle(r,v0,v1,v2,t,ba,bb) ) continue;
                if( i_f_hit == -1 || t < d_f_best ) { i_f_hit = i_f; d_f_best = t; }
            }
        }
        if( i_f_hit == -1 ) return;
        
        // find partition with i_f_hit
        for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
            auto mmp0a = l_mmps0a[i_a];
            if( !mmp0a->sif1.count(i_f_hit) ) continue;
            
            if( add ) {
                is_a[i_a] = true;
                for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
                    if( conflicts[i_a][i_b] ) is_b[i_b] = false;
                }
            } else {
                is_a[i_a] = false;
            }
        }
        fn_update_mergemesh();
    };
    
    auto pnl_3dviewb = new VP_CallBacks();
    pnl_3dviewb->name = "meshb";
    pnl_3dviewb->background = pbg_Black;
    pnl_3dviewb->data = &l_mm;
    pnl_3dviewb->camera = camera;
    pnl_3dviewb->s_display = {
        to_string("File: %s",mb->filename.c_str()),
        to_string("Counts: %d verts, %d faces", (int)mb->n_v, (int)mb->n_f ),
        to_string("Changes: %d parts, +%d faces", (int)l_mmps0b.size(), num_add_faces0b),
    };
    pnl_3dviewb->fndraw = [](VP_CallBacks *vp) {
        vector<MeshMatch*> *l_mm = (vector<MeshMatch*> *)vp->data;
        MeshMatch *mm = (*l_mm)[1];
        draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatchb0, mat_unmatchb0_ );
    };
    pnl_3dviewb->fnmousedown = [&](VP_CallBacks*) {
        if( pnl_3dviewb->mouse_but != MOUSE_LBUTTON && pnl_3dviewb->mouse_but != MOUSE_RBUTTON ) return;
        bool add = ( pnl_3dviewb->mouse_but == MOUSE_LBUTTON );
        
        ray3f r = pnl_3dviewb->get_ray();
        int i_f_hit = -1; float d_f_best = 0.0f;
        for( int i_f = 0; i_f < mm0b->m1->n_f; i_f++ ) {
            vector<int> &f = mm0b->m1->finds[i_f];
            int n_i = f.size();
            for( int i0 = 0, i1 = 1, i2 = 2; i2 < n_i; i1++,i2++ ) {
                vec3f &v0 = mm0b->m1->vpos[f[i0]];
                vec3f &v1 = mm0b->m1->vpos[f[i1]];
                vec3f &v2 = mm0b->m1->vpos[f[i2]];
                float t,ba,bb;
                if( !intersect_triangle(r,v0,v1,v2,t,ba,bb) ) continue;
                if( i_f_hit == -1 || t < d_f_best ) { i_f_hit = i_f; d_f_best = t; }
            }
        }
        if( i_f_hit == -1 ) return;
        
        // find partition with i_f_hit
        for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
            auto mmp0b = l_mmps0b[i_b];
            if( !mmp0b->sif1.count(i_f_hit) ) continue;
            
            if( add ) {
                is_b[i_b] = true;
                for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
                    if( conflicts[i_a][i_b] ) is_a[i_a] = false;
                }
            } else {
                is_b[i_b] = false;
            }
        }
        fn_update_mergemesh();
    };
    
    auto pnl_merge = new VP_CallBacks();
    pnl_merge->name = "merge";
    pnl_merge->background = pbg_Black;
    pnl_merge->camera = camera;
    pnl_merge->s_display = { "Merged" };
    pnl_merge->lfn_draw = [&]{
        int n_f = finds.size();
        vec3f color0 = mat_match.diffuse, color1 = { mat_match.diffuse.x * 0.5f, mat_match.diffuse.y * 0.5f, 1.0f };
        vec3f color0_ = mat_match_.diffuse, color1_ = { mat_match_.diffuse.x * 0.5f, mat_match_.diffuse.y * 0.5f, 1.0f };
        
        glBegin(GL_TRIANGLES);
        for( int i_f = 0; i_f < n_f; i_f++ ) {
            if( !fshow[i_f] ) continue;
            set_material( fmats[i_f], fmats_[i_f] );
            //draw_face( finds[i_f], vpos );
            
            if( i_f < m0->n_f ) {
                draw_face( finds[i_f], fnorms[i_f], vpos, m0->vpos, color0, color0_, color1, color1_ );
            } else {
                draw_face( finds[i_f], vpos );
            }
        }
        glEnd();
        
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
                if( !fshow[i_f] ) continue;
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
    };
    
    auto pnl_merge2 = new VP_CallBacks();
    pnl_merge2->name = "merge";
    pnl_merge2->background = pbg_Black;
    pnl_merge2->camera = camera;
    pnl_merge2->s_display = { "","" };
    pnl_merge2->lfn_draw = [&]{
        if( mm == nullptr ) return;
        mm->draw( glob_mesh_faces, glob_mesh_edges, mat_match, mat_match_ );
    };
    
    auto f_update_mm = [&]{
        delete mm;
        mm = subd_mesh(m0, vpos, finds, fshow, fmats, fcolor, subd_level);
        mm->save_light("merge.lmesh");
        
        
        /*mm = new SimpleMesh();
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
            mm->fmats_.push_back(fmats_[i_f]);
            mm->fcolor.push_back(fcolor[i_f]);
        }
        mm->clean();
        mm->recompute_normals();
        
        for( int l = 0; l < subd_level; l++ ) {
            auto mm_ = mm->subd_catmullclark();
            delete mm;
            mm = mm_;
        }*/
        
        pnl_merge2->s_display[0] = to_string("Merged + Subdivision (%d)",subd_level);
        pnl_merge2->s_display[1] = to_string("Counts: %d verts, %d faces", (int)mm->vpos.size(), (int)mm->finds.size() );
    };
    auto f_update_mm_mesh0 = [&]{
        //draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatchb0, mat_unmatchb0_ );
        delete mm;
        
        mm = new SimpleMesh();
        mm->vpos = m0->vpos;
        for( int i_v = 0; i_v < m0->vpos.size(); i_v++ ) mm->vinfo.push_back( { 0, true } );
        for( int i_f = 0; i_f < m0->finds.size(); i_f++ ) {
            mm->finds.push_back(m0->finds[i_f]);
            
            if( mm0a->fm01[i_f] == -1 && mm0b->fm01[i_f] == -1 ) {
                mm->fmats.push_back( mat_unmatch0ab );
                mm->fmats_.push_back( mat_unmatch0ab_ );
            } else if( mm0a->fm01[i_f] == -1 ) {
                mm->fmats.push_back( mat_unmatch0a );
                mm->fmats_.push_back( mat_unmatch0a_ );
            } else if( mm0b->fm01[i_f] == -1 ) {
                mm->fmats.push_back( mat_unmatch0b );
                mm->fmats_.push_back( mat_unmatch0b_ );
            } else {
                mm->fmats.push_back( mat_match );
                mm->fmats_.push_back( mat_match_ );
            }
            mm->fcolor.push_back(true);
        }
        mm->clean();
        mm->recompute_normals();
        
        for( int l = 0; l < subd_level; l++ ) {
            auto mm_ = mm->subd_catmullclark();
            delete mm;
            mm = mm_;
        }
        
        pnl_merge2->s_display[0] = to_string("Subdivision (%d)",subd_level);
        pnl_merge2->s_display[1] = to_string("Counts: %d verts, %d faces", (int)mm->vpos.size(), (int)mm->finds.size() );
    };
    auto f_update_mm_mesh = [&](Mesh *m, vector<ivert> &vm, vector<iface> &fm, material &mat_unmatch, material &mat_unmatch_ ){
        //draw_mesh( mm->m1, mm->m0, mm->vm10, mm->fm10, mat_unmatchb0, mat_unmatchb0_ );
        delete mm;
        
        mm = new SimpleMesh();
        mm->vpos = m->vpos;
        for( int i_v = 0; i_v < m->vpos.size(); i_v++ ) {
            if( vm[i_v] == -1 ) {
                mm->vinfo.push_back( { 0, true } );
            } else {
                float dist = lengthSqr( m0->vpos[vm[i_v]] - m->vpos[i_v] );
                float d = sqrt(dist); // 0.1f / (0.1f + dist);
                mm->vinfo.push_back( { d, true } );
            }
        }
        for( int i_f = 0; i_f < m->finds.size(); i_f++ ) {
            mm->finds.push_back(m->finds[i_f]);
            if( fm[i_f] != -1 ) {
                mm->fmats.push_back( mat_match );
                mm->fmats_.push_back( mat_match_ );
                mm->fcolor.push_back(false);
            } else {
                mm->fmats.push_back( mat_unmatch );
                mm->fmats_.push_back( mat_unmatch_ );
                mm->fcolor.push_back(true);
            }
        }
        mm->clean();
        mm->recompute_normals();
        
        for( int l = 0; l < subd_level; l++ ) {
            auto mm_ = mm->subd_catmullclark();
            delete mm;
            mm = mm_;
        }
        
        pnl_merge2->s_display[0] = to_string("Subdivision (%d)",subd_level);
        pnl_merge2->s_display[1] = to_string("Counts: %d verts, %d faces", (int)mm->vpos.size(), (int)mm->finds.size() );
    };
    
    
    vector<ViewPanel*> originals = { pnl_3dviewa, pnl_3dview0, pnl_3dviewb };
    auto pnl_originals = new VP_Divide_N("3dviews", originals, DivideType::LEFTRIGHT);
    
    vector<ViewPanel*> merged = { nullptr, pnl_merge, pnl_merge2 };
    auto pnl_merged = new VP_Divide_N("merged", merged, DivideType::LEFTRIGHT );
    
    vector<ViewPanel*> grid = { pnl_originals, pnl_merged };
    pnl_main->panel = new VP_Divide_N("grid", grid, DivideType::TOPBOTTOM );
    
    pnl_properties->add( new Prop_Text("View Options",TEXT_CENTER));
    int bgchoice = BackgroundType::Black;
    auto prop_background = new Prop_Bind_Enum("Background", BackgroundType::Names, &bgchoice );
    prop_background->fn_change = [&](void*) {
        switch( bgchoice ) {
            case BackgroundType::Gradient: {
                pnl_3dview0->background = pbg_Maya;
                pnl_3dviewa->background = pbg_Maya;
                pnl_3dviewb->background = pbg_Maya;
                pnl_merge->background   = pbg_Maya;
                pnl_merge2->background  = pbg_Maya;
            } break;
            case BackgroundType::Black: {
                pnl_3dview0->background = pbg_Black;
                pnl_3dviewa->background = pbg_Black;
                pnl_3dviewb->background = pbg_Black;
                pnl_merge->background   = pbg_Black;
                pnl_merge2->background  = pbg_Black;
            } break;
            case BackgroundType::White: {
                pnl_3dview0->background = pbg_White;
                pnl_3dviewa->background = pbg_White;
                pnl_3dviewb->background = pbg_White;
                pnl_merge->background   = pbg_White;
                pnl_merge2->background  = pbg_White;
            } break;
        }
    };
    pnl_properties->add(prop_background);
    pnl_properties->add( new Prop_Bind_Float("Lens", &camera->d, 0.001f, 1.0f ));
    bool b_drawtext = false;
    auto prop_drawtext = new Prop_Bind_Bool("Text", &b_drawtext);
    prop_drawtext->lfn_change = [&](void*) {
        pnl_3dview0->b_display = pnl_3dviewa->b_display = pnl_3dviewb->b_display = b_drawtext;
        pnl_merge->b_display = pnl_merge2->b_display = b_drawtext;
    };
    pnl_properties->add( prop_drawtext );
    pnl_properties->add( new Prop_Text("Mesh",TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_Bool(" Edges", &glob_mesh_edges));
    pnl_properties->add( new Prop_Bind_Int("  Darkness", &glob_mesh_edges_darkness, 1, 5));
    pnl_properties->add( new Prop_Bind_Bool(" Faces", &glob_mesh_faces));
    pnl_properties->add( new Prop_Bind_Bool("  Same Only", &glob_mesh_faces_sameonly));
    pnl_properties->add( new Prop_Bind_Bool("  Changes Only", &glob_mesh_faces_chgsonly));
    pnl_properties->add( new Prop_Bind_Bool("  Two-Toned", &glob_mesh_faces_twotoned));
    pnl_properties->add( new Prop_Bind_Float(" Part Offset", &glob_part_offset, 0.0f, 10.0f));
    pnl_properties->add( new Prop_Text("Catmull-Clark Subd", TEXT_LEFT) );
    pnl_properties->add( new Prop_Bind_Int(" Level", &subd_level, 0, 5 ) );
    pnl_properties->add( new Prop_Button(" Mesh 0", nullptr, [&](void*){
        f_update_mm_mesh0();
    }));
    pnl_properties->add( new Prop_Button(" Mesh A", nullptr, [&](void*){
        f_update_mm_mesh( ma, mm0a->vm10, mm0a->fm10, mat_unmatcha0, mat_unmatcha0_ );
    }));
    pnl_properties->add( new Prop_Button(" Mesh B", nullptr, [&](void*){
        f_update_mm_mesh( mb, mm0b->vm10, mm0b->fm10, mat_unmatchb0, mat_unmatchb0_ );
    }));
    pnl_properties->add( new Prop_Button(" Merge", nullptr, [&](void*){
        f_update_mm();
    }));
    pnl_properties->add( new Prop_Divider(' ') );
    
    // JUST LOAD THE INPUT PLY FILES IN MODELING SOFTWARE!!
    //pnl_properties->add( new Prop_Button("Merge Manually", nullptr, [&](void*) {
    //    printf("\n\nNot implemented, yet\n\n");
    //}));
    //pnl_properties->add( new Prop_Divider(' ') );
    
    string fn_merged = "merged.ply";
    pnl_properties->add( new Prop_Text("Filename", TEXT_LEFT));
    pnl_properties->add( new Prop_Bind_String(": ",&fn_merged) );
    pnl_properties->add( new Prop_Button("Save",nullptr,[&](void*){
        auto m = new SimpleMesh();
        m->vpos = vpos;
        for( int i_f = 0; i_f < finds.size(); i_f++ ) {
            if( !fshow[i_f] ) continue;
            m->finds.push_back(finds[i_f]);
        }
        m->clean();
        
        m->xform( -glob_offset, 1.0f / glob_scale );
        m->save_ply(fn_merged.c_str(), to_string("%s %s %s",m0->filename.c_str(),ma->filename.c_str(),mb->filename.c_str()).c_str());
        
        delete m;
    }));
    pnl_properties->add( new Prop_Divider(' ') );
    
    pnl_properties->add( new Prop_Text("Merge Options", TEXT_CENTER) );
    pnl_properties->add( new Prop_Text("Blend Verts", TEXT_LEFT) );
    auto prop_ab0a = new Prop_Bind_Float(" 0->a", &alpha_blend0a, 0.0f, 1.0f );
    prop_ab0a->lfn_change = [&](void*) { fn_update_pos(); };
    pnl_properties->add( prop_ab0a );
    auto prop_ab0b = new Prop_Bind_Float(" 0->b", &alpha_blend0b, 0.0f, 1.0f );
    prop_ab0b->lfn_change = [&](void*) { fn_update_pos(); };
    pnl_properties->add( prop_ab0b );
    pnl_properties->add( new Prop_Text(" Conflicted", TEXT_LEFT) );
    auto prop_abx = new Prop_Bind_Float("  0",&alpha_blend.x,0.0f,1.0f);
    prop_abx->data = &alpha_blend;
    prop_abx->lfn_change = [&](void*data){
        vec3f *ab = (vec3f*)data;
        float s = ab->x + ab->y + ab->z;
        ab->y += (1.0f - s) / 2.0f;
        ab->z += (1.0f - s) / 2.0f;
        if( ab->y < 0.0f ) { ab->z += ab->y; ab->y = 0.0f; }
        if( ab->z < 0.0f ) { ab->y += ab->z; ab->z = 0.0f; }
        fn_update_pos();
    };
    pnl_properties->add( prop_abx );
    auto prop_aby = new Prop_Bind_Float("  a",&alpha_blend.y,0.0f,1.0f);
    prop_aby->data = &alpha_blend;
    prop_aby->lfn_change = [&](void*data){
        vec3f *ab = (vec3f*)data;
        float s = ab->x + ab->y + ab->z;
        ab->x += (1.0f - s) / 2.0f;
        ab->z += (1.0f - s) / 2.0f;
        if( ab->x < 0.0f ) { ab->z += ab->x; ab->x = 0.0f; }
        if( ab->z < 0.0f ) { ab->x += ab->z; ab->z = 0.0f; }
        fn_update_pos();
    };
    pnl_properties->add( prop_aby );
    auto prop_abz = new Prop_Bind_Float("  b",&alpha_blend.z,0.0f,1.0f);
    prop_abz->data = &alpha_blend;
    prop_abz->lfn_change = [&](void*data){
        vec3f *ab = (vec3f*)data;
        float s = ab->x + ab->y + ab->z;
        ab->x += (1.0f - s) / 2.0f;
        ab->y += (1.0f - s) / 2.0f;
        if( ab->x < 0.0f ) { ab->y += ab->x; ab->x = 0.0f; }
        if( ab->y < 0.0f ) { ab->x += ab->y; ab->y = 0.0f; }
        fn_update_pos();
    };
    pnl_properties->add( prop_abz );
    
    pnl_properties->add( new Prop_Text("Part:Conflicts", TEXT_LEFT) );
    for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
        string pname = to_string(" a%02d",i_a);
        bool first = true;
        for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
            if( !conflicts[i_a][i_b] ) continue;
            char sep = ( first ? ':' : ',' ); first = false;
            pname += to_string("%cb%02d",sep,i_b);
        }
        auto p = new Prop_Bind_Bool(pname, &is_a[i_a]);
        p->data = new int(i_a);
        p->lfn_change = [&](void*data){
            int i_a = *(int*)data;
            printf("i_a=%d\n",i_a);
            if(is_a[i_a]) {
                for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
                    if( conflicts[i_a][i_b] ) is_b[i_b] = false;
                }
            }
            fn_update_mergemesh();
        };
        pnl_properties->add( p );
    }
    for( int i_b = 0; i_b < l_mmps0b.size(); i_b++ ) {
        string pname = to_string(" b%02d",i_b);
        bool first = true;
        for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
            if( !conflicts[i_a][i_b] ) continue;
            char sep = ( first ? ':' : ',' ); first = false;
            pname += to_string("%ca%02d",sep,i_a);
        }
        auto p = new Prop_Bind_Bool(pname, &is_b[i_b]);
        p->data = new int(i_b);
        p->lfn_change = [&](void*data){
            int i_b = *(int*)data;
            printf("i_b=%d\n",i_b);
            if(is_b[i_b]) {
                for( int i_a = 0; i_a < l_mmps0a.size(); i_a++ ) {
                    if( conflicts[i_a][i_b] ) is_a[i_a] = false;
                }
            }
            fn_update_mergemesh();
        };
        pnl_properties->add( p );
    }
    
    pnl_properties->add( new Prop_Divider() );
    add_camera_properties( camera );
    
    
    viewer->run();
}

