#include <igl/readOFF.h>
#include <igl/opengl/glfw/Viewer.h>
#include <GLFW/glfw3.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>
#include <igl/opengl/glfw/imgui/ImGuizmoPlugin.h>


#include <iostream>
#include <string>
#include <vector>

#include <cmath>
#include <igl/unproject_onto_mesh.h>
#include <igl/vertex_triangle_adjacency.h>
#include "igl/adjacency_list.h"
#include <igl/avg_edge_length.h>
#include <igl/per_face_normals.h>

#include <algorithm>
#include <numeric>
#include <igl/barycenter.h>
#include <igl/doublearea.h>
#include <igl/decimate.h>
#include <igl/upsample.h>
#include <igl/loop.h>
#include <igl/gaussian_curvature.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/principal_curvature.h>
#include <igl/dijkstra.h>

#include <set>
#include <unordered_set>
#include <map>




//Closest-to-chosen-face Vertex
int nearest_vid(int x, int y, Eigen::MatrixXd V, int adj_vids[])
    {
    double dx, dy;
    std::vector<double> dists;

    //Calculate each face's vertex distance from cursor
    for (int i = 0; i < 3; i++) 
    {
        dx = x - V(adj_vids[i], 0);
        dy = y - V(adj_vids[i], 1);
        dists.push_back(dx * dx + dy * dy);
    }

    //Minimum vertex distance index 
    int min_element_index = std::distance(std::begin(dists), std::min_element(std::begin(dists), std::end(dists)));
    return adj_vids[min_element_index];
    }


//1-ring faces around vid
std::vector<int> one_ring_faces(int vid, Eigen::MatrixXd V, Eigen::MatrixXi F) 
{
    std::vector<std::vector<int>> VI;
    std::vector<std::vector<int>> VF;

    igl::vertex_triangle_adjacency(V.rows(), F, VF, VI);
    std::vector<int> _1_ring_faces = VF[vid];

    return _1_ring_faces;
}


//2-ring faces around vid
std::vector<int> two_ring_faces(int vid, Eigen::MatrixXd V, Eigen::MatrixXi F)
{
    
    
    std::vector<int> adjacent_vertices;// = get_adjacent_vertices(vid, F);
    std::unordered_set<int> adjacent_faces;
    std::vector<int> one_ring_vid = one_ring_faces(vid, V, F);
    std::vector<int> one_ring_adjacent;

    std::vector<std::vector<int>> A;
    igl::adjacency_list(F, A);
    adjacent_vertices = A[vid];

    for (int adjacent_vertex : adjacent_vertices)
    {
        one_ring_adjacent = one_ring_faces(adjacent_vertex, V ,F);

        for (int face : one_ring_adjacent) 
        {
            if (std::find(one_ring_vid.begin(), one_ring_vid.end(), face) != one_ring_vid.end()) 
                continue;
           
            adjacent_faces.insert(face);
        }
    }
    std::vector<int> two_ring;
    std::unordered_set<int>::iterator it;
    for (it = adjacent_faces.begin(); it != adjacent_faces.end(); it++) {
        two_ring.push_back(*it);
    }
    return two_ring;
}


//FID - Colored areas to whole surface ratio
double area_ratio(std::vector<int> fids, Eigen::MatrixXd V, Eigen::MatrixXi F) {
    
    Eigen::VectorXd total_area;
    igl::doublearea(V, F, total_area);
    total_area = total_area.array() / 2;

    std::vector<double> area;
    for (int i = 0; i < F.rows(); i++) 
    {
        if (std::find(fids.begin(), fids.end(), i) != fids.end())
        {
            area.push_back(total_area(i));
        }
    }

    double area_sum = std::accumulate(area.begin(), area.end(), 0.0);
    return (area_sum / total_area.sum());
}


//Add Normal Vectors
void F_normals(igl::opengl::glfw::Viewer& viewer, std::vector<int> fids, Eigen::RowVector3d color, Eigen::MatrixXd V, Eigen::MatrixXi F)
{
    Eigen::MatrixXd Bc, N_faces;
    igl::barycenter(V, F, Bc);
    igl::per_face_normals(V, F, N_faces);

    const double average = igl::avg_edge_length(V, F);

    for (int i = 0; i < F.rows(); i++) 
    {
        if (std::find(fids.begin(), fids.end(), i) != fids.end())
            viewer.data_list[viewer.selected_core_index].add_edges(Bc.row(i), Bc.row(i) + average * N_faces.row(i), color);
    }
}


