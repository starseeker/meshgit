#include "meshgit.h"


void execute(string fn_execute);

int main( int argc, char **argv ) {
    vector<Mesh*> meshes; vector<string> fn_meshes; vector<string> l_map_fns;
    string fn_execute;
    MeshGitViews::Enum view = MeshGitViews::None;
    
    mat_unmatch0.diffuse   = { 0.90f, 0.20f, 0.20f }; mat_unmatch0.ambient   = { 1.00f, 0.50f, 0.50f };
    mat_unmatch1.diffuse   = { 0.20f, 0.90f, 0.20f }; mat_unmatch1.ambient   = { 0.50f, 1.00f, 0.50f };
    mat_unmatch01.diffuse  = { 0.90f, 0.40f, 0.10f }; mat_unmatch01.ambient  = { 1.00f, 0.80f, 0.50f };
    
    mat_unmatch0a.diffuse  = { 1.00f, 0.20f, 0.20f }; mat_unmatch0a.ambient  = { 1.00f, 0.50f, 0.50f };
    mat_unmatcha0.diffuse  = { 0.10f, 1.00f, 0.10f }; mat_unmatcha0.ambient  = { 0.50f, 1.00f, 0.50f };
    mat_unmatch0b.diffuse  = { 0.40f, 0.10f, 0.10f }; mat_unmatch0b.ambient  = { 0.40f, 0.20f, 0.20f };
    mat_unmatchb0.diffuse  = { 0.10f, 0.40f, 0.10f }; mat_unmatchb0.ambient  = { 0.20f, 0.40f, 0.20f };
    mat_unmatch0ab.diffuse = { 0.90f, 0.90f, 0.10f }; mat_unmatch0ab.ambient = { 0.70f, 0.70f, 0.30f };
    
    material_setdarker(mat_match,mat_match_);
    material_setdarker(mat_unmatch0, mat_unmatch0_); material_setdarker(mat_unmatch1, mat_unmatch1_);
    material_setdarker(mat_unmatch01, mat_unmatch01_);
    material_setdarker(mat_unmatch0a,mat_unmatch0a_); material_setdarker(mat_unmatcha0,mat_unmatcha0_);
    material_setdarker(mat_unmatch0b,mat_unmatch0b_); material_setdarker(mat_unmatchb0,mat_unmatchb0_);
    material_setdarker(mat_unmatch0ab,mat_unmatch0ab_);
    
    int w=1920, h=1080;
    bool reterror = false;
    bool printhelp = false;
    string window_title;
    
    map<string,MeshGitViews::Enum> map_cmd_view = {
        { "view",           MeshGitViews::View },
        { "diff",           MeshGitViews::Diff },
        { "diff-load",      MeshGitViews::Diff_Load },
        { "diff-save",      MeshGitViews::Diff_Save },
        { "diff-sequence",  MeshGitViews::Diff_Sequence },
        { "diff-explore",   MeshGitViews::Diff_Explore },
        { "diff-compare",   MeshGitViews::Diff_Compare },
        { "diff-timed",     MeshGitViews::Diff_Timed },
        { "merge",          MeshGitViews::Merge },
        { "info",           MeshGitViews::Info },
        { "execute",        MeshGitViews::Execute },
    };
    map<string,MeshGitCommands::Enum> map_cmd_cmd = {
        { "none",           MeshGitCommands::None },
        { "meshgit",        MeshGitCommands::MeshGit },
        { "noadjacency",    MeshGitCommands::NoAdjacency },
        { "exact",          MeshGitCommands::Exact },
        { "greedy1000",     MeshGitCommands::Greedy1000 },
        { "greedy2000",     MeshGitCommands::Greedy2000 },
        { "greedy3000",     MeshGitCommands::Greedy3000 },
        { "greedy",         MeshGitCommands::Greedy },
        { "greedybacktrack",MeshGitCommands::GreedyBacktrack },
    };
    
    MeshGitCommands::Enum command;
    
    
    if( argc >= 2 ) {
        if( map_cmd_view.count(argv[1]) ) {
            view = map_cmd_view[argv[1]];
        } else {
            printf( "Error! Unknown command '%s'\n", argv[1] );
            reterror = true;
        }
    } else {
        printf( "Error! Expected command\n" );
        reterror = true;
    }
    
    switch( view ) {
        case MeshGitViews::None: {
            reterror = true;
        } break;
        case MeshGitViews::View: {
            for( int i = 2; i < argc; i++ ) {
                fn_meshes.push_back(argv[i]);
                if( window_title.length() ) window_title += ", ";
                window_title += argv[i];
            }
            window_title = "MeshGit.View: " + window_title;
        } break;
        case MeshGitViews::Diff: {
            if( argc == 4 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                window_title = to_string("MeshGit.Diff2: %s -> %s",argv[2],argv[3]);
            } else if( argc == 5 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                fn_meshes.push_back(argv[4]);
                window_title = to_string("MeshGit.Diff3: %s <- %s -> %s",argv[3],argv[2],argv[4]);
            } else {
                printf( "Error! Expected either 2 or 3 mesh files\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Diff_Load: {
            if( argc == 5 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                l_map_fns.push_back(argv[4]);
                window_title = to_string("MeshGit.Diff2: %s -> %s",argv[2],argv[3]);
            } else {
                printf( "Error! Expected either 2 mesh files and filename\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Diff_Save: {
            if( argc == 6 and map_cmd_cmd.count(argv[5]) ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                l_map_fns.push_back(argv[4]);
                command = map_cmd_cmd[argv[5]];
            } else {
                printf( "Error! Expected either 2 mesh files, filename, and valid command\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Diff_Explore: {
            if( argc == 4 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                window_title = to_string("MeshGit.Diff_Explore: %s -> %s",argv[2],argv[3]);
            } else {
                printf( "Error! Expected 2 mesh files\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Diff_Sequence: {
            if( argc >= 4 ) {
                window_title = "";
                for( int i = 2; i < argc; i++ ) {
                    fn_meshes.push_back(argv[i]);
                    if( window_title.length() ) {
                        window_title = window_title + ", " + argv[i];
                    } else {
                        window_title = argv[i];
                    }
                }
                window_title = "MeshGit.Diff_Sequence: " + window_title;
            } else {
                printf( "Error! Expected at least 2 mesh files\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Diff_Compare: {
            if( argc >= 5 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                window_title = to_string("MeshGit.Diff_Compare: %s -> %s",argv[2],argv[3]);
                for( int i = 4; i < argc; i++ ) {
                    l_map_fns.push_back( argv[i] );
                }
            } else {
                printf( "Error! Expected 2 mesh files and at least 1 map filename\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Diff_Timed: {
            if( argc == 4 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                window_title = "";
            } else {
                printf( "Error! Expected 2 mesh files\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Merge: {
            if( argc == 5 ) {
                fn_meshes.push_back(argv[2]);
                fn_meshes.push_back(argv[3]);
                fn_meshes.push_back(argv[4]);
                window_title = to_string("MeshGit.Merge: %s <- %s -> %s",argv[3],argv[2],argv[4]);
            } else {
                printf( "Error! Expected 3 mesh files\n" );
                reterror = true;
            }
        } break;
        case MeshGitViews::Info: {
            window_title = "";
            for( int i = 2; i < argc; i++ ) {
                fn_meshes.push_back(argv[i]);
                if( window_title.length() ) {
                    window_title = window_title + ", " + argv[i];
                } else {
                    window_title = argv[i];
                }
            }
        } break;
        case MeshGitViews::Execute: {
            if( argc == 3 ) {
                fn_execute = argv[2];
                window_title = to_string("MeshGit.Execute: %s", fn_execute.c_str());
            } else {
                printf("Error! Expected execution file\n");
                reterror = true;
            }
        } break;
        default: { assert(false); } break;
    }
    
    if( printhelp || reterror ) {
        printf("Usage: %s <command> <mesh0.ply> <mesh1.ply> <...>\n", argv[0]);
        printf("    Loads given meshes (as .ply files) and performs given command\n");
        printf("Commands\n"
               "  diff <0.ply> <1.ply>\n"
               "    Performs 2-way diff\n"
               "  diff-load <0.ply> <1.ply> <comp.map>\n"
               "    Loads 2-way diff\n"
               "  diff-save <0.ply> <1.ply> <comp.map> <command>\n"
               "    Performs 2-way diff using command and saves to file\n"
               "      commands: meshgit, exact, noadjacency, greedy, greedybacktrack\n"
               "  diff <0.ply> <a.ply> <b.ply>\n"
               "    Performs 3-way diff\n"
               "  diff-sequence <0.ply> <1.ply> <...>\n"
               "    Performs a sequence diff.  Same as 2-way diff if given only two mesh files\n"
               "  diff-compare <0.ply> <1.ply> <comp0.map> <...>\n"
               "    Compares different matching algorithms\n"
               "  diff-timed <0.ply> <1.ply>\n"
               "    Reports timing for computing a 2-way diff\n"
               "  merge <0.ply> <a.ply> <b.ply>\n"
               "    Merges changes 0->a and 0->b onto 0\n"
               "  info <0.ply> <...>\n"
               "    Dumps information on mesh(es) to terminal\n"
               );
        exit( (reterror ? 1 : 0) );
    }
    
    if( view == MeshGitViews::Execute ) {
        glutInit( &argc, argv );
        execute(fn_execute);
        return 0;
    }
    
    
    dlog.start("Loading and initializing meshes");
    for( string &fn : fn_meshes ) meshes.push_back( new Mesh(fn) );
    MeshMatch::init_meshes(meshes, glob_offset, glob_scale);
    dlog.end();
    
    if( view == MeshGitViews::Info ) {
        dlog.start("Mesh Information");
        int n_meshes = meshes.size();
        for( int i_mesh = 0; i_mesh < n_meshes; i_mesh++ ) {
            auto mesh = meshes[i_mesh];
            dlog.start( "Mesh: %s", mesh->filename.c_str() );
            
            dlog.start(" Counts" );
            map<int,const char*> ngon_names = { {1,"verts"}, {2,"edges"}, {3," tris"}, {4,"quads"}, {5,"5gons"}, {6,"6gons"} };
            
            map<int,int> ngon;
            //ngon[1] = mesh->n_v;
            for( auto &inds : mesh->finds ) ngon[inds.size()] += 1;
            
            dlog.print( "verts: % 7d", mesh->n_v );
            dlog.start( "ngons: % 7d", mesh->n_f );
            for( auto c : ngon ) dlog.print( "%s: % 7d", ngon_names[c.first], c.second );
            dlog.end_quiet();
            dlog.print( "comps: % 7d", mesh->components.size() );
            
            dlog.end_quiet();
            dlog.end_quiet();
        }
        dlog.end_quiet();
        return 0;
    }
    
    if( view == MeshGitViews::Diff_Timed ) {
        dlog.start("Diff Timing");
        auto mm = new MeshMatch(meshes[0],meshes[1]);
        mm->algorithm();
        dlog.end();
        return 0;
    }
    
    if(view == MeshGitViews::Diff_Save) {
        dlog.start("Diff Save");
        auto mm = new MeshMatch(meshes[0],meshes[1]);
        switch(command) {
            case MeshGitCommands::None: {
                mm->reset();
            } break;
            case MeshGitCommands::MeshGit: {
                mm->algorithm();
            } break;
            case MeshGitCommands::NoAdjacency: {
                mm->costs.stretch = 0.0f;
                mm->costs.unm = 0.0f;
                mm->costs.mis = 0.0f;
                mm->assign_no_geo_cost();
                mm->init_greedy();
                mm->assign_greedy_step();
                mm->cleanup();
            } break;
            case MeshGitCommands::Exact: {
                mm->assign_exact(1, 0.1f);
                mm->cleanup();
            } break;
            case MeshGitCommands::Greedy1000: {
                mm->reset();
                mm->assign_no_geo_cost();
                mm->init_greedy();
                mm->assign_greedy_step(1000);
                mm->cleanup();
            } break;
            case MeshGitCommands::Greedy2000: {
                mm->reset();
                mm->assign_no_geo_cost();
                mm->init_greedy();
                mm->assign_greedy_step(2000);
                mm->cleanup();
            } break;
            case MeshGitCommands::Greedy3000: {
                mm->reset();
                mm->assign_no_geo_cost();
                mm->init_greedy();
                mm->assign_greedy_step(3000);
                mm->cleanup();
            } break;
            case MeshGitCommands::Greedy: {
                mm->reset();
                mm->assign_no_geo_cost();
                mm->init_greedy();
                mm->assign_greedy_step();
                mm->cleanup();
            } break;
            case MeshGitCommands::GreedyBacktrack: {
                mm->reset();
                mm->assign_no_geo_cost();
                mm->init_greedy();
                mm->assign_greedy_step();
                mm->cleanup();
                mm->unassign_small_patches(0.08f);
            } break;
            default: {
                error("not implemented");
            } break;
        };
        mm->save(l_map_fns[0].c_str());
        dlog.end();
        return 0;
    }
    
    
    
    glutInit( &argc, argv );
    
    h = ( glutGet(GLUT_SCREEN_WIDTH)>2200 ? 1080 : 720 );
    if( h == 1080 ) { w = 1920; }
    else if( h == 720 ) { w = 960; }
    else w = h;
    
    
    
    
    // setup window
    
    // TODO: UPDATE!!!!
    auto pnl_help = new VP_ScrollTextView();
    pnl_help->s_display = {
        "Help Menu",
        "",
        "General",
        "  esc     - exit",
        "  f1      - toggle help menu",
        "  f2      - save screenshot of window",
        "   +shift - screenshot with alpha=0 for background",
        "  f3      - save screenshot of panel under cursor",
        "   +shift - screenshot with alpha=0 for background",
        "  f11     - toggle properties",
        "  f12     - toggle layout: maximize panel under mouse / default",
        "",
        "Properties Panel",
        "  checkbox",
        "    lmb   - toggle value: on/off",
        "  enumeration",
        "    lmb   - change to next",
        "    rmb   - change to prev",
        "  float / integer",
        "    lmb   - change value",
        "    home  - set to minimum value",
        "    end   - set to maximum value",
        "  button",
        "    lmb   - execute"
        "",
        "3D View",
        "",
        "General Camera Commands (if applicable)",
        "  mmb     - rotate (turntable)",
        "   +shift - pan",
        "   +ctrl  - dolly / zoom",
        "  .       - center camera to origin",
        "   +shift - fly to origin",
        "  1/3/7   - front / right / top",
        "   +shift - back / left / bottom",
        "  5       - toggle projection: perspective / orthographic",
    };
    pnl_help->background = pbg_DarkGray;
    
    pnl_properties = new VP_PropertyView();
    pnl_properties->name = "properties";
    pnl_properties->background = pbg_DarkGray;
    
    pnl_main = new VP_Container();
    
    Viewer *viewer = new Viewer( window_title, pnl_main, pnl_properties, pnl_help, w, h );
    viewer->fn_idle = [](void *data) { };
    
    switch( view ) {
        case MeshGitViews::None: {
            reterror = true;
        } break;
        case MeshGitViews::View: {
            simple_viewer( viewer, meshes );
        } break;
        case MeshGitViews::Diff_Explore: {
            diff_2way_explore( viewer, meshes );
        } break;
        case MeshGitViews::Diff: {
            if( meshes.size() == 2 ) diff_2way( viewer, meshes );
            else diff_3way( viewer, meshes );
        } break;
        case MeshGitViews::Diff_Load: {
            diff_2way(viewer, meshes, l_map_fns[0]);
        } break;
        case MeshGitViews::Diff_Sequence: {
            diff_seq( viewer, meshes );
        } break;
        case MeshGitViews::Diff_Compare: {
            diff_compare( viewer, meshes, l_map_fns );
        } break;
        case MeshGitViews::Merge: {
            merge( viewer, meshes );
        } break;
        default: { assert(false); } break;
    }
    
    return 0;
}


// NOT ROBUST!!!
void execute(string fn_execute) {
    vector<Mesh*> meshes;
    int w,h;
    
    // set default window height and width
    h = ( glutGet(GLUT_SCREEN_WIDTH)>2200 ? 1080 : 720 );
    if( h == 1080 ) { w = 1920; }
    else if( h == 720 ) { w = 960; }
    else w = h;
    
    auto pnl_help = new VP_ScrollTextView();
    pnl_help->s_display = {
        "Help Menu",
        "",
        "General",
        "  esc     - exit",
        "  f1      - toggle help menu",
        "  f2      - save screenshot of window",
        "   +shift - screenshot with alpha=0 for background",
        "  f3      - save screenshot of panel under cursor",
        "   +shift - screenshot with alpha=0 for background",
        "  f11     - toggle properties",
        "  f12     - toggle layout: maximize panel under mouse / default",
        "",
        "Properties Panel",
        "  checkbox",
        "    lmb   - toggle value: on/off",
        "  enumeration",
        "    lmb   - change to next",
        "    rmb   - change to prev",
        "  float / integer",
        "    lmb   - change value",
        "    home  - set to minimum value",
        "    end   - set to maximum value",
        "  button",
        "    lmb   - execute"
        "",
        "3D View",
        "",
        "General Camera Commands (if applicable)",
        "  mmb     - rotate (turntable)",
        "   +shift - pan",
        "   +ctrl  - dolly / zoom",
        "  .       - center camera to origin",
        "   +shift - fly to origin",
        "  1/3/7   - front / right / top",
        "   +shift - back / left / bottom",
        "  5       - toggle projection: perspective / orthographic",
    };
    pnl_help->background = pbg_DarkGray;
    
    pnl_properties = new VP_PropertyView();
    pnl_properties->name = "properties";
    pnl_properties->background = pbg_DarkGray;
    
    pnl_main = new VP_Container();
    
    char buffer[2000];
    FILE *fp = fopen(fn_execute.c_str(),"rt");
    
    // reads a single line from file (ASSUMES < 2000 chars)
    // will strip space at beginning
    auto read_line=[&]{
        if(feof(fp)) { buffer[0] = 0; return; }
        for(int i=0;i<1999;i++) {
            buffer[i] = fgetc(fp);
            if(buffer[i]=='\n' || buffer[i]=='\0') { buffer[i] = '\0'; break; }
        }
        buffer[1999] = 0;
        int is; for(is=0;buffer[is]==32;is++)
        for(int i=is;buffer[i-1]!=0;i++) buffer[i-is] = buffer[is];
    };
    
    auto line_starts=[&](const char *beg)->bool {
        for(int i=0;beg[i]!='\0';i++) if(buffer[i]!=beg[i]) return false;
        return true;
    };
    auto line_equals=[&](const char *str)->bool { return strcmp(buffer,str)==0; };
    auto line_after=[&](const char *beg)->const char* { return &buffer[strlen(beg)]; };
    
    while(true) {
        read_line();
        if(line_equals("") || line_starts("#")) continue;
        
        if(line_equals("quit")) { break; }
        
        if(line_starts("window ")) {
            auto opts = line_after("window ");
            sscanf(opts, "%d %d", &w, &h);
            continue;
        }
        
        if(line_starts("mesh ")) {
            auto fn = line_after("mesh ");
            meshes.push_back(new Mesh(fn));
            continue;
        }
        if(line_equals("init")) {
            MeshMatch::init_meshes(meshes, glob_offset, glob_scale);
            continue;
        }
        if(line_equals("diff")) {
            Viewer *viewer = new Viewer( "Diff", pnl_main, pnl_properties, pnl_help, w, h );
            viewer->fn_idle = [](void *data) { };
            if( meshes.size() == 2 ) diff_2way( viewer, meshes );
            else diff_3way( viewer, meshes );
            //Viewer *viewer = new Viewer( "diff", pnl_main, pnl_properties, pnl_help, w, h );
            //viewer->fn_idle = [](void *data) { };
            continue;
        }
        
        error_va("unknown command: \"%s\"", buffer);
    }
    
    fclose(fp);
}