// color vertices in given viewport based on gaussian curvature
void color_vertices(int view_idx,Eigen::RowVector3d elliptic_color, Eigen::RowVector3d hyperbolic_color, Eigen::RowVector3d parabolic_color, 
                    igl::opengl::glfw::Viewer& viewer, Eigen::MatrixXd V, Eigen::MatrixXi F)
{
    Eigen::VectorXd K;

    igl::gaussian_curvature(V, F, K);
    Eigen::MatrixXd colors(K.rows(), 3);

    for (int i = 0; i < K.size(); i++) {
        // Elliptic point
        if (K[i] > 0)
            colors.row(i) = elliptic_color;
        // hyperbolic
        else if (K[i] < 0)
            colors.row(i) = hyperbolic_color;
        // parabolic
        else
            colors.row(i) = parabolic_color;
    }
    viewer.data_list[view_idx].set_colors(colors);
}


void GaussCurv_color(int view_idx, igl::opengl::glfw::Viewer& viewer, Eigen::MatrixXd V, Eigen::MatrixXi F) 
{
    Eigen::VectorXd K;
    igl::gaussian_curvature(V, F, K);
    Eigen::SparseMatrix<double> M, Minv;
    igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_VORONOI, M);
    igl::invert_diag(M, Minv);

    K = (Minv * K).eval();

    viewer.data_list[view_idx].set_mesh(V, F);
    viewer.data_list[view_idx].set_data(K);
}


void GaussDir_color(int view_idx, igl::opengl::glfw::Viewer& viewer, Eigen::MatrixXd V, Eigen::MatrixXi F) 
{
    Eigen::MatrixXd PD1, PD2;
    Eigen::VectorXd PV1, PV2;

    igl::principal_curvature(V, F, PD1, PD2, PV1, PV2);

    Eigen::VectorXd H = 0.5 * (PV1 + PV2);

    viewer.data_list[view_idx].set_mesh(V, F);
    viewer.data_list[view_idx].set_data(H);

    const double avg = igl::avg_edge_length(V, F);

    const Eigen::RowVector3d red(0.8, 0.2, 0.2), blue(0.2, 0.2, 0.8);
    viewer.data_list[view_idx].add_edges(V + PD1 * avg, V - PD1 * avg, blue);
    viewer.data_list[view_idx].add_edges(V + PD2 * avg, V - PD2 * avg, red);
}


 int main(int argc, char* argv[])
 {

#pragma region Multiple Viewports

        igl::opengl::glfw::Viewer viewer;

        Eigen::MatrixXd V,V1,V2,V3,V4;
        Eigen::MatrixXd U,U1,U2,U3,U4;       
        Eigen::MatrixXi F,F1,F2,F3,F4;
        Eigen::MatrixXi G,G1,G2,G3,G4;

        int viewport_index = 0;
        bool window_click = false;

        //read data off of our meshes
        igl::readOBJ(std::string("res/Models/tutorial_data") + "/cube.obj", V1, F1);                   
        igl::readOBJ(std::string("res/Models/tutorial_data") + "/sphere.obj", V2, F2);
        igl::readOFF(std::string("res/Models/tutorial_data") + "/bunny.off", V3, F3);
        igl::readOBJ(std::string("res/Models/tutorial_data") + "/arm.obj", V4, F4);

        //load our meshes                                                                               
        viewer.load_mesh_from_file(std::string("res/Models/tutorial_data") + "/cube.obj");
        viewer.data().set_face_based(true);
        viewer.core().align_camera_center(V1, F1);

        viewer.load_mesh_from_file(std::string("res/Models/tutorial_data") + "/sphere.obj");
        viewer.data().set_face_based(true);
        viewer.core().align_camera_center(V2, F2);

        viewer.load_mesh_from_file(std::string("res/Models/tutorial_data") + "/bunny.off");
        viewer.data().set_face_based(true);
        viewer.core().align_camera_center(V3, F3);

        viewer.load_mesh_from_file(std::string("res/Models/tutorial_data") + "/arm.obj");
        viewer.data().set_face_based(true);
        viewer.core().align_camera_center(V4, F4);


        unsigned int upper_left_view, upper_right_view, lower_left_view, lower_right_view;
        

        int model1_id = viewer.data_list[0].id;
        int model2_id = viewer.data_list[1].id;
        int model3_id = viewer.data_list[2].id;
        int model4_id = viewer.data_list[3].id;

        viewer.callback_init = [&](igl::opengl::glfw::Viewer&)                // 1280 X 800 by default
        {
            viewer.core().viewport = Eigen::Vector4f(0, 0, 640, 400);

            lower_left_view = viewer.core_list[0].id;
            upper_left_view = viewer.append_core(Eigen::Vector4f(0, 400, 640, 400), true);
            upper_right_view = viewer.append_core(Eigen::Vector4f(640, 400, 640, 400), true);
            lower_right_view = viewer.append_core(Eigen::Vector4f(640, 0, 640, 400), true);

            //Choosing which model will be viewed on each viewport
            viewer.data(model2_id).set_visible(false, lower_left_view);        
            viewer.data(model3_id).set_visible(false, lower_left_view);
            viewer.data(model4_id).set_visible(false, lower_left_view);

            viewer.data(model1_id).set_visible(false, upper_left_view);
            viewer.data(model3_id).set_visible(false, upper_left_view);
            viewer.data(model4_id).set_visible(false, upper_left_view);

            viewer.data(model1_id).set_visible(false, upper_right_view);
            viewer.data(model2_id).set_visible(false, upper_right_view);
            viewer.data(model4_id).set_visible(false, upper_right_view);

            viewer.data(model1_id).set_visible(false, lower_right_view);
            viewer.data(model2_id).set_visible(false, lower_right_view);
            viewer.data(model3_id).set_visible(false, lower_right_view);

            return false;
        };


        viewer.callback_post_resize = [&](igl::opengl::glfw::Viewer& v, int w, int h) {                         

            v.core(upper_left_view).viewport = Eigen::Vector4f(0, h / 2, w / 2, h / 2);
            v.core(upper_right_view).viewport = Eigen::Vector4f(w / 2, h / 2, w - (w / 2), h / 2);
            v.core(lower_left_view).viewport = Eigen::Vector4f(0, 0, w / 2, h / 2);
            v.core(lower_right_view).viewport = Eigen::Vector4f(w / 2, 0, w - (w / 2), h / 2);

            return true;
        };


#pragma endregion 

        
#pragma region Face Selection 


        viewer.callback_mouse_down =
            [&](igl::opengl::glfw::Viewer& viewer, int, int)->int
        {
            //Viewport - Mesh selection 
            viewer.select_hovered_core();
            viewport_index = viewer.selected_core_index;
            window_click = true; 

            switch (viewport_index)
            {
            case 0:
                V = V1;
                F = F1;
                std::cout << "Viewport : " << viewport_index << "\n";
                break;

            case 1:
                V = V2;
                F = F2;
                std::cout << "Viewport : " << viewport_index << "\n";
                break;

            case 2:
                V = V3;
                F = F3;
                std::cout << "Viewport : " << viewport_index << "\n";
                break;

            case 3:
                V = V4;
                F = F4;
                std::cout << "Viewport : " << viewport_index << "\n";
                break;
            }

            int fid;
            int vid;

            Eigen::Vector3f bc;

            // Cast a ray in the view direction starting from the mouse position
            double x = viewer.current_mouse_x;
            double y = 2 * viewer.core().viewport(3) - viewer.current_mouse_y;

            //Face ID - Face's Vertexes - Closest Vertex 
            if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core_list[viewport_index].view,
                viewer.core_list[viewport_index].proj, viewer.core_list[viewport_index].viewport, V, F, fid, bc))
            {

                std::cout << "Face ID : " << fid << "\n";

                int adj_vids[3] = { F(fid, 0), F(fid, 1), F(fid, 2) };
                vid = nearest_vid(x, y, V, adj_vids);
                std::cout << "Closest VID : " << vid << "\n";

                std::vector<int> _one_ring_faces = one_ring_faces(vid, V, F);
                std::vector<int> _two_ring_faces = two_ring_faces(vid, V, F);

                std::vector<int> CastFid = { fid };

                double fid_area = area_ratio(CastFid, V, F);
                double one_ring_area = area_ratio(_one_ring_faces, V, F);
                double two_ring_area = area_ratio(_two_ring_faces, V, F);
                std::cout << "FID Area : " << fid_area << "\n";
                std::cout << "1-RING Area : " << one_ring_area << "\n";
                std::cout << "2-RING Area : " << two_ring_area << "\n\n";

                //color the 1-ring, 2-ring faces
                Eigen::MatrixXd colors(F.rows(), 3);
                Eigen::RowVector3d red(1, 0, 0), green(0, 1, 0), yellow(1, 1, 0);

                for (int i = 0; i < F.rows(); i++)
                {

                    if (std::find(_one_ring_faces.begin(), _one_ring_faces.end(), i) != _one_ring_faces.end())
                    {
                        colors.row(i) = red;
                    }

                    else if (std::find(_two_ring_faces.begin(), _two_ring_faces.end(), i) != _two_ring_faces.end())
                    {
                        colors.row(i) = green;
                    }

                    else
                    {
                        colors.row(i) = yellow;
                    }
                }
                viewer.data_list[viewer.selected_core_index].set_colors(colors);

                //Render the normal vectors 
                viewer.data_list[viewer.selected_core_index].clear_edges();
                F_normals(viewer, _one_ring_faces, yellow, V, F);
                F_normals(viewer, _two_ring_faces, yellow, V, F);

                return 0;
            }
            return vid;
        };
        std::cout << R"(Usage:[click]  Pick face on shape)" << "\n\n";
        std::cout << "Left Click anywhere in libigl viewer to initialise it" << "\n\n";

#pragma endregion 


#pragma region ImGui Interface

        //ImGui Setup
        igl::opengl::glfw::imgui::ImGuiMenu menu;
        viewer.plugins.push_back(&menu);

        bool GaussCurvSwitch, GaussDirSwitch, SCswitch, GeoDistSwitch;
        bool clicked = false;
        int li = -1;

        //ImGui Configuration
        menu.callback_draw_viewer_menu = [&]()
        {
            
            //Selected Viewport
            if (ImGui::InputInt("selectedViewport", &viewport_index))
            {
                viewport_index = std::max(0, std::min(3, viewport_index));

                viewer.data_list[viewport_index].clear();

                switch (viewport_index)
                {
                case 0:
                    V = V1;
                    F = F1;
                    G = G1;
                    U = U1;
                    break;

                case 1:
                    V = V2;
                    F = F2;
                    G = G2;
                    U = U2;
                    break;

                case 2:
                    V = V3;
                    F = F3;
                    G = G3;
                    U = U3;
                    break;

                case 3:
                    V = V4;
                    F = F4;
                    G = G4;
                    U = U4;
                    break;
                }

                viewer.data_list[viewport_index].set_mesh(V, F);
            }

            //decimate - subdivide                  
            if (ImGui::Button("decimate", ImVec2(-1, 0)))
                {
                    clicked = true;
                    if (window_click)
                    {
                        Eigen::VectorXi J;
                        igl::decimate(V, F, 1000, U, G, J);

                        viewer.data_list[viewport_index].clear();
                        viewer.data_list[viewport_index].set_mesh(U, G);

                        switch (viewport_index)
                        {
                        case 0:
                            V1 = U;
                            F1 = G;
                            V = V1;
                            F = F1;
                            break;

                        case 1:
                            V2 = U;
                            F2 = G;
                            V = V2;
                            F = F2;
                            break;

                        case 2:
                            V3 = U;
                            F3 = G;
                            V = V3;
                            F = F3;
                            break;

                        case 3:
                            V4 = U;
                            F4 = G;
                            V = V4;
                            F = F4;
                            break;
                        }
                        viewer.data_list[viewport_index].set_face_based(true);
                    }                  
                }

            if (ImGui::Button("subdivide", ImVec2(-1, 0)))
                {   
                    clicked = true;
                    if (window_click)
                    {
                        igl::upsample(V, F, U, G);

                        viewer.data_list[viewport_index].clear();
                        viewer.data_list[viewport_index].set_mesh(U, G);

                        switch (viewport_index)
                        {
                        case 0:
                            V1 = U;
                            F1 = G;
                            V = V1;
                            F = F1;
                            break;

                        case 1:
                            V2 = U;
                            F2 = G;
                            V = V2;
                            F = F2;
                            break;

                        case 2:
                            V3 = U;
                            F3 = G;
                            V = V3;
                            F = F3;
                            break;

                        case 3:
                            V4 = U;
                            F4 = G;
                            V = V4;
                            F = F4;
                            break;
                        }
                        viewer.data_list[viewport_index].set_face_based(true);
                    }                                                   
                }
                       
            //Flag Check
            if (li != viewport_index || clicked)
            {
                GaussCurvSwitch = false;
                GaussDirSwitch = false;
                GeoDistSwitch = false;
                SCswitch = false;
                clicked = false;
                li = viewport_index;
            }

            //Early Crash Prevention
            if (window_click)
            {
                //Gaussian Curvature
                if (ImGui::Checkbox("Gaussian Curvature", &GaussCurvSwitch))
                {
                    viewer.data_list[viewport_index].clear();

                    if (GaussCurvSwitch)
                    {
                        GaussDirSwitch = false;
                        GeoDistSwitch = false;
                        SCswitch = false;
                        GaussCurv_color(viewport_index, viewer, V, F);
                    }

                    else
                    {
                        switch (viewport_index)
                        {
                        case 0:
                            V = V1;
                            F = F1;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 1:
                            V = V2;
                            F = F2;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 2:
                            V = V3;
                            F = F3;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 3:
                            V = V4;
                            F = F4;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;
                        }
                        viewer.data_list[viewport_index].set_mesh(V, F);
                    }
                }

                //Curvature Directions
                if (ImGui::Checkbox("Gaussian Directions", &GaussDirSwitch))
                {
                    viewer.data_list[viewport_index].clear();

                    if (GaussDirSwitch)
                    {
                        GaussCurvSwitch = false;
                        GeoDistSwitch = false;
                        SCswitch = false;
                        GaussDir_color(viewport_index, viewer, V, F);
                    }

                    else
                    {
                        switch (viewport_index)
                        {
                        case 0:
                            V = V1;
                            F = F1;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 1:
                            V = V2;
                            F = F2;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 2:
                            V = V3;
                            F = F3;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 3:
                            V = V4;
                            F = F4;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;
                        }
                        viewer.data_list[viewport_index].set_mesh(V, F);
                    }
                }

                //Surface Classification
                if (ImGui::Checkbox("Surface classification", &SCswitch)) {

                    if (SCswitch)
                    {
                        GaussCurvSwitch = false;
                        GaussDirSwitch = false;
                        GeoDistSwitch = false;
                        Eigen::RowVector3d purple(0.5, 0, 0.5), green(0, 1, 0), red(1, 0, 0);
                        color_vertices(viewport_index, purple, green, red, viewer, V, F);
                    }

                    else
                    {
                        viewer.data_list[viewport_index].clear();
                        switch (viewport_index)
                        {
                        case 0:
                            V = V1;
                            F = F1;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 1:
                            V = V2;
                            F = F2;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 2:
                            V = V3;
                            F = F3;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 3:
                            V = V4;
                            F = F4;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;
                        }
                        viewer.data_list[viewport_index].set_mesh(V, F);
                    }
                }

                if (ImGui::Checkbox("Geodesic Distance", &GeoDistSwitch)) {

                    if (GeoDistSwitch)
                    {
                        GaussCurvSwitch = false;
                        GaussDirSwitch = false;
                        SCswitch = false;                                   
                    }

                    else
                    {
                        switch (viewport_index)
                        {
                        case 0:
                            V = V1;
                            F = F1;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 1:
                            V = V2;
                            F = F2;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 2:
                            V = V3;
                            F = F3;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;

                        case 3:
                            V = V4;
                            F = F4;
                            std::cout << "Viewport : " << viewport_index << "\n";
                            break;
                        }
                        viewer.data_list[viewport_index].set_mesh(V, F);
                    }
                }
            }
        };


#pragma endregion panagia mou teleiwse

        viewer.launch();
        return EXIT_SUCCESS;
 }


